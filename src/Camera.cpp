#include "Camera.h";

Camera::Camera() {
	this->_radius = 0.f;
	this->_rotation_xz = 0.f;
	this->_rotation_xy = 0.f;
	this->_front = glm::vec3(0.f);
}

void Camera::updateViewMatrix() {
	//this->_front = 

	// si el vector es el mismo no puede usarse
	if (!Camera::equalVector(this->_front, glm::vec3(0.0f, 1.0f, 0.0f))) {
		glm::vec3 p = this->getPosition();
		Object::setCameraPosition(p);

		Object::setViewMatrix(glm::lookAt(
			p, // the position of your camera, in world space
			this->_front, // where you want to look at, in world space
			glm::vec3(0.0f, 1.0f, 0.0f) // orientation of the camera
		));
	}
}

glm::vec3 Camera::getPosition() {
	// calculate camera's position
	glm::vec3 dir(cos(glm::radians(this->_rotation_xz)) * cos(glm::radians(this->_rotation_xy)), sin(glm::radians(this->_rotation_xy)), sin(glm::radians(this->_rotation_xz)) * cos(glm::radians(this->_rotation_xy)));
	dir = glm::normalize(dir);
	glm::vec3 pos(this->_front + dir * this->_radius);

	return pos;
}

void Camera::setRotation(glm::vec2 r) {
	this->_rotation_xz = r.x;
	if (r.y > -89.f && r.y < 89.f) this->_rotation_xy = r.y;

	this->updateViewMatrix();
}

glm::vec2 Camera::getRotation() {
	return glm::vec2(this->_rotation_xz, this->_rotation_xy);
}

void Camera::setRadius(float zoom) {
	if (zoom < MAX_CLOSE) {
		this->_radius = zoom;
		this->updateViewMatrix();
	}
}

float Camera::getRadius() {
	return this->_radius;
}

void Camera::setFront(glm::vec3 look_at) {
	this->_front = look_at;
	this->updateViewMatrix();
}