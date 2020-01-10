#pragma once

#include "entity_wave.h"
#include <algorithm>
#include <unordered_set>

class Level {
public:
    float waveAmp, waveFreq, waveHeight, waveSpeed;
    float waveAmpInit, waveFreqInit, waveSpeedInit;
    float spawnIntervalMin, spawnIntervalMax;
    float spawnIntervalMinInit, spawnIntervalMaxInit;

    EntityPlayer* player;
    EntityWave* wave;

    std::vector<Entity*> entities;

    Level() { 
        waveAmp = waveAmpInit = 70.0f;
        waveFreq = waveFreqInit = 0.003f;
        waveSpeed = waveSpeedInit = 800.0f;
        waveHeight = 400.0f;
        spawnIntervalMin = spawnIntervalMinInit = 0.5f;
        spawnIntervalMax = spawnIntervalMaxInit = 1.5f;
    }

    void init();
    void update(float dt);
    void render(GLuint shaderProgram);
    void cleanup();
    void reset();
    void handleDifficulty();
    
    void spawnEntity(Entity* entity);
    void destroyEntity(Entity* entity);

private:
    float waveOffset, spawnTick, spawnIntervalNext, timeElapsed;
    bool lastUp;

    float getWavePointY(float x);
    float getWavePointAngle(float x);
};