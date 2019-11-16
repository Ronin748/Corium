//
// Created by Roninkoi on 21.12.2015.
//

#ifndef CORIUM_PHYS_H
#define CORIUM_PHYS_H

#include "corium.h"

const float PHYS_EPSILON = 0.005f;

class Phys {
public:
glm::vec3 pos = glm::vec3(0.0f);
glm::vec3 posold = glm::vec3(0.0f);
glm::vec3 sv = glm::vec3(0.0f); // slow v for sleeping

glm::vec3 rot = glm::vec3(0.0f);
glm::fquat qrot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // quaternion rep
glm::fquat qrrot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

glm::vec3 srotV = glm::vec3(0.0f); // slow rotv

glm::vec3 rotV = glm::vec3(0.0f);
glm::vec3 rotA = glm::vec3(0.0f);

glm::vec3 rotRV = glm::vec3(0.0f);

glm::vec3 s = glm::vec3(1.0f);

glm::vec3 rpos = glm::vec3(0.0f);
glm::vec3 rrot = glm::vec3(0.0f);

glm::vec3 rotOffs = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 v = glm::vec3(0.0f);
glm::vec3 a = glm::vec3(0.0f);
glm::vec3 forces = glm::vec3(0.0f);
glm::vec3 forcesold = glm::vec3(0.0f);

glm::vec3 torx = glm::vec3(0.0f);
glm::vec3 torxold = glm::vec3(0.0f);

float I = 1.0f;
glm::vec3 collisionCenter = glm::vec3(0.0f);
glm::vec3 collisionNormal = glm::vec3(0.0f);

bool inRange = true;

bool isColliding = false;
int collisionNum = 0;
int scollisionNum = 0;

glm::vec3 p = glm::vec3(0.0f);
float m = 1.0f;
glm::vec3 mc = glm::vec3(0.0f); // mass center not used

bool isStatic = false;
bool isSemiStatic = false;

bool isGravity = true;

void update();

void tick();

void applyForce(glm::vec3 force);

void setRot(glm::vec3 r);

glm::mat4 getMatrix();
glm::mat4 getRMatrix();

Phys()
{
}
};

// build vector projection matrix
glm::mat3 getVecProjMatrix(glm::vec3 vector)
{
        glm::mat3 vecProj = glm::mat3(1.0f);

        // projection matrix
        vecProj[0] = glm::vec3(vector.x * vector.x, vector.x * vector.y,vector.x * vector.z);
        vecProj[1] = glm::vec3(vector.y * vector.x, vector.y * vector.y,vector.y * vector.z);
        vecProj[2] = glm::vec3(vector.z * vector.x, vector.z * vector.y,vector.z * vector.z);

        return vecProj;
}

// bigger than zero
inline bool btz(glm::vec3 vector)
{
        if (glm::length(vector) > 0.0f)
                return true;
        else
                return false;
}

// bigger than epsilon
inline bool bte(glm::vec3 vector)
{
        if (glm::length(vector) > PHYS_EPSILON)
                return true;
        else
                return false;
}

// less than epsilon
inline bool lte(glm::vec3 vector)
{
        if (glm::length(vector) <= PHYS_EPSILON)
                return true;
        else
                return false;
}

// not null

template<typename nullval>

bool nn(const nullval &value)
{
        if (!(value != value)) {
                return true;
        }
        else
                return false;
}

inline bool nnv(glm::vec3 value)
{
        return !(std::isnan(value.x)) && !(std::isnan(value.y)) && !(std::isnan(value.z));
        /*if (!(value != value)) {
                return true;
           }
           else
                return false;*/
}

// glm normalize returns nan if 0
glm::vec3 normalize(glm::vec3 v)
{
        if (glm::length(v) > 0) {
                return glm::normalize(v);
        }
        else {
                return v;
        }
}

// distance between vectors
inline float vecDiff(glm::vec3 v1, glm::vec3 v2)
{
        return glm::length(v1 - v2);
}

// vector from v1 to v2
inline glm::vec3 vecFrom(glm::vec3 v1, glm::vec3 v2)
{
        return (v2 - v1);
}

#endif //CORIUM_PHYS_H
