#ifndef CORIUM_CRAB_H
#define CORIUM_CRAB_H

class Crab : public Entity {
public:
Anims anims;

void think()
{
        if (closestEnemy != NULL) {
                float dist = vecDiff(phys.pos, closestEnemy->phys.pos);
                if (dist <= attackRange)
                        attack(closestEnemy);
                else if (dist <= chaseRange)
                        chase(closestEnemy);
                //gamePrint(toString(dist));
        }
}

void chase(Entity *another)
{
        glm::vec3 dv = vecFrom(phys.pos, another->phys.rpos + glm::vec3(0.0f, -2.0f, 0.0f));
        phys.v = spd*normalize(dv);
        dv = glm::vec3(dv.x, 0.0f, dv.z);
        dv = normalize(dv);
        phys.rot.y = -acos(glm::dot(dv, glm::vec3(1.0f, 0.0f, 0.0f))) + M_PI / 2.0f;
        if (dv.z < 0.0f) phys.rot.y = -phys.rot.y + M_PI;
        phys.setRot(phys.rot);
}

void attack(Entity *another)
{

}

void tick()
{
        phys.v = glm::vec3(0.0f);

        //think();

        anims.anims[0].interpolation = 0;
        anims.anims[0].spd = 0.2f;
        anims.phys = phys;
        anims.tick();

        update(); // object?
}

void load()
{
        anims.clearAnims();
        anims.loadTemplate("gfx/models/anim/crab/crab_0.obj", "gfx/entity.png");
        anims.addAnim("gfx/models/anim/crab/crab_", 3, 0.1f, {1, 0, 2, 0});
        spd = 0.1f;
        chaseRange = 100.0f;
        attackRange = 10.0f;
}

void draw(Renderer *renderer)
{
        anims.draw(renderer);
}

void drawShadows(Renderer *renderer)
{
        anims.drawShadows(renderer);
}

Crab() : Entity()
{

}
};

#endif // CORIUM_CRAB_H
