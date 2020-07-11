#include <wiringPi.h>
#include <mcp23x17rpi.h>
#include <stdio.h>
#include <neopixel.h>
#include <unordered_map>
#include <vector>
#include <alsa/asoundlib.h>
#include <math.h>

#include "tones.h"

using namespace std;

#define MERLIN_LIGHTS 11

#define OFF       0
#define RED      88
#define GREEN   248  
#define BLUE    168

int brightness = 4;

int mcp23x17_x20_handle = -1;
int mcp23x17_x20_address = 0x20;
int mcp23x17_x20_inta_pin;
int mcp23x17_x20_intb_pin;

MCP23x17_GPIO keypadButton[MERLIN_LIGHTS];
MCP23x17_GPIO newGameButton;
MCP23x17_GPIO hitMeButton;
MCP23x17_GPIO sameGameButton;
MCP23x17_GPIO computerTurnButton;

bool gameActive = false;

int sampleRate = 48000;

unordered_map<int, int> pixelMap;
int pixelColor[MERLIN_LIGHTS];   // color of each pixel
int pixelState[MERLIN_LIGHTS];   // state of keypad pixel

#define STRIP_TYPE              WS2811_STRIP_RGB
#define TARGET_FREQ             WS2811_TARGET_FREQ
#define GPIO_PIN                18   // BCM numbering system
#define DMA                     10   // DMA=Direct Memory Access
int led_count = MERLIN_LIGHTS;  // number of pixels in your led strip

// sound card stuff
char* soundCardName;

snd_pcm_t* keypadSoundHandle[MERLIN_LIGHTS];

#define maxNotes 11
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

wavHeaderType wavHeader;


pthread_t threadCreate(void* (*method)(void*), const char* description, void *args) {
    pthread_t threadId;
    int status = pthread_create(&threadId, NULL, method, args);
    if (status != 0) {
        printf("%s::thread create failed %d--%s\n", description, status, strerror(errno));
        exit(9);
    }
    pthread_detach(threadId);
    return threadId;
}

int random(int low, int high) {
    double r = (double)rand() / RAND_MAX;

    return low + (r * (1 + high - low));
}

int render() {
    for (int i = 0; i < MERLIN_LIGHTS; ++i) {
        neopixel_setPixel(i, pixelColor[i]);
    }
    neopixel_render();
}

void swapState(int i) {
    if (pixelState[i] == 1) {
        pixelState[i] = 0;
        pixelColor[pixelMap[i]] = neopixel_wheel(BLUE);
    } else {
        pixelState[i] = 1;
        pixelColor[pixelMap[i]] = neopixel_wheel(GREEN);
    }
}

void randomizeBoard() {
    for (int i = 1; i <= 9; ++i) {
        pixelState[i] = random(0, 1);
        pixelColor[pixelMap[i]] = (pixelState[i] == 0) ? neopixel_wheel(BLUE) : neopixel_wheel(GREEN);
    }
    render();
    gameActive = true;
}

void swapKey(int i) {
    //vector<int> todo;

    switch (i) {
    case 1:  swapState(1); swapState(2); swapState(4); swapState(5); break;
    case 3:  swapState(2); swapState(3); swapState(5); swapState(6); break;
    case 7:  swapState(7); swapState(8); swapState(4); swapState(5); break;
    case 9:  swapState(8); swapState(9); swapState(5); swapState(6); break;

    case 2:  swapState(1); swapState(2); swapState(3); break;
    case 4:  swapState(1); swapState(4); swapState(7); break;
    case 6:  swapState(3); swapState(6); swapState(9); break;
    case 8:  swapState(7); swapState(8); swapState(9); break;

    case 5:  swapState(5); swapState(2); swapState(4); swapState(6); swapState(8); break;
    default: pixelColor[pixelMap[i]] = 0;

    }
    render();
    if (pixelState[1] == 1 &&
        pixelState[2] == 1 &&
        pixelState[3] == 1 &&
        pixelState[4] == 1 &&
        pixelState[5] == 0 &&
        pixelState[6] == 1 &&
        pixelState[7] == 1 &&
        pixelState[8] == 1 &&
        pixelState[9] == 1 
        ) {
        gameActive = false;
    }
}

void sameGameActivation(MCP23x17_GPIO gpio, int value) {
    printf("SameGame Button pressed\n");
    randomizeBoard();
}

void sendHeader(snd_pcm_t* soundCardHandle) {
    int err;

    if ((err = snd_pcm_set_params(soundCardHandle,
        SND_PCM_FORMAT_S16_LE,
        SND_PCM_ACCESS_RW_INTERLEAVED,
        1,  // channels
        sampleRate,  // sps
        0,  // software resample
        500000)) < 0) {   /* latency 0.5sec */

        fprintf(stderr,"Playback open error: %s\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }
}

void drainSound(snd_pcm_t* soundCardHandle) {
    int err = snd_pcm_drain(soundCardHandle);
    if (err < 0) {
        fprintf(stderr, "snd_pcm_drain failed: %s\n", snd_strerror(err));
    }
}

static void generate_sine(unsigned char* data, snd_pcm_format_t format,
    wavHeaderType* wavHeader, snd_pcm_uframes_t offset,
    int count, double* _phase, float freq) {

    static double max_phase = 2. * M_PI;
    double phase = *_phase;
    double step = max_phase * freq / (double)wavHeader->sampleRate;
    int format_bits = snd_pcm_format_width(format);
    unsigned int maxval = (1 << (format_bits - 1)) - 1;
    int bps = format_bits / 8;  /* bytes per sample */
    int phys_bps = snd_pcm_format_physical_width(format) / 8;
    int big_endian = snd_pcm_format_big_endian(format) == 1;
    int to_unsigned = snd_pcm_format_unsigned(format) == 1;
    int is_float = (format == SND_PCM_FORMAT_FLOAT_LE ||
        format == SND_PCM_FORMAT_FLOAT_BE);

    for (int k = 0; k < count; ++k) {
        union {
            float f;
            int i;
        } fval;
        int res, i;
        if (is_float) {
            fval.f = sin(phase);
            res = fval.i;
        }
        else {
            res = sin(phase) * maxval;
        }
        if (to_unsigned)
            res ^= 1U << (format_bits - 1);
        for (int chn = 0; chn < wavHeader->channels; chn++) {
            long wavIndex = k * wavHeader->bitsPerSample / 8 + chn * wavHeader->bitsPerSample / 8;
            /* Generate data in native endian format */
            if (!big_endian) {
                for (i = 0; i < bps; i++)
                    *(data + k + phys_bps - 1 - i) = (res >> i * 8) & 0xff;
            }
            else {
                for (i = 0; i < bps; i++)
                    *(data + k + i) = (res >> i * 8) & 0xff;
            }
        }
        phase += step;
        if (phase >= max_phase)
            phase -= max_phase;
    }
    *_phase = phase;
}


void playTone(snd_pcm_t* soundCardHandle, float freq) {
    
    long dataSize = wavHeader.blockAlign * wavHeader.sampleRate * .5;

    void* data = malloc(dataSize);
    memset(data, 0, dataSize);

    chunkHeader chunkHeader;
    strncpy(chunkHeader.chunkID, "data", 4);
    chunkHeader.chunkSize = dataSize;


    long samples = wavHeader.sampleRate * .45;
    snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;
    double phase = 1.0;
    if (freq > 0) {
        generate_sine((unsigned char*)data, format, &wavHeader, 0, samples, &phase, freq);
    }


    int segmetSize = (wavHeader.bitsPerSample / 8) * wavHeader.channels;
    snd_pcm_sframes_t bytesWritten;
    bytesWritten = snd_pcm_writei(soundCardHandle, data, dataSize / segmetSize);

    if (bytesWritten < 0) {
        fprintf(stderr, "snd_pcm_writei failed: %s\n", snd_strerror(bytesWritten));
        exit(EXIT_FAILURE);
    }

    if (bytesWritten > 0 && bytesWritten < dataSize / segmetSize) {
        fprintf(stderr, "data write error (expected %li, wrote %li)\n", dataSize, bytesWritten);
    }


    free(data);
}


void* buttonTone(void* args) {
    int button = *((int*)args);
    snd_pcm_t* soundCardHandle=keypadSoundHandle[button];

    if (button < maxNotes) {
        sendHeader(soundCardHandle);
        playTone(soundCardHandle, noteHz[button]);
        drainSound(soundCardHandle);
    }
    pthread_exit(0);
}

void keypadButtonActivation(MCP23x17_GPIO gpio, int value) {
    printf("keypad button activation\n");

    for (int i = 0; i < MERLIN_LIGHTS; ++i) {
        if (gpio == keypadButton[i]) {
            printf("keypad key=%d button activation port=%c pin=%d value=%d\n", i, mcp23x17_getPort(gpio)+'A', mcp23x17_getPin(gpio), value);
            if (value == 0) {
                if (gameActive) {
                    pixelColor[pixelMap[i]] = neopixel_wheel(RED);
                }

                int* button = (int*) malloc(sizeof(int));
                *button = i;
                threadCreate(buttonTone, "buttonTone", button);
                
            } else {
                if (gameActive) {
                    swapKey(i);
                } 
            }
        }
    }
    render();
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

snd_pcm_t* openSoundCard(const char* soundCardName) {
    int err;

    if (strlen(soundCardName) == 0) {
        soundCardName = "default";
    }

    snd_pcm_t* soundCardHandle;

    if ((err = snd_pcm_open(&soundCardHandle, soundCardName, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
        printf("Playback open error: %s\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }

    return soundCardHandle;
}                                             // end of setup function

void initWavHeader() {
    strncpy(wavHeader.chunkID, "RIFF", 4);
    strncpy(wavHeader.format, "WAVE", 4);
    strncpy(wavHeader.subChunk1ID, "fmt ", 4);
    wavHeader.fileSize = 9;
    wavHeader.subChunk1Size = 16;
    wavHeader.audioFormat = 1;
    wavHeader.channels = 1;
    wavHeader.sampleRate = sampleRate;
    wavHeader.bitsPerSample = 16;
    wavHeader.blockAlign = wavHeader.bitsPerSample / 8;
    wavHeader.byteRate = wavHeader.sampleRate * wavHeader.channels * wavHeader.blockAlign;

}

bool setup() {
    int rc;
    mcp23x17_setDebug(false);
    initWavHeader();

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
        keypadButton[i] = getEnvMCP23x17_GPIO(name);
        printf("keypad key=%d config:  port=%c  pin=%d\n", i, mcp23x17_getPort(keypadButton[i]) + 'A', mcp23x17_getPin(keypadButton[i]));

        mcp23x17_setPinInputMode(keypadButton[i], TRUE, keypadButtonActivation);
    }

    newGameButton = getEnvMCP23x17_GPIO("NEW_GAME");
    hitMeButton = getEnvMCP23x17_GPIO("HIT_ME");
    sameGameButton = getEnvMCP23x17_GPIO("SAME_GAME");
    computerTurnButton = getEnvMCP23x17_GPIO("COMP_TURN");

    mcp23x17_setPinInputMode(sameGameButton, TRUE, sameGameActivation);

    pixelMap[0] = 3;
    pixelMap[1] = 2;
    pixelMap[2] = 4;
    pixelMap[3] = 10;
    pixelMap[4] = 1;
    pixelMap[5] = 5;
    pixelMap[6] = 9;
    pixelMap[7] = 0;
    pixelMap[8] = 6;
    pixelMap[9] = 8;
    pixelMap[10] = 7;


    int ret = neopixel_init(STRIP_TYPE, WS2811_TARGET_FREQ, DMA, GPIO_PIN, led_count);

    if (ret != 0) {
        fprintf(stderr, "neopixel initialization failed: %s\n", neopixel_error(ret));
        return false;
    }

    for (int i = 0; i < MERLIN_LIGHTS; ++i) {
        keypadSoundHandle[i]= openSoundCard(getenv("AUDIODEV"));
    }

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

int main(int argc, char** argv) {
    if (!commandLineOptions(argc, argv)) {
        return 2;
    }

    if (!setup()) {
        fprintf(stderr, "initializaion failed\n");
        return 2;
    }

    neopixel_setBrightness(brightness);
    for (int i = 0; i < MERLIN_LIGHTS; ++i) {
        pixelColor[i] = OFF; // neopixel_wheel(BLUE);
    }
    render();

    randomizeBoard();

    while (true)
	{
        fflush(stdout);
		delay(25);
	}
    
    neopixel_close();
	return 0;
}