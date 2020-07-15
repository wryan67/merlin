#include "ColorSelector.h"

namespace Games {
    ColorSelector::ColorSelector() {
        gameName = "Color Selector";
        gameWav = "selectcolors.wav";
    }

    void ColorSelector::announceGame() {
        clearBoard();
        playWav(gameWav, false);
    }

    void ColorSelector::announceSameGame() {
        clearBoard();
        playWav("samegame.wav", false);
    }

    void ColorSelector::restartGame() {
        printf("%s -- brightness=%d\n", gameName, brightness);

        customColors.clear();
        palletMap.clear();
        int i = 0;
        palletMap[i++] = -1;  // 0 off

        palletMap[i++] = 0;   //1 Solid Green
        palletMap[i++] = 32;  //2 lime
        palletMap[i++] = 63;  //3 orange
        palletMap[i++] = 85;  //4 Solid red
        palletMap[i++] = 127; //5 magenta
        palletMap[i++] = 168; //6 blue
        palletMap[i++] = 212; //7 cyan
        palletMap[i++] = 226; //8 turquise
        palletMap[i++] = 256; //9 white

        palletMap[i++] = -1;  //10 off

        for (pair<int, int> e : palletMap) {
            setPixelColor(e.first, e.second);
        }

        render();

        eSpeak("select player color");
        isActive = true;
    }

    bool ColorSelector::isTaken(int button) {
        if (mapContainsValue(customColors, palletMap[button])) {
            isActive = false;
            playWav("buzzer.wav", true);
            isActive = true;
            return true;
        }
        return false;
    }

    void ColorSelector::keypadButtonReleased(int button) {
        if (debug) fprintf(stderr, "Magic Square -- key pressed:  %d\n", button);
        setPixelColor(button, -1);
        render();
        if (button < 1 || button > 9) {
            isActive = false;
            playWav("buzzer.wav", true);
            isActive = true;
            return;
        }

        if (customColors.size() < 1) {
            customColors[PLAYER] = palletMap[button];
            playerColor = customColors[PLAYER];
            eSpeak("select computer color");
            return;
        }

        if (customColors.size() < 2) {
            if (isTaken(button)) {
                return;
            }
            customColors[COMPUTER] = palletMap[button];
            computerColor = customColors[COMPUTER];
            eSpeak("select flash color");
            return;
        }

        if (isTaken(button)) {
            return;
        }

        keyFlashColor = palletMap[button];
        startGame(0);
    }
}