// ---------------------------------------------------------------------------
// game state managment demo with framerate
// Usage:		press R to restart the level
//				press esc to quit
// ---------------------------------------------------------------------------



#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include "game.h"
#include <iostream>

sf::RenderWindow window;


sf::Clock clock1;
double frametime = 0;
long	framenumber = 0;
// key manager, for one time pressing
bool Rdown = false;


// game state list
enum { LEVEL1 = 0, RESTART, QUIT };
// variables to keep track the current, previous and next game state
unsigned int	gGameStateInit;
unsigned int	gGameStateCurr;
unsigned int	gGameStatePrev;
unsigned int	gGameStateNext;


int main()
{
    // create window
    window.create(sf::VideoMode(1280, 720), "Game");
    window.setFramerateLimit(60);

    // Initialize Game State (to level 1)
    gGameStateInit = LEVEL1;
    gGameStateCurr = gGameStateInit;
    gGameStatePrev = gGameStateInit;
    gGameStateNext = gGameStateInit;


    // app loop
    while (gGameStateCurr != QUIT)
    {
        if (gGameStateCurr == RESTART) {
            gGameStateCurr = gGameStatePrev;
            gGameStateNext = gGameStateCurr;
        }

        GameInit();
        framenumber = 0;

        // game loop
        while (gGameStateCurr == gGameStateNext) {

            // measure fps
            sf::Time elapsed1 = clock1.getElapsedTime();
            frametime = elapsed1.asSeconds();
            clock1.restart();
            framenumber++;

            int state = 0;

            // handle event
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    window.close();
            }

            // Check if User want to restart level
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::R) && !Rdown) {
                gGameStateNext = RESTART;
                Rdown = true;
            }
            if (!sf::Keyboard::isKeyPressed(sf::Keyboard::W) && Rdown) { Rdown = false; }

            // Check if the ESC key was pressed or the window was closed
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) || !window.isOpen()) {
                gGameStateNext = QUIT;
            }


            GameUpdate(frametime, framenumber, state);
            GameDraw(frametime);

            // Check return state from Update()
            if (state == 2) {
                gGameStateNext = RESTART;
            }



        }

        GameClean();

        gGameStatePrev = gGameStateCurr;
        gGameStateCurr = gGameStateNext;

    }


    return 0;
}