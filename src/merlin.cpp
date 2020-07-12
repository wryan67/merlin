#include "Common.h"

#include <wiringPi.h>
#include <neopixel.h>

#include "games/AllGames.h"

using namespace Games;


int brightness = 10;

int mcp23x17_x20_handle = -1;
int mcp23x17_x20_address = 0x20;
int mcp23x17_x20_inta_pin;
int mcp23x17_x20_intb_pin;

MCP23x17_GPIO newGameButton;
MCP23x17_GPIO hitMeButton;
MCP23x17_GPIO sameGameButton;
MCP23x17_GPIO computerTurnButton;

bool debug=false;


#define STRIP_TYPE              WS2811_STRIP_RGB
#define TARGET_FREQ             WS2811_TARGET_FREQ
#define GPIO_PIN                18   // BCM numbering system
#define DMA                     10   // DMA=Direct Memory Access
int led_count = MERLIN_LIGHTS;  // number of pixels in your led strip

// sound card stuff
char* soundCardName;


int currentGame=0;

NewGame game0;
TicTacToe game1;
MagicSquare game2;
MagicSquare game3;
MagicSquare game4;
MagicSquare game5;
MagicSquare game6;

GameEngine* games[7] = {
    &game0,
    &game1,
    &game2,
    &game3,
    &game4,
    &game5,
    &game6,
};






void keypadButtonActivation(MCP23x17_GPIO gpio, int value) {
    printf("keypad button activation\n");

    games[currentGame]->keypadButtonActivation(gpio, value);

}





void sameGameActivation(MCP23x17_GPIO gpio, int value) {
    if (value != 0) {
        return;
    }
    char cmd[256];
    printf("SameGame Button pressed\n");
    sprintf(cmd,"play %s/projects/merlin/wav/samegame.wav 2> /dev/null &",getenv("HOME"));
    system(cmd);
    games[currentGame]->restartGame();
}

void newGameActivation(MCP23x17_GPIO gpio, int value) {
    if (value != 0) {
        return;
    }
    games[currentGame]->isActive = false;
    startGame(0);
}



int envGetInteger(const char* var, const char* format) {
    if (!var) {
        fprintf(stderr, "Could not locate NULL in the environment variables\n");
        exit(EXIT_FAILURE);
    }
    char* buf = getenv(var);
    if (buf) {
        int value;
        int offset = 0;
        if (strcmp(format, "%x") == 0 && strncmp(buf, "0x", 2) == 0) {
            offset += 2;
        }
        sscanf(&buf[offset], format, &value);
        return value;
    }
    else {
        fprintf(stderr, "Could not locate '%s' in the environment variables\n", var);
        exit(EXIT_FAILURE);
    }
}



bool setup() {
    int rc;
    rc = neopixel_init(STRIP_TYPE, WS2811_TARGET_FREQ, DMA, GPIO_PIN, led_count + 10);

    if (rc != 0) {
        fprintf(stderr, "neopixel initialization failed: %s\n", neopixel_error(rc));
        return false;
    }


    mcp23x17_setDebug(false);
    
    int seed;
    FILE* fp;
    fp = fopen("/dev/urandom", "r");
    fread(&seed, sizeof(seed), 1, fp);
    fclose(fp);
    srand(seed);

    rc=wiringPiSetup();
    if (rc != 0) {
        fprintf(stderr, "WiringPi setup failed\n");
        return false;
    }

    mcp23x17_x20_address = envGetInteger("MCP23017_ADDRESS", "%x");
    mcp23x17_x20_inta_pin = envGetInteger("MCP23017_INTA_PIN", "%d");
    mcp23x17_x20_intb_pin = envGetInteger("MCP23017_INTB_PIN", "%d");

    fprintf(stderr, "mcp23x17 address=%02x; int-a=%d int-b=%d\n", 
        mcp23x17_x20_address, mcp23x17_x20_inta_pin, mcp23x17_x20_intb_pin);

    mcp23x17_x20_handle = mcp23x17_setup(0, mcp23x17_x20_address, mcp23x17_x20_inta_pin, mcp23x17_x20_intb_pin);
    if (mcp23x17_x20_handle < 0) {
        fprintf(stderr, "mcp23017 at address 0x20 could not be initialized\n");
        return false;
    }

    for (int i = 0; i < 11; ++i) {
        char name[32];
        sprintf(name, "KEYPAD_%d", i);
        MCP23x17_GPIO button = getEnvMCP23x17_GPIO(name);
        printf("keypad key=%d config:  port=%c  pin=%d\n", i, mcp23x17_getPort(button) + 'A', mcp23x17_getPin(button));

        mcp23x17_setPinInputMode(button, TRUE, keypadButtonActivation);
    }

    newGameButton = getEnvMCP23x17_GPIO("NEW_GAME");
    hitMeButton = getEnvMCP23x17_GPIO("HIT_ME");
    sameGameButton = getEnvMCP23x17_GPIO("SAME_GAME");
    computerTurnButton = getEnvMCP23x17_GPIO("COMP_TURN");

    mcp23x17_setPinInputMode(sameGameButton, TRUE, sameGameActivation);
    mcp23x17_setPinInputMode(newGameButton,  TRUE, newGameActivation);

    return true;
}


bool usage() {
    fprintf(stderr, "usage: berlin  [-a mcp23017_address] [-b brightness] \n");
    fprintf(stderr, "b = brightness [0-255]; default=16\n");

    return false;
}

bool commandLineOptions(int argc, char** argv) {
    int c, index;

    if (argc < 0) {
        return usage();
    }

    while ((c = getopt(argc, argv, "b:")) != -1)
        switch (c) {
        case 'b':
            sscanf(optarg, "%d", &brightness);
            break;
        case '?':
            if (optopt == 'b')
                fprintf(stderr, "Option -%c requires an argument.\n", optopt);
            else if (isprint(optopt))
                fprintf(stderr, "Unknown option `-%c'.\n", optopt);
            else
                fprintf(stderr, "Unknown option character \\x%x.\n", optopt);

            return usage();
        default:
            abort();
        }

    return true;
}

void startGame(int newGame) {
    printf("startig new game: %d\n", newGame);

    currentGame = newGame;
    games[currentGame]->announceGame();
    games[currentGame]->restartGame();

}

int main(int argc, char** argv) {
    if (!commandLineOptions(argc, argv)) {
        return 2;
    }

    setuid(0);
    if (!setup()) {
        fprintf(stderr, "initializaion failed\n");
        return 2;
    }

    neopixel_setBrightness(brightness);

    games[currentGame]->initPixels();

    startGame(0);

    while (true)
	{
        fflush(stdout);
		delay(250);
	}
    
    neopixel_close();
	return 0;
}