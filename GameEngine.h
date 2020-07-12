#pragma once
#include <mcp23x17rpi.h>
#include <neopixel.h>
#include <unordered_map>
#include <vector>
#include <alsa/asoundlib.h>
#include "tones.h"
#include <neopixel.h>


#include "Common.h"
#include "pthread.h"

using namespace std;

namespace Games {




    class GameEngine {
    protected:
        wavHeaderType wavHeader;
        int sampleRate = 48000;

        bool keyTonesAudible[MERLIN_LIGHTS] = {
//          0     1     2     3     4     5     6     7     8     9     10
            true, true, true, true, true, true, true, true, true, true, true
        };

        MCP23x17_GPIO keypadButton[MERLIN_LIGHTS];

        snd_pcm_t* keypadSoundHandle[MERLIN_LIGHTS];

        float noteHz[maxNotes] = {
          0,        // rest
          195.9977, // 1  G3
          261.6256, // 3  C4
          293.6648, // 4  D4
          329.6276, // 5  E4
          349.2282, // 6  F4
          391.9954, // 7  G4
          440.0000, // 8  A4
          493.8833, // 9  B4
          523.2511, // 10 C5
          587.3295, // 11 D5
        };

        GameEngine();
        virtual ~GameEngine();

        int pixelColor[MERLIN_LIGHTS];   // color of each pixel
        int pixelState[MERLIN_LIGHTS];   // state of keypad pixel
        unordered_map<int, int> pixelMap;

        void initWavHeader();
        void render();
        void swapState(int i);
        void playAchivement();


        const char* gameName;
        const char* gameWav;


    public:
        virtual void announceGame();
        virtual void keypadPressed(int button);
        pthread_t threadCreate(void* (Games::GameEngine::*)(void*), const char* description, void* args);
        virtual void restartGame();

        void initPixels();

        bool isActive;

        void setPixelColor(int button, int color);

        void keypadButtonActivation(MCP23x17_GPIO gpio, int value);
    };
}