#include "App.hpp"
#include "base/Main.hpp"
#include "gpu/GLContext.hpp"
#include "3d/Mesh.hpp"
#include "io/File.hpp"
#include "io/StateDump.hpp"
#include "base/Random.hpp"

#include "Subdiv.hpp"

#include <stdio.h>
#include <conio.h>

#include <vector>
#include <map>

using namespace FW;

namespace FW {

void MeshWithConnectivity::fromMesh( const Mesh<VertexPNC>& m )
{
	positions.resize(m.numVertices());
	normals.resize(m.numVertices());
	colors.resize(m.numVertices());

	for (int i = 0; i < m.numVertices(); ++i) {
		positions[i] = m.vertex(i).p;
		normals[i] = m.vertex(i).n;
		colors[i] = m.vertex(i).c.getXYZ();
	}

	indices.reserve(m.numTriangles());

	// move indices
	for (int i = 0; i < m.numSubmeshes(); ++i)
		for (int t = 0; t < m.indices(i).getSize(); ++t)
			indices.push_back(m.indices(i)[t]);

	computeConnectivity();
}

// assumes vertices and indices are already filled in.
void MeshWithConnectivity::computeConnectivity()
{
	// assign default values. boundary edges (no neighbor on other side) are denoted by -1.
	neighborTris.assign(indices.size(), Vec3i(-1,-1,-1));
	neighborEdges.assign(indices.size(), Vec3i(-1,-1,-1));

	// bookkeeping: map edges (vert0, vert1) to (triangle, edge_number) pairs
	typedef std::map<std::pair<int, int>, std::pair<int, int>> edgemap_t;
	edgemap_t M;

	for (int i = 0; i < (int)indices.size(); ++i) {
		// vertex index is also an index for the corresponding edge starting at that vertex
		for (int j = 0; j < 3; ++j) {
			int v0 = indices[i][j];
			int v1 = indices[i][(j+1)%3];
			auto it = M.find(std::make_pair(v1, v0));
			if (it == M.end()) {
				// edge not found, add myself to mapping
				// (opposite direction than when finding because we look for neighbor edges)
				M[std::make_pair(v0, v1)] = std::make_pair(i, j);
			} else {
				if (it->second.first == -1)	{
					FW::printf( "Non-manifold edge detected\n" );
				} else {
					// other site found, let's fill in the data
					int other_t = it->second.first;
					int other_e = it->second.second;

					neighborTris[i][j] = other_t;
					neighborEdges[i][j] = other_e;

					neighborTris[other_t][other_e] = i;
					neighborEdges[other_t][other_e] = j;

					it->second.first = -1;
				}
			}
		}
	}
	
}

// Run a debug version of the subdivision pass where we only subdivide the one triangle
// that's under the mouse cursor. Returns a list of positions that need to be drawn by App
std::vector<Vec3f> MeshWithConnectivity::debugHighlight(Vec2f mousePos, Mat4f worldToClip)
{
	Vec2i closestIdx = -1;
	float minCost = 1e9;

	// loop through vertices and find the one that's closest to our mouse click
	for (int i = 0; i < indices.size(); ++i)
		for(int j = 0; j < 3; ++j)
		{
			int idx = indices[i][j];
			Vec4f clip = worldToClip * Vec4f(positions[idx], 1.0f);
			Vec3f clipPos = clip.getXYZ() / clip.w;
			float depth = clip.w;

			// use a cost function that prefers points that are closer to camera
			float dist = (clipPos.getXY() - mousePos).length();
			float cost = dist + depth * .01f;

			if (cost < minCost)
			{
				minCost = cost;
				closestIdx = Vec2i(i, j);
			}
		}

	// If we found no valid vertices, return
	if (closestIdx.x == -1)
	{
		std::cout << "no vertices found under mouse position, aborting debug!\n";
		return std::vector<Vec3f>();
	}

	// clear debug data from previous calls
	highlightIndices.clear();

	// Call subdivision with the debugPass flag on to get debug data
	debugPass = true;
	debugVertexIdx = closestIdx;
	LoopSubdivision();

	// Set flag to false so we can run actual subdivision later
	debugPass = false;

	// create position vector out of highlight indices
	std::vector<Vec3f> debugPoints;
	for (auto& idx : highlightIndices)
	{
		Vec3f pos = positions[idx];
		Vec3f n = normals[idx];

		pos += n * .001f;
		debugPoints.push_back(pos);
	}

	// return debug data so that App can draw it
	return debugPoints;
}

void MeshWithConnectivity::toMesh(Mesh<VertexPNC>& dest) {
	dest.resetVertices((int)positions.size());
	for (size_t i = 0; i < positions.size(); ++i) {
		dest.mutableVertex((int)i).p = positions[i];
		dest.mutableVertex((int)i).n = normals[i];
		dest.mutableVertex((int)i).c = Vec4f(colors[i], 1.0f);
	}
	dest.resizeSubmeshes(1);
	dest.mutableIndices(0).replace(0, dest.indices(0).getSize(), &indices[0], (int)indices.size());
}

void MeshWithConnectivity::LoopSubdivision() {
	// generate new (odd) vertices

	const static float f1by8  = 1.0 / 8;
	const static float f3by8  = 3.0 / 8;
	const static float f3by16 = 3.0 / 16;

	// visited edge -> vertex position information
	// Note that this is different from the one in computeConnectivity()
	typedef std::map<std::pair<int, int>, int> edgemap_t;
	edgemap_t new_vertices;

	// The new data must be doublebuffered or otherwise some of the calculations below would
	// not read the original positions but the newly changed ones, which is slightly wrong.
	std::vector<Vec3f> new_positions(positions.size());
	std::vector<Vec3f> new_normals(normals.size());
	std::vector<Vec3f> new_colors(colors.size());

	highlightIndices.clear();

	// If we're debugging, skip this part since we're only interested in the 1-ring portion. Feel free to change this if you need to.
	if (!debugPass)
	{
		for (size_t i = 0; i < indices.size(); ++i)
			for (int j = 0; j < 3; ++j) {
				int v0 = indices[i][j];
				int v1 = indices[i][(j + 1) % 3];

				// Map the edge endpoint indices to new vertex index.
				// We use min and max because the edge direction does not matter when we finally
				// rebuild the new faces (R3); this is how we always get unique indices for the map.
				auto edge = std::make_pair(min(v0, v1), max(v0, v1));

				// With naive iteration, we would find each edge twice, because each is part of two triangles
				// (if the mesh does not have any holes/empty borders). Thus, we keep track of the already
				// visited edges in the new_vertices map. That requires the small R3 task below in the 'if' block.
				if (new_vertices.find(edge) == new_vertices.end()) {
					int v2 = indices[i][(j + 2) % 3];

					int i_n = neighborTris[i][j];
					int j_n = (neighborEdges[i][j] + 2) % 3;

					int v3 = i_n == -1 ? 0 : indices[i_n][j_n];
					// YOUR CODE HERE (R4): compute the position for odd (= new) vertex.
					// You will need to use the neighbor information to find the correct vertices and then combine the four corner vertices with the correct weights.
					// Be sure to see section 3.2 in the handout for an in depth explanation of the neighbor index tables; the scheme is somewhat involved.
					Vec3f pos, col, norm;

					pos  = f3by8 * (positions[v0] + positions[v1]) + f1by8 * (positions[v2] + positions[v3]);
					col  = f3by8 * (colors[v0]	  + colors[v1])	   + f1by8 * (colors[v2]    + colors[v3]);
					norm = f3by8 * (normals[v0]   + normals[v1])   + f1by8 * (normals[v2]   + normals[v3]);

					new_positions.push_back(pos);
					new_colors.push_back(col);
					new_normals.push_back(norm);

					new_vertices[edge] = new_positions.size() - 1;				
				}
			}
	}
	// compute positions for even (old) vertices
	std::vector<bool> vertexComputed(new_positions.size(), false);

	for (int i = 0; i < (int)indices.size(); ++i) {
		for (int j = 0; j < 3; ++j) {
			int v0 = indices[i][j];

			// If we're doing the debug pass, set vertex index to the one under mouse position
			if (debugPass)
			{
				i = debugVertexIdx.x;
				j = debugVertexIdx.y;
				v0 = indices[i][j];
			}

			int v = indices[i][(j + 1) % 3];
			if(debugPass) highlightIndices.push_back(v);

			// don't redo if this one is already done
			if (vertexComputed[v0] && !debugPass)
				continue;

			vertexComputed[v0] = true;

			int n = 1;
			Vec3f pos  = positions[v];
			Vec3f col  = colors[v];
			Vec3f norm = normals[v];

			int j_n = (neighborEdges[i][j] + 1) % 3;
			int tri = neighborTris[i][j];

			while (tri != i) {
				if (tri == -1) break;
				v = indices[tri][(j_n + 1) % 3];

				if(debugPass) highlightIndices.push_back(v);

				n++;
				pos  += positions[v];
				col  += colors[v];
				norm += normals[v];


				// temp variable to store neighbor tri
				int tri_n = neighborTris[tri][j_n];
				j_n = (neighborEdges[tri][j_n] + 1) % 3;

				tri = tri_n;
			}

			if (tri != -1) {
				float B = n == 3 ? f3by16 : 3.0f / (8 * n);

				new_positions[v0] = (1.0f - n * B) * positions[v0] + B * pos;
				new_colors[v0]    = (1.0f - n * B) * colors[v0]    + B * col;
				new_normals[v0]   = (1.0f - n * B) * normals[v0]   + B * norm;
			}
			else {
				new_positions[v0] = positions[v0];
				new_colors[v0]    = colors[v0];
				new_normals[v0]   = normals[v0];
			}


			// Stop here if we're doing the debug pass since we don't actually need to modify the mesh
			if (debugPass)
				return;
		}
	}

	// Again, if we're doing the debug pass, we only care about our 1-ring so we can stop now
	if (debugPass)
		return;

	// and then, finally, regenerate topology
	// every triangle turns into four new ones
	std::vector<Vec3i> new_indices;
	new_indices.reserve(indices.size()*4);
	for (size_t i = 0; i < indices.size(); ++i) {
		Vec3i even = indices[i]; // start vertices of e_0, e_1, e_2

		auto edge_a = std::make_pair(min(even[0], even[1]), max(even[0], even[1]));
		auto edge_b = std::make_pair(min(even[1], even[2]), max(even[1], even[2]));
		auto edge_c = std::make_pair(min(even[2], even[0]), max(even[2], even[0]));

		// The edges edge_a, edge_b and edge_c now define the vertex indices via new_vertices.
		// (The mapping is done in the loop above.)
		// The indices define the smaller triangle inside the indices defined by "even", in order.
		// Read the vertex indices out of new_vertices to build the small triangle "odd"
		Vec3i odd = Vec3i(new_vertices[edge_a], new_vertices[edge_b], new_vertices[edge_c]);

		// Then, construct the four smaller triangles from the surrounding big triangle  "even"
		// and the inner one, "odd". Push them to "new_indices".
		new_indices.push_back(Vec3i(even[0],  odd[0],  odd[2]));
		new_indices.push_back(Vec3i( odd[0], even[1],  odd[1]));
		new_indices.push_back(Vec3i( odd[2],  odd[1], even[2]));
		new_indices.push_back(Vec3i( odd[0],  odd[1],  odd[2]));
	}

	indices = std::move(new_indices);
	positions = std::move(new_positions);
	normals = std::move(new_normals);
	colors = std::move(new_colors);
}

} // namespace FW
