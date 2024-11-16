/*
* 
* 09 - Animación
*/

#include <iostream>
#include <stdlib.h>

// GLAD: Multi-Language GL/GLES/EGL/GLX/WGL Loader-Generator
// https://glad.dav1d.de/
#include <glad/glad.h>

// GLFW: https://www.glfw.org/
#include <GLFW/glfw3.h>

// GLM: OpenGL Math library
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Model loading classes
#include <shader_m.h>
#include <camera.h>
#include <model.h>
#include <animatedmodel.h>
#include <material.h>
#include <light.h>
#include <cubemap.h>

#include <irrKlang.h>
using namespace irrklang;

// Functions
bool Start();
bool Update();

// Definición de callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// Gobals
GLFWwindow* window;

// Tamaño en pixeles de la ventana
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;

// Definición de cámara (posición en XYZ)
Camera camera(glm::vec3(0.0f, 2.0f, 10.0f));
Camera camera3rd(glm::vec3(0.0f, 0.0f, 0.0f));

// Controladores para el movimiento del mouse
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Variables para la velocidad de reproducción
// de la animación
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float elapsedTime = 0.0f;

glm::vec3 position(0.0f,0.0f, 0.0f);
glm::vec3 forwardView(0.0f, 0.0f, 1.0f);
float     trdpersonOffset = 1.5f;
float     scaleV = 0.025f;
float     rotateCharacter = 0.0f;
float	  door_offset = 0.0f;
float	  door_rotation = 0.0f;

// Shaders
Shader *mLightsShader;
Shader *proceduralShader;
Shader *staticShader;
Shader *wavesShader;

Shader *cubemapShader;
Shader *dynamicShader;

// Carga la información del modelo
Model	*house;
Model   *door;
Model   *moon;
Model   *gridMesh;


//modelos estaticos
Model* isla; 
Model* isla12; 
Model* isla13; 
Model* arena4; 
Model* plano; 
Model* estacionamiento; 
Model* edificio; 
Model* edificio2; 
Model* muro; 
Model* edificio3; 
Model* visor;
Model* carpa; 
Model* mesa; 
Model* lata; 
Model* barco; 

// Modelos animados
AnimatedModel* character01;
AnimatedModel* baile; 
AnimatedModel* baile2; 
AnimatedModel* baile3; 

float tradius = 10.0f;
float theta = 0.0f;
float alpha = 0.0f;

// Cubemap
CubeMap *mainCubeMap;

// Light gLight;
std::vector<Light> gLights;

// Materiales
Material material01;

float proceduralTime = 0.0f;
float wavesTime = 0.0f;

// Audio
ISoundEngine *SoundEngine = createIrrKlangDevice();

// selección de cámara
bool    activeCamera = 1; // activamos la primera cámara

// Entrada a función principal
int main()
{
	if (!Start())
		return -1;

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		if (!Update())
			break;
	}

	glfwTerminate();
	return 0;

}

bool Start() {
	// Inicialización de GLFW

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Creación de la ventana con GLFW
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Animation", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// Ocultar el cursor mientras se rota la escena
	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: Cargar todos los apuntadores
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}

	// Activación de buffer de profundidad
	glEnable(GL_DEPTH_TEST);

	// Compilación y enlace de shaders
	mLightsShader = new Shader("shaders/11_PhongShaderMultLights.vs", "shaders/11_PhongShaderMultLights.fs");
	proceduralShader = new Shader("shaders/12_ProceduralAnimation.vs", "shaders/12_ProceduralAnimation.fs");
	wavesShader = new Shader("shaders/13_wavesAnimation.vs", "shaders/13_wavesAnimation.fs");
	cubemapShader = new Shader("shaders/10_vertex_cubemap.vs", "shaders/10_fragment_cubemap.fs");
	dynamicShader = new Shader("shaders/10_vertex_skinning-IT.vs", "shaders/10_fragment_skinning-IT.fs");
	staticShader = new Shader("shaders/10_vertex_simple.vs", "shaders/10_fragment_simple.fs");

	// Máximo número de huesos: 100
	dynamicShader->setBonesIDs(MAX_RIGGING_BONES);

	// Dibujar en malla de alambre
	// glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

	house = new Model("models/IllumModels/House03.fbx");
	door = new Model("models/IllumModels/Door.fbx");
	moon = new Model("models/IllumModels/moon.fbx");
	gridMesh = new Model("models/IllumModels/grid.fbx");

	//modelos estaticos 
	isla = new Model("models/isla11.fbx");
	isla12 = new Model("models/arena3.fbx");
	isla13 = new Model("models/arena2.fbx");
	arena4 = new Model("models/arena4.fbx"); 
	plano = new Model("models/calle.fbx"); 
	estacionamiento = new Model("models/estacionamiento.fbx");
	edificio = new Model("models/city3.fbx"); 
	edificio2 = new Model("models/city2.fbx");
	muro = new Model("models/muro.fbx");
	edificio3 = new Model("models/city4.fbx"); 
	visor = new Model("models/visor.fbx");
	carpa = new Model("models/carpa2.fbx"); 
	mesa = new Model("models/mesa777.fbx"); 
	lata = new Model("models/lata.fbx"); 
	barco = new Model("models/barco3.fbx"); 

	//animaciones 
	character01 = new AnimatedModel("models/IllumModels/KAYA.fbx");
	baile = new AnimatedModel("models/IllumModels/baile.fbx"); 
	baile2 = new AnimatedModel("models/baile2.fbx"); 
	baile3 = new AnimatedModel("models/baile4.fbx",2); 

	// Cubemap
	vector<std::string> faces
	{
		"textures/cubemap/01/posx.jpg",
		"textures/cubemap/01/negx.jpg",
		"textures/cubemap/01/posy.jpg",
		"textures/cubemap/01/negy.jpg",
		"textures/cubemap/01/posz.jpg",
		"textures/cubemap/01/negz.jpg"
	};
	mainCubeMap = new CubeMap();
	mainCubeMap->loadCubemap(faces);

	camera3rd.Position = position;
	camera3rd.Position.y += 1.7f;
	camera3rd.Position -= trdpersonOffset * forwardView;
	camera3rd.Front = forwardView;

	// Lights configuration
	
	Light light01;
	light01.Position = glm::vec3(5.0f, 2.0f, 5.0f);
	light01.Color = glm::vec4(0.2f, 0.0f, 0.0f, 1.0f);
	gLights.push_back(light01);

	Light light02;
	light02.Position = glm::vec3(-5.0f, 2.0f, 5.0f);
	light02.Color = glm::vec4(0.0f, 0.2f, 0.0f, 1.0f);
	gLights.push_back(light02);

	Light light03;
	light03.Position = glm::vec3(5.0f, 2.0f, -5.0f);
	light03.Color = glm::vec4(0.0f, 0.0f, 0.2f, 1.0f);
	gLights.push_back(light03);

	Light light04;
	light04.Position = glm::vec3(-5.0f, 2.0f, -5.0f);
	light04.Color = glm::vec4(0.2f, 0.2f, 0.0f, 1.0f);
	gLights.push_back(light04);
	
	// SoundEngine->play2D("sound/EternalGarden.mp3", true);

	return true;
}


void SetLightUniformInt(Shader* shader, const char* propertyName, size_t lightIndex, int value) {
	std::ostringstream ss;
	ss << "allLights[" << lightIndex << "]." << propertyName;
	std::string uniformName = ss.str();

	shader->setInt(uniformName.c_str(), value);
}
void SetLightUniformFloat(Shader* shader, const char* propertyName, size_t lightIndex, float value) {
	std::ostringstream ss;
	ss << "allLights[" << lightIndex << "]." << propertyName;
	std::string uniformName = ss.str();

	shader->setFloat(uniformName.c_str(), value);
}
void SetLightUniformVec4(Shader* shader, const char* propertyName, size_t lightIndex, glm::vec4 value) {
	std::ostringstream ss;
	ss << "allLights[" << lightIndex << "]." << propertyName;
	std::string uniformName = ss.str();

	shader->setVec4(uniformName.c_str(), value);
}
void SetLightUniformVec3(Shader* shader, const char* propertyName, size_t lightIndex, glm::vec3 value) {
	std::ostringstream ss;
	ss << "allLights[" << lightIndex << "]." << propertyName;
	std::string uniformName = ss.str();

	shader->setVec3(uniformName.c_str(), value);
}


bool Update() {
	// Cálculo del framerate
	float currentFrame = (float)glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	// Procesa la entrada del teclado o mouse
	processInput(window);

	// Renderizado R - G - B - A
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 projection;
	glm::mat4 view;

	if (activeCamera) {
		// Cámara en primera persona
		projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		view = camera.GetViewMatrix();
	}
	else {
		// cámara en tercera persona
		projection = glm::perspective(glm::radians(camera3rd.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		view = camera3rd.GetViewMatrix();
	}

	// Cubemap (fondo)
	{
		mainCubeMap->drawCubeMap(*cubemapShader, projection, view);
	}
	
	 {
		mLightsShader->use();

		// Activamos para objetos transparentes
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		mLightsShader->setMat4("projection", projection);
		mLightsShader->setMat4("view", view);

		// Aplicamos transformaciones del modelo
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(7.5f, 4.3f, 15.0f)); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.4f, 0.3f));	// it's a bit too big for our scene, so scale it down
		mLightsShader->setMat4("model", model);

		// Configuramos propiedades de fuentes de luz
		mLightsShader->setInt("numLights", (int)gLights.size());
		for (size_t i = 0; i < gLights.size(); ++i) {
			SetLightUniformVec3(mLightsShader, "Position", i, gLights[i].Position);
			SetLightUniformVec3(mLightsShader, "Direction", i, gLights[i].Direction);
			SetLightUniformVec4(mLightsShader, "Color", i, gLights[i].Color);
			SetLightUniformVec4(mLightsShader, "Power", i, gLights[i].Power);
			SetLightUniformInt(mLightsShader, "alphaIndex", i, gLights[i].alphaIndex);
			SetLightUniformFloat(mLightsShader, "distance", i, gLights[i].distance);
		}
		
		mLightsShader->setVec3("eye", camera.Position);

		// Aplicamos propiedades materiales
		mLightsShader->setVec4("MaterialAmbientColor", material01.ambient);
		mLightsShader->setVec4("MaterialDiffuseColor", material01.diffuse);
		mLightsShader->setVec4("MaterialSpecularColor", material01.specular);
		mLightsShader->setFloat("transparency", material01.transparency);

		house->Draw(*mLightsShader);

		// model = glm::mat4(1.0f);

		// Actividad 5.1
		// Efecto de puerta corrediza
		// model = glm::translate(model, glm::vec3(0.418f + door_offset, 0.0f, 6.75f));
		
		// Efecto de puerta con bisagra
		// model = glm::rotate(model, glm::radians(door_rotation), glm::vec3(0.0f, 1.0f, 0.0f));

		// model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		// mLightsShader->setMat4("model", model);
		// door->Draw(*mLightsShader);
	}

	glUseProgram(0);

	{
		// Activamos el shader del plano
		staticShader->use();

		// Activamos para objetos transparentes
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Aplicamos transformaciones de proyección y cámara (si las hubiera)
		staticShader->setMat4("projection", projection);
		staticShader->setMat4("view", view);

		// Aplicamos transformaciones del modelo
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-8.0f, -1.0f, 9.0f)); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(7.0f, 7.0f, 10.0f));	// it's a bit too big for our scene, so scale it down
		staticShader->setMat4("model", model);

		isla12->Draw(*staticShader);

		//dibuujamos la segunda isla 
		model = glm::mat4(2.0f);
		model = glm::translate(model, glm::vec3(6.0f, -1.0f, 2.5f)); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(10.0f, 10.0f, 9.0f));	// it's a bit too big for our scene, so scale it down
		staticShader->setMat4("model", model);

		isla13->Draw(*staticShader);


		//dibujamos la tercer isla 
		model = glm::mat4(2.0f);
		model = glm::translate(model, glm::vec3(8.0f, -1.0f, 13.0f)); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(8.0f, 8.0f, 13.0f));	// it's a bit too big for our scene, so scale it down
		staticShader->setMat4("model", model);

		isla->Draw(*staticShader);

		//dibujamos la cuarta isla 
		model = glm::mat4(2.0f);
		model = glm::translate(model, glm::vec3(-5.0f, -1.0f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(15.0f, 15.0f, 5.0f));	// it's a bit too big for our scene, so scale it down
		staticShader->setMat4("model", model);

		arena4->Draw(*staticShader);



		//plano de la ciudad 
		model = glm::mat4(2.0f);
		model = glm::translate(model, glm::vec3(10.0f, -1.0f, -11.0f)); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(9.0f, 9.0f, 2.8f));	// it's a bit too big for our scene, so scale it down
		staticShader->setMat4("model", model);

		plano->Draw(*staticShader);


		//plano del estacionamiento 
		model = glm::mat4(2.0f);
		model = glm::translate(model, glm::vec3(-2.5f, 1.8f, -6.0f)); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(3.5f, 5.0f, 2.8f));	// it's a bit too big for our scene, so scale it down
		staticShader->setMat4("model", model);

		estacionamiento->Draw(*staticShader);


		//primer edificio (rojo) 
		model = glm::mat4(2.0f);
		model = glm::translate(model, glm::vec3(8.6f, 5.6f, -7.0f)); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 4.0f));	// it's a bit too big for our scene, so scale it down
		staticShader->setMat4("model", model);

		edificio->Draw(*staticShader);

		//segundo edificio (azul) 
		model = glm::mat4(2.0f);
		model = glm::translate(model, glm::vec3(7.9f, 5.7f, -18.5f)); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 4.0f));	// it's a bit too big for our scene, so scale it down
		staticShader->setMat4("model", model);

		edificio2->Draw(*staticShader);

		//modelo de muro 
		model = glm::mat4(2.0f);
		model = glm::translate(model, glm::vec3(11.4f, 2.7f, -19.7f)); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f, 0.3f, 1.0f));	// it's a bit too big for our scene, so scale it down
		staticShader->setMat4("model", model);

		muro->Draw(*staticShader);

		//edificio 3 (espejos) 
		model = glm::mat4(2.0f);
		model = glm::translate(model, glm::vec3(18.0f, 3.7f, -12.5f)); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f, 1.0f, 1.9f));	// it's a bit too big for our scene, so scale it down
		staticShader->setMat4("model", model);

		edificio3->Draw(*staticShader);

		//visor 1 
		model = glm::mat4(2.0f);
		model = glm::translate(model, glm::vec3(2.05f, 1.2f, -8.6f)); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));	// it's a bit too big for our scene, so scale it down
		staticShader->setMat4("model", model);

		visor->Draw(*staticShader);


		//visor 2 
		model = glm::mat4(2.0f);
		model = glm::translate(model, glm::vec3(-14.0f, 1.0f, 8.2f)); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));	// it's a bit too big for our scene, so scale it down
		staticShader->setMat4("model", model);

		visor->Draw(*staticShader);


		//visor 3
		model = glm::mat4(2.0f);
		model = glm::translate(model, glm::vec3(-4.0f, 4.6f, 13.0f)); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));	// it's a bit too big for our scene, so scale it down
		staticShader->setMat4("model", model);

		visor->Draw(*staticShader);

		//carpa 
		model = glm::mat4(2.0f);
		model = glm::translate(model, glm::vec3(-9.0f, 2.9f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 0.125f));	// it's a bit too big for our scene, so scale it down
		staticShader->setMat4("model", model);


		carpa->Draw(*staticShader);

		//mesa
		model = glm::mat4(2.0f);
		model = glm::translate(model, glm::vec3(-9.0f, 1.0f, 2.0f)); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));	// it's a bit too big for our scene, so scale it down
		staticShader->setMat4("model", model);


		mesa->Draw(*staticShader);

		//lata 
		model = glm::mat4(2.0f);
		model = glm::translate(model, glm::vec3(-9.0f, 1.57f, 2.0f)); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));	// it's a bit too big for our scene, so scale it down
		staticShader->setMat4("model", model);


		lata->Draw(*staticShader);

		//segunda lata 
		model = glm::mat4(2.0f);
		model = glm::translate(model, glm::vec3(-9.0f, 1.59f, 1.6f)); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));	// it's a bit too big for our scene, so scale it down
		staticShader->setMat4("model", model);


		lata->Draw(*staticShader);

		//tercer lata 
		model = glm::mat4(2.0f);
		model = glm::translate(model, glm::vec3(-9.2f, 1.60f, 1.4f)); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));	// it's a bit too big for our scene, so scale it down
		staticShader->setMat4("model", model);


		lata->Draw(*staticShader);


		//cuarta lata 
		model = glm::mat4(2.0f);
		model = glm::translate(model, glm::vec3(-9.1f, 1.59f, 1.4f)); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));	// it's a bit too big for our scene, so scale it down
		staticShader->setMat4("model", model);


		lata->Draw(*staticShader);



		//quinta lata 
		model = glm::mat4(2.0f);
		model = glm::translate(model, glm::vec3(-8.9f, 1.57f, 1.7f)); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));	// it's a bit too big for our scene, so scale it down
		staticShader->setMat4("model", model);


		lata->Draw(*staticShader);



		//barco  
		model = glm::mat4(2.0f);
		model = glm::translate(model, glm::vec3(-19.8f, 0.0f, -18.7f)); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));	// it's a bit too big for our scene, so scale it down
		staticShader->setMat4("model", model);


		barco->Draw(*staticShader);





	}
	

	glUseProgram(0);


	// Actividad 5.2
	
	{
		// Activamos el shader de Phong
		proceduralShader->use();

		// Activamos para objetos transparentes
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Aplicamos transformaciones de proyección y cámara (si las hubiera)
		proceduralShader->setMat4("projection", projection);
		proceduralShader->setMat4("view", view);

		// Aplicamos transformaciones del modelo
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		proceduralShader->setMat4("model", model);

		proceduralShader->setFloat("time", proceduralTime);
		proceduralShader->setFloat("radius", 5.0f);
		proceduralShader->setFloat("height", 15.0f);

		moon->Draw(*proceduralShader);
		proceduralTime += 0.001;

	}

	glUseProgram(0);
	

	// Actividad 5.3
	
	{
		// Activamos el shader de Phong
		wavesShader->use();

		// Activamos para objetos transparentes
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Aplicamos transformaciones de proyección y cámara (si las hubiera)
		wavesShader->setMat4("projection", projection);
		wavesShader->setMat4("view", view);

		// Aplicamos transformaciones del modelo
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		wavesShader->setMat4("model", model);

		wavesShader->setFloat("time", wavesTime);
		wavesShader->setFloat("radius", 5.0f);
		wavesShader->setFloat("height", 5.0f);

		gridMesh->Draw(*wavesShader);
		wavesTime += 0.01;

	}

	glUseProgram(0);
	
	
	// Objeto animado
	
	{
		character01->UpdateAnimation(deltaTime);

		// Activación del shader del personaje
		dynamicShader->use();

		// Aplicamos transformaciones de proyección y cámara (si las hubiera)
		dynamicShader->setMat4("projection", projection);
		dynamicShader->setMat4("view", view);

		// Aplicamos transformaciones del modelo
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 2.0f, 0.0f));
		model = glm::translate(model, position); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(rotateCharacter), glm::vec3(0.0, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.007f, 0.007f, 0.007f));	// it's a bit too big for our scene, so scale it down

		dynamicShader->setMat4("model", model);

		dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, character01->gBones);

		// Dibujamos el modelo
		character01->Draw(*dynamicShader);
	}
	


	// Objeto animado

	{
		baile->UpdateAnimation(deltaTime);

		// Activación del shader del personaje
		dynamicShader->use();

		// Aplicamos transformaciones de proyección y cámara (si las hubiera)
		dynamicShader->setMat4("projection", projection);
		dynamicShader->setMat4("view", view);

		// Aplicamos transformaciones del modelo
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-9.0f, 1.1f, 0.0f));
		//model = glm::translate(model, position); // translate it down so it's at the center of the scene
		//model = glm::rotate(model, glm::radians(rotateCharacter), glm::vec3(0.0, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.001f, 0.001f, 0.001f));	// it's a bit too big for our scene, so scale it down

		dynamicShader->setMat4("model", model);

		dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, baile->gBones);

		// Dibujamos el modelo
		baile->Draw(*dynamicShader);
	}

	glUseProgram(0); 


	//segundo baile 
	{
		baile2->UpdateAnimation(deltaTime);

		// Activación del shader del personaje
		dynamicShader->use();

		// Aplicamos transformaciones de proyección y cámara (si las hubiera)
		dynamicShader->setMat4("projection", projection);
		dynamicShader->setMat4("view", view);

		// Aplicamos transformaciones del modelo
		glm::mat4 model = glm::mat4(2.0f);
		model = glm::translate(model, glm::vec3(-10.0f, 1.1f, 2.0f));
		//model = glm::translate(model, position); // translate it down so it's at the center of the scene
		//model = glm::rotate(model, glm::radians(rotateCharacter), glm::vec3(0.0, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.0006f, 0.0006f, 0.0006f));	// it's a bit too big for our scene, so scale it down

		dynamicShader->setMat4("model", model);

		dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, baile2->gBones);

		// Dibujamos el modelo
		baile2->Draw(*dynamicShader);
	}

	glUseProgram(0);

	//tercer baile 
	{
		baile3->UpdateAnimation(deltaTime);

		// Activación del shader del personaje
		dynamicShader->use();

		// Aplicamos transformaciones de proyección y cámara (si las hubiera)
		dynamicShader->setMat4("projection", projection);
		dynamicShader->setMat4("view", view);

		// Aplicamos transformaciones del modelo
		glm::mat4 model = glm::mat4(3.0f);
		model = glm::translate(model, glm::vec3(-7.7f, 1.1f, 1.5f));
		//model = glm::translate(model, position); // translate it down so it's at the center of the scene
		//model = glm::rotate(model, glm::radians(rotateCharacter), glm::vec3(0.0, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.0006f, 0.0006f, 0.0006f));	// it's a bit too big for our scene, so scale it down

		dynamicShader->setMat4("model", model);

		dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, baile3->gBones);

		// Dibujamos el modelo
		baile3->Draw(*dynamicShader);
	}

	glUseProgram(0);

	// glfw: swap buffers 
	glfwSwapBuffers(window);
	glfwPollEvents();

	return true;
}

// Procesamos entradas del teclado
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
		door_offset += 0.01f;
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
		door_offset -= 0.01f;
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		door_rotation += 1.f;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		door_rotation -= 1.f;

	// Character movement
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {

		position = position + scaleV * forwardView;
		camera3rd.Front = forwardView;
		camera3rd.ProcessKeyboard(FORWARD, deltaTime);
		camera3rd.Position = position;
		camera3rd.Position.y += 3.0f;
		camera3rd.Position -= trdpersonOffset * forwardView;

	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		position = position - scaleV * forwardView;
		camera3rd.Front = forwardView;
		camera3rd.ProcessKeyboard(BACKWARD, deltaTime);
		camera3rd.Position = position;
		camera3rd.Position.y += 3.0f;
		camera3rd.Position -= trdpersonOffset * forwardView;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		rotateCharacter += 0.5f;

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(rotateCharacter), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::vec4 viewVector = model * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
		forwardView = glm::vec3(viewVector);
		forwardView = glm::normalize(forwardView);

		camera3rd.Front = forwardView;
		camera3rd.Position = position;
		camera3rd.Position.y += 3.0f;
		camera3rd.Position -= trdpersonOffset * forwardView;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		rotateCharacter -= 0.5f;

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(rotateCharacter), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::vec4 viewVector = model * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
		forwardView = glm::vec3(viewVector);
		forwardView = glm::normalize(forwardView);

		camera3rd.Front = forwardView;
		camera3rd.Position = position;
		camera3rd.Position.y += 3.0f;
		camera3rd.Position -= trdpersonOffset * forwardView;
	}

	if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS)
		activeCamera = 0;
	if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS)
		activeCamera = 1;
	
}

// glfw: Actualizamos el puerto de vista si hay cambios del tamaño
// de la ventana
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// glfw: Callback del movimiento y eventos del mouse
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = (float)xpos;
		lastY = (float)ypos;
		firstMouse = false;
	}

	float xoffset = (float)xpos - lastX;
	float yoffset = lastY - (float)ypos; 

	lastX = (float)xpos;
	lastY = (float)ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: Complemento para el movimiento y eventos del mouse
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll((float)yoffset);
}
