//
// Created by Roninkoi on 14.02.2018
//

#include "phys/sys.h"

void Sys::update()
{
	++ticks;

	finish();

	for (auto &object : objects) {
		if (!object->phys.isStatic && object->phys.inRange && (btz(object->phys.v) || fd)) {
			if (object->phys.scollisionNum > 1)
				object->phys.forces /= object->phys.scollisionNum; // hack
			if (object->phys.collisionNum > 1) {
				object->phys.collisionNormal /= object->phys.collisionNum;
				object->phys.collisionCenter /= object->phys.collisionNum;
			}

			object->phys.forces -=
					object->phys.v * glm::length(object->phys.v) * air_resistance;        // air resistance

			if (object->phys.isColliding) {
				object->phys.forces -= (object->phys.v) * friction * object->phys.m;         // fr
			}

			object->phys.rotRV = glm::vec3(0.0f);
			glm::vec3 nt = glm::cross(object->phys.collisionCenter, object->phys.forces);

			if (btz(nt))
				if (nnv(nt)) {
					object->phys.rotRV = glm::cross((object->phys.srotV) * object->phys.I,
													object->phys.collisionCenter);

					object->phys.torques += nt;
					object->phys.rot += normalize(nt) * glm::length(object->phys.rotV);
					object->phys.qrot *= glm::quat(normalize(nt) * glm::length(object->phys.rotV));
				}

			if (object->phys.isGravity) { // gravity doesn't cause torque
				object->phys.forces += field * object->phys.m;
			}

			object->update();

			if (object->phys.isColliding) {
				// damping
				object->phys.rotV *= 0.65f;
				object->phys.v *= 0.95f;
			}

			// speed limit
			if (glm::length(object->phys.v) > 0.9f * object->physMesh.bsRadius) {
				object->phys.v = (0.9f * object->physMesh.bsRadius) * glm::normalize(object->phys.v);
			}
		}

		if (lte(object->phys.sv) && lte(object->phys.srotV) && object->phys.isColliding && !fd) {
			object->phys.v = glm::vec3(0.0f);
			object->phys.rotV = glm::vec3(0.0f);
		}
		object->phys.collisionCenter = glm::vec3(0.0f);
		object->phys.collisionNormal = glm::vec3(0.0f);
		object->phys.collisionNum = 0;
		object->phys.scollisionNum = 0;
		object->phys.isColliding = false;
	}

	if (multithreading) {
		t1 = std::thread(
				&Sys::updateObjs, this, 0, (int) (objects.size() / 2.0f), 0,
				(int) (objects.size() / 2.0f));
		t2 = std::thread(
				&Sys::updateObjs, this, (int) (objects.size() / 2.0f), objects.size(),
				(int) (objects.size() / 2.0f), objects.size());

		t3 = std::thread(&Sys::updateObjsM, this, 0, (int) (objects.size() / 2.0f),
						 (int) (objects.size() / 2.0f), objects.size());
	} else {
		updateObjs(0, objects.size(), 0, objects.size());
	}
}

void Sys::updateObjs(int i_min, int i_max, int j_min, int j_max)
{
	for (int i = i_min; i < i_max; ++i) {
		for (int j = std::max(i + 1, j_min); j < j_max; ++j) {
			// culling of objects
			bool bst = objects[i]->physMesh.bsIsect(&objects[j]->physMesh);
			bool st = !objects[i]->phys.isStatic || !objects[j]->phys.isStatic;
			bool iz = btz(objects[i]->phys.v) || btz(objects[j]->phys.v) || fd;
			bool ir = objects[i]->phys.inRange && objects[j]->phys.inRange;

			if (!bst || !st || !iz || !ir)
				continue;

			// triangle intersection test of meshes ~n^2
			bool isects = objects[i]->physMesh.intersects(&objects[j]->physMesh);

			if (!isects)
				continue;

			objects[i]->phys.isColliding = true;
			objects[j]->phys.isColliding = true;
			++objects[i]->phys.collisionNum;
			++objects[j]->phys.collisionNum;

			glm::vec3 collisionVeci = normalize(
					objects[i]->physMesh.collisionNormal - 0.0f * objects[j]->physMesh.collisionNormal * 1.0f);
			glm::vec3 collisionVecj = normalize(objects[j]->physMesh.collisionNormal);
			//collisionVecj = -collisionVeci;

			glm::vec3 newForce = glm::vec3(0.0f);
			glm::vec3 newForcei = glm::vec3(0.0f);
			glm::vec3 newForcej = glm::vec3(0.0f);

			glm::vec3 rotiv = glm::vec3(0.0f);
			glm::vec3 rotjv = glm::vec3(0.0f);

			bool ivalid = !objects[i]->phys.isStatic && nnv(collisionVeci);
			bool jvalid = !objects[j]->phys.isStatic && nnv(collisionVecj);

			if (objects[i]->phys.isStatic) objects[j]->phys.scollisionNum += 1;
			if (objects[j]->phys.isStatic) objects[i]->phys.scollisionNum += 1;

			glm::mat3 vecProji = getVecProjMatrix(collisionVeci);
			glm::mat3 vecProjj = getVecProjMatrix(collisionVecj);

			if (ivalid && nnv(objects[i]->physMesh.collisionCenter)) {
				rotiv = objects[i]->phys.rotRV * 1.0f;
			}
			if (jvalid && nnv(objects[j]->physMesh.collisionCenter)) {
				rotjv = objects[j]->phys.rotRV * 1.0f;
			}

			int fnum = 0;
			if (ivalid) {
				glm::vec3 fi =
						(objects[i]->phys.v * 1.0f + 0.0f * (objects[i]->phys.a)) * (objects[i]->phys.m) *
						1.0f +
						objects[i]->phys.forces * 0.0f + rotiv * 0.0f;
				newForce += fi;
				++fnum;
			}
			if (jvalid) {
				glm::vec3 fj =
						(objects[j]->phys.v * 1.0f + 0.0f * (objects[j]->phys.a)) * (objects[j]->phys.m) *
						1.0f +
						objects[j]->phys.forces * 0.0f + rotjv * 0.0f;
				newForce += fj;
				++fnum;
			}

			newForce *= 1.0f;         // bounce

			float ijm = 1.0f;
			float jim = 1.0f;

			if (ivalid && jvalid) {
				ijm = 0.5f;
				jim = 0.5f;
			}

			newForcei = collisionVeci * glm::length((newForce * ijm) * vecProji);
			newForcej = collisionVecj * glm::length((newForce * jim) * vecProjj);

			if (ivalid) {
				objects[i]->phys.forces +=
						collisionVeci * glm::length(field) * objects[i]->phys.m * 1.0f + newForcei;
				objects[i]->phys.collisionNormal += collisionVeci;
				objects[i]->phys.pos -= objects[i]->phys.v * 0.5f;

				glm::vec3 posi = collisionVeci * (glm::length((objects[i]->phys.v + objects[i]->phys.a)));
				glm::vec3 posip =
						collisionVeci * (glm::length((objects[i]->phys.v + objects[i]->phys.a) * vecProji));
				float pr = 1.0f;
				float pul = glm::length(posi);
				float ppl = glm::length(posip);
				if (ppl > 0.0f && pul > 0.0f) {
					pr = std::min(pul / ppl, 2.0f);
				}
				// position correction
				glm::vec3 pc = ((posi / pr) * 1.0f + 1.0f * posip) * 0.5f;
				if (nnv(pc)) {
					objects[i]->phys.pos += pc * 0.5f;
					objects[i]->phys.v += collisionVeci * PHYS_EPSILON * 1.0f;
				}
				if (nnv(objects[i]->physMesh.collisionCenter)) {
					objects[i]->phys.collisionCenter += objects[i]->physMesh.collisionCenter;
				}
			}

			if (jvalid) {
				objects[j]->phys.forces +=
						collisionVecj * glm::length(field) * objects[j]->phys.m * 1.0f + newForcej;
				objects[j]->phys.collisionNormal += collisionVecj;

				glm::vec3 posj = collisionVecj * (glm::length((objects[j]->phys.v + objects[j]->phys.a)));
				glm::vec3 posjp =
						collisionVecj * (glm::length((objects[j]->phys.v + objects[j]->phys.a) * vecProjj));
				float pr = 1.0f;
				float pul = glm::length(posj);
				float ppl = glm::length(posjp);
				if (ppl > 0.0f && pul > 0.0f) {
					pr = std::min(pul / ppl, 2.0f);
				}
				// position correction
				glm::vec3 pc = ((posj / pr) * 1.0f + 1.0f * posjp) * 0.5f;
				if (nnv(pc)) {
					objects[j]->phys.pos += pc * 0.5f;
					objects[j]->phys.v += collisionVecj * PHYS_EPSILON * 1.0f;
				}
				if (nnv(objects[j]->physMesh.collisionCenter)) {
					objects[j]->phys.collisionCenter += objects[j]->physMesh.collisionCenter;
				}
			}
		}
	}
}

