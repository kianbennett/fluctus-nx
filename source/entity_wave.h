#pragma once

#include <cmath>
#include "entity.h"

class EntityWave : public Entity {
public:
    EntityWave(glm::vec4 colour) : Entity(0, 0, 1, 1, new Renderer({}, colour)) {}
    void updateWave(float offset, const float amp, const float freq, const float height);
private:
	float tick = 0;
};