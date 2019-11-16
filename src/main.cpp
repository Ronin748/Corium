#include "game/game.h"
#include "util/coriumlang.h"

Game game;

// below are temp functions for testing

void gamePrint(std::string s)
{
        game.print(s);
}

void gameRefresh()
{
        game.refresh();
}

void gameScript(std::string s)
{
        runScript(s, &game);
}

void gameDrawLine(glm::vec3 l0, glm::vec3 l1)
{
        game.renderer.drawLine(l0, l1);
}

int main(int argc, char *argv[])
{
        game.init(); // initializes engine and loads game assets
        game.start(); // launches main game loop

        return 0;
}
