#pragma once

#include "glfunctions.h" // glm::vec3
#include <vector>

#define MAX_LIGHTS 5

class Light {
private:
	static std::vector<glm::vec3> _light_sources;

public:
	static void Light::addLight(glm::vec3 pos) { Light::_light_sources.push_back(pos); }

	static void Light::emptyLights() { Light::_light_sources.clear(); }

	static std::vector<glm::vec3> getPositions() { return Light::_light_sources; }
	static int getLights() { return Light::_light_sources.size(); }
};