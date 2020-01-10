#include "renderer.h"

Renderer::Renderer(std::vector<float> vertices, glm::vec4 colour) : 
		vertices(vertices), colour(colour), vao(0), vbo(0) {
    init();
}

Renderer::~Renderer() {
	glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);    
}

void Renderer::init() {
    // Get pointer to vector array by getting pointer to first value (vector is guaranteed to be contiguous)
    float* vertexData = &vertices[0];
    // Create vertex buffer
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    // Bind buffer and assign vertex array
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertexData, GL_STATIC_DRAW);
    // Set vertex attributes (i.e. new vertex every 3 values)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
    // Unbind buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Renderer::render(GLuint shaderProgram, glm::mat4 transformation) {
    // Set shader values (model transformation matrix and vertex colour)
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(transformation));
    glUniform4fv(glGetUniformLocation(shaderProgram, "color"), 1, glm::value_ptr(colour));
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 3);
    glBindVertexArray(0);
}

// Resets the data in the vertex buffer
void Renderer::updateVertexData() {
    float* vertexData = &vertices[0];

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertexData, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}