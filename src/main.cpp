#include <vcl/vcl.hpp>
#include <iostream>

#include "terrain.hpp"
#include "bird.h"
#include "tree.hpp"
#include "mushroom.hpp"
#include "dimensions.hpp"
#include "interpolation.hpp"
#include "Flamethrower.hpp"
#include "GravityBalls.hpp"
#include "Skybox.hpp"
#include "FractalSphere.hpp"

#include "utils.hpp"
#include "GuiWindow.hpp"
#include "scene.hpp"
#include "obj_loader.h"
#include "user_interaction.h"

#include "Smoke.hpp"
#include "VortexPosition.hpp"
#include "Vortex.hpp"
#include "Collision.hpp"

using namespace vcl;

// In order to track user interactions with window
user_interaction_parameters user;

// Views used in the scene, each one with a camera
global_view global_scene;
fps_view head_scene;

// Some constants
int const width = 1280, height = 1024;
float scene_environment::aspect = (float)width / (float)height;
float ball_lauching_speed = 20.0f;
float launch_angle = pi / 10.0f;
bool kill_all_balls = false;

float Smoke::vorticity = 3.5f;
float Smoke::randomness = 0.27f;
float Smoke::initial_offset = 0.32;

float Vortex::vorticity = 28.660;
float Vortex::randomness = 1.835;
float Vortex::initial_offset = 0.376;
float Vortex::max_radius = 0.02;


bool enable_collision = true;
float CollisionStructure::alpha = 0.1f;
bool CollisionStructure::show_hitbox = true;


// GUI
GuiWindow gui_window("GUI");

// Init functions
GLFWwindow*		initialize_window(int width, int height);
void			startup();
void constexpr	initialize_dimensions();
void			initialize_data();

// Callbacks when interacting with window
void mouse_move_callback(GLFWwindow* window, double xpos, double ypos);
void window_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void sliderCallback(int value);
void kill_balls(bool checkbox);

// Collisions
void animate_scene(GLFWwindow* window, float dt);
bool check_camera_collision();
bool treat_ball_collision(CollisionStructure*, Particle&);

// Display and draw
void set_fov(int fov);
void display(const scene_environment& scene);

// User dynamic
void move_user(GLFWwindow* window, float dt);

// Elements on scene
Skybox skybox;
Terrain ter;
Bird bird;
Tree tree;

#define FRACTAL_ITERATION 3
#define FRACTAL_SPHERE_SIZE 3.0f

FractalSphere fractal_sphere;

DefaultMeshDrawable mushroom;
DefaultMeshDrawable billboard_grass;
DefaultMeshDrawable billboard_bird;
DefaultMeshDrawable billboard_flowers;
Flamethrower flame_thrower;

#define BALLS_REBOUNCE_LOSS 0.7f;
GravityBalls balls;

#define NUM_RAND_VORTEX 3
#define RAND_VORTEX_SPEED_RELATIVE 0.5f // relative to terrain size
#define VORTEX_NUM_PARTICLES 300 // 2000

Smoke smoke;
Vortex descr_vortex;
VortexPosition descr_vortex_point;

Vortex rand_vortex[NUM_RAND_VORTEX];
vcl::vec3 rand_vortex_points[NUM_RAND_VORTEX];

CollisionStructure camera_col;

// Random positions
std::vector<vcl::vec2> random_tree_positions;
std::vector<vcl::vec2> random_mushroom_positions;
std::vector<vcl::vec2> random_grass_positions;
std::vector<vcl::vec2> random_flowers_positions;


const int NUM_TREES = 10;
const int NUM_MUSHROOMS = 100;
const int NUM_GRASS = 70;
const int NUM_FLOWERS = 50;
/**********************************LOOP*****************************************/

int main(int, char* argv[])
{

	std::cout << "Test collision" << std::endl;

	std::cout << test_collision_1() << std::endl;
	std::cout << test_collision_2() << std::endl;
	std::cout << test_collision_3() << std::endl;
	std::cout << test_collision_4() << std::endl;
	std::cout << test_collision_5() << std::endl;
	std::cout << test_collision_6() << std::endl;


	std::cout << "Done" << std::endl;

	std::cout << "Run " << argv[0] << std::endl;

	GLFWwindow* window = initialize_window(width, height);
	std::cout << opengl_info_display() << std::endl;;

	startup();
	std::cout << "Start animation loop ..." << std::endl;

	glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(window))
	{
		global_scene.light = global_scene.camera.position();
		float dt = user.fps_record.update();
		dt *= user.time_scale;

		glClearColor(234.0f / 255.0f, 81.0f / 255.0f, 162.0f / 255.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
		
		if (user.fps_record.event) {
			std::string const title = "VCL Display - " + str(user.fps_record.fps) + " fps";
			glfwSetWindowTitle(window, title.c_str());
		}

		user.cursor_on_gui = gui_window.isFocused();

		// Animate
		animate_scene(window, dt);

		// First, display scene
		if (user.gui.first_person)
			display(head_scene);
		else
			display(global_scene);
		// Then GUI window
		gui_window.draw();
		gui_window.display(window);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	imgui_cleanup();
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

/**********************************INIT*****************************************/

GLFWwindow* initialize_window(int width, int height) {
	GLFWwindow* window = create_window(width, height);
	window_size_callback(window, width, height);
	std::cout << opengl_info_display() << std::endl;;

	imgui_init(window);
	glfwSetCursorPosCallback(window, mouse_move_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	return window;
}

void startup() {
	std::cout << "Initialize dimensions ..." << std::endl;
	initialize_dimensions();

	std::cout << "Initialize data ..." << std::endl;
	initialize_data();

	// setting up user parameters
	user.gui.display_frame = false;
	user.gui.first_person = true;
	user.gui.fov = 60;
	user.speed = 10.0f;
	user.time_scale = 1.0f;
	user.capture_mouse = false;
	user.fps_record.start();

	// Resetting
	set_fov(user.gui.fov);

	gui_window.addCheckbox("Frame", user.gui.display_frame);
	gui_window.addCheckbox("First Person", user.gui.first_person);
	gui_window.addSliderInt("FOV", user.gui.fov, 60, 120, sliderCallback);
	gui_window.addSliderFloat("Ball launching speed", ball_lauching_speed, 10.0f, 100.0f);
	gui_window.addCheckbox("Kill all balls", kill_all_balls, kill_balls);
	gui_window.addSliderFloat("Time scale", user.time_scale, 0.01f, 3.0f);
	gui_window.addSliderFloat("Smoke vorticity", Smoke::vorticity, 1.0f, 20.0f);
	gui_window.addSliderFloat("Smoke randomness", Smoke::randomness, 0.1f, 2.5f);
	gui_window.addSliderFloat("Smoke initial equation offset", Smoke::initial_offset, 0.0f, 0.75f);
	gui_window.addCheckbox("Enable collision", enable_collision);
	gui_window.addCheckbox("Show hitboxes", CollisionStructure::show_hitbox);
	gui_window.addSliderFloat("Hitboxes alpha", CollisionStructure::alpha, 0.0f, 1.0f);
	gui_window.addSliderFloat("Vortex vorticity", Vortex::vorticity, 5.0f, 50.0f);
	gui_window.addSliderFloat("Vortex randomness", Vortex::randomness, 0.1f, 10.0f);
	gui_window.addSliderFloat("Vortex initial equation offset", Vortex::initial_offset, 0.0f, 0.5f);
	gui_window.addSliderFloat("Vortex effect max radius", Vortex::max_radius, 0.0f, 0.5f);

}

void constexpr initialize_dimensions() {
	// terrain dimensions
	dimensions::terrain.length = 15.0f;

	// tree dimensions
	dimensions::tree.height = 2.5f;
	dimensions::tree.radius = 0.6f;

	// mushroom dimensions
	dimensions::mushroom.height = 0.13f;
	dimensions::mushroom.radius = 0.1f;

}

void initialize_data()
{
	// Initialize default drawables
	DefaultMeshDrawable::setDefaultMeshShader();
	DefaultMeshDrawable::setDefaultMeshTexture();

	GLuint const shader_uniform_color = opengl_create_shader_program(opengl_shader_preset("single_color_vertex"), opengl_shader_preset("single_color_fragment"));
	curve_drawable::default_shader = shader_uniform_color;
	segments_drawable::default_shader = shader_uniform_color;

	user.global_frame = mesh_drawable(mesh_primitive_frame());

	global_scene.camera.distance_to_center = 2.5f;
	global_scene.camera.look_at({ 7,-10,10 }, { 0,0,0 }, { 0,0,1 });

	std::cout << "Loading terrain" << std::endl;

	// Create visual terrain surface
	ter.create(dimensions::terrain.length);
	perlin_noise_parameters param = ter.perlin_param();
	param.terrain_height *= 400.0f * dimensions::terrain.length;

	ter.deform_terrain_perlin();
	ter.shading().phong.specular = 0.0f; // non-specular terrain material
	ter.set_texture("assets/textures/texture_grass.png");

	std::cout << "Loading default objects" << std::endl;

	// bird
	bird.create();
	buffer<vcl::vec3> key_positions = { {-1,1,0}, {-1,1,0}, {0,1,0}, {1,1,0}, {1,2,0}, {2,2,0}, {2,0,0}, {1.5,-1,0}, {1,-1,0}, {0,-0.5,0}, {-1,-0.5,0}, {-1,1,0}, {-1,1,0} };
	buffer<float> key_times = { -1.0f, 0.0f, 1.0f, 2.0f, 2.5f, 3.0f, 3.5f, 3.75f, 4.5f, 5.0f, 6.0f, 7.0f, 8.0f};
	bird.set_animation(AnimationType::CardinalSplineCatmullRom, key_positions, key_times);

	// fps view
	//head_scene.camera.distance_to_center = 2.5f;
	head_scene.height = 1.0f;
	vec3 initial_pos = ter.get_true_terrain_height(0.0f, -1.0f);
	head_scene.set_position_from_ground(initial_pos);
	head_scene.camera.look_at(initial_pos, initial_pos + vec3({0, 1, 0}), { 0,0,1 }); 
	head_scene.light = vec3({ 10,10,200 });
	std::cout << head_scene.camera.position_camera << std::endl;


	// tree
	tree.create();
	random_tree_positions = get_random_points(NUM_TREES, dimensions::tree.radius / dimensions::terrain.length);

	// mushrooms
	mushroom.create(create_mushroom());
	random_mushroom_positions = get_random_points(NUM_MUSHROOMS);

	// grass
	billboard_grass.create(mesh_primitive_quadrangle({ -1,0,0 }, { 1,0,0 }, { 1,0,2 }, { -1,0,2 }));
	billboard_grass.get_drawable().transform.scale = 0.4f;
	billboard_grass.set_texture("assets/textures/grass.png");
	random_grass_positions = get_random_points(NUM_GRASS);

	// bird
	/*billboard_bird.create(mesh_primitive_quadrangle());
	billboard_bird.get_drawable().transform.translate = { 0.5f, -1, 5.0f };
	billboard_bird.set_texture("assets/textures/bird.png");*/

	// flowers
	billboard_flowers.create(mesh_primitive_quadrangle({ -1,0,0 }, { 1,0,0 }, { 1,0,2 }, { -1,0,2 }));
	billboard_flowers.get_drawable().transform.scale = 0.4f;
	billboard_flowers.set_texture("assets/textures/redflowers.png");
	random_flowers_positions = get_random_points(NUM_FLOWERS);

	// Flame thrower
	flame_thrower.create();

	// Smoke
	smoke.initialize_dynamics(5000, &flame_thrower.get_barrel_pos(), vec3({ 0,0,1 }), 0.7f);
	smoke.setup("shaders/smoke_vert.glsl", "shaders/smoke_frag.glsl", "assets/textures/better_smoke_2.png");

	std::cout << "Loading vortex" << std::endl;

	// Vortex
	// first descriptive. Catmull Rom on sampled circle with radial oscillation
	buffer<vec3> vortex_key_pos;
	buffer<float> vortex_key_times;

	int N = 40;
	float radius = dimensions::terrain.length;
	float radial_freq = 5.0f;
	for (int i = 0; i < N + 2; i++) {
		float new_r = (radius) * (1.0f + 0.2f * std::cos(2 * radial_freq * pi * i / N)) / 2.0f;
		vortex_key_pos.push_back(ter.get_bilinear_interpolated_terrain_height(new_r * std::cos(2 * pi * i / N), new_r * std::sin(2 * pi * i / N)));
		vortex_key_times.push_back(i);
	}

	descr_vortex_point.set_animation(AnimationType::CardinalSplineCatmullRom, vortex_key_pos, vortex_key_times);
	descr_vortex.initialize_dynamics(VORTEX_NUM_PARTICLES, &descr_vortex_point.get_position(), vec3({ 0,0,1 }), 2.0f);
	descr_vortex.setup("shaders/smoke_vert.glsl", "shaders/smoke_frag.glsl", "assets/textures/better_smoke.png");

	// Random vortex
	for (int i = 0; i < NUM_RAND_VORTEX; i++) {
		auto p = get_random_points(1, 0.0f)[0];
		rand_vortex_points[i] = ter.get_normalized_terrain_height(p.x, p.y);
		rand_vortex[i].initialize_dynamics(VORTEX_NUM_PARTICLES, &rand_vortex_points[i], vec3({ 0,0,1 }), 2.0f);
		rand_vortex[i].setup("shaders/smoke_vert.glsl", "shaders/smoke_frag.glsl", "assets/textures/better_smoke.png");
	}

	// camera collision
	camera_col.primitive = new CollisionBall({ 0,0,head_scene.height }, head_scene.height);
	//camera_col.create(mesh_primitive_sphere(head_scene.height)); // sphere will be centered in structure pos
	//camera_col.get_drawable().shading.color = vec3({ 1,0,0 });
	//camera_col.get_drawable().shading.alpha = 0.1f;


	// Ball
	balls.create(0.05f, 100);

	// skybox
	skybox.create("assets/cubemaps/kiara_dawn.png", 50.0f);

	// sphere
	std::cout << "Creating fractal sphere" << std::endl;
	fractal_sphere.create(FRACTAL_SPHERE_SIZE, vcl::vec3({ 0,0, FRACTAL_SPHERE_SIZE }) + ter.get_bilinear_interpolated_terrain_height(0.0, 0.0), FRACTAL_ITERATION);
}


/**********************************Callbacks*****************************************/


void sliderCallback(int value) {
	head_scene.set_projection(value);
	global_scene.set_projection(value);
}

void window_size_callback(GLFWwindow*, int width, int height)
{
	glViewport(0, 0, width, height);
	scene_environment::aspect = width / static_cast<float>(height);
	set_fov(user.gui.fov);
}


void mouse_move_callback(GLFWwindow* window, double xpos, double ypos)
{
	vec2 const  p1 = glfw_get_mouse_cursor(window, xpos, ypos);
	vec2 const& p0 = user.mouse_prev;
	glfw_state state = glfw_current_state(window);

	if (user.gui.first_person && user.capture_mouse) {
		auto& camera = head_scene.camera;
		rotation r_pitch = rotation({ 1,0,0 }, head_scene.pitch);
		rotation r_yaw = rotation({ 0,1,0 }, head_scene.yaw);
		camera.orientation_camera *= inverse(r_pitch) * inverse(r_yaw);
		head_scene.pitch += p1.y - p0.y;
		head_scene.yaw -= p1.x - p0.x;
		r_pitch = rotation({ 1,0,0 }, head_scene.pitch);
		r_yaw = rotation({ 0,1,0 }, head_scene.yaw);
		camera.orientation_camera *= r_yaw * r_pitch;

		// do not forget : camera coordinates : x is right, y is up, z is such that direct base... so pointing behind. pfiouuuu

		//camera.manipulator_rotate_roll_pitch_yaw(0.0f, p1.y - p0.y, -(p1.x - p0.x));
	}
	else {
		auto& camera = global_scene.camera;
		if (!user.cursor_on_gui) {
			if (state.mouse_click_left && !state.key_ctrl)
				camera.manipulator_rotate_trackball(p0, p1);
			if (state.mouse_click_left && state.key_ctrl)
				camera.manipulator_translate_in_plane(p1 - p0);
			if (state.mouse_click_right)
				camera.manipulator_scale_distance_to_center((p1 - p0).y);
		}
	}

	user.mouse_prev = p1;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	vec2 direction({ 0,0 });

	int GLFW_VALUE;

	switch (key) {
	case GLFW_KEY_ESCAPE:
		if (action == GLFW_PRESS) {
			GLFW_VALUE = user.capture_mouse ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_VALUE);
			user.capture_mouse = !user.capture_mouse;
		}
		break;
	}

}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && user.capture_mouse) {
 		balls.launch(flame_thrower.get_barrel_pos(), 
						ball_lauching_speed * (head_scene.camera.front() + head_scene.camera.up() * 0.1f), 200);
	}
}

void kill_balls(bool checkbox) {
	if (checkbox) {
		for (auto& part : balls.get_particles()) part.kill();
		kill_all_balls = false;
	}
}



/**********************************DRAW/DISPLAY*****************************************/


void set_fov(int fov) {
	global_scene.set_projection(fov);
	head_scene.set_projection(fov);
}


void display(const scene_environment& scene) {
	draw(ter.get_drawable(), scene);
	if (user.gui.display_frame) draw(user.global_frame, scene);


	if (user.gui.first_person) {
		flame_thrower.draw_fps(head_scene);
	}


	for (auto& pos : random_mushroom_positions) {
		mushroom.move(ter.get_normalized_terrain_height(pos[0], pos[1]));
		draw(mushroom, scene);
	}

	// draw bird
	draw(bird, scene);

	// draw ball
	//if (!ball.isDead()) draw(ball, scene);
	draw(balls, scene);

	// draw skybox
	skybox.move(scene.get_camera()->position());
	draw(skybox, scene);

	draw(fractal_sphere, scene);

	// draw vortex
	draw(descr_vortex, scene);
	for (int i = 0; i < NUM_RAND_VORTEX; i++) {
		draw(rand_vortex[i], scene);
	}

	// draw hit boxes just before transparent elements
	for (auto& pos : random_tree_positions) {
		tree.move(ter.get_normalized_terrain_height(pos[0], pos[1]));

		draw(tree, scene);
	}
	// Enable use of alpha component as color blending for transparent elements
	//  new color = previous color + (1-alpha) current color
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Disable depth buffer writing
	//  - Transparent elements cannot use depth buffer
	//  - They are supposed to be display from furest to nearest elements
	glDepthMask(false);

	//billboard_bird.rotate(scene.get_camera()->orientation());
	//draw(billboard_bird, scene);

	for (auto& pos : random_grass_positions) {
		billboard_grass.move(ter.get_normalized_terrain_height(pos[0], pos[1]));
		billboard_grass.rotate(rotation());
		draw(billboard_grass, scene);
		billboard_grass.rotate(rotation(vec3{ 0,0,1 }, 3.14f / 2));
		draw(billboard_grass, scene);
	}

	for (auto& pos : random_flowers_positions) {
		billboard_flowers.move(ter.get_normalized_terrain_height(pos[0], pos[1]));
		billboard_flowers.rotate(rotation());
		draw(billboard_flowers, scene);
		billboard_flowers.rotate(rotation(vec3{ 0,0,1 }, 3.14f / 2));
		draw(billboard_flowers, scene);
	}

	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	draw(smoke, scene);

	//draw(camera_col, scene);

	glDepthMask(true);
	glDisable(GL_BLEND);
}

/**********************************DYNAMICS*****************************************/
/****************************ANIMATION ADN COLLISION********************************/


void animate_scene(GLFWwindow* window, float dt) {
	if (user.gui.first_person) {
		move_user(window, dt);
	}

	// move particles
	smoke.update(dt);

	// move catmull rom vortex
	descr_vortex_point.update(dt);
	descr_vortex.update(dt);

	// move random vortex
	for (int i = 0; i < NUM_RAND_VORTEX; i++) {
		vec3 prev_pos = rand_vortex_points[i];
		random_vortex_move(rand_vortex_points[i], dimensions::terrain.length * RAND_VORTEX_SPEED_RELATIVE, dt);
		// we don't care for speed parameter, rand_vortex will just not move and be placed at good distance from edge
		if (ter.bounce_on_sides(rand_vortex_points[i], prev_pos, 1.0f)) {
			//  we can take previous pos
			rand_vortex_points[i] = prev_pos;
		}
		else {
			rand_vortex_points[i] = ter.get_bilinear_interpolated_terrain_height(rand_vortex_points[i].x, rand_vortex_points[i].y);
		}

		// update
		rand_vortex[i].update(dt);
	}


	// move bird
	bird.update(dt);

	// move ball
	balls.update(dt);
	for (auto& part : balls.get_particles()) {
		if (!part.isDead()) {
			if (treat_ball_collision(balls.structure, part)) {
				part.speed *= BALLS_REBOUNCE_LOSS;
			}
		}
	}
	/*
	if (!ball.isDead()) {
		ball.update(dt);
		if (treat_ball_collision()) {
			// advance until there are no more collision
			//ball.position() += 10.0f * ball.speed() * dt;
		}
	}*/

}

void move_user(GLFWwindow* window, float dt) {
	vec2 direction(get_keyboard_direction(window));
	if (norm(direction) == 0) return;
	// now according to orientation (looking for x and z in camera axis...).
	vec2 dpos = head_scene.camera.right().xy() * direction.x + head_scene.camera.front().xy() * direction.y;
	dpos /= norm(dpos);
	dpos *= user.speed * dt;
	vec3 previous_pos = head_scene.get_ground_position();

	// Get new position
	dpos += previous_pos.xy();
	head_scene.set_position_from_ground(ter.get_bilinear_interpolated_terrain_height(dpos[0], dpos[1]));

	// Now check if collision
	if (check_camera_collision()) {
		// reverting
		head_scene.set_position_from_ground(previous_pos);
	}
}

bool check_camera_collision() {
	int max_det = 1000;

	// detect collision with camera
	camera_col.primitive->local_pos = head_scene.get_camera()->position();
	camera_col.move(head_scene.get_camera()->position());

	vcl::vec3 ground_pos = head_scene.get_ground_position();

	// terrain. We must always check
	if (ter.distance_to_sides(ground_pos) <= 1.0f)
		return true;

	if (!enable_collision) return false;

	// detect with trees
	for (auto& pos : random_tree_positions) {
		tree.structure->move_first_primitive(ter.get_normalized_terrain_height(pos[0], pos[1]));
		if (camera_col.intersects(tree.structure, max_det)) {
			return true;
		}
	}

	return false;
}

// Does ball rebounce if possible.
// The ball may die or just continue. returns true if collision
bool treat_ball_collision(CollisionStructure* ball_structure, Particle& part) {
	int max_det = 1000;
	
	// we must first move the structure
	ball_structure->move_first_primitive(part.position);

	// detect terrain sides
	// collision rebounds
	/*if (enable_collision)
		ter.bounce_on_sides(part.position, part.speed, 1.0f);
	else if (ter.distance_to_sides(part.position) <= 1.0f) {
			part.kill();
			return true;
		}*/
	ter.bounce_on_sides(part.position, part.speed, 1.0f);


	// detect the ground and rebounce
	if (ter.point_intersects_ground(part.position)) {
		part.rebounce_on_surface(ter.get_normal(part.position.x, part.position.y));
		part.position = ter.get_bilinear_interpolated_terrain_height(part.position.x, part.position.y);
		return true;
	}

	// Now optional part
	if (!enable_collision) return false;


	// detect with trees
	for (auto& pos : random_tree_positions) {
		auto tree_pos = ter.get_normalized_terrain_height(pos[0], pos[1]);
		tree.structure->move_first_primitive(tree_pos);
		CollisionPrimitive* prim = ball_structure->intersects(tree.structure, max_det);
		if (prim) {
			// approximate rebounce with sphere. See dossier for all the geometry
			vec3 normal_to_sphere = part.position - (tree_pos + vec3({ 0,0,dimensions::tree.height / 2.0f }));
			part.rebounce_on_surface(normal_to_sphere);

			// Getting away
			do {
				part.position += part.speed * 0.0001f;
			} while (ball_structure->intersects(tree.structure, max_det) && max_det > 0);
			return true;
		}
	}

	// detect with bird
	if (ball_structure->intersects(static_cast<CollisionStructure*>(&bird), max_det)) {
		part.kill();
		bird.mark_hit();
		return true;
	}

	// detect fractal_sphere
	CollisionPrimitive* prim = ball_structure->intersects(fractal_sphere.structure, max_det);
	if (prim) {
		vec3 normal_to_sphere = part.position - prim->pos();
		part.rebounce_on_surface(normal_to_sphere);

		// Getting away
		/*do {
			part.position += part.speed * 0.001f;
		} while (ball_structure->intersects(tree.structure, max_det) && max_det > 0);
		return true;*/
	}

	return false;

}




// On plonge les vecteurs positions souris dans un espace projectif de dimension 3
		// On fait en sorte que p0 \equiv (0,1,0)
		// dès lors on normalise p1 sur la sphère unité dans cette espace projectif de dimension 3
		// et on demande rotation_between_vectors et on est bon. Le terme "espace projectif" n'est peut être
		// pas le meilleur pour parler de cette transformation mais bon
		//vec3 p0_proj({ 0,1,0 });
		//vec3 p1_proj(p1 - p0, 0); p1_proj += p0_proj;
		// Maintenant il faut faire deux rotations : une suivant chaque axe. Ca ne donne pas le même résultat !!!


		//Difference in x rotates along z axis
		// // WTFFF// WTFFF// WTFFF// WTFFF// WTFFF// WTFFF// WTFFF// WTFFF// WTFFF// WTFFF// WTFFF
		// // WTFFF// WTFFF// WTFFF// WTFFF// WTFFF// WTFFF// WTFFF// WTFFF// WTFFF// WTFFF// WTFFF
		// // WTFFF// WTFFF// WTFFF// WTFFF// WTFFF// WTFFF// WTFFF// WTFFF// WTFFF// WTFFF// WTFFF
		// Let's make it rotate along y axis...
		/*vec3 p0_x({ 1,0,0 });
		vec3 p1_x({ 1,0,0 }); p1_x.z = p1.x - p0.x;
		p1_x /= norm(p1_x);
		vec3 axis; float angle;
		//rotation::quaternion_to_axis_angle(rotation_between_vector(p1_x, p0_x).quat(), axis, angle);
		//std::cout << "Angle : " << angle << "; Axis : " << axis << "\n";

		camera.orientation_camera *= (rotation_between_vector(p0_x, p1_x)); // La on inverse l'ordre car x z donne base indirecte

		// WTFFF
		// // WTFFF// WTFFF// WTFFF// WTFFF// WTFFF// WTFFF// WTFFF// WTFFF// WTFFF// WTFFF// WTFFF
		// // WTFFF// WTFFF// WTFFF// WTFFF// WTFFF// WTFFF// WTFFF// WTFFF// WTFFF// WTFFF// WTFFF
		// // WTFFF// WTFFF// WTFFF// WTFFF// WTFFF// WTFFF// WTFFF// WTFFF// WTFFF// WTFFF// WTFFF



		// Difference in y rotates along x axis
		vec3 p0_y({ 0,0,1 });
		vec3 p1_y({ 0,0,1 }); p1_y.y = p1.y - p0.y;
		p1_y /= norm(p1_y);
		camera.orientation_camera *= (rotation_between_vector(p1_y, p0_y));*/

		//camera.orientation_camera = camera.orientation_camera * (rotation_between_vector(p1_proj_y, p0_proj));
		//p1_proj /= norm(p1_proj);
		/*std::cout << "Previous : ";
		std::cout << camera.orientation() << "\n Rotation : ";
		vec3 axis; float angle;
		rotation::quaternion_to_axis_angle(rotation_between_vector(p1_proj, p0_proj).quat(), axis, angle);
		std::cout << "Angle : " << angle << "; Axis : " << axis << "\n";
		camera.orientation_camera *= (rotation_between_vector(p1_proj, p0_proj));
		std::cout << camera.orientation() << "\n\n";*/

		/*vec3 p0_proj({ 0,1,0 });

		vec3 cp = cross_product(p_actual, p_deriv);
		float theta = std::acos(dot(p_deriv, p_actual));*/
		//camera.manipulator_rotate_trackball(p1, p0);

		// il faudrait arctan mais bon osef apa







/*bool treat_ball_collision() {
	int max_det = 1000;

	// detect terrain sides
	if (ter.distance_to_sides(ball.position()) <= 1.0f) {
		ball.kill();
		return true;
	}

	// detect the ground and rebounce
	if (ter.point_intersects_ground(ball.position())) {
		ball.rebounce_on_surface(ter.get_normal(ball.position().x, ball.position().y));
		ball.position() = ter.get_bilinear_interpolated_terrain_height(ball.position().x, ball.position().y);
		return true;
	}

	// Now optional part
	if (!enable_collision) return false;


	// detect with trees
	for (auto& pos : random_tree_positions) {
		auto tree_pos = ter.get_normalized_terrain_height(pos[0], pos[1]);
		tree.structure->move_first_primitive(tree_pos);
		CollisionPrimitive* prim = ball.structure->intersects(tree.structure, max_det);
		if (prim) {
			// approximate rebounce with sphere. See dossier for all the geometry
			vec3 normal_to_sphere = ball.position() - (tree_pos + vec3({ 0,0,dimensions::tree.height / 2.0f }));
			ball.rebounce_on_surface(normal_to_sphere);

			// Getting away
			do {
				ball.launch(ball.position() + ball.speed() * 0.05f, ball.speed(), 200);
			} while (ball.structure->intersects(tree.structure, max_det) && max_det > 0);
			return true;
		}
	}

	// detect with bird
	if (ball.structure->intersects(static_cast<CollisionStructure*>(&bird), max_det)) {
		ball.kill();
		bird.mark_hit();
		return true;
	}

	return false;

}*/