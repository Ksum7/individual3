#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <GL/glew.h>

#include "imgui.h" // necessary for ImGui::*, imgui-SFML.h doesn't include imgui.h

#include "imgui-SFML.h" // for ImGui::SFML::* functions and SFML-specific overloads

#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <iostream>
#include "camera.h"
#include "painter.h"
#include "lib/ImGuiFileDialog/ImGuiFileDialog.h"
#include "painter_state.h"
#include<filesystem>

using namespace sf;

void floatPicker(GLfloat* val, std::string label, std::string nameSpace) {
	ImGui::Text(label.c_str());
	ImGui::DragFloat(("##" + label + nameSpace).c_str(), val, 0.1F);
}

void vectorPicker(glm::vec3* vec, std::string label, std::string nameSpace) {
	ImGui::Text(label.c_str());
	ImGui::DragFloat(("X##" + label + nameSpace).c_str(), &(vec->x), 0.1F);
	ImGui::DragFloat(("Y##" + label + nameSpace).c_str(), &(vec->y), 0.1F);
	ImGui::DragFloat(("Z##" + label + nameSpace).c_str(), &(vec->z), 0.1F);
}

void intencityPicker(GLfloat* val, std::string label, std::string nameSpace) {
	ImGui::Text(label.c_str());
	ImGui::DragFloat(("##" + label + nameSpace).c_str(), val, 0.01, 0, 1);
}

void conePicker(GLfloat* val, std::string label, std::string nameSpace) {
	ImGui::Text(label.c_str());
	ImGui::DragFloat(("##" + label + nameSpace).c_str(), val, 1.0, 0.1, 180);
}


void pointSourceEditor(GLfloat* intensity, glm::vec3* pos) {
	if (ImGui::CollapsingHeader("Point source")) {
		intencityPicker(intensity, "Intensity", "pointSource");
		vectorPicker(pos, "Position", "pointSource");
	}
}

void directionalSourceEditor(GLfloat* intensity, glm::vec3* direction) {
	if (ImGui::CollapsingHeader("Directional source")) {
		intencityPicker(intensity, "Intensity", "directionalSource");
		vectorPicker(direction, "Direction", "directionalSource");
	}
}

void spotlightSourceEditor(GLfloat* intensity, glm::vec3* pos, glm::vec3* direction, GLfloat* cone) {
	if (ImGui::CollapsingHeader("Spotlight source")) {
		intencityPicker(intensity, "Intensity", "spotlightSource");
		vectorPicker(pos, "Position", "spotlightSource");
		vectorPicker(direction, "View point", "spotlightSource");
		conePicker(cone, "Cone", "spotlightSource");
	}
}

void shadingPicker(std::string title, int* picked) {
	if (ImGui::CollapsingHeader(title.c_str())) {
		ImGui::RadioButton(("Phong##" + title).c_str(), picked, Shading::Phong);
		ImGui::RadioButton(("Toon##" + title).c_str(), picked, Shading::Toon);
		ImGui::RadioButton(("Rim##" + title).c_str(), picked, Shading::Rim);
	}
}

std::string vec3ToStr(glm::vec3 vec3) {
	return "(" +
		std::to_string(vec3.x) + ", " +
		std::to_string(vec3.y) + ", " +
		std::to_string(vec3.z) + ")";
}

int main() {
	std::srand(std::time(0));
	sf::RenderWindow window(sf::VideoMode(600, 600), "Individual 3", sf::Style::Default, sf::ContextSettings(24));
	window.setFramerateLimit(60);
	window.setVerticalSyncEnabled(true);
	window.setActive(true);
	std::filesystem::path currentPath = std::filesystem::current_path();

	Camera camera = Camera(glm::vec3(0.0f, 5.0f, 20.0f), 1.0f);
	auto state = PainterState(camera);
	auto painter = Painter(state);
	std::vector<glm::vec3> platformTranslations;
	for (int i = -50; i < 50; ++i) {
		for (int j = -50; j < 50; ++j) {
			platformTranslations.push_back(glm::vec3(i * 10.0f, 0.0f, j * 10.0f));
		}
	}

	std::vector<glm::vec3> houseTranslations;
	for (int i = 0; i < 5; ++i) {
		float randomX = static_cast<float>(std::rand()) / RAND_MAX * 80.0f - 40.0f;
		float randomZ = static_cast<float>(std::rand()) / RAND_MAX * 80.0f - 40.0f;
		houseTranslations.push_back(glm::vec3(randomX, 0.0f, randomZ));
	}

	std::vector<glm::vec3> cloudTranslations;
	for (int i = 0; i < 10; ++i) {
		float randomX = static_cast<float>(std::rand()) / RAND_MAX * 80.0f - 40.0f;
		float randomY = static_cast<float>(std::rand()) / RAND_MAX * 5.0f;
		float randomZ = static_cast<float>(std::rand()) / RAND_MAX * 80.0f - 40.0f;
		cloudTranslations.push_back(glm::vec3(randomX, 5.0f + randomY, randomZ));
	}

	painter.Init();
	painter.state.platform = new Model(currentPath.string() + "\\platform\\scene.gltf", platformTranslations.data(), platformTranslations.size());
	painter.state.airship = new Model(currentPath.string() + "\\airship\\airship.obj");
	painter.state.tree = new Model(currentPath.string() + "\\tree\\tree.obj");
	painter.state.gift = new Model(currentPath.string() + "\\coffee\\scene.gltf");
	painter.state.house = new Model(currentPath.string() + "\\house\\house.obj", houseTranslations.data(), houseTranslations.size());
	painter.state.balloon = new Model(currentPath.string() + "\\coffee\\scene.gltf");
	painter.state.cloud = new Model(currentPath.string() + "\\coffee\\scene.gltf", cloudTranslations.data(), cloudTranslations.size());

	// delete some excess meshes
	/*painter.state.warrior->meshes.erase(painter.state.warrior->meshes.begin());
	painter.state.warrior->meshes.erase(painter.state.warrior->meshes.begin());
	painter.state.warrior->meshes.erase(painter.state.warrior->meshes.begin());
	painter.state.warrior->meshes.erase(painter.state.warrior->meshes.begin());
	painter.state.warrior->meshes.erase(painter.state.warrior->meshes.begin());*/

	GLboolean firstMouse = true;
	GLfloat lastX = 0, lastY = 0;
	sf::Clock clock;
	GLboolean isFocused = false;
	sf::Vector2i centerWindow;

	if (!ImGui::SFML::Init(window)) return -1;

	int shade1 = Shading::Phong;
	int shade2 = Shading::Toon;

	sf::Clock deltaClock;
	while (window.isOpen()) {

		sf::Event event;
		while (window.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(window, event);
			bool isImGuiHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);

			if (event.type == sf::Event::Closed)
				window.close();
			else if (event.type == sf::Event::Resized) {
				glViewport(
					0,
					0,
					event.size.width,
					event.size.height
				);
				painter.state.camera.processResize(event.size.width, event.size.height);
			}
			else if (event.type == sf::Event::KeyPressed) {
				painter.state.camera.processKeyboard(event.key.code);
			}
			else if (event.type == sf::Event::MouseMoved && isFocused) {
				GLfloat xoffset = event.mouseMove.x - centerWindow.x;
				GLfloat yoffset = centerWindow.y - event.mouseMove.y;
				lastX = event.mouseMove.x;
				lastY = event.mouseMove.y;
				sf::Mouse::setPosition(centerWindow, window);
				painter.state.camera.processMouseMovement(xoffset, yoffset);
			}

			if (!isImGuiHovered && event.type == sf::Event::MouseButtonPressed) {
				isFocused = true;
				window.setMouseCursorVisible(false);
				window.setMouseCursorGrabbed(true);
				centerWindow.x = window.getSize().x / 2;
				centerWindow.y = window.getSize().y / 2;
				sf::Mouse::setPosition(centerWindow, window);
				auto pos = sf::Mouse::getPosition();
			}
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape || event.type == sf::Event::LostFocus) {
				window.setMouseCursorVisible(true);
				window.setMouseCursorGrabbed(false);
				isFocused = false;
				firstMouse = true;
			}
		}

		//ImGui::SFML::Update(window, deltaClock.restart());


		//ImGui::Begin("Individual 3");

		//ImGui::Text("Camera position:");
		//ImGui::Text(vec3ToStr(painter.state.camera.position).c_str());

		//directionalSourceEditor(&painter.state.directionalSource.intensity, &painter.state.directionalSource.direction);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		painter.Draw();

		//ImGui::End();
		//ImGui::SFML::Render(window);
		window.display();
	}

	painter.Release();
	return 0;
}
