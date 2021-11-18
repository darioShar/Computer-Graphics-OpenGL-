#include "Smoke.hpp"

Smoke::Smoke()
	: ParticleSystem()
	, vbo(0)
	, vao(0)
	, shader(0)
	, texture(0)
{
	shading.color = SMOKE_COLOR;
	shading.attenuation_coeffs = SMOKE_ATTENUATION;
	shading.default_size = SMOKE_DEFAULT_SIZE;
	shading.ttl = 0.0f;
}


void Smoke::setup_gpu_data() {
	// 2.2 Create VBO - Send data to GPU
	// ******************************************* //

	// First, VAO
	// Create an empty VAO identifiant
	glGenVertexArrays(1, &vao);
	// Activate the VAO designated by the variable "vao"
	glBindVertexArray(vao);

	// Create an empty VBO identifiant
	glGenBuffers(1, &vbo);
	// Activate the VBO designated by the variable "vbo"
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// Send data to GPU: Fill the currently designated VBO with the buffer of data passed as parameter
	glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(num_points * sizeof(Particle)), &particles[0], GL_DYNAMIC_DRAW); opengl_check;
	//glBufferSubData(GL_ARRAY_BUFFER, 0, num_points * sizeof(Particle), &particles[0]);



	// Activate the use of the variable at index layout=0 in the shader
	glEnableVertexAttribArray(0);
	// index in the layout, size, type, rest ok

	// here position : 3 floats
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), nullptr); opengl_check;

	// speed : 3 floats
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)(3 * sizeof(float))); opengl_check;

	// ttl : 1 float
	glEnableVertexAttribArray(2);
	// index in the layout, size, type, rest ok
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)(6 * sizeof(float))); opengl_check;


	// As a good practice, disable VBO and VAO after their use
	glBindVertexArray(0); opengl_check;
	glBindBuffer(GL_ARRAY_BUFFER, 0); opengl_check;

	// Now data is loaded in GPU
}



void Smoke::setup(const std::string& shader_vert, const std::string& shader_frag, const std::string& texture_file)
{
	// setup shader and texture
	shader = vcl::opengl_create_shader_program(
		vcl::read_text_file(shader_vert), vcl::read_text_file(shader_frag));
	texture = vcl::opengl_texture_to_gpu(vcl::image_load_png(texture_file));
}

void Smoke::initialize_dynamics(int _num_points, const vcl::vec3* pos, const vcl::vec3& speed, float ttl) {
	pos_to_follow = pos;
	ParticleSystem::initialize_dynamics(_num_points, *pos, speed, ttl);
	srand(time(0));

	for (auto& particle : particles)
		modulate_smoke_particle_reset(particle);

	shading.ttl = ttl;
	setup_gpu_data();
}

void Smoke::update_smoke_particle(Particle& particle, float dt) {
	// Euler semi-implicite, même si ici on se fiche de ne pas être stable. en fait c'est même plus partique
	//speed += vcl::vec3(2.0f * position.z * position.y, position.y, -2.0f * position.x * position.y) * dt;
	float x = (particle.position.x - pos_to_follow->x);
	float y = (particle.position.y - pos_to_follow->y);
	float z = (particle.position.z - pos_to_follow->z);

	if (std::abs(x) > 0.1f) x *= 0.1f / std::abs(x);
	if (std::abs(y) > 0.1f) y *= 0.1f / std::abs(y);

	particle.speed.x = -2.0f * y * vorticity * z;
	particle.speed.z = std::abs(z) + initial_offset;
	particle.speed.y = 2.0f * x * vorticity * z;
	particle.speed -= PARTICLE_SPEED_DRAG * particle.speed * dt;
	particle.speed += Particle::random_move(randomness);
	particle.position += particle.speed * dt;

	//position += random_move(0.5f*dt);

	//position += speed * dt * (float)rand() / (float)RAND_MAX;
}

void Smoke::modulate_smoke_particle_reset(Particle& particle) {
	particle.speed *= PARTICLE_SPEED_DRAG;
	particle.ttl *= ((float)rand() / (float)RAND_MAX + PARTICLE_TIME_UNCERTAINTY);
}



void Smoke::update(float dt) {

	for (auto& particle : particles) {
		particle.update(dt);
		if (particle.isDead()) {
			particle.reset(*pos_to_follow, init_speed, shading.ttl);
			modulate_smoke_particle_reset(particle);
		}
		else 
			update_smoke_particle(particle, dt);
	}

	// send data to gpu
	// hey vao
	glBindVertexArray(vao);   opengl_check;
	glBindBuffer(GL_ARRAY_BUFFER, vbo); opengl_check;
	glBufferSubData(GL_ARRAY_BUFFER, 0, num_points * sizeof(Particle), &particles[0]);  opengl_check;

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0); opengl_check;

}
