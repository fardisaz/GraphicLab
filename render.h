#pragma once
#ifndef RENDER_H
#define RENDER_H

// GLFW - glad - stb_image
#include <glad/glad.h>
#include <GLFW/glfw3.h>
//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// GL includes
#include "shader.h"
#include "camera.h"
#include "model.h"
#include "shape.h"

#include <iostream>

namespace visualisation
{

	class render
	{
	private:
		shape* room;
		unsigned int wallVAO, floorVAO, doorVAO, windowVAO;
		vector<unsigned int> roomTextures;
		vector<unsigned int> roomVAO;
		vector<const char*> imgDir;
		vector<string> modelPath;
		vector<glm::vec3> modelPosition;
		vector<glm::vec3> modelScale;
		vector<double> modelAngle;
		vector<Model> models;
		vector<glm::vec3> roomPosition;
		vector<glm::vec3> roomScale;
		double w;
		double l;
		double h;
	public:
		render(shape* room);
		~render();
		void getSize();
		void visualise();
		void setCameraPosition();
		GLFWwindow* intiframework();
		Shader setupshaders();
		void loadFurnitures();
		void loadRoom();
		void setRoomVertices(float vertices[], int size);
		void loadDoorAndWindow();
		void loadTextures(Shader shader);

	};
}

#endif