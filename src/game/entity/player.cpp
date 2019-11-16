//
// Created by Roninkoi on 18.12.2015.
//

#include "player.h"
#include "util/objParser.h"

// do this using quaternions
bool camMoved = false;
float camTicks = 0.0f;
void Player::getPlayerCam()
{
        camTicks += 1.0f;
        if (glm::length(phys.rotV) > 0.0f) {
                camMoved = true;
                camTicks = 0.0f;
        }
        if (camTicks > 180.0f) { // 3 seconds
                camMoved = false;
                camTicks = 0.0f;
        }

        // clamp camera
        if (phys.rot.x > M_PI*0.5f) phys.rot.x = M_PI*0.5f;
        if (phys.rot.x < -M_PI*0.5f) phys.rot.x = -M_PI*0.5f;

        phys.rot.y = phys.rot.y - floor(phys.rot.y/(M_PI*2.0f))*M_PI*2.0f;

        if (camera->camAI && !flying) {
                if (camMoved) {
                        camera->rot = phys.rot;
                }
                else if (moving) {
                        float lma = -lastMovAngle + M_PI;

                        if (fabs(lma - camera->rot.y) > M_PI) // discontinuity
                                camera->rot.y += 2 * M_PI * ((lma - camera->rot.y) / fabs(lma - camera->rot.y));

                        if (fabs(camera->rot.y-lma)-0.6f < M_PI*0.5f) {
                                float r = cos(std::max(0.0f, fabs(camera->rot.y-lma)-0.6f))*0.05f;
                                camera->rot.y = camera->rot.y * (1.0f-r) + (r) * lma;
                        }
                        phys.rot.y = camera->rot.y;
                        phys.rot.x = camera->rot.x;
                }
                if (!camMoved && (moving || glm::length(phys.v) > 0.1f))
                        camera->rot.x = (asin(playerLastMov.y) * 1.0f + 0.4f) * 0.05f + 0.95f * camera->rot.x;
        }
        else {
                camera->rot = phys.rot;
        }
        camera->pos = camera->pos*0.7f -renderPos*0.3f;
}

void Player::getInput(Input *input)
{
        phys.rotV = glm::vec3(0.0f);

        if (input->focused) phys.rotV.y = (float) input->mousePosDiff.x / (1000.0f / input->mouseSpd);
        if (input->focused) phys.rotV.x = (float) input->mousePosDiff.y / (1000.0f / input->mouseSpd);

        int count;
        const float *axes = glfwGetJoystickAxes(input->jsnum, &count);

        if (count >= 4) {
                float ax = axes[2] * 0.05f;
                float ay = axes[3] * 0.05f;

                if (fabs(ax) < 0.01f)
                        ax = 0.0f;
                if (fabs(ay) < 0.01f)
                        ay = 0.0f;

                phys.rotV.y += ax;
                phys.rotV.x += ay;
        }

        if (input->getKey(GLFW_KEY_LEFT)) {
                phys.rotV.y = -0.05;
        }
        if (input->getKey(GLFW_KEY_RIGHT)) {
                phys.rotV.y = 0.05;
        }
        if (input->getKey(GLFW_KEY_UP)) {
                phys.rotV.x = -0.05;
        }
        if (input->getKey(GLFW_KEY_DOWN)) {
                phys.rotV.x = 0.05;
        }

        //getPlayerCam();

        moving = false;
        playerMov = phys.v;

        if (onGround || flying) {
                if (input->keyDown[65]) {
                        phys.v.z += -0.05 * cos(phys.rot.y - 3.14159 / 2.0) * spd * (1.0f + (int) flying * 3);
                        phys.v.x += 0.05 * sin(phys.rot.y - 3.14159 / 2.0) * spd * (1.0f + (int) flying * 3);
                        moving = true;
                }
                if (input->keyDown[68]) {
                        phys.v.z += 0.05 * cos(phys.rot.y - 3.14159 / 2.0) * spd * (1.0f + (int) flying * 3);
                        phys.v.x += -0.05 * sin(phys.rot.y - 3.14159 / 2.0) * spd * (1.0f + (int) flying * 3);
                        moving = true;
                }
                if (input->keyDown[87]) {
                        phys.v.z += -0.05 * cos(phys.rot.y) * spd * (1.0f + (int) flying * 3);
                        phys.v.x += 0.05 * sin(phys.rot.y) * spd * (1.0f + (int) flying * 3);
                        moving = true;
                }
                if (input->keyDown[83]) {
                        phys.v.z += 0.05 * cos(phys.rot.y) * spd * (1.0f + (int) flying * 3);
                        phys.v.x += -0.05 * sin(phys.rot.y) * spd * (1.0f + (int) flying * 3);
                        moving = true;
                }
                if (count >= 2) {
                        float ax = axes[0];
                        float ay = axes[1];

                        if (fabs(ax) < 0.2f)
                                ax = 0.0f;
                        if (fabs(ay) < 0.2f)
                                ay = 0.0f;
                        if (fabs(ax) > 0.0f || fabs(ay) > 0.0f) {
                                moving = true;
                                phys.v.z += 0.05 * cos(phys.rot.y - 3.14159 / 2.0) * spd * (1.0f + (int) flying * 3) * ax;
                                phys.v.x += -0.05 * sin(phys.rot.y - 3.14159 / 2.0) * spd * (1.0f + (int) flying * 3) * ax;

                                phys.v.z += 0.05 * cos(phys.rot.y) * spd * (1.0f + (int) flying * 3) * ay;
                                phys.v.x += -0.05 * sin(phys.rot.y) * spd * (1.0f + (int) flying * 3) * ay;
                        }
                }
        }

        if (input->keyDown[70] && flying) {
                phys.v.y += -0.05 * spd * (1.0f + (int) flying * 3);
        }
        if (input->keyDown[82] && flying) {
                phys.v.y += 0.05 * spd * (1.0f + (int) flying * 3);
        }

        playerMov -= phys.v;
}

void Player::loadPlayer()
{
        loadMesh("gfx/models/ico.obj", "gfx/models/icos.obj");
        clipMesh = loadObj("gfx/models/ico.obj");

        playerAnims.clearAnims();
        playerAnims.loadTemplate("gfx/models/anim/dragon/dragon_3.obj", "gfx/dragon.png");
        playerAnims.addAnim("gfx/models/anim/dragon/dragon_", 5, 0.09f, {0});
        playerAnims.addAnim("gfx/models/anim/dragon/dragonflame", 3, 0.05f, {0, 1, 2});
        playerAnims.addAnim("gfx/models/anim/dragon/dragon_4.obj", 0, 0.05f, {0});
}

void Player::restartPlayer()
{
        health = 100.0f;

        phys.v = glm::vec3(0.0f);

        phys.pos = startPos;
        phys.rot = startRot;
        renderPos = startPos;
}

void Player::draw(Renderer *renderer)
{
        playerAnims.draw(renderer);
        //physMesh.draw(renderer, &test);
}

void Player::drawShadows(Renderer *renderer)
{
        playerAnims.drawShadows(renderer);
}

void Player::collide(Sys *ps)
{
        physMesh.objMatrix = glm::mat4(1.0f);
        physMesh.objMatrix = glm::translate(physMesh.objMatrix, phys.pos);
        physMesh.objMatrix = glm::translate(physMesh.objMatrix, glm::vec3(0.0f, -1.5f+0.5f, 0.0f));
        physMesh.objMatrix = glm::rotate(physMesh.objMatrix, lastMovAngle, glm::vec3(0.0f, 1.0f, 0.0f));
        physMesh.objMatrix = glm::scale(physMesh.objMatrix, glm::vec3(1.2f));

        physMesh.update();

        mesh.objMatrix = physMesh.objMatrix;
        mesh.update();

        clipMesh.objMatrix = glm::mat4(1.0f);
        clipMesh.objMatrix = glm::translate(clipMesh.objMatrix, phys.pos);
        clipMesh.objMatrix = glm::translate(clipMesh.objMatrix,
                                            glm::vec3(0.0f, -1.7f, 0.0f) + glm::vec3(0.0f, 0.0f, 0.0f));
        clipMesh.objMatrix = glm::scale(clipMesh.objMatrix, glm::vec3(0.5f, 3.2f, 0.5f));

        clipMesh.update();

        // pm r 1.46
        // cm r 2.33

        if (glm::length(ps->field) > 0.0f && onGround) {
                // friction
                phys.v.x *= 0.8f;
                phys.v.z *= 0.8f;
        }
        else {
                // air resistance
                phys.v *= glm::vec3(0.99f);
        }

        colliding = false;

        onGround = false;

        float bouncyness = 0.01f;

        for (int i = 0; i < ps->objects.size(); ++i) {
                if (clipMesh.bsIsect(&ps->objects[i]->physMesh)) {
                        if (ps->objects[i]->physMesh.sphereIsect(&clipMesh, physMesh.bsCenter + normalize(ps->field)*2.2f, 0.8f)
                            || ps->objects[i]->physMesh.sphereIsect(&clipMesh, physMesh.bsCenter + normalize(ps->field)*1.4f, 0.8f)) {
                                if (!(clipMesh.collisionNormal != clipMesh.collisionNormal)) {
                                        onGround = true;
                                }
                        }
                }
                if (physMesh.bsIsect(&ps->objects[i]->physMesh)) {         // bounding sphere test
                        if (ps->objects[i]->physMesh.sphereIsect(&physMesh, physMesh.bsCenter, 1.7f)) {
                                if (ps->objects[i]->phys.isStatic && !ps->objects[i]->phys.isSemiStatic) { // map geometry only
                                        colliding = true;

                                        // gravity damping
                                        phys.v.y *= 0.2;

                                        glm::vec3 normalVec = glm::normalize(physMesh.collisionNormal);
                                        glm::mat3 vecProj = getVecProjMatrix(normalVec);

                                        glm::vec3 newForce = normalVec * (glm::length((phys.v- playerMov) * vecProj))*phys.m;
                                        glm::vec3 pc = normalVec * (glm::length((phys.v - playerMov)));
                                        if (nnv(pc))
                                                phys.pos += pc;

                                        if (!(newForce != newForce)) {
                                                phys.forces += newForce;
                                        }
                                }
                                else {
                                        ps->objects[i]->phys.isStatic = false;

                                        glm::vec3 normalVec = glm::normalize(physMesh.collisionNormal);
                                        glm::mat3 vecProj = getVecProjMatrix(normalVec);

                                        glm::vec3 newForce = (normalVec)*(glm::length(((phys.v - playerMov) * phys.m + (ps->objects[i]->phys.v) * ps->objects[i]->phys.m) * vecProj));

                                        newForce *= 0.6f;

                                        if (!(newForce != newForce)) {
                                                phys.forces += newForce + 1.0f*normalVec*glm::length(ps->field) *phys.m;

                                                if (sprinting) newForce = newForce * 1.4f;
                                                else newForce = newForce * 0.8f;

                                                ps->objects[i]->phys.forces += -newForce;
                                                glm::vec3 pc = normalVec * (glm::length((phys.v - playerMov)));
                                                if (nnv(pc))
                                                        phys.pos += pc;
                                                phys.v += normalVec*PHYS_EPSILON;
                                                ps->objects[i]->phys.v += -normalVec*PHYS_EPSILON;
                                        }
                                }
                        }
                }
        }

        if (!colliding) phys.forces += ps->field*phys.m;

        float c = std::max(0.0f, std::min(0.95f, glm::length(phys.v)));
        if (glm::length(renderPos - phys.pos) > 0.05f)
                renderPos = phys.pos * (0.05f + c) + renderPos * (0.95f - c);

        phys.rpos = renderPos; // fix this later

        if (glm::length(phys.v) > 0.05f) playerLastMov = -phys.v * 0.07f + playerLastMov * 0.93f;
}

// do player rot diff using quaternions
void Player::tick()
{
        if (health < 0.0f) {
                health = 0.0f;
        }
        if (flying) {
                phys.v.y *= 0.4f;
                phys.v.x *= 0.5f;
                phys.v.z *= 0.5f;
                renderPos = phys.pos;
                health = 1000.0f;
        }

        getPlayerCam();

        playerAnims.phys.pos = renderPos;

        lastMovAngle = 0.0f;
        glm::vec3 movvec = glm::vec3(playerLastMov.x, 0.0f, playerLastMov.z);
        if (glm::length(movvec) > 0.0f)
                lastMovAngle = (acos(glm::dot(glm::normalize(movvec), glm::vec3(0.0f, 0.0f, -1.0f))));

        if (playerLastMov.x > 0.0f) {
                lastMovAngle = -lastMovAngle;
        }

        playerAnims.phys.setRot(glm::vec3(0.0f, lastMovAngle, 0.0f));

        playerAnims.phys.pos.y -= 2.5f;

        playerAnims.animRunning[0] = true;
        playerAnims.animRunning[1] = flaming;
        playerAnims.animRunning[2] = sprinting;

        if (moving && !flying) {
                // main running animation
                playerAnims.anims[0].setAnim(0.09f, {0, 1, 2});

                // set animation speed based on player speed
                playerAnims.anims[0].setSpd(0.09f * sqrt(glm::length(playerMov)) * 4.0f);
        }
        else {
                playerAnims.anims[0].setAnim(0.2f, {3});
        }
        if (!onGround) {
                // falling animation
                playerAnims.anims[0].setAnim(0.2f, {1}, true);
        }

        if (flaming) {
                // flaming animation
                playerAnims.anims[1].setAnim(0.05f, {0, 1, 2});
        }
        else {
                playerAnims.anims[1].setAnim(0.1f, {2});
        }

        if (!flying)
                playerAnims.tick();
}
