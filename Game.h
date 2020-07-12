#pragma once
#include <mcp23x17rpi.h>
#include <neopixel.h>

#include "Common.h"
#include "pthread.h"

namespace Games {

    class Game {
    protected:

        void initWavHeader();
        void render();
        void randomizeBoard();
        void swapKey(int i);
        void swapState(int i);
        void playAchivement();
        //void* buttonTone(void* args);

    public:
        Game();
        virtual ~Game() {}

        void restartGame();

        bool isActive;

        void setPixelColor(int button, int color);

        void keypadButtonActivation(MCP23x17_GPIO gpio, int value);
    };
}