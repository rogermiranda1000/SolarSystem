#version 330

uniform sampler2D u_texture;
uniform sampler2D u_texture_transparency;

in vec2 v_uv;
in vec3 v_normal;

out vec4 fragColor;

void main(void)
{
	// Transparencia
	float fade = texture(u_texture_transparency, v_uv).x;

	vec4 texture_color = texture(u_texture, v_uv);

	vec3 final_color = texture_color.xyz;
	fragColor =  vec4(final_color, fade);
}
