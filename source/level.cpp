#include "level.h"

void Level::init() {
    wave = new EntityWave(glm::vec4(0.35f, 0.35f, 0.35f, 1));
    player = new EntityPlayer();
    spawnIntervalNext = spawnIntervalMin;
}

void Level::update(float dt) {
    handleDifficulty();

    wave->update(dt);
    player->update(dt);
    for(int i = 0; i < (int) entities.size(); i++) entities[i]->update(dt);

    timeElapsed += dt;
    waveOffset += dt * waveSpeed;
    wave->updateWave(waveOffset, waveAmp, waveFreq, waveHeight);

    spawnTick += dt;
    if(spawnTick >= spawnIntervalNext) {
        // Set the next spawn interval as a random value between max and min
        spawnIntervalNext = spawnIntervalMin + ((rand() % (int) ((spawnIntervalMax - spawnIntervalMin) * 100.0f)) / 100.0f); // 0.5 - 1.5s
        spawnTick = 0;
        // 1 in 6 chance of spawning a coin instead of a spike
        int r = rand() % 6;
        spawnEntity(r ? (Entity*) new EntitySpike() : (Entity*) new EntityCoin());
    }

    player->initY = getWavePointY(player->x);
    player->initRot = getWavePointAngle(player->x);

    for(int i = (int) entities.size() - 1; i >= 0; i--) {
        // Move entities to the left at the same speed as the wave
        entities[i]->x -= waveSpeed * dt;
        // Destroy entity if it goes too far to the left
        if(entities[i]->x < -100) {
            destroyEntity(entities[i]);
            entities.erase(entities.begin() + i);
        }
        if(player->isColliding(entities[i])) {
            switch (entities[i]->collisionBehaviouor) {
                case RESET:
                    reset();
                    return;
                    break;
                case SCORE:
                    destroyEntity(entities[i]);
                    entities.erase(entities.begin() + i);
                    break;
            }
        }
    }
}

void Level::render(GLuint shaderProgram) {
    wave->render(shaderProgram);
    player->render(shaderProgram);
    for(int i = 0; i < (int) entities.size(); i++) entities[i]->render(shaderProgram);
}

void Level::cleanup() {
    destroyEntity(wave);
    destroyEntity(player);
    for(int i = 0; i < (int) entities.size(); i++) destroyEntity(entities[i]);
    entities.clear();
}

void Level::reset() {
    waveSpeed = waveSpeedInit;
    spawnIntervalMin = spawnIntervalMinInit;
    spawnIntervalMax = spawnIntervalMaxInit;
    waveAmp = waveAmpInit;
    waveFreq = waveFreqInit;
    timeElapsed = 0;
    spawnTick = 0;
    waveOffset = 0;
    cleanup();
    init();
}

void Level::handleDifficulty() {
    if(timeElapsed > 1) { 
        waveSpeed = std::clamp(waveSpeedInit + (timeElapsed * 5), 0.0f, 1600.0f);
        spawnIntervalMin = std::clamp(spawnIntervalMinInit - (timeElapsed / 60), 0.15f, FLT_MAX);
        spawnIntervalMax = std::clamp(spawnIntervalMaxInit - (timeElapsed / 50), 0.4f, FLT_MAX);
        waveAmp = std::clamp(waveAmpInit + (timeElapsed / 4.0f), 0.0f, 100.0f);
        // waveFreq = std::clamp(waveFreqInit + (timeElapsed * 0.00005f), 0.0f, 0.005f);
    }
}

void Level::spawnEntity(Entity* entity) {
    // If last was up, only 1/3 chance of being up again, to avoid having too many on one side in a row
    if(lastUp) {
        entity->up = !(rand() % 4); 
    } else {
        entity->up = rand() % 4;
    }
    lastUp = entity->up;

    entity->rot = getWavePointAngle(entity->x);
    entity->y = getWavePointY(entity->x) + (waveHeight / 2 - 50) * (entity->up ? -1 : 1);
    if(!entity->up) entity->height *= -1;
    entities.push_back(entity);
}

void Level::destroyEntity(Entity* entity) {
    // printf("Destroy entity\n");
    delete entity;
}

float Level::getWavePointY(float x) {
    return 720.0f / 2 + waveAmp * sin(waveFreq * (x - 21 + waveOffset));
}

// Get tangent angle by getting the angle between two points on the curve slightly apart
float Level::getWavePointAngle(float x) {
    float x1 = x - 1;
    float y1 = getWavePointY(x1);
    float x2 = x + 1;
    float y2 = getWavePointY(x2);
    return atan((y2 - y1) / (x2 - x1));
}