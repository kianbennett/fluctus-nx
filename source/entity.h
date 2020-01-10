#pragma once

#include <EGL/egl.h>    // EGL library
#include <EGL/eglext.h> // EGL extensions
#include <glad/glad.h>  // glad library (OpenGL loader)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "renderer.h"

enum CollisionBehaviour { RESET, SCORE };

class Entity {
public:
    float x, y;
    float width, height;
    float rot;
    bool up;
    Renderer* renderer;
    CollisionBehaviour collisionBehaviouor;

    Entity(float x, float y, float width, float height, Renderer* renderer);
    ~Entity();

    virtual void update(float dt);
    void render(GLuint shaderProgram);
    bool isColliding(Entity* other);

    // virtual void onCollideWithPlayer(Level* level) {}
};

class EntityPlayer : public Entity {
public:
    bool isFlipping;
    float initY, initRot;
    EntityPlayer() : Entity(200, 360.0f, 100, 100, new QuadRenderer(glm::vec4(0.51f, 0.76f, 0.89f, 1.0f))) {
        initY = y;
        y = initY + 200.0f - 50.0f;
        up = false;
    }

    void update(float dt) {
        float targetY = initY + (200.0f - 50.0f) * (up ? -1 : 1);
        float flipSpeed = 1500;
        if(isFlipping) {
            if(up) {
                y -= dt * flipSpeed;
                if(y <= targetY) {
                    isFlipping = false;
                    y = targetY;
                }
            } else {
                y += dt * flipSpeed;
                if(y >= targetY) {
                    isFlipping = false;
                    y = targetY;
                }
            }

            float dist = y - initY;
            rot = initRot + 90 + dist / 150.0f * 90;
            rot /= 180.0f / 3.14159f; // convert to radians
        } else {
            y = targetY;
            rot = initRot;
        }
    }

    void flip() {
        if (isFlipping) return;
        up = !up;
        isFlipping = true;
    }
};

class EntitySpike : public Entity {
public:
    EntitySpike() : Entity(1380.0f, 0, 100, 100, new TriRenderer(glm::vec4(0.8f, 0.8f, 0.8f, 1.0f))) {
        collisionBehaviouor = RESET;
    }
};

class EntityCoin : public Entity {
public:
    EntityCoin() : Entity(1380.0f, 0, 50, 50, new QuadRenderer(glm::vec4(1.0f, 0.83f, 0.0f, 1.0f))) {
        collisionBehaviouor = SCORE;
    }

    void update(float dt) {
        Entity::update(dt);
        rot += dt * 5;
    }
};