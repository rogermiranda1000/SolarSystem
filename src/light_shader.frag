#version 330

out vec4 fragColor;

in vec2 v_uv;
in vec3 v_normal;
in vec3 v_vertex;

uniform sampler2D u_texture;
uniform sampler2D u_texture_normal;
uniform sampler2D u_texture_specular;
uniform sampler2D u_texture_transparency;

uniform float u_lights;
uniform vec3 u_light_pos[5];

uniform vec3 u_cam_pos;

uniform float u_ambient;
uniform vec3 u_specular;
uniform float u_shininess;
uniform float u_difuse;

mat3 cotangent_frame(vec3 N, vec3 p, vec2 uv) {
	// obtener vectores del borde del triángulo de pixeles
	vec3 dp1 = dFdx(p);
	vec3 dp2 = dFdy(p);
	vec2 duv1 = dFdx(uv);
	vec2 duv2 = dFdy(uv);

	// resolver el sistema lineal
	vec3 dp2perp = cross(dp2, N);
	vec3 dp1perp = cross(N, dp1);
	vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
	vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;

	// construir un scale-invariant frame
	float invmax = inversesqrt(max(dot(T,T), dot(B,B)));
	return mat3(T * invmax, B * invmax, N);
}

// siendo N el vértice interpolado normal y
// V el view vector (vertex to eye)
vec3 perturbNormal(vec3 N, vec3 V, vec2 textcoord, vec3 normal_pixel) {
	normal_pixel = normal_pixel * 2.0 - 1.0;
	mat3 TBN = cotangent_frame(N, V, textcoord);
	return normalize(TBN * normal_pixel);
}

void main(void)
{
	// Transparencia
	float fade = texture(u_texture_transparency, v_uv).x;

	// calcular la normal con normal map
	vec3 texture_normal = texture(u_texture_normal, v_uv).xyz;
	vec3 N = normalize(v_normal);
	N = perturbNormal(N, v_vertex, v_uv, texture_normal);

	// specular map
	float factorLuz = texture(u_texture_specular, v_uv).x;
	
	vec4 texture_color = texture(u_texture, v_uv);

	vec3 color = texture_color.xyz*u_ambient; // ambient light
	
	vec3 E = normalize(u_cam_pos - v_vertex); // el vector E no cambia een función de las luces
	for (int i = 0; i < u_lights; i++) {
		vec3 L = normalize(u_light_pos[i] - v_vertex);
		//vec3 R = normalize(-reflect(L,N));
		vec3 H = normalize(L+E); // vector entre L y E
		
		float NdotL = max(dot(N, L), 0.0);
		//float RdotE = max(dot(R, E), 0.0);
		float NdotH = max(dot(N, H), 0.0);
		
		color += texture_color.xyz * NdotL * u_difuse; // diffuse color
		color += u_specular * pow(/*RdotE*/ NdotH, u_shininess) * factorLuz; // specular color
	}
	
	fragColor = vec4(color, fade);
}
