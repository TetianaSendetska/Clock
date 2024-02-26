#include <iostream>
#include <string>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

using namespace std;

//Window size
const GLint WIDTH = 800, HEIGHT = 600;
const float toRadiants = 3.14159265f / 180.0f;

GLuint VAO, VBO, EBO, shader, uniformModel, uniformColor;

//vertex shader
static const char* vshader = "                             \n\
#version 330                                               \n\
                                                           \n\
layout (location = 0) in vec3 pos;                         \n\
                                                           \n\
  uniform mat4 model;                                      \n\
  out vec4 vCol;                                           \n\
  uniform float color;                                     \n\
                                                           \n\
void main () {                                             \n\
    gl_Position = model * vec4(pos.x, pos.y, pos.z,  1.0); \n\
	vCol = vec4(1.0, color, color, 1.0);                   \n\
														   \n\
                                                           \n\
}";

//Fragment Shader
static const char* fshader = "                             \n\
#version 330                                               \n\
                                                           \n\
in vec4 vCol;                                              \n\
                                                           \n\
out vec4 color;                                            \n\
                                                           \n\
void main () {                                             \n\
    color = vCol;                                          \n\
}";

void CreateClock() {
	GLfloat vertices[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.5f, 0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f,//рамка

		-0.0f, -0.5f, 0.0f,
		0.0f, -0.4f, 0.0f,

		0.5f, 0.0f, 0.0f,
		0.4f, 0.0f, 0.0f,

		0.0f, 0.5f, 0.0f,
		0.0f, 0.4f, 0.0f,

		-0.5f, 0.0f, 0.0f,
		-0.4f, 0.0f, 0.0f, //прямі

		0.3f, 0.5f, 0.0f,
		0.28f, 0.4f, 0.0f,

		0.3f, -0.5f, 0.0f,
		0.28f, -0.4f, 0.0f,

		-0.3f, -0.5f, 0.0f,
		-0.28f, -0.4f, 0.0f,

		-0.3f, 0.5f, 0.0f,
		-0.28f, 0.4f, 0.0f, //кутові верхні

		0.5f, 0.3f, 0.0f,
		0.4f, 0.28f, 0.0f,

		0.5f, -0.3f, 0.0f,
		0.4f, -0.28f, 0.0f,

		-0.5f, 0.3f, 0.0f,
		-0.4f, 0.28f, 0.0f,

		-0.5f, -0.3f, 0.0f,
		-0.4f, -0.28f, 0.0f, //кутові бічні

		0.0f, 0.0f, 0.0f,
		0.0f, 0.3f, 0.0f,

		0.0f, 0.0f, 0.0f,
		0.0f, 0.17f, 0.0f
	};

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

void AddShader(GLuint program, const char* shaderCode, GLenum shaderType)
{
	GLuint theShader = glCreateShader(shaderType);

	const GLchar* theCode[1];
	theCode[0] = shaderCode;

	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);

	glShaderSource(theShader, 1, theCode, codeLength);
	glCompileShader(theShader);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result) {
		glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
		cout << "Error compiling the " << shaderType << " shader:\n" << eLog;
		return;
	}

	glAttachShader(program, theShader);
}
void CompileShader()
{
	shader = glCreateProgram();
	if (!shader) {
		cout << "Error creating shader program";
		return;
	}

	AddShader(shader, vshader, GL_VERTEX_SHADER);
	AddShader(shader, fshader, GL_FRAGMENT_SHADER);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glLinkProgram(shader);
	glGetProgramiv(shader, GL_LINK_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		cout << "Error linking program\n" << eLog;
		return;
	}

	glValidateProgram(shader);
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		cout << "Error validatig program\n" << eLog;
		return;
	}

	uniformModel = glGetUniformLocation(shader, "model");
	uniformColor = glGetUniformLocation(shader, "color");
}

int main() {

	//Initialise glfw
	if (!glfwInit()) {
		printf("GLFW initialisation failed");
		glfwTerminate();
		return 1;
	}

	//Setup glfw windows properties
	//Opengl version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Core profile = No Backwards Compatability 
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);


	//Create window
	GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Clock", NULL, NULL);
	if (!mainWindow) {
		printf("GLFW window creation failed");
		glfwTerminate();
		return 1;
	}

	//Get buffer size info
	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	//Set context for glew to use
	glfwMakeContextCurrent(mainWindow);

	//Allow modern extension features
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK) {
		printf("GLEW initialization failed");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	//setup Viewport
	glViewport(0, 0, bufferWidth, bufferHeight);
	float angleMinute = 0;
	float angleHour = 0;
	float rotationMinuteSpeed = 0.01;
	float rotationHourSpeed = 0.001;

	CreateClock();
	CompileShader();
	
	//Loop until window closed 
	while (!glfwWindowShouldClose(mainWindow)) 
	{
		//get and handle user input events
		glfwPollEvents();

		//Clear window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader);

		glBindVertexArray(VAO);

		glm::mat4 model(1.0f);
		float color = 1.0f;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(uniformColor, color);
		glDrawArrays(GL_LINE_LOOP, 0, 4);
		color = 0.0f;
		glUniform1f(uniformColor, color);
		for (int i = 4; i < 28; i += 2) {
			glDrawArrays(GL_LINE_STRIP, i, 2);
		}
		
		if (glfwGetKey(mainWindow, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			angleMinute -= rotationMinuteSpeed * 2.0f;
			angleHour -= rotationHourSpeed * 2.0f;
		}
		else if (glfwGetKey(mainWindow, GLFW_KEY_LEFT) == GLFW_PRESS) {
			angleMinute += rotationMinuteSpeed * 0.5f;
			angleHour += rotationHourSpeed * 0.5f;
		}

		color = 1.0f;
		glUniform1f(uniformColor, color);
		model = glm::mat4(1.0f);
		model = glm::rotate(model, angleMinute * toRadiants, glm::vec3(0.0f, 0.0f, 1.0f));
		angleMinute = angleMinute - 0.01;
		
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_LINE_STRIP, 28, 2);

		model = glm::mat4(1.0f);
		model = glm::rotate(model, angleHour * toRadiants, glm::vec3(0.0f, 0.0f, 1.0f));
		angleHour = angleHour - 0.001;

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_LINE_STRIP, 30, 2);
	
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glBindVertexArray(0);
		glUseProgram(0);

		glfwSwapBuffers(mainWindow);

	}

	return 0;
}