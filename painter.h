#pragma once

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>

#include "painter_state.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include<filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace sf;

class Painter {

	const static GLuint shadersNumber = 3;

	GLuint instancedProgram, defaultProgram;


	bool readFile(const std::string& filename, std::string& content) {
		std::ifstream file(filename);

		if (!file.is_open()) {
			std::cerr << "Unable to open the file: " << filename << std::endl;
			return false;
		}

		std::stringstream buffer;
		buffer << file.rdbuf();
		content = buffer.str();

		file.close();

		return true;
	}

	void ShaderLog(unsigned int shader)
	{
		int infologLen = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLen);
		if (infologLen > 1)
		{
			int charsWritten = 0;
			std::vector<char> infoLog(infologLen);
			glGetShaderInfoLog(shader, infologLen, &charsWritten, infoLog.data());
			std::cout << "InfoLog: " << infoLog.data() << std::endl;
		}
	}

	void InitShader() {
		std::string content;
		const char* cstrContent;

		GLuint instancedVShader, defaultVShader;

		readFile("shaders/shader.vert", content);
		cstrContent = content.c_str();
		defaultVShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(defaultVShader, 1, &cstrContent, NULL);
		glCompileShader(defaultVShader);
		std::cout << "default vertex shader" << std::endl;
		ShaderLog(defaultVShader);

		readFile("shaders/instanced_shader.vert", content);
		cstrContent = content.c_str();
		instancedVShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(instancedVShader, 1, &cstrContent, NULL);
		glCompileShader(instancedVShader);
		std::cout << "instanced vertex shader" << std::endl;
		ShaderLog(instancedVShader);

		GLuint fShader;

		readFile("shaders/shader.frag", content);
		cstrContent = content.c_str();
		fShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fShader, 1, &cstrContent, NULL);
		glCompileShader(fShader);
		std::cout << "fragment shader" << std::endl;
		ShaderLog(fShader);

		defaultProgram = glCreateProgram();
		glAttachShader(defaultProgram, defaultVShader);
		glAttachShader(defaultProgram, fShader);
		glLinkProgram(defaultProgram);
		int link_ok;
		glGetProgramiv(defaultProgram, GL_LINK_STATUS, &link_ok);
		if (!link_ok) {
			std::cout << "error attach shaders \n";
			return;
		}

		instancedProgram = glCreateProgram();
		glAttachShader(instancedProgram, instancedVShader);
		glAttachShader(instancedProgram, fShader);
		glLinkProgram(instancedProgram);
		glGetProgramiv(instancedProgram, GL_LINK_STATUS, &link_ok);
		if (!link_ok) {
			std::cout << "error attach shaders \n";
			return;
		}
	}

	/*void InitShader() {
		GLuint vShaders[shadersNumber];
		std::filesystem::path currentPath = std::filesystem::current_path();
		std::cout << "Current Path: " << currentPath.string() << std::endl;

		std::string  vertShaderText = get_file_contents((currentPath.string() + "\\shaders\\shader.vert").c_str());
		const char* vertShader = vertShaderText.c_str();
		for (int i = 0; i < shadersNumber; i++) {
			vShaders[i] = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vShaders[i], 1, &vertShader, NULL);

			glCompileShader(vShaders[i]);
			std::cout << "vertex shader" << i << std::endl;
			ShaderLog(vShaders[i]);
		}

		GLuint fShaders[shadersNumber];
		std::string  fragShaderText = get_file_contents((currentPath.string() + "\\shaders\\shader.frag").c_str());
		const char* fragShader = vertShaderText.c_str();
		for (int i = 0; i < shadersNumber; i++) {
			fShaders[i] = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fShaders[i], 1, &fragShader, NULL);
			glCompileShader(fShaders[i]);
			std::cout << "fragment shader" << i << std::endl;
			ShaderLog(fShaders[i]);
		}


		for (int i = 0; i < shadersNumber; i++) {
			Programs[i] = glCreateProgram();
			glAttachShader(Programs[i], vShaders[i]);
			glAttachShader(Programs[i], fShaders[i]);
			glLinkProgram(Programs[i]);
			int link_ok;
			glGetProgramiv(Programs[i], GL_LINK_STATUS, &link_ok);
			if (!link_ok) {
				std::cout << "error attach shaders \n";
				return;
			}
		}
	}*/

	void ReleaseShader() {
		glUseProgram(0);
		glDeleteProgram(instancedProgram);
		glDeleteProgram(defaultProgram);
	}

	GLfloat deegressToRadians(GLfloat deegres) {
		return deegres * 3.141592f / 180.0f;
	}


public:
	Painter(PainterState& painterState) : state(painterState) {}

	PainterState state;

	glm::vec3 giftPos = glm::vec3(-1.0f);

	void Draw() {
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		
		if (state.platform != nullptr) {
			glUseProgram(instancedProgram);
			glm::mat4 platformMat = glm::scale(glm::mat4(1.0f), glm::vec3(5.f))
				* glm::rotate(glm::mat4(1.0f), deegressToRadians(90), glm::vec3(-1.0f, 0.0f, 0.0f));
			(state.platform->Draw(instancedProgram, platformMat, state.camera, state.pointSource, state.spotlightSource, state.directionalSource));
			glUseProgram(0);
		}

		if (state.airship != nullptr) {
			glUseProgram(defaultProgram);
			glm::vec3 fVec = glm::normalize(state.camera.front);
			fVec *= 3;
			glm::vec3 uVec = glm::normalize(state.camera.up);
			uVec *= -0.5;
			glm::mat4 airshipMat = glm::translate(glm::mat4(1.0f), fVec)
				* glm::translate(glm::mat4(1.0f), uVec)
				* glm::inverse(state.camera.getViewMatrix())
				* glm::rotate(glm::mat4(1.0f), deegressToRadians(115), glm::vec3(0.0f, 1.0f, 0.0f))
				* glm::rotate(glm::mat4(1.0f), deegressToRadians(10), glm::vec3(0.0f, 0.0f, 1.0f))
				* glm::scale(glm::mat4(1.0f), glm::vec3(0.002f));
			(state.airship->Draw(defaultProgram, airshipMat, state.camera, state.pointSource, state.spotlightSource, state.directionalSource));
			glUseProgram(0);
		}

		if (state.tree != nullptr) {
			glUseProgram(defaultProgram);
			glm::mat4 treeMat = glm::scale(glm::mat4(1.0f), glm::vec3(0.03f))
				* glm::rotate(glm::mat4(1.0f), deegressToRadians(90), glm::vec3(-1.0f, 0.0f, 0.0f));
			(state.tree->Draw(defaultProgram, treeMat, state.camera, state.pointSource, state.spotlightSource, state.directionalSource));
			glUseProgram(0);
		}

		if (state.house != nullptr) {
			glUseProgram(instancedProgram);
			glm::mat4 houseMat = glm::scale(glm::mat4(1.0f), glm::vec3(0.003f))
				* glm::rotate(glm::mat4(1.0f), deegressToRadians(90), glm::vec3(-1.0f, 0.0f, 0.0f));
			(state.house->Draw(instancedProgram, houseMat, state.camera, state.pointSource, state.spotlightSource, state.directionalSource));
			glUseProgram(0);
		}

		if (state.cloud != nullptr) {
			glUseProgram(instancedProgram);
			glm::mat4 cloudMat = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f))
				* glm::rotate(glm::mat4(1.0f), deegressToRadians(90), glm::vec3(-1.0f, 0.0f, 0.0f));
			(state.cloud->Draw(instancedProgram, cloudMat, state.camera, state.pointSource, state.spotlightSource, state.directionalSource));
			glUseProgram(0);
		}

		if (state.gift != nullptr && giftPos.y > 0.5f) {
			glUseProgram(defaultProgram);
			glm::mat4 giftMat = glm::translate(glm::mat4(1.0f), giftPos) 
				* glm::scale(glm::mat4(1.0f), glm::vec3(0.01f))
				* glm::rotate(glm::mat4(1.0f), deegressToRadians(90), glm::vec3(-1.0f, 0.0f, 0.0f));
			(state.gift->Draw(defaultProgram, giftMat, state.camera, state.pointSource, state.spotlightSource, state.directionalSource));
			giftPos.y -= 0.05;
			glUseProgram(0);
		}

		glUseProgram(0);
	}

	void Init() {
		glewInit();
		InitShader();
	}

	void Release() {
		ReleaseShader();
	}

};
