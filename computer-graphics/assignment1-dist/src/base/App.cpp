#define _CRT_SECURE_NO_WARNINGS

#include "App.hpp"

#include "utility.hpp"
#include "base/Main.hpp"
#include "gpu/GLContext.hpp"
#include "gpu/Buffer.hpp"

#include <array>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <type_traits>

using namespace FW;
using namespace std;

// Anonymous namespace. This is a C++ way to define things which
// do not need to be visible outside this file.
namespace {

enum VertexShaderAttributeLocations {
	ATTRIB_POSITION = 0,
	ATTRIB_NORMAL = 1,
	ATTRIB_COLOR = 2
};

const Vertex reference_plane_data[] = {
	{ Vec3f(-1, -1, -1), Vec3f(0, 1, 0) },
	{ Vec3f( 1, -1, -1), Vec3f(0, 1, 0) },
	{ Vec3f( 1, -1,  1), Vec3f(0, 1, 0) },
	{ Vec3f(-1, -1, -1), Vec3f(0, 1, 0) },
	{ Vec3f( 1, -1,  1), Vec3f(0, 1, 0) },
	{ Vec3f(-1, -1,  1), Vec3f(0, 1, 0) }
};

vector<Vertex> loadExampleModel() {
	static const Vertex example_data[] = {
		{ Vec3f( 0.0f,  0.5f, 0), Vec3f(0.0f, 0.0f, -1.0f) },
		{ Vec3f(-0.5f, -0.5f, 0), Vec3f(0.0f, 0.0f, -1.0f) },
		{ Vec3f( 0.5f, -0.5f, 0), Vec3f(0.0f, 0.0f, -1.0f) }
	};
	vector<Vertex> vertices;
	for (auto v : example_data)
		vertices.push_back(v);
	return vertices;
}

vector<Vertex> unpackIndexedData(
	const vector<Vec3f>& positions,
	const vector<Vec3f>& normals,
	const vector<array<unsigned, 6>>& faces)
{
	vector<Vertex> vertices;

	// This is a 'range-for' loop which goes through all objects in the container 'faces'.
	// '&' gives us a reference to the object inside the container; if we omitted '&',
	// 'f' would be a copy of the object instead.
	// The compiler already knows the type of objects inside the container, so we can
	// just write 'auto' instead of having to spell out 'array<unsigned, 6>'.
	Vertex v0, v1, v2;

	// Iterate through sets of vertex indices
	for (auto& f : faces) {

		//Positions == even in f, normals == odd
		v0.position = positions[f[0]];
		v1.position = positions[f[2]];
		v2.position = positions[f[4]];

		v0.normal = normals[f[1]];
		v1.normal = normals[f[3]];
		v2.normal = normals[f[5]];

		vertices.push_back(v0); vertices.push_back(v1); vertices.push_back(v2);
	}

	return vertices;
};

// This is for testing your unpackIndexedData implementation.
// You should get a tetrahedron like in example.exe.
vector<Vertex> loadIndexedDataModel() {
	static const Vec3f point_data[] = {
		Vec3f(0.0f, 0.407f, 0.0f),
		Vec3f(0.0f, -0.3f, -0.5f),
		Vec3f(0.433f, -0.3f, 0.25f),
		Vec3f(-0.433f, -0.3f, 0.25f),
	};
	static const Vec3f normal_data[] = {
		Vec3f(0.8165f, 0.3334f, -0.4714f),
		Vec3f(0.0f, 0.3334f, 0.9428f),
		Vec3f(-0.8165f, 0.3334f, -0.4714f),
		Vec3f(0.0f, -1.0f, 0.0f)
	};
	static const unsigned face_data[][6] = {
		{0, 0, 1, 0, 2, 0},
		{0, 2, 3, 2, 1, 2},
		{0, 1, 2, 1, 3, 1},
		{1, 3, 3, 3, 2, 3}
	};
	vector<Vec3f> points(point_data, point_data + SIZEOF_ARRAY(point_data));
	vector<Vec3f> normals(normal_data, normal_data + SIZEOF_ARRAY(normal_data));
	vector<array<unsigned, 6>> faces;
	for (auto arr : face_data) {
		array<unsigned, 6> f;
		copy(arr, arr+6, f.begin());
		faces.push_back(f);
	}
	return unpackIndexedData(points, normals, faces);
}

// Generate an upright cone with tip at (0, 0, 0), a radius of 0.25 and a height of 1.0.
// You can leave the base of the cone open, like it is in example.exe.
vector<Vertex> loadUserGeneratedModel() {
	static const float radius = 0.25f;
	static const float height = 1.0f;
	static const unsigned faces = 40;
	static const float angle_increment = 2 * FW_PI / faces;

	// Empty array of Vertex structs; every three vertices = one triangle
	vector<Vertex> vertices;
	
	Vertex v0, v1, v2;

	// Generate one face at a time
	for(auto i = 0u; i < faces; ++i)	{
		// Vertex positions
		v0.position = Vec3f();
		v1.position = Vec3f(FW::cos(angle_increment * i) * radius, -height, FW::sin(angle_increment * i) * radius);
		v2.position = Vec3f(FW::cos(angle_increment * (i+1)) * radius, -height, FW::sin(angle_increment * (i+1)) * radius);

		// Calculate the normal of the face from the positions and use it for all vertices.
		auto cross = (v2.position - v1.position).cross(v1.position);
		cross.normalize();
		v0.normal = v1.normal = v2.normal = cross;
		
		// add the vertices to the array.
		vertices.push_back(v0); vertices.push_back(v1); vertices.push_back(v2);
	}
	return vertices;
}

}

App::App(void)
:   common_ctrl_			(CommonControls::Feature_Default & ~CommonControls::Feature_RepaintOnF5),
	current_model_			(MODEL_EXAMPLE),
	timer_					(Timer(true)),
	model_changed_			(true),
	shading_toggle_			(false),
	shading_mode_changed_	(false),
	fullscreen_toggle_		(false),
	fullscreen_changed_		(false),
	animating_				(false),
	camera_angles_			(Vec3f(0, 0, 0)),
	camera_position_		(Vec3f(0, 0, 2.1f)),
	obj_translation_		(Mat4f()),
	obj_rotation_			(Mat4f()),
	obj_scale_				(Mat4f()),
	obj_angles_				(Vec3f(0, 0, 0))
{
	static_assert(is_standard_layout<Vertex>::value, "struct Vertex must be standard layout to use offsetof");
	initRendering();
	
	common_ctrl_.showFPS(true);
	common_ctrl_.addToggle((S32*)&current_model_, MODEL_EXAMPLE,			FW_KEY_1, "Triangle (1)",				&model_changed_);
	common_ctrl_.addToggle((S32*)&current_model_, MODEL_USER_GENERATED,		FW_KEY_2, "Generated cone (2)",			&model_changed_);
	common_ctrl_.addToggle((S32*)&current_model_, MODEL_FROM_INDEXED_DATA,	FW_KEY_3, "Unpacked tetrahedron (3)",	&model_changed_);
	common_ctrl_.addToggle((S32*)&current_model_, MODEL_FROM_FILE,			FW_KEY_4, "Model loaded from file (4)",	&model_changed_);
	common_ctrl_.addSeparator();
	common_ctrl_.addToggle(&shading_toggle_,								FW_KEY_T, "Toggle shading mode (T)",	&shading_mode_changed_);
	common_ctrl_.addToggle(&fullscreen_toggle_,								FW_KEY_F, "Toggle fullscreen (F)",	&fullscreen_changed_);

	window_.setTitle("Assignment 1");

	window_.addListener(this);
	window_.addListener(&common_ctrl_);

	window_.setSize( Vec2i(1200, 800) );
}

void App::streamGeometry(const std::vector<Vertex>& vertices) {
	// Load the vertex buffer to GPU.
	glBindBuffer(GL_ARRAY_BUFFER, gl_.dynamic_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	vertex_count_ = vertices.size();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

bool App::handleEvent(const Window::Event& ev) {

	static const float obj_transform_delta = 0.02f;	
	static const float camera_movement_delta = 0.015f;	

	if (model_changed_)	{
		model_changed_ = false;

		switch (current_model_)
		{
		case MODEL_EXAMPLE:
			streamGeometry(loadExampleModel());
			break;
		case MODEL_FROM_INDEXED_DATA:
			streamGeometry(loadIndexedDataModel());
			break;
		case MODEL_USER_GENERATED:
			streamGeometry(loadUserGeneratedModel());
			break;
		case MODEL_FROM_FILE:
			{
				auto filename = window_.showFileLoadDialog("Load new mesh");
				if (filename.getLength()) {
					streamGeometry(loadObjFileModel(filename.getPtr()));
				} else {
					current_model_ = MODEL_EXAMPLE;
					model_changed_ = true;
				}
			}
			break;
		default:
			assert(false && "invalid model type");
		}
	}
	
	if (shading_mode_changed_) {
		common_ctrl_.message(shading_toggle_ ?
			"Directional light shading using vertex normals; direction to light (0.5, 0.5, -0.6)" :
			"High visibility shading, color from vertex ID");
		shading_mode_changed_ = false;
	}

	if (fullscreen_changed_) {
		window_.toggleFullScreen();
		fullscreen_changed_ = false;
	}


	if (ev.type == Window::EventType_KeyDown) {
		keys_pressed_[ev.key] = true;

		if (ev.key == FW_KEY_R)
			animating_ = !animating_;
	}
	
	if (ev.type == Window::EventType_KeyUp)
		keys_pressed_[ev.key] = false;

	if (ev.type == Window::EventType_Mouse) {
		if (keys_pressed_[FW_KEY_MOUSE_LEFT]) {
			// camera y == screen x somewhy ?? not sure why this way works
			camera_angles_.y -= ev.mouseDelta.x / 60.0;
			camera_angles_.x -= ev.mouseDelta.y / 60.0;

		}
		// EXTRA: you can put your mouse controls here.
		// Event::mouseDelta gives the distance the mouse has moved.
		// Event::mouseDragging tells whether some mouse buttons are currently down.
		// If you want to know which ones, you have to keep track of the button down/up events
		// (e.g. FW_KEY_MOUSE_LEFT).
	}

	if (ev.type == Window::EventType_Close) {
		window_.showModalMessage("Exiting...");
		delete this;
		return true;
	}




	window_.setVisible(true);
	if (ev.type == Window::EventType_Paint)
		render();

	// check conditions approx every "frame", where fps is 60
	if (timer_.getElapsed() >= 0.016) {

		bool is_shift = keys_pressed_[FW_KEY_SHIFT];
		bool is_ctrl = keys_pressed_[FW_KEY_CONTROL];

		if (animating_) {
			obj_angles_.x += 0.01 * FW_PI;
		}

		// Object x-axis transformations
		if (keys_pressed_[FW_KEY_LEFT]) {
			if (is_shift)
				obj_scale_.m00 -= obj_transform_delta;
			else if (is_ctrl)
				obj_angles_.x -= obj_transform_delta;
			else
				obj_translation_ *= Mat4f::translate(Vec3f(-obj_transform_delta, 0, 0));
		}
		if (keys_pressed_[FW_KEY_RIGHT]) {
			if (is_shift)
				obj_scale_.m00 += obj_transform_delta;
			else if (is_ctrl)
				obj_angles_.x += obj_transform_delta;
			else
				obj_translation_ *= Mat4f::translate(Vec3f(obj_transform_delta, 0, 0));
		}

		// Object y-axis transformations
		if (keys_pressed_[FW_KEY_UP]) {
			if (is_shift)
				obj_scale_.m11 += obj_transform_delta;
			else if (is_ctrl)
				obj_angles_.y += obj_transform_delta;
			else
				obj_translation_ *= Mat4f::translate(Vec3f(0, obj_transform_delta, 0));
		}
		if (keys_pressed_[FW_KEY_DOWN]) {
			if (is_shift)
				obj_scale_.m11 -= obj_transform_delta;
			else if (is_ctrl)
				obj_angles_.y -= obj_transform_delta;
			else
				obj_translation_ *= Mat4f::translate(Vec3f(0, -obj_transform_delta, 0));
		}

		// Object z-axis transformations
		if (keys_pressed_[FW_KEY_PAGE_UP]) {
			if (is_shift)
				obj_scale_.m22 += obj_transform_delta;
			else if (is_ctrl)
				obj_angles_.z += obj_transform_delta;
			else
				obj_translation_ *= Mat4f::translate(Vec3f(0, 0, obj_transform_delta));
		}
		if (keys_pressed_[FW_KEY_PAGE_DOWN]) {
			if (is_shift)
				obj_scale_.m22 -= obj_transform_delta;
			else if (is_ctrl)
				obj_angles_.z -= obj_transform_delta;
			else
				obj_translation_ *= Mat4f::translate(Vec3f(0, 0, -obj_transform_delta));
		}

		// Camera movement
		if (keys_pressed_[FW_KEY_W])
			camera_position_.z -= camera_movement_delta;
		if (keys_pressed_[FW_KEY_S])
			camera_position_.z += camera_movement_delta;
		if (keys_pressed_[FW_KEY_A])
			camera_position_.x += camera_movement_delta;
		if (keys_pressed_[FW_KEY_D])
			camera_position_.x -= camera_movement_delta;
		if (keys_pressed_[FW_KEY_SPACE]) 
			camera_position_.y -= camera_movement_delta;
		if (keys_pressed_[FW_KEY_ALT])
			camera_position_.y += camera_movement_delta;

		// restart timer 
		timer_.start();
	}

	window_.repaint();

	return false;
}

void App::initRendering() {
	// Ask the Nvidia framework for the GLContext object associated with the window.
	// As a side effect, this initializes the OpenGL context and lets us call GL functions.
	auto ctx = window_.getGL();
	
	// Create vertex attribute objects and buffers for vertex data.
	glGenVertexArrays(1, &gl_.static_vao);
	glGenVertexArrays(1, &gl_.dynamic_vao);
	glGenBuffers(1, &gl_.static_vertex_buffer);
	glGenBuffers(1, &gl_.dynamic_vertex_buffer);
	
	// Set up vertex attribute object for static data.
	glBindVertexArray(gl_.static_vao);
	glBindBuffer(GL_ARRAY_BUFFER, gl_.static_vertex_buffer);
	glEnableVertexAttribArray(ATTRIB_POSITION);
	glVertexAttribPointer(ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) 0);
	glEnableVertexAttribArray(ATTRIB_NORMAL);
	glVertexAttribPointer(ATTRIB_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) offsetof(Vertex, normal));

	// Load the static data to the GPU; needs to be done only once.
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * SIZEOF_ARRAY(reference_plane_data), reference_plane_data, GL_STATIC_DRAW);
	
	// Set up vertex attribute object for dynamic data. We'll load the actual data later, whenever the model changes.
	glBindVertexArray(gl_.dynamic_vao);
	glBindBuffer(GL_ARRAY_BUFFER, gl_.dynamic_vertex_buffer);
	glEnableVertexAttribArray(ATTRIB_POSITION);
	glVertexAttribPointer(ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) 0);
	glEnableVertexAttribArray(ATTRIB_NORMAL);
	glVertexAttribPointer(ATTRIB_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) offsetof(Vertex, normal));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	// Compile and link the shader program.
	// We use the Nvidia FW for creating the program; it's not difficult to do manually,
	// but takes about half a page of OpenGL boilerplate code.
	// This shader program will be used to draw everything except the user interface.
	// It consists of one vertex shader and one fragment shader.
	auto shader_program = new GLContext::Program(
		"#version 330\n"
		FW_GL_SHADER_SOURCE(
		layout(location = 0) in vec4 aPosition;
		layout(location = 1) in vec3 aNormal;
		
		out vec4 vColor;
		
		uniform mat4 uModelToWorld;
		uniform mat4 uWorldToClip;
		uniform float uShading;
		
		const vec3 distinctColors[6] = vec3[6](
			vec3(0, 0, 1), vec3(0, 1, 0), vec3(0, 1, 1),
			vec3(1, 0, 0), vec3(1, 0, 1), vec3(1, 1, 0));
		const vec3 directionToLight = normalize(vec3(0.5, 0.5, -0.6));
		
		void main()
		{
			// EXTRA: oops, someone forgot to transform normals here...
			float clampedCosine = clamp(dot(aNormal, directionToLight), 0.0, 1.0);
			vec3 litColor = vec3(clampedCosine);
			vec3 generatedColor = distinctColors[gl_VertexID % 6];
			// gl_Position is a built-in output variable that marks the final position
			// of the vertex in clip space. Vertex shaders must write in it.
			gl_Position = uWorldToClip * uModelToWorld * aPosition;
			vColor = vec4(mix(generatedColor, litColor, uShading), 1);
		}
		),
		"#version 330\n"
		FW_GL_SHADER_SOURCE(
		in vec4 vColor;
		out vec4 fColor;
		void main()
		{
			fColor = vColor;
		}
		));
	// Tell the FW about the program so it gets properly destroyed at exit.
	ctx->setProgram("shaders", shader_program);

	// Get the IDs of the shader program and its uniform input locations from OpenGL.
	gl_.shader_program = shader_program->getHandle();
	gl_.world_to_clip_uniform = glGetUniformLocation(gl_.shader_program, "uWorldToClip");
	gl_.model_to_world_uniform = glGetUniformLocation(gl_.shader_program, "uModelToWorld");
	gl_.shading_toggle_uniform = glGetUniformLocation(gl_.shader_program, "uShading");
}

void App::render() {
	// Clear screen.
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Enable depth testing.
	glEnable(GL_DEPTH_TEST);

	float dim_x = window_.getSize().x;
	float dim_y = window_.getSize().y;
	//glViewport(0, 0, min_dim, min_dim);
	
	// Set up a matrix to transform from world space to clip space.
	// Clip space is a [-1, 1]^3 space where OpenGL expects things to be
	// when it starts drawing them.

	Mat4f C = Mat4f();
	C.setCol(3, Vec4f(camera_position_, 1));

	Mat3f cam_rot =
		Mat3f::rotation(Vec3f(1, 0, 0), camera_angles_.x) *
		Mat3f::rotation(Vec3f(0, 1, 0), camera_angles_.y);

	Mat4f R;
	R.setCol(0, Vec4f(cam_rot.getCol(0), 0));
	R.setCol(1, Vec4f(cam_rot.getCol(1), 0));
	R.setCol(2, Vec4f(cam_rot.getCol(2), 0));
	
	// Simple perspective.
	static const float fnear = 0.1f, ffar = 8.0f;
	Mat4f P;
	P.setCol(0, Vec4f(1, 0, 0, 0));
	P.setCol(1, Vec4f(0, dim_x/dim_y, 0, 0));
	P.setCol(2, Vec4f(0, 0, (ffar+fnear)/(ffar-fnear), 1));
	P.setCol(3, Vec4f(0, 0, -2*ffar*fnear/(ffar-fnear), 0));

	Mat4f world_to_clip = P * C * R;
	
	// Set active shader program.
	glUseProgram(gl_.shader_program);
	glUniform1f(gl_.shading_toggle_uniform, shading_toggle_ ? 1.0f : 0.0f);
	glUniformMatrix4fv(gl_.world_to_clip_uniform, 1, GL_FALSE, world_to_clip.getPtr());

	// Draw the reference plane. It is already in world coordinates.
	auto identity = Mat4f();
	glUniformMatrix4fv(gl_.model_to_world_uniform, 1, GL_FALSE, identity.getPtr());
	glBindVertexArray(gl_.static_vao);
	glDrawArrays(GL_TRIANGLES, 0, SIZEOF_ARRAY(reference_plane_data));
	
	// Compose object rotation matrix
	Mat3f obj_rot = 
		Mat3f::rotation(Vec3f(1, 0, 0), obj_angles_.x) * 
		Mat3f::rotation(Vec3f(0, 1, 0), obj_angles_.y) * 
		Mat3f::rotation(Vec3f(0, 0, 1), obj_angles_.z);

	obj_rotation_.setCol(0, Vec4f(obj_rot.getCol(0), 0));
	obj_rotation_.setCol(1, Vec4f(obj_rot.getCol(1), 0));
	obj_rotation_.setCol(2, Vec4f(obj_rot.getCol(2), 0));

	// Set the model space -> world space transform to translate the model according to user input.
	Mat4f modelToWorld = obj_translation_ * obj_rotation_ * obj_scale_;
	
	// Draw the model with your model-to-world transformation.
	glUniformMatrix4fv(gl_.model_to_world_uniform, 1, GL_FALSE, modelToWorld.getPtr());
	glBindVertexArray(gl_.dynamic_vao);
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertex_count_);

	// Undo our bindings.
	glBindVertexArray(0);
	glUseProgram(0);
	
	// Check for OpenGL errors.
	GLContext::checkErrors();
	
	// Show status messages. You may find it useful to show some debug information in a message.
	common_ctrl_.message(sprintf("Click and drag to rotate camera"), "instructions0");
	common_ctrl_.message(sprintf("WASD, Space/Alt: move camera"), "instructions1");
	common_ctrl_.message(sprintf("Arrow keys & PageUp/PageDown: translate object"), "instructions2");
	common_ctrl_.message(sprintf("SHIFT + translation controls: scale object"), "instructions3");
	common_ctrl_.message(sprintf("CTRL + translation controls: rotate object"), "instructions4");
	common_ctrl_.message(sprintf("F: toggle fullscreen"), "instructions5");

}

vector<Vertex> App::loadObjFileModel(string filename) {
	window_.showModalMessage(sprintf("Loading mesh from '%s'...", filename.c_str()));

	vector<Vec3f> positions, normals;
	vector<array<unsigned, 6>> faces;

	// Open input file stream for reading.
	ifstream input(filename, ios::in);

	// Read the file line by line.
	string line;
	while(getline(input, line)) {
		// Replace any '/' characters with spaces ' ' so that all of the
		// values we wish to read are separated with whitespace.
		for (auto& c : line)
			if (c == '/')
				c = ' ';
			
		// Temporary objects to read data into.
		array<unsigned, 6>  f; // Face index array
		Vec3f               v;
		string              s;

		// Create a stream from the string to pick out one value at a time.
		istringstream        iss(line);

		// Read the first token from the line into string 's'.
		// It identifies the type of object (vertex or normal or ...)
		iss >> s;

		if (s == "v") { // vertex position
			iss >> v.x; iss >> v.y; iss >> v.z; 
			positions.push_back(v);
		} 
		else if (s == "vn") { // normal
			iss >> v.x; iss >> v.y; iss >> v.z; 
			normals.push_back(v);
		} 
		else if (s == "f") { // face
			unsigned sink; // Temporary variable for reading the unused texture indices.
			iss >> f[0]; iss >> sink; iss >> f[1];
			iss >> f[2]; iss >> sink; iss >> f[3];
			iss >> f[4]; iss >> sink; iss >> f[5];

			// obj indexes (uncool) to normal indexes (very cool) 
			f[0]--; f[1]--; f[2]--; f[3]--; f[4]--; f[5]--;

			faces.push_back(f);
		}
	}
	common_ctrl_.message(("Loaded mesh from " + filename).c_str());
	return unpackIndexedData(positions, normals, faces);
}

void FW::init(void) {
	new App;
}
