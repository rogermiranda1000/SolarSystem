#pragma once

#include "glfunctions.h" // glm::vec3
#include <glm/gtx/rotate_vector.hpp>

#include "Object.h"

#define MAX_CLOSE -1.f

class Camera {
private:
	float _radius; // zoom
	float _rotation_xz; // "giro"
	float _rotation_xy; // "subir"
	glm::vec3 _front;

	void Camera::updateViewMatrix();
	static bool Camera::equalVector(glm::vec2 a, glm::vec2 b) { return (a.x == b.x && a.y == b.y); }
	static bool Camera::equalVector(glm::vec3 a, glm::vec3 b) { return (a.x == b.x && a.y == b.y && a.z == b.z); }
public:
	Camera();

	void Camera::setRadius(float zoom);
	float Camera::getRadius();
	void Camera::setRotation(glm::vec2 r);
	glm::vec2 Camera::getRotation();
	glm::vec3 Camera::getPosition();
	void Camera::setFront(glm::vec3 look_at);
};
