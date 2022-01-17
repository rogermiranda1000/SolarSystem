/*  by Roger Miranda LaSalle (roger.miranda@students.salle.url.edu) */

#include "Object.h"
#include "RotationPoint.h"
#include "Camera.h"

//include some standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <math.h>
#include <time.h>

//include OpenGL & some custom code files
#include "glfunctions.h" //include all OpenGL stuff
#include "Shader.h" // class to compile shaders

// divisiones para cada eje en la rotación de la cámara
#define X_SENSITIVITY 6.f
#define Y_SENSITIVITY 12.f
#define ZOOM_SENSITIVITY 80.f

// pantalla
#define VIEWPORT_WIDTH 1920
#define VIEWPORT_HEIGHT 1080
#define FOV 90.f // field of view

using namespace std;

RotationPoint* center;
Object* skybox;

//global variables to help us do things
double mouse_x, mouse_y; //variables storing mouse position
const glm::vec3 g_backgroundColor(0.2f, 0.2f, 0.2f); // background colour - a GLM 3-component vector


Camera* camera;
int camera_index = 0;
void setCameraFocus(int focus) {
	if (camera_index == focus) return; // focus on current
	if (focus < 0 || focus >= center->getRotantesLenght()) return;

	center->getRotante(camera_index)->setCamera(nullptr); // disable last camera
	center->getRotante(focus)->setCamera(camera); // enable new camera
	camera_index = focus;
}

double last_frame = 0;
void showCurrentFPS() {
	char txt[15];

	double current = glfwGetTime(), dif = current - last_frame;
	sprintf(txt, "FPS: %.2f", dif != 0 && dif ? 1/dif : -1);
	last_frame = current;

	cout << txt << endl;
}

void load()
{
	std::cout << "Using " << glGetString(GL_RENDERER) << std::endl;
	// max textures (min. 16)
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &Texture::max_textures);
	std::cout << "Max textures: " << Texture::max_textures << std::endl;

	//load the shaderd
	Shader simpleShader("src/shader.vert", "src/shader.frag");
	Object::g_simpleShader = simpleShader.program;
	Shader lightShader("src/light_shader.vert", "src/light_shader.frag");
	Object::g_lightningShader = lightShader.program;

	Object::setProjectionMatrix (glm::perspective (
		FOV,
		(float)VIEWPORT_WIDTH/VIEWPORT_HEIGHT, // aspect ratio
		0.1f, // near plane
		800.0f // far plane
	));

	Object::vao_list.clear();
	Texture::active_textures.clear();
	RotationPoint::emptyLights();

	last_frame = glfwGetTime();

	Object::default_normal = new Texture(std::string(BASEPATH) + "/default_normal.bmp");
	Object::default_specular = new Texture(std::string(BASEPATH) + "/default_specular.bmp");
	Object::default_transparency = new Texture(std::string(BASEPATH) + "/default_transparency.bmp");

	center = new RotationPoint(glm::vec3(0.f,0.f,0.f));
	center->addRotationPoint(new RotationPoint( glm::vec3(0.f, 0.f, 0.f), 0.f, new Object("real_earth", true, "low_real_earth", 60.f, true, true, false, glm::vec3(0.0f, -8.f, 0.0f), glm::vec2(0, 0), 0.1f, false) ));
	RotationPoint* sun = new RotationPoint(glm::vec3(0.f, 0.f, 4.f), 6.f);
	RotationPoint* light_source = new RotationPoint( glm::vec3(0.f, 0.f, -1.2f), 2.f, new Object("sun", false) );
	sun->addRotationPoint(light_source);
	light_source->setLight();
	light_source = new RotationPoint( glm::vec3(0.f, 0.f, 1.2f), 2.f, new Object("sun", false) );
	sun->addRotationPoint(light_source);
	light_source->setLight();
	for (float y = -2.f; y <= 2.f; y += 0.5f) {
		for (float x = 0; x < 20.f; x += 2.f) sun->addRotationPoint((new RotationPoint(glm::vec3(0.f, 0.f, 3.f), 20.f, new Object("solar_panel", true, "low_solar_panel", 5.f), y))->setLinkRotation(true)->setT0(x));
	}
	center->addRotationPoint(sun);
	center->addRotationPoint((new RotationPoint( glm::vec3(0.f, 0.f, -4.f), 6.f, new Object("moon", true, true, false, false) ))->setLinkRotation(true));
	RotationPoint* mars = new RotationPoint(glm::vec3(0.f, 0.f, 35.f), 20.f, new Object("mars", true, 1.5f));
	RotationPoint* phobos = new RotationPoint(glm::vec3(0.f, 0.f, 4.f), 4.5f, new Object("phobos", true, 0.9f));
	phobos->addRotationPoint(new RotationPoint( glm::vec3(0.f, 0.f, 0.f), 0.f, new Object("BFG10000", true, "low_BFG10000", 10.f, false,  false, false, glm::vec3(0.0f, 0.97f, 0.0f), glm::vec2(0, 0), 0.0005f, false) ));
	mars->addRotationPoint(phobos);
	mars->addRotationPoint(new RotationPoint( glm::vec3(0.f, 0.f, 7.f), 9.5f, new Object("deimos", true, 0.7f) ));
	center->addRotationPoint(mars);
	center->addRotationPoint(new RotationPoint( glm::vec3(0.f, 0.f, 45.f), 25.f, new Object("venus", true, 2.f) ));
	center->addRotationPoint(new RotationPoint( glm::vec3(0.f, 0.f, 70.f), 30.f, new Object("mercury", true, 0.9f) ));
	center->addRotationPoint(new RotationPoint( glm::vec3(0.f, 0.f, 110.f), 9.f, (new Object("jupiter", true, 30.f))->setRotationTime(30.f) ));
	RotationPoint* saturn = new RotationPoint( glm::vec3(0.f, 0.f, 220.f), 40.f, new Object("saturn", true, 20.f) );
	saturn->addRotationPoint(new RotationPoint( glm::vec3(0.f, 0.f, 0.f), 0.f, new Object("saturn_rings", true, std::string(""), -1.f, false, false, true, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(90.0f, 0.0f), 100.f, true) ));
	center->addRotationPoint(saturn);
	center->addRotationPoint(new RotationPoint( glm::vec3(0.f, 0.f, 260.f), 50.f, new Object("uranus", true, 6.f) ));
	center->addRotationPoint(new RotationPoint( glm::vec3(0.f, 0.f, 300.f), 65.f, new Object("neptune", true, 6.f) ));

	camera = new Camera();
	camera->setRadius(-10.f);
	center->getRotante(0)->setCamera(camera);

	skybox = new Object("skybox", false);
}

// ------------------------------------------------------------------------------------------
// This function actually draws to screen and called non-stop, in a loop
// ------------------------------------------------------------------------------------------
void draw()
{
	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// semitransparencia
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// draw only 1 face
	glEnable(GL_CULL_FACE);

	glDisable(GL_DEPTH_TEST);
	glCullFace(GL_FRONT);

	skybox->setTransformMatrix(camera->getPosition());
	skybox->draw();


	// los objetos lejanos no han de dibujarse
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);

	RotationPoint::updateLightsPositions();
	center->tick(glfwGetTime());

	//showCurrentFPS();
}

void destroy() {
	// destroy objects
	delete center; // it will destroy all the sub-objects
	delete camera;
	delete skybox;
	for (std::map<Texture*, int>::iterator it = Texture::active_textures.begin(); it != Texture::active_textures.end(); ++it)
	{
		delete it->first;
	}
}

bool left_pressed = false;
bool right_pressed = false;
float first_cursor_x;
float first_cursor_y;
// ------------------------------------------------------------------------------------------
// This function is called every time you click the mouse
// ------------------------------------------------------------------------------------------
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	bool *cursor = NULL, *other_cursor = NULL;

	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
		// return to the main planet
		setCameraFocus(0);
	}



	if (button == GLFW_MOUSE_BUTTON_LEFT && (action == GLFW_PRESS || action == GLFW_RELEASE)) {
		cursor = &left_pressed;
		other_cursor = &right_pressed;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && (action == GLFW_PRESS || action == GLFW_RELEASE)) {
		cursor = &right_pressed;
		other_cursor = &left_pressed;
	}

	if (cursor != NULL && other_cursor != NULL) {
		*cursor = action;
		if (action) {
			*other_cursor = false; // deactivate the other (just in case)
			first_cursor_x = mouse_x;
			first_cursor_y = mouse_y;
		}
	}
}

void onMouseMove(GLFWwindow* window, double xpos, double ypos) {
	if (!left_pressed && !right_pressed) return;

	
	if (left_pressed) {
		// rotate
		camera->setRotation( camera->getRotation() + glm::vec2((xpos - first_cursor_x) / X_SENSITIVITY, -(ypos - first_cursor_y) / Y_SENSITIVITY) );
		first_cursor_x = xpos;
		first_cursor_y = ypos;
	}
	else {
		// zoom
		camera->setRadius(camera->getRadius() + (xpos - first_cursor_x) / ZOOM_SENSITIVITY);
		first_cursor_x = xpos;
	}
}

// change between planets
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	//cout << xoffset << ", " << yoffset << endl;
	int f = camera_index - yoffset, max = center->getRotantesLenght() - 1;
	if (f < 0) f = max;
	else if (f > max) f = 0;

	setCameraFocus(f);
}

// ------------------------------------------------------------------------------------------
// This function is called every time you press a screen
// ------------------------------------------------------------------------------------------
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	//quit
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, 1);
	//reload
	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		destroy();
		load();
	}
}

int main(void)
{
	//setup window and boring stuff, defined in glfunctions.cpp
	GLFWwindow* window;
	if (!glfwInit())return -1;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(VIEWPORT_WIDTH, VIEWPORT_HEIGHT, "Solar system", nullptr, nullptr);
	//window = glfwCreateWindow(VIEWPORT_WIDTH, VIEWPORT_HEIGHT, "Solar system", glfwGetPrimaryMonitor(), nullptr);
	if (!window) {glfwTerminate();	return -1;}
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	glewInit();

	//input callbacks
	glfwSetCursorPosCallback(window, onMouseMove);
	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);

	//load all the resources
	load();

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
		draw();
        
        // Swap front and back buffers
        glfwSwapBuffers(window);
        
        // Poll for and process events
        glfwPollEvents();
        
        //mouse position must be tracked constantly (callbacks do not give accurate delta)
        glfwGetCursorPos(window, &mouse_x, &mouse_y);
    }

    glfwTerminate(); //terminate glfw and exit

	destroy();

    return 0;
}


