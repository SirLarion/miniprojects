#include "curve.h"
#include "extra.h"
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
using namespace std;
using namespace FW;

namespace {

// Approximately equal to.  We don't want to use == because of
// precision issues with floating point.
inline bool approx(const Vec3f& lhs, const Vec3f& rhs) {
	const float eps = 1e-8f;
	return (lhs - rhs).lenSqr() < eps;
}


Mat4f bernsteinSpline() {
    Mat4f B;
	B.setRow(0, Vec4f(1, -3,  3, -1));
	B.setRow(1, Vec4f(0,  3, -6,  3));
	B.setRow(2, Vec4f(0,  0,  3, -3));
	B.setRow(3, Vec4f(0,  0,  0,  1));
    return B;
}

Mat4f bSpline() {
    Mat4f B;
    B.setRow(0, Vec4f(1, -3,  3, -1));
    B.setRow(1, Vec4f(4,  0, -6,  3));
    B.setRow(2, Vec4f(1,  3,  3, -3));
    B.setRow(3, Vec4f(0,  0,  0,  1));
    B *= 1.0 / 6;
    return B;
}



// This is the core routine of the curve evaluation code. Unlike
// evalBezier, this is only designed to work on 4 control points.
// Furthermore, it requires you to specify an initial binormal
// Binit, which is iteratively propagated throughout the curve as
// the curvepoints are generated. Any other function that creates
// cubic splines can use this function by a corresponding change
// of basis.
Curve coreBezier(const Vec3f& p0,
				 const Vec3f& p1,
				 const Vec3f& p2,
				 const Vec3f& p3,
				 const Vec3f& Binit,
				 unsigned steps) {

	Curve R(steps);

    // geometry
    Mat4f P;
    P.setCol(0, Vec4f(p0, 0));
    P.setCol(1, Vec4f(p1, 0));
    P.setCol(2, Vec4f(p2, 0));
    P.setCol(3, Vec4f(p3, 0));

    // spline
    Mat4f B = bernsteinSpline();

	for (unsigned i = 0; i < steps; ++i) {
        // power basis
        float t = (float)i / steps;
        Vec4f T = Vec4f(1, t, pow(t, 2), pow(t, 3));
        Vec4f V = P * B * T;
        R[i].V = V.getXYZ();
	}

	return R;
}    

// overloaded version with Vec4f to use Mat4f columns easier
Curve coreBezier(const Vec4f& p0,
				 const Vec4f& p1,
				 const Vec4f& p2,
				 const Vec4f& p3,
				 const Vec3f& Binit,
				 unsigned steps) {

	Curve R(steps);

    // geometry
    Mat4f P;
    P.setCol(0, p0);
    P.setCol(1, p1);
    P.setCol(2, p2);
    P.setCol(3, p3);

    // spline
    Mat4f B = bernsteinSpline();

	for (unsigned i = 0; i < steps; ++i) {
        // power basis
        float t = (float)i / steps;
        Vec4f T = Vec4f(1, t, pow(t, 2), pow(t, 3));
        Vec4f V = P * B * T;
        R[i].V = V.getXYZ();
	}

	return R;
}  

} // namespace

Curve coreBezier(const Vec3f& p0,
	const Vec3f& p1,
	const Vec3f& p2,
	const Vec3f& p3,
	const Vec3f& Binit,
	const float begin, const float end, const float errorbound, const float minstep) {

	// YOUR CODE HERE(EXTRA): Adaptive tessellation

	return Curve();
}
    
// the P argument holds the control points and steps gives the amount of uniform tessellation.
// the rest of the arguments are for the adaptive tessellation extra.
Curve evalBezier(const vector<Vec3f>& P, unsigned steps, bool adaptive, float errorbound, float minstep) {
    // Check
    if (P.size() < 4 || P.size() % 3 != 1) {
        cerr << "evalBezier must be called with 3n+1 control points." << endl;
		_CrtDbgBreak();
		exit(0);
	}

    Curve R;

    for(unsigned i = 0; i < P.size()-1; i += 3) {
        Curve sub = coreBezier(P[i], P[i+1], P[i+2], P[i+3], Vec3f(), steps);
        R.insert(R.end(), sub.begin(), sub.end());
    }

	// EXTRA CREDIT NOTE:
    // Also compute the other Vec3fs for each CurvePoint: T, N, B.
    // A matrix [N, B, T] should be unit and orthogonal.
    // Also note that you may assume that all Bezier curves that you
    // receive have G1 continuity. The T, N and B vectors will not
	// have to be defined at points where this does not hold.

    cerr << "\t>>> evalBezier has been called with the following input:" << endl;

	// to append to a std::vector, use std::insert.
	// for example, to add 'b' to the end of 'a', we'd call 'a.insert(a.end(), b.begin(), b.end());'

    cerr << "\t>>> Control points (type vector<Vec3f>): "<< endl;
    for (unsigned i = 0; i < P.size(); ++i) {
        cerr << "\t>>> "; printTranspose(P[i]); cerr << endl;
    }

    cerr << "\t>>> Steps (type steps): " << steps << endl;

    return R;
}

// the P argument holds the control points and steps gives the amount of uniform tessellation.
// the rest of the arguments are for the adaptive tessellation extra.
Curve evalBspline(const vector<Vec3f>& P, unsigned steps, bool adaptive, float errorbound, float minstep) {
    // Check
    if (P.size() < 4) {
        cerr << "evalBspline must be called with 4 or more control points." << endl;
        exit(0);
    }

    Curve R;
    // YOUR CODE HERE (R2):
    // We suggest you implement this function via a change of basis from
	// B-spline to Bezier.  That way, you can just call your evalBezier function.
    Mat4f B_1 = bSpline();
    Mat4f B_2 = bernsteinSpline();

    for (unsigned i = 0; i < P.size() -3; i++) {
        Mat4f G;
        G.setCol(0, Vec4f(P[i],   0));
        G.setCol(1, Vec4f(P[i+1], 0));
        G.setCol(2, Vec4f(P[i+2], 0));
        G.setCol(3, Vec4f(P[i+3], 0));
        
        Mat4f P_bezier = G * B_1 * B_2.inverted();
        Curve sub = coreBezier(
            P_bezier.getCol(0), 
            P_bezier.getCol(1), 
            P_bezier.getCol(2), 
            P_bezier.getCol(3),
            Vec3f(), steps
        );

        R.insert(R.end(), sub.begin(), sub.end());
    }

    cerr << "\t>>> evalBSpline has been called with the following input:" << endl;

    cerr << "\t>>> Control points (type vector< Vec3f >): "<< endl;
    for (unsigned i = 0; i < P.size(); ++i) {
        cerr << "\t>>> "; printTranspose(P[i]); cerr << endl;
    }

    cerr << "\t>>> Steps (type steps): " << steps << endl;

    // Return an empty curve right now.
    return R;
}

Curve evalCircle(float radius, unsigned steps) {
    // This is a sample function on how to properly initialize a Curve
    // (which is a vector<CurvePoint>).
    
    // Preallocate a curve with steps+1 CurvePoints
    Curve R(steps+1);

    // Fill it in counterclockwise
    for (unsigned i = 0; i <= steps; ++i) {
        // step from 0 to 2pi
        float t = 2.0f * (float)M_PI * float(i) / steps;

        // Initialize position
        // We're pivoting counterclockwise around the y-axis
        R[i].V = radius * Vec3f(FW::cos(t), FW::sin(t), 0);
        
        // Tangent vector is first derivative
        R[i].T = Vec3f(-FW::sin(t), FW::cos(t), 0);
        
        // Normal vector is second derivative
        R[i].N = Vec3f(-FW::cos(t), -FW::sin(t), 0);

        // Finally, binormal is facing up.
        R[i].B = Vec3f(0, 0, 1);
    }

    return R;
}

void drawCurve(const Curve& curve, float framesize) {
    // Save current state of OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    // Setup for line drawing
    glDisable(GL_LIGHTING); 
    glColor4f(1, 1, 1, 1);
    glLineWidth(1);
    
	if (framesize >= 0) {
		// Draw curve
		glBegin(GL_LINE_STRIP);
		for (unsigned i = 0; i < curve.size(); ++i) {
			glVertex(curve[i].V);
		}
		glEnd();
	}

    glLineWidth(1);

    // Draw coordinate frames if framesize nonzero
    if (framesize != 0.0f) {
		framesize = FW::abs(framesize);
        Mat4f M;

        for (unsigned i = 0; i < curve.size(); ++i) {
            M.setCol( 0, Vec4f( curve[i].N, 0 ) );
            M.setCol( 1, Vec4f( curve[i].B, 0 ) );
            M.setCol( 2, Vec4f( curve[i].T, 0 ) );
            M.setCol( 3, Vec4f( curve[i].V, 1 ) );

            glPushMatrix();
            glMultMatrixf(M.getPtr());
            glScaled(framesize, framesize, framesize);
            glBegin(GL_LINES);
            glColor3f( 1, 0, 0 ); glVertex3d( 0, 0, 0 ); glVertex3d( 1, 0, 0 );
            glColor3f( 0, 1, 0 ); glVertex3d( 0, 0, 0 ); glVertex3d( 0, 1, 0 );
            glColor3f( 0, 0, 1 ); glVertex3d( 0, 0, 0 ); glVertex3d( 0, 0, 1 );
            glEnd();
            glPopMatrix();
        }
    }
    
    // Pop state
    glPopAttrib();
}

