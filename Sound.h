#include <alsa/asoundlib.h>

struct wavHeaderType
{
    char     chunkID[4];      // "RIFF" - literal
    int32_t  fileSize;        // size of entire file minus 8 bytes
    char     format[4];       // "WAVE" - big-endian format
    char     subChunk1ID[4];  // "fmt " - literal
    int32_t  subChunk1Size;   // size of sub-chunk1 minus 8 bytes
    int16_t  audioFormat;     // 1=PCM, uncompressed
    int16_t  channels;        // 1=Mono, 2=Stereo, etc.
    int32_t  sampleRate;      // SPS
    int32_t  byteRate;        // SampleRate * NumChannels * BitsPerSample/8
    int16_t  blockAlign;      // NumChannels * BitsPerSample/8 
                              // BlockAlign is the number of bytes for one sample, including all channels
    int16_t  bitsPerSample;   // 8, 16, etc.
};

struct chunkHeader {
    char     chunkID[4];  // if "data", then it is a data chunk
    int32_t  chunkSize;   // the size of the data chunk
};

snd_pcm_t* openSoundCard(const char* soundCardName);