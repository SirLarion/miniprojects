#include "particle_systems.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>

using namespace std;
using namespace FW;

namespace {

	inline Vec3f fGravity(float mass) {
		return Vec3f(0, -9.8f * mass, 0);
	}

	// force acting on particle at pos1 due to spring attached to pos2 at the other end
	inline Vec3f fSpring(const Vec3f& pos1, const Vec3f& pos2, float k, float rest_length) {
		Vec3f d = pos2 - pos1;
		float dLen = d.length();
		Vec3f F = -k * (dLen - rest_length) * d / dLen;

		return F;
	}

	inline Vec3f fDrag(const Vec3f& v, float k) {
		return -k * v;
	}

} // namespace

void SimpleSystem::reset() {
	current_state_ = State(1, Vec3f(0, radius_, 0));
}

State SimpleSystem::evalF(const State& state) const {
	State f(1, Vec3f(-state[0].y, state[0].x, 0));
	return f;
}

#ifdef EIGEN_SPARSECORE_MODULE_H
// using the implicit Euler method, the simple system should converge towards origin -- as opposed to the explicit Euler, which diverges outwards from the origin.
void SimpleSystem::evalJ(const State&, SparseMatrix& result, bool initial) const {
	if (initial) {
		result.coeffRef(1, 0) = 1.0f;
		result.coeffRef(0, 1) = -1.0f;
	}
}
#endif

Points SimpleSystem::getPoints() {
	return Points(1, current_state_[0]);
}

Lines SimpleSystem::getLines() {
	static const auto n_lines = 50u;
	auto l = Lines(n_lines * 2);
	const auto angle_incr = 2 * FW_PI / n_lines;
	for (auto i = 0u; i < n_lines; ++i) {
		l[2 * i] = l[2 * i + 1] =
			Vec3f(radius_ * FW::sin(angle_incr * i), radius_ * FW::cos(angle_incr * i), 0);
	}
	rotate(l.begin(), l.begin() + 1, l.end());
	return l;
}

void SpringSystem::reset() {
	const auto start_pos = Vec3f(0.1f, -0.5f, 0.0f);
	const auto spring_k = 30.0f;
	const auto rest_length = 0.5f;
	current_state_ = State(4);

	// Set the initial state for a particle system with one particle fixed
	// at origin and another particle hanging off the first one with a spring.
	// Place the second particle initially at start_pos.
	current_state_[0] = Vec3f();
	current_state_[1] = Vec3f();
	current_state_[2] = start_pos;
	current_state_[3] = Vec3f();

	spring_ = Spring(0, 2, spring_k, rest_length);
}

State SpringSystem::evalF(const State& state) const {
	const auto drag_k = 0.5f;
	const auto mass = 1.0f;
	State f(4);
	// Return a derivative for the system as if it was in state "state".
	// You can use the fGravity, fDrag and fSpring helper functions for the forces.
	f[0] = state[1];
	f[1] = Vec3f();
	f[2] = state[3];
	f[3] = (fGravity(mass) + fDrag(f[2], drag_k) + fSpring(state[0], state[2], spring_.k, spring_.rlen))/mass;

	return f;
}

#ifdef EIGEN_SPARSECORE_MODULE_H

// This is a very useful read for the Jacobians of the spring forces. It deals with spring damping as well, we don't do that -- our drag is simply a linear damping of velocity (that results in some constants in the Jacobian).
// http://blog.mmacklin.com/2012/05/04/implicitsprings/

void SpringSystem::evalJ(const State& state, SparseMatrix& result, bool initial) const {
	const auto drag_k = 0.5f;
	const auto mass = 1.0f;
	// EXTRA: Evaluate the Jacobian into the 'result' matrix here. Only the free end of the spring should have any nonzero values related to it.
}
#endif

Points SpringSystem::getPoints() {
	auto p = Points(2);
	p[0] = current_state_[0]; p[1] = current_state_[2];
	return p;
}

Lines SpringSystem::getLines() {
	auto l = Lines(2);
	l[0] = current_state_[0]; l[1] = current_state_[2];
	return l;
}

// Set the initial state for a pendulum system with n_ particles
// connected with springs into a chain from start_point to end_point with uniform intervals.
// The rest length of each spring is its length in this initial configuration.
void PendulumSystem::reset() {
	const auto spring_k = 1000.0f;
	const auto start_point = Vec3f(0);
	const auto end_point = Vec3f(0.05, -1.5, 0);
	const auto direction = end_point.normalized();
	const float rest_length = end_point.length() / (n_-1);
	current_state_ = State(2 * n_);

	current_state_[0] = start_point;
	current_state_[1] = Vec3f(0);

	springs_.clear();
	cout << "cleared" << endl;

	for (auto i = 1u; i < n_; i++) {
		current_state_[2 * i] = i * rest_length * direction;
		current_state_[2 * i + 1] = Vec3f(0);
		springs_.push_back(Spring(i - 1, i, spring_k, rest_length));
	}
}

State PendulumSystem::evalF(const State& state) const {
	const auto drag_k = 0.5f;
	const auto mass = 0.5f;
	auto f = State(2 * n_);

	for (auto i = 1u; i < n_; i++) {
		f[2 * i] = state[2 * i + 1];
		f[2 * i + 1] = (fGravity(mass) + fDrag(f[2 * i], drag_k))/mass;
	}
	for (const auto& s : springs_) {
		const auto i1 = 2 * s.i1;
		const auto i2 = 2 * s.i2;
		Vec3f force = fSpring(state[i1], state[i2], s.k, s.rlen);
		f[i1+1] -= force / mass;
		f[i2+1] += force / mass;
	}

	// first particle is static
	f[0] = state[1];
	f[1] = Vec3f();

	return f;
}

#ifdef EIGEN_SPARSECORE_MODULE_H

void PendulumSystem::evalJ(const State& state, SparseMatrix& result, bool initial) const {

	const auto drag_k = 0.5f;
	const auto mass = 0.5f;

	// EXTRA: Evaluate the Jacobian here. Each spring has an effect on four blocks of the matrix -- both of the positions of the endpoints will have an effect on both of the velocities of the endpoints.
}
#endif


Points PendulumSystem::getPoints() {
	auto p = Points(n_);
	for (auto i = 0u; i < n_; ++i) {
		p[i] = current_state_[i * 2];
	}
	return p;
}

Lines PendulumSystem::getLines() {
	auto l = Lines();
	for (const auto& s : springs_) {
		l.push_back(current_state_[2 * s.i1]);
		l.push_back(current_state_[2 * s.i2]);
	}
	return l;
}

unsigned ClothSystem::index(unsigned x, unsigned y) const { return 2 * (x + y * x_); };

void ClothSystem::reset() {
	const auto spring_k = 300.0f;
	const auto width = 1.5f, height = 1.5f; // width and height of the whole grid
	const auto x_rest = width / (x_-1);
	const auto y_rest = height / (y_-1);
	const auto d_rest = FW::sqrt(x_rest * x_rest + y_rest * y_rest);
	const auto fx_rest = 2 * x_rest;
	const auto fy_rest = 2 * y_rest;
	
	current_state_ = State(2 * x_*y_);

	for (auto j = 0u; j < y_; j++) {
		for (auto i = 0u; i < x_; i++) {
			current_state_[index(i, j)] = Vec3f(i * x_rest, 0, j * y_rest);
			current_state_[index(i, j) + 1] = Vec3f(0);

			// structural springs
			if (i >= 1) {
				springs_.push_back(Spring(index(i-1, j), index(i, j), spring_k, x_rest));
			}
			if (j >= 1) {
				springs_.push_back(Spring(index(i, j-1), index(i, j), spring_k, y_rest));
			}
			// shear springs
			if (i >= 1 && j >= 1) {
				springs_.push_back(Spring(index(i-1, j-1), index(i, j), spring_k, d_rest));
			}
			if (i + 1 < x_ && j >= 1) {
				springs_.push_back(Spring(index(i+1, j-1), index(i, j), spring_k, d_rest));
			}
			// flex springs
			if (i >= 2) {
				springs_.push_back(Spring(index(i-2, j), index(i, j), spring_k, fx_rest));
			}
			if (j >= 2) {
				springs_.push_back(Spring(index(i, j-2), index(i, j), spring_k, fy_rest));
			}

		}
	}
}

State ClothSystem::evalF(const State& state) const {
	const auto drag_k = 0.08f;
	static const auto mass = 0.025f;
	auto f = State(2 * x_*y_);

	for (auto j = 0u; j < y_; j++) {
		for (auto i = 0u; i < x_; i++) {
			f[index(i, j)] = state[index(i, j) + 1];
			f[index(i, j) + 1] = (fGravity(mass) + fDrag(f[index(i, j)], drag_k)) / mass;
		}
	}
	for (const auto& s : springs_) {
		Vec3f force = fSpring(state[s.i1], state[s.i2], s.k, s.rlen);
		f[s.i1+1] -= force / mass;
		f[s.i2+1] += force / mass;
	}

	// both ends of first row are static
	f[0] = state[1];
	f[1] = Vec3f();
	f[2 * (x_-1)] = state[2 * (x_-1) + 1];
	f[2 * (x_-1) + 1] = Vec3f();

	return f;
}

#ifdef EIGEN_SPARSECORE_MODULE_H

void ClothSystem::evalJ(const State& state, SparseMatrix& result, bool initial) const {
	const auto drag_k = 0.08f;
	static const auto mass = 0.025f;

	// EXTRA: Evaluate the Jacobian here. The code is more or less the same as for the pendulum.
}

#endif

Points ClothSystem::getPoints() {
	auto p = Points(x_*y_);
	for (auto j = 0u; j < y_; j++) {
		for (auto i = 0u; i < x_; ++i) {
			p[i + j*x_] = current_state_[index(i, j)];
		}
	}
	return p;
}

Lines ClothSystem::getLines() {
	auto l = Lines();
	for (const auto& s : springs_) {
		l.push_back(current_state_[s.i1]);
		l.push_back(current_state_[s.i2]);
	}
	return l;
}
State FluidSystem::evalF(const State&) const {
	return State();
}

