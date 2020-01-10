#include "entity.h"

Entity::Entity(float x, float y, float width, float height, Renderer* renderer) :
        x(x), y(y), width(width), height(height), rot(0), renderer(renderer) {
}

Entity::~Entity() {
    delete renderer;
}

void Entity::update(float dt) {
    // rot += dt * 2;
}

void Entity::render(GLuint shaderProgram) {
    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, glm::vec3(x, y, 0));
    model = glm::rotate(model, rot, glm::vec3(0, 0, 1));
    model = glm::scale(model, glm::vec3(width, height, 1));

    renderer->render(shaderProgram, model);
}

bool Entity::isColliding(Entity* other) {
    if(x + width / 2 > other->x - other->width / 2 && x - width / 2 < other->x + width / 2 &&
            y + height / 2 > other->y - other->height / 2 && y - height / 2 < other->y + height / 2) {
        return true;
    } else {
        return false;
    }
}