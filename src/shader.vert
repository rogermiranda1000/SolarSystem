#version 330
 
in vec3 a_vertex;

in vec2 a_uv;
out vec2 v_uv;

in vec3 a_normal;
out vec3 v_normal;

uniform mat4 u_model;
uniform mat4 u_projection_view;

void main()
{
	// position of the vertex
	gl_Position = u_projection_view * u_model * vec4(a_vertex, 1.0);

	// pass the texture to the fragment shader
	v_uv = a_uv;

	// pass the normals to the fragment shader
	v_normal = a_normal;
}

