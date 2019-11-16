//
// Created by Roninkoi on 2.2.2016.
//

#ifndef CORIUM_SYS_H
#define CORIUM_SYS_H

#include <thread>
#include <mutex>
#include <mingw.thread.h>
#include <mingw.mutex.h>

#include "game/obj/obj.h"
#include "mesh.h"

class Sys {
public:
std::vector<Obj *> objects;

glm::vec3 field;

float air_resistance = 0.1f;
float collision_energy_loss = 1.0f;
float friction = 0.1f;

float bouncyness = 0.15f;         // 0.17

std::mutex mtx;

int collisions = 0;

// fully dynamic
bool fd = true;

void updateObjs(int i_min, int i_max, int j_min, int j_max);

void updateObjsM(int i_min, int i_max, int j_min, int j_max)
{
        mtx.lock();
        updateObjs(i_min, i_max, j_min, j_max);
        mtx.unlock();
}

// be careful with threads
bool multithreading = false;

std::thread t1;
std::thread t2;
std::thread t3;

int ticks = 0;

void finish()
{
        if (multithreading) {
                if (t1.joinable())
                        t1.join();
                if (t2.joinable())
                        t2.join();
                if (t3.joinable())
                        t3.join();
        }
}

void update();

Sys()
{
        field = glm::vec3(0.0f);
}
};

#endif //CORIUM_SYS_H
