#include "entity_wave.h"

// Wave is made up of triangles with vertices set to the points on a sine wave
void EntityWave::updateWave(float offset, const float amp, const float freq, const float height) {
    int segments = 30;
    float screenWidth = 1280.0f, screenHeight = 720.0f;
    float segmentWidth = screenWidth / segments;

    // Resize the vector if it doesn't match the required size
    if((int) renderer->vertices.size() != segments * 18) {
        renderer->vertices.resize(segments * 18);
    }

    for(int i = 0; i < segments; i++) {
        float x = i * segmentWidth;
        float nextX = (i + 1) * segmentWidth;
        float y = screenHeight / 2 + amp * sin(freq * (x + offset));
        float nextY = screenHeight / 2 + amp * sin(freq * (nextX + offset));
        renderer->vertices[i * 18 + 0] = x;				    // top left
        renderer->vertices[i * 18 + 1] = y + height / 2;
        renderer->vertices[i * 18 + 3] = x + segmentWidth;	// bottom right
        renderer->vertices[i * 18 + 4] = nextY - height / 2;
        renderer->vertices[i * 18 + 6] = x;					// bottom left
        renderer->vertices[i * 18 + 7] = y - height / 2;
        renderer->vertices[i * 18 + 9] = x;					// top left
        renderer->vertices[i * 18 + 10] = y + height / 2;
        renderer->vertices[i * 18 + 12] = x + segmentWidth;	// top right
        renderer->vertices[i * 18 + 13] = nextY + height / 2;
        renderer->vertices[i * 18 + 15] = x + segmentWidth; // bottom right
        renderer->vertices[i * 18 + 16] = nextY - height / 2;
    }

    renderer->updateVertexData();
}