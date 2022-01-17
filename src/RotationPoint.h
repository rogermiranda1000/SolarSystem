#pragma once

#include "glfunctions.h" // glm::vec3
#include <vector>

#include "Object.h"
#include "Camera.h"
#include "Light.h"

// librerias para mover objetos
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class RotationPoint {
private:
	glm::vec3 _pos;
	float _rotation_time;
	float _deviation;
	glm::mat4 _model_matrix;
	bool _link_rotation; // the object will preserve the father's rotation
	float _t0; // initial time (deviate the rotation)

	Object* _object;
	Camera* _cam;
	
	std::vector<RotationPoint*> _rotantes;
	static std::vector<RotationPoint*> _light_sources;

public:
	static void RotationPoint::emptyLights() { RotationPoint::_light_sources.clear(); }

	RotationPoint(glm::vec3 pos, float r_time, Object* object, float deviation);
	// delegating constructors
	RotationPoint(glm::vec3 pos, float r_time, Object* object) : RotationPoint(pos, r_time, object, 0.f) {}
	RotationPoint(glm::vec3 pos, float r_time, float deviation) : RotationPoint(pos, r_time, nullptr, deviation) {}
	RotationPoint(glm::vec3 pos, float r_time) : RotationPoint(pos, r_time, nullptr, 0.f) {}
	RotationPoint(glm::vec3 pos) : RotationPoint(pos, 0.f, nullptr, 0.f) {}

	~RotationPoint();

	void RotationPoint::tick(float elapsed_time);

	void RotationPoint::addRotationPoint(RotationPoint* rp);

	/* SETTERS/GETTERS */
	glm::vec3 RotationPoint::getPosition();
	float RotationPoint::getRotationTime();
	float RotationPoint::getDeviation();
	void RotationPoint::setModelMatrix(glm::mat4 model);
	RotationPoint *RotationPoint::setLinkRotation(bool l);
	RotationPoint *RotationPoint::setT0(float t0);
	float RotationPoint::getT0();
	void RotationPoint::setCamera(Camera* cam);
	int RotationPoint::getRotantesLenght();
	RotationPoint *RotationPoint::getRotante(int index);
	glm::vec3 RotationPoint::getRealPosition();
	void RotationPoint::setLight();

	static void RotationPoint::updateLightsPositions() {
		Light::emptyLights();

		for (RotationPoint* rp : RotationPoint::_light_sources) Light::addLight(rp->getRealPosition());
	}
};
