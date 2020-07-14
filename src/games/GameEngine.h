#pragma once
#include <mcp23x17rpi.h>
#include <neopixel.h>
#include <unordered_map>
#include <vector>

#include <alsa/asoundlib.h>
#include "../audio/tones.h"


#include "../Common.h"

using namespace std;




namespace Games {

#define EMPTY     0
#define HUMAN     1
#define COMPUTER  2
#define CAT       3

    void* buttonTone(float freq, wavHeaderType* wavHeader, snd_pcm_t* soundCardHandle);

    class GameEngine {
    protected:
        bool interruptFlag;
        wavHeaderType wavHeader;
        snd_pcm_t* globalSoundCardHandle;
        int sampleRate = 48000;

        int keyFlashColor = 88;
        int playerColor = 248;
        int computerColor = 168;

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
        void clearBoard();
        virtual ~GameEngine();

        int pixelColor[MERLIN_LIGHTS];   // color of each pixel
        int pixelState[MERLIN_LIGHTS];   // state of keypad pixel
        unordered_map<int, int> pixelMap;

        void initWavHeader();
        void render();
        void playAchivement();
        void playFailed();
        void playWav(const char* filename, bool background);


        const char* gameName;
        const char* gameWav;

        void setPixelColor(int button, int wheelColor);

        virtual void keyTone(int button);
        virtual void keypadButtonReleased(int button);

    public:
        void initPixels();
        virtual void eSpeak(char *message);

        virtual void announceGame();
        virtual void announceSameGame();
        virtual void restartGame();
        virtual void interrupt();
        virtual void computerTurn();
        virtual void hitMe();
        virtual void keypadButtonActivation(MCP23x17_GPIO gpio, int value);

        void printVector(const char* message, vector<int> &cards);


        bool isActive;

    };
}
