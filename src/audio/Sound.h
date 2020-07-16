#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <alsa/asoundlib.h>
#include <alsa/asoundlib.h>

#ifndef NULL
#define NULL 0
#endif

extern bool debug;

struct wavFileHeader
{
    char     fileType[4];       // "RIFF" - literal
    int32_t  fileSize;          // size of entire file minus 8 bytes
    char     fileFormat[4];     // "WAVE" - literal
};

struct wavChunkHeader {
    char     chunkID[4];  // if "fmt ", then it is a format chunk
                          // if "data", then it is a data chunk
    int32_t  chunkSize;   // the size of the data chunk
};

struct wavFormat
{
    int16_t  audioFormat;     // 1=PCM, uncompressed
    int16_t  channels;        // 1=Mono, 2=Stereo, etc.
    int32_t  sampleRate;      // SPS
    int32_t  byteRate;        // SampleRate * NumChannels * BitsPerSample/8
    int16_t  blockAlign;      // NumChannels * BitsPerSample/8 
                              // BlockAlign is the number of bytes for one sample, including all channels
    int16_t  bitsPerSample;   // 8, 16, etc.
};

// Auto managed using AUDIODEV enviornment variable
void drainSound(snd_pcm_t* soundCardHandle);
void playWavFile(char* filename, float volume);
void playTone(float freq, float duration, wavFormat* wavFormat);


// Only needed if you have more than 1 sound card
snd_pcm_t* getSoundCardHandle();
snd_pcm_t* openSoundCard(const char* soundCardName);
void       closeSoundCard(snd_pcm_t* soundCardHandle);

// Only needed if you have more than 1 sound card
void playTone(snd_pcm_t* soundCardHandle, float freq, float duration, wavFormat* wavForamt);
void playWavFile(snd_pcm_t* soundCardHandle, char* filename, float volume);


// use at your own risk
void generate_sine(unsigned char* data, snd_pcm_format_t format,
    wavFormat* wavFormat, snd_pcm_uframes_t offset,
    int count, double* _phase, float freq);
