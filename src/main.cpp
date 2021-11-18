#include "vcl/vcl.hpp"
#include <iostream>

#include "terrain.hpp"
#include "tree.hpp"
#include "mushroom.hpp"
#include "dimensions.hpp"
#include "utils.hpp"
#include "interpolation.hpp"
#include "obj_loader.h"

using namespace vcl;

struct gui_parameters {
	bool display_frame = true;
	bool add_sphere = true;
};

struct user_interaction_parameters {
	vec2 mouse_prev;
	timer_fps fps_record;
	mesh_drawable global_frame;
	gui_parameters gui;
	bool cursor_on_gui;
};
user_interaction_parameters user;

struct scene_environment
{
	camera_around_center camera;
	mat4 projection;
	vec3 light;
};
scene_environment scene;


void mouse_move_callback(GLFWwindow* window, double xpos, double ypos);
void window_size_callback(GLFWwindow* window, int width, int height);

void constexpr initialize_dimensions();
void initialize_data();
void display_scene();
void display_interface();

// Elements
mesh terrain;
mesh_drawable terrain_visual;
mesh_drawable tree;
mesh_drawable mushroom;
mesh_drawable billboard_grass;
mesh_drawable billboard_bird;
mesh_drawable billboard_flowers;

timer_interval timer;
hierarchy_mesh_drawable bird;

mesh_drawable obj;

// Random positions
std::vector<vcl::vec2> random_tree_positions;
std::vector<vcl::vec2> random_mushroom_positions;
std::vector<vcl::vec2> random_grass_positions;
std::vector<vcl::vec2> random_flowers_positions;

// animation positions
buffer<vec3> key_positions;
buffer<float> key_times;


const int NUM_TREES = 40;
const int NUM_MUSHROOMS = 100;
const int NUM_GRASS = 70;
const int NUM_FLOWERS = 50;

int main(int, char* argv[])
{
	std::cout << "Run " << argv[0] << std::endl;

	int const width = 1280, height = 1024;
	GLFWwindow* window = create_window(width, height);
	window_size_callback(window, width, height);
	std::cout << opengl_info_display() << std::endl;;

	imgui_init(window);
	glfwSetCursorPosCallback(window, mouse_move_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);
	
	std::cout << "Initialize dimensions ..." << std::endl;
	initialize_dimensions();

	std::cout<<"Initialize data ..."<<std::endl;
	initialize_data();


	std::cout<<"Start animation loop ..."<<std::endl;
	user.fps_record.start();
	glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(window))
	{
		scene.light = scene.camera.position();
		user.fps_record.update();
		
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
		imgui_create_frame();
		if(user.fps_record.event) {
			std::string const title = "VCL Display - "+str(user.fps_record.fps)+" fps";
			glfwSetWindowTitle(window, title.c_str());
		}

		ImGui::Begin("GUI",NULL,ImGuiWindowFlags_AlwaysAutoResize);
		user.cursor_on_gui = ImGui::IsAnyWindowFocused();

		if(user.gui.display_frame) draw(user.global_frame, scene);

		display_interface();
		display_scene();

		ImGui::End();
		imgui_render_frame(window);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	imgui_cleanup();
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

void constexpr initialize_dimensions() {
	// terrain dimensions
	dimensions::terrain.length = 10.0f;

	// tree dimensions
	dimensions::tree.height = 1.5f;
	dimensions::tree.radius = 0.4f;

	// mushroom dimensions
	dimensions::mushroom.height = 0.13f;
	dimensions::mushroom.radius = 0.1f;

}

void initialize_data()
{
	GLuint const shader_mesh = opengl_create_shader_program(opengl_shader_preset("mesh_vertex"), opengl_shader_preset("mesh_fragment"));
	GLuint const shader_uniform_color = opengl_create_shader_program(opengl_shader_preset("single_color_vertex"), opengl_shader_preset("single_color_fragment"));
	GLuint const texture_white = opengl_texture_to_gpu(image_raw{1,1,image_color_type::rgba,{255,255,255,255}});
	mesh_drawable::default_shader = shader_mesh;
	mesh_drawable::default_texture = texture_white;
	curve_drawable::default_shader = shader_uniform_color;
	segments_drawable::default_shader = shader_uniform_color;
	
	user.global_frame = mesh_drawable(mesh_primitive_frame());
	user.gui.display_frame = false;
	scene.camera.distance_to_center = 2.5f;
	scene.camera.look_at({7,-10,10}, {0,0,0}, {0,0,1});

    // Create visual terrain surface
	terrain = create_terrain();
	deform_terrain_perlin(terrain, perlin_noise_parameters()); // perlin noise
    terrain_visual = mesh_drawable(terrain);
	terrain_visual.shading.phong.specular = 0.0f; // non-specular terrain material
	image_raw im = image_load_png("assets/textures/texture_grass.png"); // texture
	GLuint texture_image_id = opengl_texture_to_gpu(im, GL_REPEAT, GL_REPEAT);
	terrain_visual.texture = texture_image_id;

	// tree
	tree = mesh_drawable(create_tree());
	random_tree_positions = get_random_points(NUM_TREES, dimensions::tree.radius / dimensions::terrain.length);

	// mushrooms
	mushroom = mesh_drawable(create_mushroom());
	random_mushroom_positions = get_random_points(NUM_MUSHROOMS);

	// grass
	billboard_grass = mesh_drawable(mesh_primitive_quadrangle({ -1,0,0 }, { 1,0,0 }, { 1,0,2 }, { -1,0,2 }));
	billboard_grass.transform.scale = 0.4f;
	billboard_grass.texture = opengl_texture_to_gpu(image_load_png("assets/textures/grass.png"));
	random_grass_positions = get_random_points(NUM_GRASS);

	// bird
	billboard_bird = mesh_drawable(mesh_primitive_quadrangle());
	billboard_bird.transform.translate = { 0.5f, -1, 1.5f };
	billboard_bird.texture = opengl_texture_to_gpu(image_load_png("assets/textures/bird.png"));

	// flowers
	billboard_flowers = mesh_drawable(mesh_primitive_quadrangle({ -1,0,0 }, { 1,0,0 }, { 1,0,2 }, { -1,0,2 }));
	billboard_flowers.transform.scale = 0.4f;
	billboard_flowers.texture = opengl_texture_to_gpu(image_load_png("assets/textures/redflowers.png"));
	random_flowers_positions = get_random_points(NUM_FLOWERS);

	// Definition of the elements of the hierarchy
	// ------------------------------------------- //

	float const radius_body = 0.25f;
	float const radius_head = radius_body * 0.8f;
	float const length_arm = radius_body;
	float const length_beak = radius_body * 0.4f;

	// The geometry of the body is an ellipsoid
	vec3 const dim_body = { 1.7f * radius_body, radius_body, radius_body };
	mesh_drawable body = mesh_drawable(mesh_primitive_ellipsoid(dim_body, { 0,0,0 }, 40, 40));

	// head
	mesh_drawable head = mesh_drawable(mesh_primitive_sphere(radius_head, { 0,0,0 }, 20, 20));

	// Geometry of the eyes: black spheres
	mesh_drawable eye = mesh_drawable(mesh_primitive_sphere(radius_head / 5.0f, { 0,0,0 }, 20, 20));
	eye.shading.color = { 0,0,0 };

	// Shoulder part and arm
	vec3 p00 = { -length_arm / 2.0f, - length_arm, 0.0f };
	vec3 p10 = { length_arm / 2.0f, -length_arm, 0.0f };
	vec3 p11 = { length_arm / 2.0f, 0.0f, 0.0f };
	vec3 p01 = { -length_arm / 2.0f, 0.0f, 0.0f };
	mesh_drawable wing_right = mesh_drawable(mesh_primitive_quadrangle(p00, p10, p11, p01));
	p10 -= {length_arm / 4.0f, 0.0f, 0.0f};
	p00 += {length_arm / 3.0f, 0.0f, 0.0f};

	mesh_drawable extremity_right = mesh_drawable(mesh_primitive_quadrangle(p00, p10, p11, p01));

	// reset
	p10 += {length_arm / 4.0f, 0.0f, 0.0f};
	p00 -= {length_arm / 3.0f, 0.0f, 0.0f};

	// translate
	p00 += {0.0f, length_arm, 0.0f};
	p10 += {0.0f, length_arm, 0.0f};
	p11 += {0.0f, length_arm, 0.0f};
	p01 += {0.0f, length_arm, 0.0f};
	mesh_drawable wing_left = mesh_drawable(mesh_primitive_quadrangle(p00, p10, p11, p01));
	p11 -= {length_arm / 4.0f, 0.0f, 0.0f};
	p01 += {length_arm / 3.0f, 0.0f, 0.0f};
	mesh_drawable extremity_left = mesh_drawable(mesh_primitive_quadrangle(p00, p10, p11, p01));

	
	// A beak
	mesh_drawable beak = mesh_drawable(mesh_primitive_cone(length_beak * 0.8f, length_beak, { 0, 0, 0 }, { 1, 0, 0 }));
	beak.shading.color = { 1.0f, 0.5f, 0.0f }; // orange

	// Build the hierarchy:
	// ------------------------------------------- //
	// Syntax to add element
	//   hierarchy.add(visual_element, element_name, parent_name, (opt)[translation, rotation])

	// The root of the hierarchy is the body
	bird.add(body, "body");

	// head
	float const angle = pi / 6.0f;
	bird.add(head, "head", "body", { dim_body.x * std::cosf(angle), 0.0f, dim_body.z * std::sinf(angle) });

	// Eyes positions are set with respect to some ratio of the body
	bird.add(eye, "eye_left", "head", radius_head* vec3(1.0f / 1.5f, 1 / 3.0f, 1 / 2.0f));
	bird.add(eye, "eye_right", "head", radius_head* vec3(1.0f / 1.5f, -1 / 3.0f, 1 / 2.0f));

	// beak
	bird.add(beak, "beak", "head", { radius_head, 0, 0 });

	// Set the left part of the body arm
	bird.add(wing_left, "wing_left", "body", { 0.0f,dim_body.y,0 }); // extremity of the spherical body
	bird.add(extremity_left, "extremity_left", "wing_left", { 0,length_arm,0 });          // place the elbow the extremity of the "shoulder cylinder"

	// Set the right part of the body arm: similar to the left part with a symmetry in x direction
	bird.add(wing_right, "wing_right", "body", { 0,-dim_body.y,0 });
	bird.add(extremity_right, "extremity_right", "wing_right", { 0, -length_arm,0 });


	// Definition of the initial data
	//--------------------------------------//
	// Key positions
	key_positions = { {-1,1,0}, {0,1,0}, {1,1,0}, {1,2,0}, {2,2,0}, {2,0,0}, {1.5,-1,0}, {1,-1,0}, {0,-0.5,0}, {-1,-0.5,0}, {-1,1,0} };
	// Key times
	key_times = { 0.0f, 1.0f, 2.0f, 2.5f, 3.0f, 3.5f, 3.75f, 4.5f, 5.0f, 6.0f, 7.0f };
	key_times *= 1.5f;

	// Set timer bounds
	//  You should adapt these extremal values to the type of interpolation
	size_t const N = key_times.size();
	timer.t_min = key_times[0];    // Start the timer at the first time of the keyframe
	timer.t_max = key_times[N - 1];  // Ends the timer at the last time of the keyframe
	timer.t = timer.t_min;

	// obj file
	mesh obj_mesh;
	load_mesh(obj_mesh, "assets/obj/Test_animation.obj");
	obj = mesh_drawable(obj_mesh);
}


void display_scene()
{
	draw(terrain_visual, scene);

	for (auto& pos : random_tree_positions) {
		tree.transform.translate = get_terrain_height(terrain, pos[0], pos[1]);
		draw(tree, scene);
	}

	for (auto& pos : random_mushroom_positions) {
		mushroom.transform.translate = get_terrain_height(terrain, pos[0], pos[1]);
		draw(mushroom, scene);
	}

	// Enable use of alpha component as color blending for transparent elements
	//  new color = previous color + (1-alpha) current color
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Disable depth buffer writing
	//  - Transparent elements cannot use depth buffer
	//  - They are supposed to be display from furest to nearest elements
	glDepthMask(false);

	billboard_bird.transform.rotate = scene.camera.orientation();
	draw(billboard_bird, scene);

	for (auto& pos : random_grass_positions) {
		billboard_grass.transform.translate = get_terrain_height(terrain, pos[0], pos[1]);
		billboard_grass.transform.rotate = rotation();
		draw(billboard_grass, scene);
		billboard_grass.transform.rotate = rotation(vec3{ 0,0,1 }, 3.14f / 2);
		draw(billboard_grass, scene);
	}

	for (auto& pos : random_flowers_positions) {
		billboard_flowers.transform.translate = get_terrain_height(terrain, pos[0], pos[1]);
		billboard_flowers.transform.rotate = rotation();
		draw(billboard_flowers, scene);
		billboard_flowers.transform.rotate = rotation(vec3{ 0,0,1 }, 3.14f / 2);
		draw(billboard_flowers, scene);
	}

	glDepthMask(true);


	// Update the current time
	timer.update();
	float const t = timer.t;


	/** *************************************************************  **/
	/** Compute the (animated) transformations applied to the elements **/
	/** *************************************************************  **/

	// body follows interpolated position
	vec3 p, p_deriv;
	interpolation(p, p_deriv, t, key_positions, key_times);

	// Now calculating rotation
	p_deriv /= norm(p_deriv);
	vec3 p_actual = vec3({ 1,0,0 });
	vec3 cp = cross_product(p_actual, p_deriv);
	float theta = std::acos(dot(p_deriv, p_actual));
	if (norm(cp) > 1e-7) {
		cp /= norm(cp);
		bird["body"].transform.rotate = rotation(cp, theta);
	}
	else {
		if (p_deriv[0] < 0.0f) bird["body"].transform.rotate = rotation({ 0,0,1 }, pi);
		else  bird["body"].transform.rotate = rotation();
	}
	//std::cout <<  p_deriv << " , " << cp << " : " << theta << std::endl;

	p -= {1, 1, 0};
	p *= 2.0f;
	p[2] += 3.0f;

	bird["body"].transform.translate = p;

	// head
	bird["head"].transform.rotate = rotation({ 0,-1,0 }, 0.4f +  0.3f * std::sin(1.1 * 2 * pi * (t -0.2)));
	
	// Rotation of the shoulder-left around the y axis
	bird["wing_left"].transform.rotate = rotation({ 1,0,0 }, 0.7f * std::sin(2 * 2 * pi * (t)));
	// Rotation of the arm-left around the y axis (delayed with respect to the shoulder)
	bird["extremity_left"].transform.rotate = rotation({ 1,0,0 }, std::sin(2 * 2 * pi * (t)));

	// Rotation of the shoulder-right around the y axis
	bird["wing_right"].transform.rotate = rotation({ -1,0,0 }, 0.7f * std::sin(2 * 2 * pi * (t)));
	// Rotation of the arm-right around the y axis (delayed with respect to the shoulder)
	bird["extremity_right"].transform.rotate = rotation({ -1,0,0 }, std::sin(2 * 2 * pi * (t)));

	// update the global coordinates
	bird.update_local_to_global_coordinates();

	// display the hierarchy
	draw(bird, scene);

	// display obj
	obj.transform.translate = { 0.0f, 0.0f, 5.0f };
	draw(obj, scene);

}


void display_interface()
{
	ImGui::Checkbox("Frame", &user.gui.display_frame);
}


void window_size_callback(GLFWwindow* , int width, int height)
{
	glViewport(0, 0, width, height);
	float const aspect = width / static_cast<float>(height);
	scene.projection = projection_perspective(50.0f*pi/180.0f, aspect, 0.1f, 100.0f);
}


void mouse_move_callback(GLFWwindow* window, double xpos, double ypos)
{
	vec2 const  p1 = glfw_get_mouse_cursor(window, xpos, ypos);
	vec2 const& p0 = user.mouse_prev;
	glfw_state state = glfw_current_state(window);

	auto& camera = scene.camera;
	if(!user.cursor_on_gui){
		if(state.mouse_click_left && !state.key_ctrl)
			scene.camera.manipulator_rotate_trackball(p0, p1);
		if(state.mouse_click_left && state.key_ctrl)
			camera.manipulator_translate_in_plane(p1-p0);
		if(state.mouse_click_right)
			camera.manipulator_scale_distance_to_center( (p1-p0).y );
	}

	user.mouse_prev = p1;
}

void opengl_uniform(GLuint shader, scene_environment const& current_scene)
{
	opengl_uniform(shader, "projection", current_scene.projection);
	opengl_uniform(shader, "view", current_scene.camera.matrix_view());
	opengl_uniform(shader, "light", current_scene.light, false);
}



