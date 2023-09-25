// 3D Graphics and Animation - Main Template
// This uses Visual Studio Code - https://code.visualstudio.com/docs/cpp/introvideos-cpp
// Two versions available -  Win64 and Apple MacOS - please see notes
// Last changed August 2022

//#pragma comment(linker, "/NODEFAULTLIB:MSVCRT")

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
using namespace std;

#define GL_SILENCE_DEPRECATION
#include <glad/glad.h> // Extend OpenGL Specfication to version 4.5 for WIN64 and versions 4.1 for Apple (note: two different files).

#include <imgui/imgui.h>			  // Load GUI library - Dear ImGui - https://github.com/ocornut/imgui
#include <imgui/imgui_impl_glfw.h>	  // Platform ImGui using GLFW
#include <imgui/imgui_impl_opengl3.h> // Platform new OpenGL - aka better than 3.3 core version.

#include <GLFW/glfw3.h> // Add GLFW - library to launch a window and callback mouse and keyboard - https://www.glfw.org

#define GLM_ENABLE_EXPERIMENTAL	 // Enable all functions include non LTS
#include <glm/glm.hpp>			 // Add helper maths library - GLM 0.9.9.9 - https://github.com/g-truc/glm - for example variables vec3, mat and operators.
#include <glm/ext.hpp>
#include <glm/gtx/transform.hpp> // Help us with transforms

#include "src/Pipeline.hpp"		// Setup pipeline and load shaders.
#include "src/Content.hpp"		// Setup content loader and drawing functions - https://github.com/KhronosGroup/glTF - https://github.com/syoyo/tinygltf 
#include "src/Debugger.hpp"		// Setup debugger functions.


#include <tinygltf/tiny_gltf.h> // Model loading library - tiny gltf - https://github.com/syoyo/tinygltf
#include "src/stb_image.h"

//#include "src/Mesh.hpp" // Simplest mesh holder and OBJ loader - can update more - from https://github.com/BennyQBD/ModernOpenGLTutorial

// Main fuctions
void startup();
void update();
void render();
void ui();
void endProgram();

// HELPER FUNCTIONS OPENGL
void hintsGLFW();
//string readShader(string name);
//void checkErrorShader(GLuint shader);
inline void errorCallbackGLFW(int error, const char *description){cout << "Error GLFW: " << description << "\n";};
void debugGL();

void APIENTRY openGLDebugCallback(GLenum source,
								  GLenum type,
								  GLuint id,
								  GLenum severity,
								  GLsizei length,
								  const GLchar *message,
								  const GLvoid *userParam);
GLenum glCheckError_(const char *file, int line);
#define glCheckError() glCheckError_(__FILE__, __LINE__)

// Setup all the message loop callbacks - do this before Dear ImGui
// Callback functions for the window and interface devices
void onResizeCallback(GLFWwindow *window, int w, int h);
void onKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
void onMouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
void onMouseMoveCallback(GLFWwindow *window, double x, double y);
void onMouseWheelCallback(GLFWwindow *window, double xoffset, double yoffset);

// VARIABLES
GLFWwindow *window; 								// Keep track of the window
auto windowWidth = 800;								// Window width					
auto windowHeight = 800;								// Window height
auto running(true);							  		// Are we still running our main loop
glm::mat4 projMatrix;							 		// Our Projection Matrix
glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 5.0f);		// Where is our camera
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);			// Camera front vector
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);		
		// Camera up vector
glm::vec3 ia = glm::vec3(0.3f, 0.2f, 0.6f);  //Ambient color
float ka = 0.3f; //Ambient strenght

glm::vec3 is = glm::vec3(0.0f, 0.0f, 0.6f); //Specular color
float ks = 0.3f; //Specular strenght

const int LIGHTS = 4;
glm::vec3 lightDisp[4];  //Diffuse lights positions
glm::vec3 id[4];	//Diffuse lights strenghts
float kd = 1.0f; //Diffuse strenght

glm::vec3 torchColor = glm::vec3(0.5f, 0.5f, 0.5f); 

GLfloat pitch = 0.0f;	
GLfloat yaw = -90.0f;	
GLfloat lastX = windowWidth / 2.0f;
GLfloat lastY = windowHeight / 2.0f;	
bool firstMouse = true;

auto aspect = (float)windowWidth / (float)windowHeight;	// Window aspect ration
auto fovy = 45.0f;									// Field of view (y axis)
bool keyStatus[1024];								// Track key strokes
auto currentTime = 0.0f;							// Framerate
auto deltaTime = 0.0f;								// time passed
auto lastTime = 0.0f;								// Used to calculate Frame rate

Pipeline pipeline;									// Add one pipeline plus some shaders.
Pipeline framebufferpipeline;
Pipeline skyboxpipeline;
Pipeline housepipeline;
Content contentbarrel;									// Add one content loader (+drawing).
Content contentsword;
Content contentcrate;
Content contentlamp;
Content contentpersonal;
Debugger debugger;									// Add one debugger to use for callbacks ( Win64 - openGLDebugCallback() ) or manual calls ( Apple - glCheckError() ) 

glm::vec3 modelPosition;									// Model position
glm::vec3 modelRotation;									// Model rotation

//Framebuffer
GLuint framebuffer;
GLuint framebufferTexture;
GLuint depthbuffer;
GLuint displayVao;
GLuint groundvao;
GLuint skyboxVao;
GLuint skyboxVbo;
GLuint skyboxEbo;
GLuint groundbuffer[2];
GLuint displayBuffer[2];

float torchlightSpotCutOff = 12.5f;
float torchlightSpotOuterCutOff = 15.0f;

static const GLfloat displayVertices[] =
{
	-3.0f, 1.0f,
	-3.0f,-3.0f,
	1.0f,-3.0f,
	-3.0f, 1.0f,
	1.0f,-3.0f,
	1.0f, 1.0f
};

static const GLfloat displayUvs[] =
{ 
	-1.0f, 1.0f,
	-1.0f, -1.0f,
	1.0f, -1.0f,
	-1.0f, 1.0f,
	1.0f, -1.0f,
	1.0f, 1.0f
};

GLuint cubemapTexture;

bool inversecolor = false;
bool alreadypressed = false;
bool alreadypressedtorch = false;
bool torchoff = false;

bool opendoor = false;
bool opendooranimation = false;
bool closedooranimation = false;


std::string facesCubemap[6] =
{
	"assets/skycube2_bmp/skyrender0001.bmp",
	"assets/skycube2_bmp/skyrender0002.bmp",
	"assets/skycube2_bmp/skyrender0003.bmp",
	"assets/skycube2_bmp/skyrender0004.bmp",
	"assets/skycube2_bmp/skyrender0005.bmp",
	"assets/skycube2_bmp/skyrender0006.bmp",
};

std::string facesCubemaptga[6] =
{
	"assets/skycube2_tga/skyrender0001.tga",
	"assets/skycube2_tga/skyrender0002.tga",
	"assets/skycube2_tga/skyrender0003.tga",
	"assets/skycube2_tga/skyrender0004.tga",
	"assets/skycube2_tga/skyrender0005.tga",
	"assets/skycube2_tga/skyrender0006.tga",
};


static const GLfloat vertex_positions[] =
{
-0.5f, 0.0f, -0.5f,
-0.5f, -0.25f, -0.5f,
0.5f, -0.25f, -0.5f,

0.5f, -0.25f, -0.5f,
0.5f, 0.0f, -0.5f,
-0.5f, 0.0f, -0.5f,

0.5f, -0.25f, -0.5f,
0.5f, -0.25f, 0.5f,
0.5f, 0.0f, -0.5f,

0.5f, -0.25f, 0.5f,
0.5f, 0.0f, 0.5f,
0.5f, 0.0f, -0.5f,

0.5f, -0.25f, 0.5f,
-0.5f, -0.25f, 0.5f,
0.5f, 0.0f, 0.5f,

-0.5f, -0.25f, 0.5f,
-0.5f, 0.0f, 0.5f,
0.5f, 0.0f, 0.5f,

-0.5f, -0.25f, 0.5f,
-0.5f, -0.25f, -0.5f,
-0.5f, 0.0f, 0.5f,

-0.5f, -0.25f, -0.5f,
-0.5f, 0.0f, -0.5f,
-0.5f, 0.0f, 0.5f,

-0.5f, -0.25f, 0.5f,
0.5f, -0.25f, 0.5f,
0.5f, -0.25f, -0.5f,

0.5f, -0.25f, -0.5f,
-0.5f, -0.25f, -0.5f,
-0.5f, -0.25f, 0.5f,

-0.5f, 0.0f, -0.5f,
0.5f, 0.0f, -0.5f,
0.5f, 0.0f, 0.5f,

0.5f, 0.0f, 0.5f,
-0.5f, 0.0f, 0.5f,
-0.5f, 0.0f, -0.5f
};

static const GLfloat vertex_colours[] =
{
0.5f, 0.5f, 0.5f,
0.5f, 0.5f, 0.5f,
0.5f, 0.5f, 0.5f,

0.5f, 0.5f, 0.5f,
0.5f, 0.5f, 0.5f,
0.5f, 0.5f, 0.5f,

0.5f, 0.5f, 0.5f,
0.5f, 0.5f, 0.5f,
0.5f, 0.5f, 0.5f,

0.5f, 0.5f, 0.5f,
0.5f, 0.5f, 0.5f,
0.5f, 0.5f, 0.5f,

0.5f, 0.5f, 0.5f,
0.5f, 0.5f, 0.5f,
0.5f, 0.5f, 0.5f,

0.5f, 0.5f, 0.5f,
0.5f, 0.5f, 0.5f,
0.5f, 0.5f, 0.5f,

0.5f, 0.5f, 0.5f,
0.5f, 0.5f, 0.5f,
0.5f, 0.5f, 0.5f,

0.5f, 0.5f, 0.5f,
0.5f, 0.5f, 0.5f,
0.5f, 0.5f, 0.5f,

0.5f, 0.5f, 0.5f,
0.5f, 0.5f, 0.5f,
0.5f, 0.5f, 0.5f,

0.5f, 0.5f, 0.5f,
0.5f, 0.5f, 0.5f,
0.5f, 0.5f, 0.5f,

0.5f, 0.5f, 0.5f,
0.5f, 0.5f, 0.5f,
0.5f, 0.5f, 0.5f,

0.5f, 0.5f, 0.5f,
0.5f, 0.5f, 0.5f,
0.5f, 0.5f, 0.5f
};

static const GLfloat skyboxVertices[] =
{
	-1.0f,-1.0f,-1.0f, // triangle 1 : begin
    -1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f, // triangle 1 : end
    1.0f, 1.0f,-1.0f, // triangle 2 : begin
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f, // triangle 2 : end
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f
};

const unsigned int number = 6;

// Holds all transformations for the walls


glm::vec3 TranslationList[number] =
{
	glm::vec3(0.0f, 0.0f, 0.0f), //Floor
	glm::vec3(0.0f, 3.25f, 0.0f), //Roof
	glm::vec3(0.0f, 1.5f, -2.25f), //Back wall
	glm::vec3(-1.5f, 1.5f, 0.0f), //Left side wall
	glm::vec3(1.25f, 1.5f, 0.0f), //Right side wall 1
	glm::vec3(0.0f, 1.5f, 2.75f) //Front door
};

glm::vec3 RotationList[number] =
{
	glm::vec3(0.0f, 0.0f, 0.0f), //Floor
	glm::vec3(0.0f, 0.0f, 0.0f), //Roof
	glm::vec3(glm::radians(90.0f), 0.0f, 0.0f), //Back wall
	glm::vec3(0.0f, 0.0f, glm::radians(90.0f)), //Left side wall
	glm::vec3(0.0f, 0.0f, glm::radians(90.0f)),
	glm::vec3(glm::radians(90.0f), 0.0f, 0.0f) //Front door
};

glm::vec3 ScaleList[number] =
{
	glm::vec3(3.0f, 1.0f, 5.0f),
	glm::vec3(3.0f, 1.0f, 5.0f),
	glm::vec3(2.75f, 1.0f, 3.0f),
    glm::vec3(3.0f, 1.0f, 5.0f),
	glm::vec3(3.0f, 1.0f, 5.0f),
	glm::vec3(3.0f, 1.0f, 3.0f)
};

int main()
{
	cout << endl << "===" << endl << "3D Graphics and Animation - Running..." << endl;

	if (!glfwInit()) // Check if GLFW is working, if not then stop all
	{
		cout << "Could not initialise GLFW...";
		return -1;
	} 

	glfwSetErrorCallback(errorCallbackGLFW); // Setup a function callback to catch and display all GLFW errors.
	hintsGLFW();							 // Setup glfw with various hints.

	const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor()); // Grab reference to monitor
	windowWidth = mode->width; windowHeight = mode->height; //fullscreen

	// Create our Window
	const auto windowTitle = "My 3D Graphics and Animation OpenGL Application"s;
	//window = glfwCreateWindow(windowWidth, windowHeight, windowTitle.c_str(), NULL, NULL);
	// Full screen
	window = glfwCreateWindow(windowWidth, windowHeight, windowTitle.c_str(), glfwGetPrimaryMonitor(), NULL);

	if (!window) // Test if Window or OpenGL context creation failed
	{
		cout << "Could not initialise GLFW...";
		glfwTerminate();
		return -1;
	} 

	glfwSetWindowPos(window, 10, 10); // Place it in top corner for easy debugging.
	glfwMakeContextCurrent(window);	  // making the OpenGL context current

	// GLAD: Load OpenGL function pointers - aka update specs to newest versions - plus test for errors.
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD...";
		glfwMakeContextCurrent(NULL);
		glfwTerminate();
		return -1;
	}

	glfwSetWindowSizeCallback(window, onResizeCallback);	   // Set callback for resize
	glfwSetKeyCallback(window, onKeyCallback);				   // Set Callback for keys
	glfwSetMouseButtonCallback(window, onMouseButtonCallback); // Set callback for mouse click
	glfwSetCursorPosCallback(window, onMouseMoveCallback);	   // Set callback for mouse move
	glfwSetScrollCallback(window, onMouseWheelCallback);	   // Set callback for mouse wheel.
	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);	// Set mouse cursor Fullscreen
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	// Set mouse cursor FPS fullscreen.

	// Setup Dear ImGui and add context	-	https://blog.conan.io/2019/06/26/An-introduction-to-the-Dear-ImGui-library.html
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO(); //(void)io;
								  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	// io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	ImGui::StyleColorsLight(); // ImGui::StyleColorsDark(); 		// Setup Dear ImGui style

	// Setup Platform/Renderer ImGui backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	const auto glsl_version = "#version 410";
	ImGui_ImplOpenGL3_Init(glsl_version);

	#if defined(__WIN64__)
		debugGL(); // Setup callback to catch openGL errors.	V4.2 or newer
	#elif(__APPLE__)
		glCheckError(); // Old way of checking for errors. Newest not implemented by Apple. Manually call function anywhere in code to check for errors.
	#endif

	glfwSwapInterval(1);			 // Ony render when synced (V SYNC) - https://www.tomsguide.com/features/what-is-vsync-and-should-you-turn-it-on-or-off
	glfwWindowHint(GLFW_SAMPLES, 2); // Multisampling - covered in lectures - https://www.khronos.org/opengl/wiki/Multisampling

	startup(); // Setup all necessary information for startup (aka. load texture, shaders, models, etc).

	cout << endl << "Starting main loop and rendering..." << endl;	

	do{											 // run until the window is closed
		auto currentTime = (float)glfwGetTime(); // retrieve timelapse
		deltaTime = currentTime - lastTime;		 // Calculate delta time
		lastTime = currentTime;					 // Save for next frame calculations.

		glfwPollEvents(); 						// poll callbacks

		update(); 								// update (physics, animation, structures, etc)
		render(); 								// call render function.
		ui();									// call function to render ui.

		#if defined(__APPLE__)
			glCheckError();				// Manually checking for errors for MacOS, Windows has a callback.
		#endif

		glfwSwapBuffers(window); 		// swap buffers (avoid flickering and tearing)

		running &= (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE); // exit if escape key pressed
 		running &= (glfwWindowShouldClose(window) != GL_TRUE);
	} while (running);

	endProgram(); // Close and clean everything up...

	// cout << "\nPress any key to continue...\n";
	// cin.ignore(); cin.get(); // delay closing console to read debugging errors.

	return 0;
}


void hintsGLFW(){
	
	auto majorVersion = 3; auto minorVersion = 3; // define OpenGL version - at least 3.3 for bare basic NEW OpenGL

	#if defined(__WIN64__)	
		majorVersion = 4; minorVersion = 5;					// Recommended for Windows 4.5, but latest is 4.6 (not all drivers support 4.6 version).
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE); // Create context in debug mode - for debug message callback
	#elif(__APPLE__)
		majorVersion = 4; minorVersion = 1; 				// Max support for OpenGL in MacOS
	#endif

	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 	// https://www.glfw.org/docs/3.3/window_guide.html
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, majorVersion); 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minorVersion);
}

void endProgram()
{
	// Clean ImGui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwMakeContextCurrent(NULL); 	// destroys window handler
	glfwTerminate();				// destroys all windows and releases resources.
}

void startup()
{
	lightDisp[0] = glm::vec3(1.25f, 2.0f, -1.0f);
	lightDisp[1] = glm::vec3(1.25f, 2.0f, 1.0f);
	lightDisp[2] = glm::vec3(-1.0f, 2.0f, 1.0f);
	lightDisp[3] = glm::vec3(1.25f, 2.0f, 0.0f);

	id[0] = glm::vec3(1.0f, 0.5f, 0.0f);
	id[1] = glm::vec3(1.0f, 0.5f, 0.0f);
	id[2] = glm::vec3(1.0f, 0.0f, 1.0f);
	id[3] = glm::vec3(1.0f, 0.2f, 1.0f);


	// Output some debugging information
	cout << "VENDOR: " << (char *)glGetString(GL_VENDOR) << endl;
	cout << "VERSION: " << (char *)glGetString(GL_VERSION) << endl;
	cout << "RENDERER: " << (char *)glGetString(GL_RENDERER) << endl;	


	// Start from the centre
	modelPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	modelRotation = glm::vec3(0.0f, 0.0f, 0.0f);

	// A few optimizations.
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	pipeline.CreatePipeline();
	pipeline.LoadShaders("shaders/vs_model.glsl","shaders/fs_model.glsl");

	//load shaders for framebuffer displays
	framebufferpipeline.CreatePipeline();
	framebufferpipeline.LoadShaders("shaders/vs_display.glsl","shaders/fs_display.glsl");

	//load shaders for skybox
	skyboxpipeline.CreatePipeline();
	skyboxpipeline.LoadShaders("shaders/vs_skybox.glsl","shaders/fs_skybox.glsl");

	//load shaders for skybox
	housepipeline.CreatePipeline();
	housepipeline.LoadShaders("shaders/vs_house.glsl","shaders/fs_model.glsl");


	// Get the correct size in pixels - E.g. if retina display or monitor scaling
	glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

	// ---------- Ground -----------------------------
	glGenVertexArrays(1, &groundvao);
	glBindVertexArray(groundvao);
	glGenBuffers(2, groundbuffer);

	glBindBuffer(GL_ARRAY_BUFFER, groundbuffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_positions), vertex_positions, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, groundbuffer[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_colours), vertex_colours, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);



	// ----------Start Framebuffer Object---------------
 	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	
	// Create a texture for the framebuffer
	glGenTextures(1, &framebufferTexture);
	//glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, framebufferTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);


	// filtering needed 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);



	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferTexture, 0);
 

	// Depth buffer texture	- Need to attach depth too otherwise depth testing will not be performed.
	glGenRenderbuffers(1, &depthbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowWidth, windowHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthbuffer);


	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete" << endl;

	
	glGenVertexArrays(1, &displayVao);		 // Create Vertex Array Object
	glGenBuffers(2, displayBuffer);        // Create a new buffer   
	glBindVertexArray(displayVao);        // Bind VertexArrayObject
	glBindBuffer(GL_ARRAY_BUFFER, displayBuffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(displayVertices), &displayVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); 
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0); // vertices
    
	glBindBuffer(GL_ARRAY_BUFFER, displayBuffer[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(displayUvs), &displayUvs, GL_STATIC_DRAW);
	
    
	glEnableVertexAttribArray(1); 
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0); //uv

	glBindVertexArray(0);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Frambufferrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr
	
	// Skybox setup

	glEnable(GL_TEXTURE_CUBE_MAP);

	glGenVertexArrays(1, &skyboxVao);
	glGenBuffers(1, &skyboxVbo);
	glBindVertexArray(skyboxVao);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), NULL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//glEnable(GL_TEXTURE_CUBE_MAP);
	glGenTextures(1, &cubemapTexture);
	//glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	int iwidth, iheight, inrChannels;
  	for(unsigned int i = 0; i < 6; i++)
	{
		unsigned char* data = stbi_load(facesCubemap[i].c_str(), &iwidth, &iheight, &inrChannels, 0);
		if(data)
		{
			stbi_set_flip_vertically_on_load(false);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, iwidth, iheight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			cout << "Failed to load skybox texture: " << facesCubemap[i] << endl;
			stbi_image_free(data);
		}
	}  

	glBindTexture(GL_TEXTURE_2D, 0);

	// ----------GLTF assets------------


	cout << endl << "Loading content..." << endl;	
	contentbarrel.LoadGLTF("assets/western_barrel/scene.gltf",1);

	cout << endl << "Loading content..." << endl;	
	contentcrate.LoadGLTF("assets/wooden_crate/scene.gltf",2);
	
	cout << endl << "Loading content..." << endl;	
	contentsword.LoadGLTF("assets/medieval_sword/scene.gltf",3);

	cout << endl << "Loading content..." << endl;	
	contentlamp.LoadGLTF("assets/industrial_bulkhead_wall_lamp_lowpoly/scene.gltf",4);

	//cout << endl << "Loading content..." << endl;	
	//contentpersonal.LoadGLTF("assets/Personal_render/Render.gltf",5);

	// Calculate proj_matrix for the first time.
	aspect = (float)windowWidth / (float)windowHeight;
	projMatrix = glm::perspective(glm::radians(fovy), aspect, 0.1f, 1000.0f);
}

void update()
{
	//Dog control
	if (keyStatus[GLFW_KEY_LEFT]) modelRotation.y += 0.05f;
	if (keyStatus[GLFW_KEY_RIGHT]) modelRotation.y -= 0.05f;
	if (keyStatus[GLFW_KEY_UP]) modelRotation.x += 0.05f;
	if (keyStatus[GLFW_KEY_DOWN]) modelRotation.x -= 0.05f;

	if (keyStatus[GLFW_KEY_Z] && torchlightSpotOuterCutOff <= 25.0f){torchlightSpotCutOff += 0.1f; torchlightSpotOuterCutOff += 0.1f;}
	if (keyStatus[GLFW_KEY_X] && torchlightSpotOuterCutOff >= 7.0f){torchlightSpotCutOff -= 0.1f; torchlightSpotOuterCutOff -= 0.1f;}

	if (keyStatus[GLFW_KEY_Q] && alreadypressed == false) inversecolor = !inversecolor; alreadypressed = true;

	if (keyStatus[GLFW_KEY_F] && alreadypressedtorch == false) torchoff = !torchoff; alreadypressedtorch = true;

	if (keyStatus[GLFW_KEY_E] && opendoor == false &&  opendooranimation == false) opendooranimation = true;
	if (keyStatus[GLFW_KEY_E] && opendoor == true  && closedooranimation == false) closedooranimation = true;

	if (not(keyStatus[GLFW_KEY_Q])) alreadypressed = false;
	if (not(keyStatus[GLFW_KEY_F])) alreadypressedtorch = false;

	if(closedooranimation == true){
		TranslationList[5].x += 0.02f;
		if(TranslationList[5].x >= 0.0f){
			closedooranimation = false;
			opendoor = false;
		}
	}

	if(opendooranimation == true){
		TranslationList[5].x -= 0.02f;
		if(TranslationList[5].x <= -2.0f){
			opendooranimation = false;
			opendoor = true;
		}
	}

	//Camera control

	GLfloat cameraSpeed = 1.0f * deltaTime;
	if (keyStatus[GLFW_KEY_W]) cameraPosition += cameraSpeed * cameraFront;
	if (keyStatus[GLFW_KEY_S]) cameraPosition -= cameraSpeed * cameraFront;
	if (keyStatus[GLFW_KEY_A]) cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (keyStatus[GLFW_KEY_D]) cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

	cameraPosition.y = 1.75f;

	//Reload
	if (keyStatus[GLFW_KEY_R]) pipeline.ReloadShaders();

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void render()
{
	//==============First Pass====================

	//---Render framebuffer to texture
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	// Clear colour buffer
	glClearColor(0.4f, 0.1f, 0.7f, 0.2f);


	// Clear deep buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Enable blend
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//-----Sky box

	glDepthFunc(GL_LEQUAL);
	glDisable(GL_DEPTH_TEST); 

	glUseProgram(skyboxpipeline.pipe.program);


	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);
	view = glm::mat4(glm::mat3(glm::lookAt(cameraPosition,cameraPosition+cameraFront,cameraUp)));
	projection = glm::perspective(glm::radians(45.0f), (float)windowWidth/windowHeight, 0.1f, 300.0f);
	glUniformMatrix4fv(glGetUniformLocation(skyboxpipeline.pipe.program,"view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(skyboxpipeline.pipe.program,"projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniform1i(glGetUniformLocation(skyboxpipeline.pipe.program, "skybox"), 0);
	
	glBindVertexArray(skyboxVao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	glEnable(GL_DEPTH_TEST); 
	glDepthFunc(GL_LESS);
	
	// Use our shader programs

	glUseProgram(pipeline.pipe.program);

	// Setup camera
	glm::mat4 viewMatrix = glm::lookAt(cameraPosition,				 // eye
									   cameraPosition + cameraFront, // centre
									   cameraUp);					 // up

	// Do some translations, rotations and scaling

	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.2f, 1.0f, -0.7f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(105.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));
	

	glm::mat4 mv_matrix = viewMatrix * modelMatrix;

	glUniform1i(glGetUniformLocation(pipeline.pipe.program, "tex"), 2);

	glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "model_matrix"), 1, GL_FALSE, &modelMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "proj_matrix"), 1, GL_FALSE, &projMatrix[0][0]);


	for(int l = 0; l < LIGHTS; l++){
		glUniform4f(glGetUniformLocation(pipeline.pipe.program, ("lights["+to_string(l)+"].lightPosition").c_str()), lightDisp[l].x, lightDisp[l].y, lightDisp[l].z, 1.0f);
		glUniform4f(glGetUniformLocation(pipeline.pipe.program, ("lights["+to_string(l)+"].id").c_str()), id[l].r, id[l].g, id[l].b, 1.0f);
	}

	glUniform4f(glGetUniformLocation(pipeline.pipe.program, "torchlightPosition"), cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0);
	glUniform4f(glGetUniformLocation(pipeline.pipe.program, "torchlightSpotDirection"), cameraFront.x, cameraFront.y, cameraFront.z, 0.0);
	glUniform4f(glGetUniformLocation(pipeline.pipe.program, "torchColor"), torchColor.r,  torchColor.g,  torchColor.b, 0.0);
	if(torchoff){
		glUniform4f(glGetUniformLocation(pipeline.pipe.program, "torchColor"), 0.0, 0.0, 0.0, 0.0);
	}
	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "torchlightSpotCutOff"), glm::cos(glm::radians(torchlightSpotCutOff)));
	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "torchlightSpotOuterCutOff"), glm::cos(glm::radians(torchlightSpotOuterCutOff)));

	glUniform4f(glGetUniformLocation(pipeline.pipe.program, "viewPosition"), cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0f);
	glUniform4f(glGetUniformLocation(pipeline.pipe.program, "ia"), ia.r, ia.g, ia.b, 1.0f);
	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "ka"), ka);
	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "kd"), kd);
	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "ks"), ks);
	glUniform4f(glGetUniformLocation(pipeline.pipe.program, "is"), is.r, is.g, is.b, 1.0f);
	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "shininess"), 32.0f);

	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "lightConstant"), 1.0f);
	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "lightLinear"), 0.7f);
	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "lightQuadratic"), 1.8f);

	contentsword.DrawModel(contentsword.vaoAndEbos,contentsword.model);

	glUseProgram(pipeline.pipe.program);


	// Do some translations, rotations and scaling

	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-0.3f, 0.25f, -0.8f));
	modelMatrix = glm::rotate(modelMatrix, 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.4f, 0.4f, 0.4f));
	

	mv_matrix = viewMatrix * modelMatrix;

	glUniform1i(glGetUniformLocation(pipeline.pipe.program, "tex"), 2);

	glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "model_matrix"), 1, GL_FALSE, &modelMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "proj_matrix"), 1, GL_FALSE, &projMatrix[0][0]);


	for(int l = 0; l < LIGHTS; l++){
		glUniform4f(glGetUniformLocation(pipeline.pipe.program, ("lights["+to_string(l)+"].lightPosition").c_str()), lightDisp[l].x, lightDisp[l].y, lightDisp[l].z, 1.0f);
		glUniform4f(glGetUniformLocation(pipeline.pipe.program, ("lights["+to_string(l)+"].id").c_str()), id[l].r, id[l].g, id[l].b, 1.0f);
	}

	glUniform4f(glGetUniformLocation(pipeline.pipe.program, "torchlightPosition"), cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0);
	glUniform4f(glGetUniformLocation(pipeline.pipe.program, "torchlightSpotDirection"), cameraFront.x, cameraFront.y, cameraFront.z, 0.0);
	glUniform4f(glGetUniformLocation(pipeline.pipe.program, "torchColor"), torchColor.r,  torchColor.g,  torchColor.b, 0.0);
	if(torchoff){
		glUniform4f(glGetUniformLocation(pipeline.pipe.program, "torchColor"), 0.0, 0.0, 0.0, 0.0);
	}
	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "torchlightSpotCutOff"), glm::cos(glm::radians(torchlightSpotCutOff)));
	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "torchlightSpotOuterCutOff"), glm::cos(glm::radians(torchlightSpotOuterCutOff)));

	glUniform4f(glGetUniformLocation(pipeline.pipe.program, "viewPosition"), cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0f);
	glUniform4f(glGetUniformLocation(pipeline.pipe.program, "ia"), ia.r, ia.g, ia.b, 1.0f);
	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "ka"), ka);
	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "kd"), kd);
	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "ks"), ks);
	glUniform4f(glGetUniformLocation(pipeline.pipe.program, "is"), is.r, is.g, is.b, 1.0f);
	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "shininess"), 32.0f);

	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "lightConstant"), 1.0f);
	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "lightLinear"), 0.7f);
	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "lightQuadratic"), 1.8f);

	contentcrate.DrawModel(contentcrate.vaoAndEbos,contentcrate.model);

	glUseProgram(pipeline.pipe.program);

	// Do some translations, rotations and scaling

	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.25f, -1.4f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.7f, 0.7f, 0.7f));
	

	mv_matrix = viewMatrix * modelMatrix;

	glUniform1i(glGetUniformLocation(pipeline.pipe.program, "tex"), 4);

	glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "model_matrix"), 1, GL_FALSE, &modelMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "proj_matrix"), 1, GL_FALSE, &projMatrix[0][0]);


	for(int l = 0; l < LIGHTS; l++){
		glUniform4f(glGetUniformLocation(pipeline.pipe.program, ("lights["+to_string(l)+"].lightPosition").c_str()), lightDisp[l].x, lightDisp[l].y, lightDisp[l].z, 1.0f);
		glUniform4f(glGetUniformLocation(pipeline.pipe.program, ("lights["+to_string(l)+"].id").c_str()), id[l].r, id[l].g, id[l].b, 1.0f);
	}

	glUniform4f(glGetUniformLocation(pipeline.pipe.program, "torchlightPosition"), cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0);
	glUniform4f(glGetUniformLocation(pipeline.pipe.program, "torchlightSpotDirection"), cameraFront.x, cameraFront.y, cameraFront.z, 0.0);
	glUniform4f(glGetUniformLocation(pipeline.pipe.program, "torchColor"), torchColor.r,  torchColor.g,  torchColor.b, 0.0);
	if(torchoff){
		glUniform4f(glGetUniformLocation(pipeline.pipe.program, "torchColor"), 0.0, 0.0, 0.0, 0.0);
	}
	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "torchlightSpotCutOff"), glm::cos(glm::radians(torchlightSpotCutOff)));
	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "torchlightSpotOuterCutOff"), glm::cos(glm::radians(torchlightSpotOuterCutOff)));

	glUniform4f(glGetUniformLocation(pipeline.pipe.program, "viewPosition"), cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0f);
	glUniform4f(glGetUniformLocation(pipeline.pipe.program, "ia"), ia.r, ia.g, ia.b, 1.0f);
	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "ka"), ka);
	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "kd"), kd);
	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "ks"), ks);
	glUniform4f(glGetUniformLocation(pipeline.pipe.program, "is"), is.r, is.g, is.b, 1.0f);
	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "shininess"), 32.0f);

	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "lightConstant"), 1.0f);
	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "lightLinear"), 0.7f);
	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "lightQuadratic"), 1.8f);

	contentbarrel.DrawModel(contentbarrel.vaoAndEbos,contentbarrel.model);

	glUseProgram(pipeline.pipe.program);

	// Do some translations, rotations and scaling

/* 	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.75f, -1.4f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.7f, 0.7f, 0.7f));
	

	mv_matrix = viewMatrix * modelMatrix;

	glUniform1i(glGetUniformLocation(pipeline.pipe.program, "tex"), 5);

	glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "model_matrix"), 1, GL_FALSE, &modelMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "proj_matrix"), 1, GL_FALSE, &projMatrix[0][0]);


	for(int l = 0; l < LIGHTS; l++){
		glUniform4f(glGetUniformLocation(pipeline.pipe.program, ("lights["+to_string(l)+"].lightPosition").c_str()), lightDisp[l].x, lightDisp[l].y, lightDisp[l].z, 1.0f);
		glUniform4f(glGetUniformLocation(pipeline.pipe.program, ("lights["+to_string(l)+"].id").c_str()), id[l].r, id[l].g, id[l].b, 1.0f);
	}

	glUniform4f(glGetUniformLocation(pipeline.pipe.program, "torchlightPosition"), cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0);
	glUniform4f(glGetUniformLocation(pipeline.pipe.program, "torchlightSpotDirection"), cameraFront.x, cameraFront.y, cameraFront.z, 0.0);
	glUniform4f(glGetUniformLocation(pipeline.pipe.program, "torchColor"), torchColor.r,  torchColor.g,  torchColor.b, 0.0);
	if(torchoff){
		glUniform4f(glGetUniformLocation(pipeline.pipe.program, "torchColor"), 0.0, 0.0, 0.0, 0.0);
	}
	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "torchlightSpotCutOff"), glm::cos(glm::radians(torchlightSpotCutOff)));
	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "torchlightSpotOuterCutOff"), glm::cos(glm::radians(torchlightSpotOuterCutOff)));

	glUniform4f(glGetUniformLocation(pipeline.pipe.program, "viewPosition"), cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0f);
	glUniform4f(glGetUniformLocation(pipeline.pipe.program, "ia"), ia.r, ia.g, ia.b, 1.0f);
	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "ka"), ka);
	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "kd"), kd);
	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "ks"), ks);
	glUniform4f(glGetUniformLocation(pipeline.pipe.program, "is"), is.r, is.g, is.b, 1.0f);
	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "shininess"), 32.0f);

	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "lightConstant"), 1.0f);
	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "lightLinear"), 0.7f);
	glUniform1f(glGetUniformLocation(pipeline.pipe.program, "lightQuadratic"), 1.8f);

	contentpersonal.DrawModel(contentpersonal.vaoAndEbos,contentpersonal.model);
	*/
	glUseProgram(pipeline.pipe.program); 

	// Do some translations, rotations and scaling

	for( unsigned int i = 0; i < LIGHTS; i++){

		modelMatrix = glm::translate(glm::mat4(1.0f), lightDisp[i]);
		modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		if(i == 2){
			modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		}
		else{
			modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		}
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f, 0.1f, 0.1f));
		
		mv_matrix = viewMatrix * modelMatrix;

		glUniform1i(glGetUniformLocation(pipeline.pipe.program, "tex"), 6);

		glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "model_matrix"), 1, GL_FALSE, &modelMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "proj_matrix"), 1, GL_FALSE, &projMatrix[0][0]);


		for(int l = 0; l < LIGHTS; l++){
			glUniform4f(glGetUniformLocation(pipeline.pipe.program, ("lights["+to_string(l)+"].lightPosition").c_str()), lightDisp[l].x, lightDisp[l].y, lightDisp[l].z, 1.0f);
			glUniform4f(glGetUniformLocation(pipeline.pipe.program, ("lights["+to_string(l)+"].id").c_str()), id[l].r, id[l].g, id[l].b, 1.0f);
		}

		glUniform4f(glGetUniformLocation(pipeline.pipe.program, "torchlightPosition"), cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0);
		glUniform4f(glGetUniformLocation(pipeline.pipe.program, "torchlightSpotDirection"), cameraFront.x, cameraFront.y, cameraFront.z, 0.0);
		glUniform4f(glGetUniformLocation(pipeline.pipe.program, "torchColor"), torchColor.r,  torchColor.g,  torchColor.b, 0.0);
		if(torchoff){
			glUniform4f(glGetUniformLocation(pipeline.pipe.program, "torchColor"), 0.0, 0.0, 0.0, 0.0);
		}
		glUniform1f(glGetUniformLocation(pipeline.pipe.program, "torchlightSpotCutOff"), glm::cos(glm::radians(torchlightSpotCutOff)));
		glUniform1f(glGetUniformLocation(pipeline.pipe.program, "torchlightSpotOuterCutOff"), glm::cos(glm::radians(torchlightSpotOuterCutOff)));

		glUniform4f(glGetUniformLocation(pipeline.pipe.program, "viewPosition"), cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0f);
		glUniform4f(glGetUniformLocation(pipeline.pipe.program, "ia"), ia.r, ia.g, ia.b, 1.0f);
		glUniform1f(glGetUniformLocation(pipeline.pipe.program, "ka"), ka);
		glUniform1f(glGetUniformLocation(pipeline.pipe.program, "kd"), kd);
		glUniform1f(glGetUniformLocation(pipeline.pipe.program, "ks"), ks);
		glUniform4f(glGetUniformLocation(pipeline.pipe.program, "is"), is.r, is.g, is.b, 1.0f);
		glUniform1f(glGetUniformLocation(pipeline.pipe.program, "shininess"), 32.0f);

		glUniform1f(glGetUniformLocation(pipeline.pipe.program, "lightConstant"), 1.0f);
		glUniform1f(glGetUniformLocation(pipeline.pipe.program, "lightLinear"), 0.7f);
		glUniform1f(glGetUniformLocation(pipeline.pipe.program, "lightQuadratic"), 1.8f);

		contentlamp.DrawModel(contentlamp.vaoAndEbos,contentlamp.model);
	}

	for (unsigned int i = 0; i < number; i++){
		glUseProgram(pipeline.pipe.program);

		modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(TranslationList[i].x, TranslationList[i].y, TranslationList[i].z));
		modelMatrix = glm::rotate(modelMatrix, RotationList[i].x, glm::vec3(1.0f, 0.0f, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, RotationList[i].y, glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, RotationList[i].z, glm::vec3(0.0f, 0.0f, 1.0f));
		modelMatrix = glm::scale(modelMatrix, ScaleList[i]);

		mv_matrix = viewMatrix * modelMatrix;

		glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "model_matrix"), 1, GL_FALSE, &modelMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(pipeline.pipe.program, "proj_matrix"), 1, GL_FALSE, &projMatrix[0][0]);


		for(int l = 0; l < LIGHTS; l++){
			glUniform4f(glGetUniformLocation(pipeline.pipe.program, ("lights["+to_string(l)+"].lightPosition").c_str()), lightDisp[l].x, lightDisp[l].y, lightDisp[l].z, 1.0f);
			glUniform4f(glGetUniformLocation(pipeline.pipe.program, ("lights["+to_string(l)+"].id").c_str()), id[l].r, id[l].g, id[l].b, 1.0f);
		}

		glUniform4f(glGetUniformLocation(pipeline.pipe.program, "torchlightPosition"), cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0);
		glUniform4f(glGetUniformLocation(pipeline.pipe.program, "torchlightSpotDirection"), cameraFront.x, cameraFront.y, cameraFront.z, 0.0);
		glUniform4f(glGetUniformLocation(pipeline.pipe.program, "torchColor"), torchColor.r,  torchColor.g,  torchColor.b, 0.0);
		if(torchoff){
			glUniform4f(glGetUniformLocation(pipeline.pipe.program, "torchColor"), 0.0, 0.0, 0.0, 0.0);
		}
		glUniform1f(glGetUniformLocation(pipeline.pipe.program, "torchlightSpotCutOff"), glm::cos(glm::radians(torchlightSpotCutOff)));
		glUniform1f(glGetUniformLocation(pipeline.pipe.program, "torchlightSpotOuterCutOff"), glm::cos(glm::radians(torchlightSpotOuterCutOff)));

		glUniform4f(glGetUniformLocation(pipeline.pipe.program, "viewPosition"), cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0f);
		glUniform4f(glGetUniformLocation(pipeline.pipe.program, "ia"), ia.r, ia.g, ia.b, 1.0f);
		glUniform1f(glGetUniformLocation(pipeline.pipe.program, "ka"), ka);
		glUniform1f(glGetUniformLocation(pipeline.pipe.program, "kd"), kd);
		glUniform1f(glGetUniformLocation(pipeline.pipe.program, "ks"), ks);
		glUniform4f(glGetUniformLocation(pipeline.pipe.program, "is"), is.r, is.g, is.b, 1.0f);
		glUniform1f(glGetUniformLocation(pipeline.pipe.program, "shininess"), 32.0f);

		glUniform1f(glGetUniformLocation(pipeline.pipe.program, "lightConstant"), 1.0f);
		glUniform1f(glGetUniformLocation(pipeline.pipe.program, "lightLinear"), 0.7f);
		glUniform1f(glGetUniformLocation(pipeline.pipe.program, "lightQuadratic"), 1.8f);

		glBindVertexArray(groundvao); 
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	//==============Second Pass===================

 	glBindFramebuffer(GL_FRAMEBUFFER, 0);	// Disable rendering framebuffer to texture - aka render normally.
	glViewport(0,0,windowWidth,windowHeight);

	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST); //not needed as we are just displaying a single quad

	glUseProgram(framebufferpipeline.pipe.program);
	glUniform1f(glGetUniformLocation(framebufferpipeline.pipe.program, "inversecolor"), inversecolor);
	glUniform1f(glGetUniformLocation(framebufferpipeline.pipe.program, "width"), windowWidth);
	glUniform1f(glGetUniformLocation(framebufferpipeline.pipe.program, "height"), windowHeight);
	glUniform1i(glGetUniformLocation(framebufferpipeline.pipe.program, "screenTexture"), 2);

	glBindVertexArray(displayVao);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, framebufferTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0); 
	glBindTexture(GL_TEXTURE_2D, 0);

	glEnable(GL_DEPTH_TEST); 

	

	#if defined(__APPLE__)
		glCheckError();
	#endif
}

void ui()
{


	ImGuiIO &io = ImGui::GetIO();
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration; 
	window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
	window_flags |= ImGuiWindowFlags_NoSavedSettings; 
	window_flags |= ImGuiWindowFlags_NoFocusOnAppearing; 
	window_flags |= ImGuiWindowFlags_NoNav;

	const auto PAD = 10.0f;
	const ImGuiViewport *viewport = ImGui::GetMainViewport();
	ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
	ImVec2 work_size = viewport->WorkSize;
	ImVec2 window_pos, window_pos_pivot;
	window_pos.x = work_pos.x + work_size.x - PAD;
	window_pos.y = work_pos.y + work_size.y - PAD;
	window_pos_pivot.x = 1.0f;
	window_pos_pivot.y = 1.0f;

	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
	window_flags |= ImGuiWindowFlags_NoMove;

	ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
	bool *p_open = NULL;
	if (ImGui::Begin("Info", nullptr, window_flags)) {
		ImGui::Text("About: 3D Graphics and Animation 2022"); // ImGui::Separator();
		ImGui::Text("Performance: %.3fms/Frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Pipeline: %s", pipeline.pipe.error?"ERROR":"OK");
	}
	ImGui::End();

	// Rendering imgui
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void onResizeCallback(GLFWwindow *window, int w, int h)
{
	windowWidth = w;
	windowHeight = h;

	// Get the correct size in pixels
	glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

	if (windowWidth > 0 && windowHeight > 0)
	{ // Avoid issues when minimising window - it gives size of 0 which fails division.
		aspect = (float)w / (float)h;
		projMatrix = glm::perspective(glm::radians(fovy), aspect, 0.1f, 1000.0f);
	}
}

void onKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
		keyStatus[key] = true;
	else if (action == GLFW_RELEASE)
		keyStatus[key] = false;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void onMouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
}

void onMouseMoveCallback(GLFWwindow *window, double x, double y)
{
	int mouseX = static_cast<int>(x);
	int mouseY = static_cast<int>(y);

	if (firstMouse) {
		lastX = (float)mouseX; lastY = (float)mouseY; firstMouse = false;
	}

	GLfloat xoffset = mouseX - lastX;
	//GLfloat yoffset = mouseY - lastY; // Reversed
	GLfloat yoffset = lastY -  mouseY; // Not reversed
	lastX = mouseX; 
	lastY = mouseY;
	GLfloat sensitivity = 0.05;
	xoffset *= sensitivity; 
	yoffset *= sensitivity;
	yaw += xoffset; 
	pitch += yoffset;
	// check for pitch out of bounds otherwise screen gets flipped
	if (pitch > 89.0f) pitch = 89.0f; 
	if (pitch < -89.0f) pitch = -89.0f;
	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void onMouseWheelCallback(GLFWwindow *window, double xoffset, double yoffset)
{
	int yoffsetInt = static_cast<int>(yoffset);
}

void APIENTRY openGLDebugCallback(GLenum source,
								  GLenum type,
								  GLuint id,
								  GLenum severity,
								  GLsizei length,
								  const GLchar *message,
								  const GLvoid *userParam)  // Debugger callback for Win64 - OpenGL versions 4.2 or better.
{
	debugger.OpenGLDebugCallback(source, type, id, severity, length, message, userParam);
}

void debugGL() // Debugger function for Win64 - OpenGL versions 4.2 or better.
{
	// Enable Opengl Debug
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback((GLDEBUGPROC)openGLDebugCallback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true);
}

GLenum glCheckError_(const char *file, int line) // Debugger manual function for Apple - OpenGL versions 3.3 to 4.1.
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR) debugger.GlGetError(errorCode, file, line);

	return errorCode;
}
