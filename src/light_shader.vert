#version 330
 
in vec3 a_vertex;
out vec3 v_vertex;

in vec2 a_uv;
out vec2 v_uv;

in vec3 a_normal;
out vec3 v_normal;

uniform mat4 u_model;
uniform mat3 u_normal_matrix;
uniform mat4 u_projection_view;

void main()
{
	// position of the vertex
	vec4 tmp = u_model * vec4(a_vertex, 1.0);
	gl_Position = u_projection_view * tmp;
	v_vertex = vec3(tmp);

	// pass the texture to the fragment shader
	v_uv = a_uv;

	// pass the normals to the fragment shader
	v_normal = u_normal_matrix * a_normal;
}

