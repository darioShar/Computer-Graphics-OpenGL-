/*#version 330 core

layout (location = 0) in vec3 position;


uniform mat4 view;
uniform mat4 projection;

uniform float default_size = 5.0;
uniform vec3 coeffs = vec3(1.0, 0.0, 0.5);

void main()
{
	vec4 camera_pos = view * vec4(position, 1.0);
	float d = length(camera_pos.xyz);
	// Attenuation wrt ditance to camera
	gl_PointSize = default_size / sqrt(coeffs[0] + coeffs[1]*d + coeffs[2]*d*d);
	gl_Position = projection * camera_pos;
}*/

#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 speed;
layout (location = 2) in float ttl;

uniform mat4 view;
uniform mat4 projection;

uniform float default_size = 5.0;
uniform vec3 coeffs = vec3(1.0, 0.0, 0.5);

uniform float total_time;

out float life_elapsed;

void main()
{
	vec4 camera_pos = view * vec4(position, 1.0);
	float d = length(camera_pos.xyz);
	// Attenuation wrt ditance to camera
	gl_PointSize = default_size / sqrt(coeffs[0] + coeffs[1]*d + coeffs[2]*d*d);
	gl_Position = projection * camera_pos;

	life_elapsed = ttl / total_time;
}
