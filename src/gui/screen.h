//
// Created by Roninkoi on 22.12.2015.
//

#ifndef CORIUM_SCREEN_H
#define CORIUM_SCREEN_H

#include "gui.h"
#include "quad.h"
#include "render/screenRenderer.h"

class Screen {
public:
GUI gui;

ScreenRenderer scrRndr;

//TextRenderer textRndr;

Quad charQuad;
Quad numQuad;

float ticks = 0.0f;

int gametime = 0;

bool console = false;

float health = 0.0f;
float power = 0.0f;

float hurt = 0.0f;

bool lose = false;
bool win = false;

bool big = false;

std::vector<std::string> text_buffer;

void tick() {
        int timing = 47;
        if (((int)ticks - 35) % timing == 0)
                renderStartText = false;

        if (((int)ticks - 35) % timing > 10)
                renderStartText = true;
}

Quad q1;
Quad q2;

Quad crmlogo;

void drawConsole()
{
        if (console) {
                charQuad.phys.pos.y = -1.2f + 1.9f - 1.23f * (int) big;
                float offs = 0.0f;
                if (!big)
                        offs = 10;
                for (int i = 0; i < text_buffer.size() - offs; ++i) {
                        drawText(text_buffer[i], charQuad);
                        charQuad.phys.pos.y += 0.12f;
                }
        }
}

void drawScreen(Renderer *renderer) {
        gui.ticks = ticks;

        scrRndr.update(renderer);

        gui.drawGUI(&scrRndr);

        drawConsole();

        scrRndr.flushBatch();
}

void drawWinScreen(Renderer *renderer) {
        gui.ticks = ticks;

        scrRndr.update(renderer);

        scrRndr.flushBatch();
}

bool renderStartText = true;

void drawStartScreen(Renderer *renderer) {
        gui.ticks = ticks;

        scrRndr.update(renderer);

        drawConsole();

        scrRndr.flushBatch();
}

void loadScreen() {
        scrRndr.init();
        gui.loadGUI();

        charQuad.loadSprite("gfx/gui.png", glm::vec4(0.0f, 0.0f, 1.0f, -1.0f));
        charQuad.phys.pos.z = -1.2f;
        charQuad.phys.pos.y = -1.2f;
        charQuad.phys.pos.x = -2.26f;
        charQuad.phys.s = glm::vec3(1.0f, 1.8f, 1.0f) * 0.07f;
        charQuad.phys.rotOffs.x = charQuad.phys.s.x * 0.5f;
        charQuad.phys.rotOffs.z = -1.0f;
        charQuad.phys.update();
        charQuad.update();

        numQuad.loadSprite("gfx/gui.png", glm::vec4(0.0f, 0.0f, 1.0f, -1.0f));

        q1.loadSprite("gfx/test2.png", glm::vec4(0.0f, 0.0f, 100.0f, -100.0f));
        q2.loadSprite("gfx/test2.png", glm::vec4(0.0f, 0.0f, 100.0f, -100.0f));

        crmlogo.loadSprite("gfx/misc/Corium_logo.png", glm::vec4(0.0f, 0.0f, 480.0f, -480.0f));
}

void drawLoadingScreen(Renderer *renderer) {
        scrRndr.update(renderer);
        //scrRndr.rts = false;

        Quad loadingQuad = charQuad;

        loadingQuad.phys.pos = glm::vec3(-0.4f, -0.9f, -1.0f);
        loadingQuad.phys.s = glm::vec3(1.0f, 1.8f, 1.0f) * 0.1f;

        drawText("Loading...", loadingQuad);
        loadingQuad.phys.pos.y += 2.1f;
        loadingQuad.phys.pos.x -= 1.0f;
        loadingQuad.phys.pos.z -= 0.5f;
        drawText("Corium engine 1.4 Roninkoi 2017", loadingQuad);

        crmlogo.phys.pos = glm::vec3(-0.5f, -0.5f, -1.0f);
        crmlogo.phys.update();
        crmlogo.update();
        scrRndr.draw(crmlogo.tex, glm::mat4(1.0f), &crmlogo.vertexData, &crmlogo.texData,
                     &crmlogo.colData, &crmlogo.indexData);

        scrRndr.flushBatch();
}

void drawText(std::string s, Quad textQuad) {
        for (int i = 0; i < s.length(); ++i) {
                //if (ticks > 60) {
                //        textQuad.phys.pos.x += (sin(i+ticks/10.0f)*0.01f);
                //        textQuad.phys.rot.z = (sin((i)*std::min(1.0f, ticks)+ticks/10.0f)*0.2f);
                //        textQuad.setCol(glm::vec4(0.5f*(sin(i+ticks/10.0f)+1.0f), 0.5f*(cos(i+ticks/15.0f)+1.0f),
                //                1.0f-0.5f*(sin(i+ticks/10.0f)+1.0f)-0.5f*(cos(i+ticks/15.0f)+1.0f), 1.0f));
                //}
                int x0 = (((int) s[i] - 32)) % 16;
                int y0 = (int) floor(((float) ((int) s[i] - 32)) / 16.0f);

                glm::vec4 textCoord = glm::vec4(6.0f * (float) x0, 11.0f * (float) y0, 6.0f, -11.0f);
                textQuad.loadSprite("", glm::vec4(textCoord.x, textCoord.y - textCoord.w, textCoord.z, textCoord.w), false);
                textQuad.phys.update();
                textQuad.update();
                scrRndr.draw(textQuad.tex, glm::mat4(1.0f), &textQuad.vertexData, &textQuad.texData,
                             &textQuad.colData, &textQuad.indexData);
                textQuad.phys.pos.x += textQuad.phys.s.x;
        }
}

Screen() {

}
};

#endif //CORIUM_SCREEN_H
