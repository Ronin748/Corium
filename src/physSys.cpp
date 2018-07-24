//
// Created by Ronin748 on 14.02.2018
//

#include <physSys.h>
#include <algorithm>

// fully dynamic
bool fd = false;

void PhysSys::update()
{
        ++ticks;

        finish();

        for (int i = 0; i < objects.size(); ++i) {
                if (!objects[i]->phys.isStatic && objects[i]->phys.inRange) {
                        if (btz(objects[i]->phys.v) || fd) {
                                if (objects[i]->phys.scollision_num > 1)
                                        objects[i]->phys.forces /= objects[i]->phys.scollision_num; // hack
                                if (objects[i]->phys.collision_num > 1) {
                                        objects[i]->phys.collision_normal /= objects[i]->phys.collision_num;
                                        objects[i]->phys.collision_center /= objects[i]->phys.collision_num;
                                }

                                objects[i]->phys.forces -=
                                        objects[i]->phys.v * glm::length(objects[i]->phys.v) * air_resistance;        // air resistance

                                if (objects[i]->phys.isColliding) {
                                        objects[i]->phys.forces -= (objects[i]->phys.v) * friction * objects[i]->phys.m;         // fr
                                }

                                objects[i]->phys.rot_rv = glm::vec3(0.0f);
                                glm::vec3 nt = glm::cross(objects[i]->phys.collision_center, objects[i]->phys.forces);

                                if (btz(nt))
                                        if (nnv(nt)) {
                                                objects[i]->phys.rot_rv = glm::cross((objects[i]->phys.srot_v + 0.0f*objects[i]->phys.rot_a) * objects[i]->phys.I,
                                                                                     objects[i]->phys.collision_center);

                                                objects[i]->phys.torx += nt;
                                                objects[i]->phys.rot += normalize(nt)*glm::length(objects[i]->phys.rot_v);

                                                objects[i]->phys.qrot *= glm::quat(normalize(nt)*glm::length(objects[i]->phys.rot_v));
                                        }

                                if (objects[i]->phys.isGravity) { // gravity doesn't cause torque
                                        objects[i]->phys.forces += field * objects[i]->phys.m;
                                }

                                if (objects[i]->phys.isColliding) {
                                        // damping
                                        objects[i]->phys.rot_v *= 0.8f;
                                        objects[i]->phys.v *= 0.95f;
                                }

                                objects[i]->update();

                                // speed limit
                                if (glm::length(objects[i]->phys.v) > 0.9f*objects[i]->physMesh.boundingSphereRadius) {
                                        objects[i]->phys.v = (0.9f*objects[i]->physMesh.boundingSphereRadius)*glm::normalize(objects[i]->phys.v);
                                }
                        }

                        if (lte(objects[i]->phys.sv) && lte(objects[i]->phys.srot_v) && objects[i]->phys.isColliding && !fd) {
                                objects[i]->phys.v = glm::vec3(0.0f);
                                objects[i]->phys.rot_v = glm::vec3(0.0f);
                        }
                        objects[i]->phys.collision_center = glm::vec3(0.0f);
                        objects[i]->phys.collision_normal = glm::vec3(0.0f);
                        objects[i]->phys.collision_num = 0;
                        objects[i]->phys.scollision_num = 0;
                        objects[i]->phys.isColliding = false;
                }
        }
        if (multithreading) {
                t1 = std::thread(
                        &PhysSys::updateObjs, this, 0, (int) (objects.size() / 2.0f), 0,
                        (int) (objects.size() / 2.0f));
                t2 = std::thread(
                        &PhysSys::updateObjs, this, (int) (objects.size() / 2.0f), objects.size(),
                        (int) (objects.size() / 2.0f), objects.size());

                t3 = std::thread(&PhysSys::updateObjsM, this, 0, (int) (objects.size() / 2.0f),
                                 (int) (objects.size() / 2.0f), objects.size());
        }
        else {
                updateObjs(0, objects.size(), 0, objects.size());
        }
}

void PhysSys::updateObjs(int i_min, int i_max, int j_min, int j_max)
{
        for (int i = i_min; i < i_max; ++i) {
                for (int j = std::max(i + 1, j_min); j < j_max; ++j) {
                        // culling of objects
                        bool bst = objects[i]->physMesh.bsi(&objects[j]->physMesh);
                        bool st = !objects[i]->phys.isStatic || !objects[j]->phys.isStatic;
                        bool iz = btz(objects[i]->phys.v) || btz(objects[j]->phys.v) || fd;
                        bool ir = objects[i]->phys.inRange && objects[j]->phys.inRange;

                        if (bst && st && iz && ir) {
                                // triangle intersection test of meshes ~n^2
                                bool isects = objects[i]->physMesh.intersects(&objects[j]->physMesh);

                                if (isects) {
                                        objects[i]->phys.isColliding = true;
                                        objects[j]->phys.isColliding = true;
                                        ++objects[i]->phys.collision_num;
                                        ++objects[j]->phys.collision_num;

                                        glm::vec3 collisionVeci = normalize(objects[i]->physMesh.collision_normal);
                                        glm::vec3 collisionVecj = normalize(objects[j]->physMesh.collision_normal);

                                        glm::vec3 newForce = glm::vec3(0.0f);
                                        glm::vec3 newForcei = glm::vec3(0.0f);
                                        glm::vec3 newForcej = glm::vec3(0.0f);

                                        glm::vec3 rotiv = glm::vec3(0.0f);
                                        glm::vec3 rotjv = glm::vec3(0.0f);

                                        bool ivalid = !objects[i]->phys.isStatic && nnv(collisionVeci);
                                        bool jvalid = !objects[j]->phys.isStatic && nnv(collisionVecj);

                                        if (objects[i]->phys.isStatic) objects[j]->phys.scollision_num += 1;
                                        if (objects[j]->phys.isStatic) objects[i]->phys.scollision_num += 1;

                                        glm::mat3 vecProji = getVecProjMatrix(collisionVeci);
                                        glm::mat3 vecProjj = getVecProjMatrix(collisionVecj);

                                        if (ivalid && nnv(objects[i]->physMesh.collision_center)) {
                                                rotiv = objects[i]->phys.rot_rv*0.5f;
                                        }
                                        if (jvalid && nnv(objects[j]->physMesh.collision_center)) {
                                                rotjv = objects[j]->phys.rot_rv*0.5f;
                                        }

                                        int fnum = 0;
                                        if (ivalid) {
                                                glm::vec3 fi = (objects[i]->phys.v + objects[i]->phys.a) * (objects[i]->phys.m) +
                                                               objects[i]->phys.forces*0.0f + rotiv; // replaced field with a for elegance
                                                newForce += fi;
                                                ++fnum;
                                        }
                                        if (jvalid) {
                                                glm::vec3 fj = (objects[j]->phys.v + objects[j]->phys.a) * (objects[j]->phys.m) +
                                                               objects[j]->phys.forces*0.0f + rotjv;
                                                newForce += fj;
                                                ++fnum;
                                        }

                                        newForce *= 1.0f; // bounce

                                        float ijm = 0.5f;
                                        float jim = 0.5f;

                                        newForcei = collisionVeci*glm::length((newForce*ijm)*vecProji);
                                        newForcej = collisionVecj*glm::length((newForce*jim)*vecProjj);

                                        if (ivalid) {
                                                objects[i]->phys.forces += collisionVeci*glm::length(field)*objects[i]->phys.m*0.5f + newForcei;
                                                objects[i]->phys.collision_normal += collisionVeci;

                                                glm::vec3 posi = collisionVeci * (glm::length((objects[i]->phys.v + objects[i]->phys.a)));
                                                glm::vec3 posip = collisionVeci * (glm::length((objects[i]->phys.v + objects[i]->phys.a)*vecProji));
                                                float pr = 1.0f;
                                                float pul = glm::length(posi);
                                                float ppl = glm::length(posip);
                                                if (ppl > 0.0f && pul > 0.0f) {
                                                        pr = std::min(pul/ppl, 2.0f);
                                                }
                                                // position correction
                                                glm::vec3 pc = ((posi/pr)*1.0f + 1.0f*posip)*0.5f;
                                                if (nnv(pc)) {
                                                        objects[i]->phys.pos += pc*0.5f;
                                                        objects[i]->phys.v += collisionVeci*PHYS_EPSILON*1.0f;
                                                }
                                                if (nnv(objects[i]->physMesh.collision_center)) {
                                                        objects[i]->phys.collision_center += objects[i]->physMesh.collision_center;
                                                }
                                        }

                                        if (jvalid) {
                                                objects[j]->phys.forces += collisionVecj*glm::length(field)*objects[j]->phys.m*0.5f + newForcej;
                                                objects[j]->phys.collision_normal += collisionVecj;

                                                glm::vec3 posj = collisionVecj * (glm::length((objects[j]->phys.v + objects[j]->phys.a)));
                                                glm::vec3 posjp = collisionVecj * (glm::length((objects[j]->phys.v + objects[j]->phys.a)*vecProjj));
                                                float pr = 1.0f;
                                                float pul = glm::length(posj);
                                                float ppl = glm::length(posjp);
                                                if (ppl > 0.0f && pul > 0.0f) {
                                                        pr = std::min(pul/ppl, 2.0f);
                                                }
                                                // position correction
                                                glm::vec3 pc = ((posj/pr)*1.0f + 1.0f*posjp)*0.5f;
                                                if (nnv(pc)) {
                                                        objects[j]->phys.pos += pc*0.5f;
                                                        objects[j]->phys.v += collisionVecj*PHYS_EPSILON*1.0f;
                                                }
                                                if (nnv(objects[j]->physMesh.collision_center)) {
                                                        objects[j]->phys.collision_center += objects[j]->physMesh.collision_center;
                                                }
                                        }
                                }
                        }
                }
        }
}