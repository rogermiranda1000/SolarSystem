#pragma once

// librerias generales
#include <iostream>
#include <string.h>
#include <map>

// librerias de OpenGL
#include "glfunctions.h"

// textura
#include "Texture.h"

// luces
#include "Light.h"

// librerias para mover objetos
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// constantes de ficheros
#define BASEPATH "assets/"

typedef struct {
	bool use_light; // shader used
	GLuint vao;
	GLuint triangles;
	GLuint vao2;
	GLuint triangles2;
	Texture *texture;
	Texture *normal_map;
	Texture *specular_map;
	Texture *transparency_map;
} LoadedObject;

class Object {
private:
	// figura
	GLuint _vao;
	GLuint _triangles;
	GLuint _vao2;
	GLuint _triangles2;
	GLuint _lod_threshold;
	Texture *_texture;
	Texture *_normal_map;
	Texture *_specular_map;
	Texture *_transparency_map;
	bool _both_faces;

	// movimiento
	glm::vec3 _position;
	glm::vec2 _rotation;
	float _size;
	glm::mat4 _transform_matrix; // matriz multiplicada
	bool _use_light;

	float _rotation_time; // axis rotation

	static glm::mat4 _projection_matrix;
	static glm::mat4 _view_matrix;
	static glm::vec3 _cam_pos;
	static glm::mat4 _projection_view_matrix; // matriz multiplicada

	glm::vec3 Object::getRelativeCameraPosition();
	static void Object::calculateProjectionViewMatrix() { Object::_projection_view_matrix = Object::_projection_matrix * Object::_view_matrix; };

public:
	// variables globales
	static GLuint g_simpleShader; // shader
	static GLuint g_lightningShader; // shader with lights
	static std::map<std::string, LoadedObject> vao_list;
	static Texture *default_normal;
	static Texture *default_specular;
	static Texture *default_transparency;

	Object(std::string path, bool use_light, std::string path_lod, float lod_threshold, bool use_normal_map, bool use_specular_map, bool use_transparency_map, glm::vec3 position, glm::vec2 rotation, float size, bool both_faces);
	// delegating constructors
	Object(std::string path, bool use_light, float size) : Object(path, use_light, std::string(""), -1.f, false, false, false, glm::vec3(0, 0, 0), glm::vec2(0, 0), size, false) {}
	Object(std::string path, bool use_light, float size, bool use_normal_map, bool use_specular_map, bool use_transparency_map) : Object(path, use_light, std::string(""), -1.f, use_normal_map, use_specular_map, use_transparency_map, glm::vec3(0, 0, 0), glm::vec2(0, 0), size, false) {}
	Object(std::string path, bool use_light, bool use_normal_map, bool use_specular_map, bool use_transparency_map) : Object(path, use_light, std::string(""), -1.f, use_normal_map, use_specular_map, use_transparency_map, glm::vec3(0, 0, 0), glm::vec2(0, 0), 1.f, false) {}
	Object(std::string path, bool use_light) : Object(path, use_light, std::string(""), -1.f, false, false, false, glm::vec3(0, 0, 0), glm::vec2(0, 0), 1.f, false) {}
	Object(std::string path, bool use_light, std::string path_lod, float lod_threshold) : Object(path, use_light, path_lod, lod_threshold, false, false, false, glm::vec3(0, 0, 0), glm::vec2(0, 0), 1.f, false) {}
	Object(std::string path, bool use_light, std::string path_lod, float lod_threshold, bool use_normal_map, bool use_specular_map, bool use_transparency_map) : Object(path, use_light, path_lod, lod_threshold, use_normal_map, use_specular_map, use_transparency_map, glm::vec3(0, 0, 0), glm::vec2(0, 0), 1.f, false) {}

	void Object::tick(float elapsed_time);
	void Object::draw();

	// getter/setter
	static void Object::setProjectionMatrix(glm::mat4 projection_matrix) {
		Object::_projection_matrix = projection_matrix;
		Object::calculateProjectionViewMatrix();
	}
	static void Object::setViewMatrix(glm::mat4 view_matrix) {
		Object::_view_matrix = view_matrix;
		Object::calculateProjectionViewMatrix();
	}
	static glm::mat4 Object::getViewMatrix() { return Object::_view_matrix; }
	static void Object::setCameraPosition(glm::vec3 cam_pos) { Object::_cam_pos = cam_pos; }
	void Object::setPosition(glm::vec3 position);
	Object *Object::setRotationTime(float rotation_time);
	glm::vec3 Object::getPosition();
	void Object::setTransformMatrix(glm::vec3 pos);
	void Object::setTransformMatrix(glm::mat4 init);
	void Object::setRotation(glm::vec2 rotation);
	glm::vec2 Object::getRotation();
	bool Object::drawingBothFaces();
	void Object::setSize(float size);
	float Object::getSize();
	void Object::flip();
};