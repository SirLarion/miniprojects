#pragma once

#include "gui/Window.hpp"
#include "gui/CommonControls.hpp"

#include <string>
#include <vector>
#include <map>


namespace FW {

struct Vertex
{
	Vec3f position;
	Vec3f normal;
};

struct glGeneratedIndices
{
	GLuint static_vao, dynamic_vao;
	GLuint shader_program;
	GLuint static_vertex_buffer, dynamic_vertex_buffer;
	GLuint model_to_world_uniform, world_to_clip_uniform, shading_toggle_uniform;
};

class App : public Window::Listener
{
private:
	enum CurrentModel
	{
		MODEL_EXAMPLE,
		MODEL_USER_GENERATED,
		MODEL_FROM_INDEXED_DATA,
		MODEL_FROM_FILE
	};

public:
						App();		// constructor
	virtual				~App() {}	// destructor

	virtual bool		handleEvent(const Window::Event& ev);

private:
						App(const App&);		// forbid copy
	App&				operator=(const App&);	// forbid assignment

	void				initRendering();
	void				render();
	std::vector<Vertex>	loadObjFileModel(std::string filename);

	void				streamGeometry(const std::vector<Vertex>& vertices);

	Window				window_;
	CommonControls		common_ctrl_;

	CurrentModel		current_model_;
	bool				model_changed_;
	bool				shading_toggle_;
	bool				shading_mode_changed_;
	bool				fullscreen_toggle_;
	bool				fullscreen_changed_;
	std::map<FW::String, bool> keys_pressed_;

	glGeneratedIndices	gl_;

	size_t				vertex_count_;

	Vec3f				camera_angles_;
	Vec3f				camera_position_;

	Mat4f				obj_translation_;
	Mat4f				obj_scale_;
	Mat4f				obj_rotation_;
	Vec3f				obj_angles_; // each coordinate a radian around its respective axis

	Timer				timer_;
	bool				animating_;
};

}
