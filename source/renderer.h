#pragma once

#include <stdio.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

class Renderer {
public:
	std::vector<float> vertices;
	glm::vec4 colour;

	Renderer(std::vector<float> vertices, glm::vec4 colour);
	~Renderer();

    void init();
	void render(GLuint shaderProgram, glm::mat4 transformation);
	void updateVertexData();

private:
	GLuint vao, vbo;
};

class QuadRenderer : public Renderer {
public:
	QuadRenderer(glm::vec4 colour) : Renderer(
		{
			-0.5f,  0.5f, 0.0f, // top left
		 	 0.5f, -0.5f, 0.0f, // bottom right
			-0.5f, -0.5f, 0.0f, // bottom left

			-0.5f, 0.5f, 0.0f, // top left
		 	 0.5f, 0.5f, 0.0f, // top right
		 	 0.5f, -0.5f, 0.0f, // bottom right
		} , colour) {}
};

class TriRenderer : public Renderer {
public:
	TriRenderer(glm::vec4 colour) : Renderer(
		{
			-0.5f, -0.5f, 0.0f, // left
		 	 0.0f, 0.5f, 0.0f, // top 
			 0.5f, -0.5f, 0.0f, // right
		} , colour) {}
};