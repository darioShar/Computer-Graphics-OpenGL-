/*#version 330 core

layout(location=0) out vec4 FragColor;

uniform sampler2D image_texture;
uniform vec4 color = vec4(1.0,1.0,1.0,1.0);

void main()
{	
	vec4 color_image_texture = texture(image_texture, gl_PointCoord);
	FragColor = color_image_texture;
	FragColor *= color;
	//FragColor[3] = 1.0;
}*/

#version 330 core

layout(location=0) out vec4 FragColor;

uniform sampler2D image_texture;
uniform vec3 color = vec3(1.0,1.0,1.0);

in float life_elapsed;

void main()
{	
	vec4 color_image_texture = texture(image_texture, gl_PointCoord);
	FragColor = color_image_texture;
	FragColor.rgb *= pow(life_elapsed, 0.25);
	FragColor.a = pow(1 - life_elapsed, 6) ;

	//if (color_image_texture.a == 0.0) FragColor = vec4(0,0,0,0);	
	//FragColor.rgb *= color;
	//FragColor[3] = 1.0;
}