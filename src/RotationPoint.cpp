#include "RotationPoint.h"

std::vector<RotationPoint*> RotationPoint::_light_sources;

RotationPoint::RotationPoint(glm::vec3 pos, float r_time, Object* object, float deviation) {
	this->_pos = pos;
	this->_rotation_time = r_time;
	this->_object = object;
	this->_deviation = deviation;
	this->_rotantes.clear();
	this->_model_matrix = glm::mat4(1.0f);
	this->_cam = nullptr;
	this->_link_rotation = false;
	this->_t0 = 0.f;
}

RotationPoint::~RotationPoint() {
	if (this->_object != nullptr) delete this->_object;
	for (RotationPoint* rp : this->_rotantes) delete rp;
}

void RotationPoint::addRotationPoint(RotationPoint* rp) {
	this->_rotantes.push_back(rp);
}

void RotationPoint::tick(float elapsed_time) {
	if (this->_object != nullptr) {
		// si se dibujan ambas caras se rota 180º y se dibuja otra vez
		if (this->_object->drawingBothFaces()) {
			this->_object->flip();

			if (!this->_link_rotation) this->_object->setTransformMatrix(this->getRealPosition());
			else this->_object->setTransformMatrix(this->_model_matrix);
			this->_object->tick(elapsed_time); // rotate object
			this->_object->draw();

			// se deshacen los cambios
			this->_object->flip();
		}

		// update object and draw
		if (!this->_link_rotation) this->_object->setTransformMatrix(this->getRealPosition());
		else this->_object->setTransformMatrix(this->_model_matrix);
		this->_object->tick(elapsed_time); // rotate object
		this->_object->draw();
	}
	if (this->_cam != nullptr) {
		this->_cam->setFront(this->getRealPosition());
	}

	for (RotationPoint* rp : this->_rotantes) {
		glm::mat4 T = glm::translate(glm::mat4(1.0f), rp->getPosition()); // posición
		// rotar x-z
		glm::mat4 Sxz = glm::mat4(1.0f);
		if (rp->getRotationTime() != 0.f) Sxz = glm::rotate(glm::mat4(1.0f), ((elapsed_time + rp->getT0()) / rp->getRotationTime()) * 360.f, glm::normalize(glm::vec3(sin(rp->getDeviation()), cos(rp->getDeviation()), 0.f)));

		rp->setModelMatrix(this->_model_matrix * Sxz * T);

		rp->tick(elapsed_time);
	}
}

glm::vec3 RotationPoint::getPosition() {
	return this->_pos;
}

float RotationPoint::getRotationTime() {
	return this->_rotation_time;
}

float RotationPoint::getDeviation() {
	return this->_deviation;
}

RotationPoint *RotationPoint::setLinkRotation(bool l) {
	this->_link_rotation = l;
	return this; // used for concatenating instructions
}

RotationPoint* RotationPoint::setT0(float t0) {
	this->_t0 = t0;
	return this; // used for concatenating instructions
}

float RotationPoint::getT0() {
	return this->_t0;
}

void RotationPoint::setModelMatrix(glm::mat4 model) {
	this->_model_matrix = model;
}

void RotationPoint::setCamera(Camera* cam) {
	this->_cam = cam;
}

int RotationPoint::getRotantesLenght() {
	return this->_rotantes.size();
}

RotationPoint* RotationPoint::getRotante(int index) {
	if (index >= this->getRotantesLenght()) return nullptr;
	return this->_rotantes[index];
}

glm::vec3 RotationPoint::getRealPosition() {
	// the last column of the model matrix tells the position
	return glm::vec3(this->_model_matrix[3]);
}

void RotationPoint::setLight() {
	RotationPoint::_light_sources.push_back(this);
}