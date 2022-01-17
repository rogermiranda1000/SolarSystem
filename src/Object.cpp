#include "Object.h"

// variables de carga del .obj
#define TINYOBJLOADER_IMPLEMENTATION // único fichero que usa esto
#include "tiny_obj_loader.h"

// global variables
GLuint Object::g_simpleShader;
GLuint Object::g_lightningShader;
std::map<std::string, LoadedObject> Object::vao_list;
glm::mat4 Object::_projection_matrix;
glm::mat4 Object::_view_matrix;
glm::mat4 Object::_projection_view_matrix; // matriz multiplicada
glm::vec3 Object::_cam_pos;
Texture *Object::default_normal;
Texture *Object::default_specular;
Texture *Object::default_transparency;

GLuint createVAO(std::vector<tinyobj::shape_t> shapes, bool use_light) {
	GLuint vao;

	// VAO del objeto
	vao = gl_createAndBindVAO();

	// Shader del objeto
	GLuint shader = use_light ? Object::g_lightningShader : Object::g_simpleShader;

	// vertex buffer
	gl_createAndBindAttribute(&(shapes[0].mesh.positions[0]),
		shapes[0].mesh.positions.size() * sizeof(float),
		shader, "a_vertex", 3);
	gl_createIndexBuffer(&(shapes[0].mesh.indices[0]),
		shapes[0].mesh.indices.size() * sizeof(unsigned int));

	// texture
	gl_createAndBindAttribute(&(shapes[0].mesh.texcoords[0]),
		shapes[0].mesh.texcoords.size() * sizeof(GLfloat),
		shader, "a_uv", 2);

	// TODO: light
	gl_createAndBindAttribute(&(shapes[0].mesh.normals[0]),
		shapes[0].mesh.normals.size() * sizeof(float),
		shader, "a_normal", 3);

	gl_unbindVAO();

	return vao;
}

LoadedObject loadObject(std::string path, std::string path_lod, bool use_light, bool use_normal_map, bool use_specular_map, bool use_transparency_map) {
	LoadedObject r;

	r.use_light = use_light;

	std::string base(BASEPATH);
	std::string file_path = base + path;
	std::string object_path = file_path + ".obj";

	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;

	bool ret = tinyobj::LoadObj(shapes, materials, err, object_path.c_str(), base.c_str());
	if (!ret) throw std::runtime_error(err);
	r.vao = createVAO(shapes, use_light);

	// nº triangulos
	r.triangles = shapes[0].mesh.indices.size() / 3;


	// LOD
	if (path_lod.compare(std::string("")) == 0) r.triangles2 = 0;
	else {
		std::string object2_path = base + path_lod + ".obj";

		std::vector<tinyobj::shape_t> shapes2;
		std::vector<tinyobj::material_t> materials2;
		std::string err2;

		bool ret2 = tinyobj::LoadObj(shapes2, materials2, err2, object2_path.c_str(), base.c_str());
		if (!ret2) throw std::runtime_error(err2);
		r.vao2 = createVAO(shapes2, use_light);

		// nº triangulos
		r.triangles2 = shapes2[0].mesh.indices.size() / 3;
	}

	r.texture = new Texture(file_path + ".bmp");
	if (use_normal_map) r.normal_map = new Texture(file_path + "_normal.bmp");
	else r.normal_map = Object::default_normal;
	if (use_specular_map) r.specular_map = new Texture(file_path + "_specular.bmp");
	else r.specular_map = Object::default_specular;
	if (use_transparency_map) r.transparency_map = new Texture(file_path + "_transparency.bmp");
	else r.transparency_map = Object::default_transparency;

	return r;
}

Object::Object(std::string path, bool use_light, std::string path_lod, float lod_threshold, bool use_normal_map, bool use_specular_map, bool use_transparency_map, glm::vec3 position, glm::vec2 rotation, float size, bool both_faces) {

	std::map<std::string, LoadedObject>::const_iterator it = Object::vao_list.find(path); // iterador

	// NOTE: if one object is drawn with a 'use_light' value, all the other objects ignore the new use_light parameter
	if (it == Object::vao_list.end()) {
		// no se ha encontrado la figura
		this->_use_light = use_light;
		LoadedObject obj = loadObject(path, path_lod, this->_use_light, use_normal_map, use_specular_map, use_transparency_map); // obtén VAO

		this->_vao = obj.vao;
		this->_triangles = obj.triangles;
		this->_vao2 = obj.vao2;
		this->_triangles2 = obj.triangles2;
		this->_texture = obj.texture;
		this->_normal_map = obj.normal_map;
		this->_specular_map = obj.specular_map;
		this->_transparency_map = obj.transparency_map;

		Object::vao_list.insert(std::pair<std::string, LoadedObject>(path, obj)); // inserta
	}
	else {
		// figura encontrada, reutiliza VAO
		this->_use_light = it->second.use_light;
		this->_vao = it->second.vao;
		this->_triangles = it->second.triangles;
		this->_vao2 = it->second.vao2;
		this->_triangles2 = it->second.triangles2;
		this->_texture = it->second.texture;
		this->_normal_map = it->second.normal_map;
		this->_specular_map = it->second.specular_map;
		this->_transparency_map = it->second.transparency_map;
	}
	this->_lod_threshold = lod_threshold;

	this->setRotation(rotation);
	this->setSize(size);
	this->setPosition(position);
	this->_rotation_time = 0.f;
	this->_both_faces = both_faces;
}

glm::vec3 Object::getRelativeCameraPosition() {
	return glm::vec3((Object::_view_matrix * this->_transform_matrix)[3]);
}

void Object::tick(float elapsed_time) {
	if (this->_rotation_time == 0.f) return;

	glm::vec2 r = this->_rotation;
	r.y = this->_rotation_time * elapsed_time;
	this->setRotation(r);
}

void Object::draw() {
	GLuint shader = (this->_use_light ? Object::g_lightningShader : Object::g_simpleShader); // select shader

	glUseProgram(shader); // usa el shader

	// LOD
	GLuint triangles;
	glm::vec3 distance = this->getRelativeCameraPosition();
	if ((this->_lod_threshold == -1.f || this->_triangles2 == 0) || glm::dot(distance, distance) <= this->_lod_threshold * this->_lod_threshold) {
		gl_bindVAO(this->_vao);
		triangles = this->_triangles;
	}
	else {
		gl_bindVAO(this->_vao2);
		triangles = this->_triangles2;
	}

	GLuint model_loc = glGetUniformLocation(shader, "u_model");
	glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(this->_transform_matrix));

	// projection & view matrix
	GLuint projection_view_loc = glGetUniformLocation(shader, "u_projection_view");
	glUniformMatrix4fv(projection_view_loc, 1, GL_FALSE, glm::value_ptr(Object::_projection_view_matrix));

	// texture
	GLuint u_texture = glGetUniformLocation(shader, "u_texture"); // find the sample uniform in the shader
	glUniform1i(u_texture, this->_texture->getTextureUnit()); // bind the sample to the texture unit

	// transparncy map
	GLuint u_texture_transparency = glGetUniformLocation(shader, "u_texture_transparency"); // find the sample uniform in the shader
	glUniform1i(u_texture_transparency, this->_transparency_map->getTextureUnit()); // bind the sample to the texture unit

	// light
	if (this->_use_light) {
		// projection & view matrix
		glm::mat3 normal_matrix = glm::mat3(glm::transpose(glm::inverse(this->_transform_matrix)));
		GLuint normal_loc = glGetUniformLocation(shader, "u_normal_matrix");
		glUniformMatrix3fv(normal_loc, 1, GL_FALSE, glm::value_ptr(normal_matrix));

		// Lights
		std::vector<glm::vec3> lights = Light::getPositions();
		int size = lights.size();
		if (size > MAX_LIGHTS) size = MAX_LIGHTS;
		GLuint light_amount_loc = glGetUniformLocation(shader, "u_lights"); // find the light uniform in the shader
		glUniform1f(light_amount_loc, size); // TODO: lights
		for (int i = 0; i < size; i++) {
			std::string name = "u_light_pos[" + std::to_string(i) + "]";
			GLuint light_pos_loc = glGetUniformLocation(shader, name.c_str()); // find the light uniform in the shader
			glUniform3f(light_pos_loc, lights[i].x, lights[i].y, lights[i].z);
		}

		GLuint cam_loc = glGetUniformLocation(shader, "u_cam_pos"); // find the light uniform in the shader
		glUniform3f(cam_loc, Object::_cam_pos.x, Object::_cam_pos.y, Object::_cam_pos.z);

		// normal map
		GLuint u_texture_normal = glGetUniformLocation(shader, "u_texture_normal"); // find the sample uniform in the shader
		glUniform1i(u_texture_normal, this->_normal_map->getTextureUnit()); // bind the sample to the texture unit

		// specular map
		GLuint u_texture_specular = glGetUniformLocation(shader, "u_texture_specular"); // find the sample uniform in the shader
		glUniform1i(u_texture_specular, this->_specular_map->getTextureUnit()); // bind the sample to the texture unit

		// TODO light material texture
		GLuint specular = glGetUniformLocation(shader, "u_specular");
		glUniform3f(specular, 1.f, 1.f, 1.f);
		GLuint shininess = glGetUniformLocation(shader, "u_shininess");
		glUniform1f(shininess, 180.f);
		GLuint ambient = glGetUniformLocation(shader, "u_ambient");
		glUniform1f(ambient, 0.2f);
		GLuint difuse = glGetUniformLocation(shader, "u_difuse");
		glUniform1f(difuse, 1.0f/Light::getLights()); // si hay 1 luz el difuso es 100%, si hay 2 50%...
	}

	glDrawElements(GL_TRIANGLES, 3 * triangles, GL_UNSIGNED_INT, 0); // dibuja
}

void Object::setTransformMatrix(glm::vec3 pos) {
	this->setTransformMatrix(glm::translate(glm::mat4(1.0f), pos));
}

void Object::setTransformMatrix(glm::mat4 init) {
	glm::mat4 T, Rx, Ry, S;

	T = glm::translate(glm::mat4(1.0f), this->_position);
	Rx = glm::rotate(glm::mat4(1.0f), this->_rotation.x, glm::vec3(1.0f, 0.0f, 0.0f)); // x
	Ry = glm::rotate(glm::mat4(1.0f), this->_rotation.y, glm::vec3(0.0f, 1.0f, 0.0f)); // y
	S = glm::scale(glm::mat4(1.0f), glm::vec3(this->_size, this->_size, this->_size));

	this->_transform_matrix = init * T * Rx * Ry * S;
}

// setters/getters
void Object::setSize(float size) {
	this->_size = size;
}

void Object::setPosition(glm::vec3 position) {
	this->_position = position;
}

glm::vec3 Object::getPosition() {
	return this->_position;
}

glm::vec2 Object::getRotation() {
	return this->_rotation;
}

Object *Object::setRotationTime(float rotation_time) {
	this->_rotation_time = rotation_time;
	return this;
}

void Object::setRotation(glm::vec2 rotation) {
	this->_rotation = rotation;
}

bool Object::drawingBothFaces() {
	return this->_both_faces;
}

float Object::getSize() {
	return this->_size;
}

void Object::flip() {
	if (this->_rotation.x >= 360.0f) this->_rotation.x -= 180.0f;
	else this->_rotation.x += 180.0f;
}