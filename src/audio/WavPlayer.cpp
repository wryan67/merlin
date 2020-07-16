#include "Sound.h"


void playWavFile(char* filename, float volume) {

    snd_pcm_t* handle = getSoundCardHandle();
    if (handle == NULL) {
        fprintf(stderr, "soundCardHandle is null\n"); fflush(stderr);
    }
    playWavFile(handle, filename, volume);
    closeSoundCard(handle);
}


void playWavFile(snd_pcm_t* soundCardHandle, char* filename, float volume) {
    printf("Playing %s ...\n"); fflush(stdout);

    int err;
    FILE* wav = fopen(filename, "r");

    if (wav == NULL) {
        fprintf(stderr,"Cannot open %s\n", filename); fflush(stderr);
        free(filename);
        return;
    }
    wavFileHeader  wavFileHeader;
    wavChunkHeader wavChunkHeader;
    wavFormat      wavFormat;
    
    int b = fread(&wavFileHeader, sizeof(wavFileHeader), 1, wav);
    if (b != 1) {
        fprintf(stderr, "Could not read header from sound file %s\n",filename);
        free(filename);
        return;
    }

    if (strncmp(wavFileHeader.fileType, "RIFF", 4) != 0) {
        printf("unexpected file descriptor, expected 'RIFF', actual='%4.4s'\n", wavFileHeader.fileType);
        return;
    }

    if (strncmp(wavFileHeader.fileFormat, "WAVE", 4) != 0) {
        printf("unexpected file type, expected 'RIFF', actual='%4.4s'\n", wavFileHeader.fileFormat);
        return;
    }


    while (true) {
        int b = fread(&wavChunkHeader, sizeof(wavChunkHeader), 1, wav);
        if (b != 1) {
            fprintf(stderr, "Could not read chunk header from sound file %s\n", filename);
            free(filename);
            return;
        }
        if (strncmp(wavChunkHeader.chunkID, "data", 4)==0) {
            fprintf(stderr, "error: data chunk found before wav format\n");
            free(filename);
            return;
        }

        if (strncmp(wavChunkHeader.chunkID, "IF", 2) == 0) {
            exit(0);
        }

        if (strncmp(wavChunkHeader.chunkID, "fmt ", 4) == 0) {
            void* wavHeaderChunk = malloc(wavChunkHeader.chunkSize);
            b = fread(wavHeaderChunk, wavChunkHeader.chunkSize, 1, wav);
            if (b != 1) {
                fprintf(stderr, "Could not read wave header from sound file %s\n", filename);
                free(filename);
                return;
            }
            memcpy(&wavFormat, wavHeaderChunk, sizeof(wavFormat));
            free(wavHeaderChunk);
            break;
        } else {
            fseek(wav, wavChunkHeader.chunkSize, SEEK_CUR);
        }
    }


    int segmetSize = (wavFormat.bitsPerSample / 8) * wavFormat.channels;


    if (debug) {
        printf("Playing %s...\n");
        printf("volume            %f\n", volume);
        printf("audio format      %d\n", wavFormat.audioFormat);
        printf("num channels      %d\n", wavFormat.channels);
        printf("sample rate       %d\n", wavFormat.sampleRate);
        printf("byte rate         %d\n", wavFormat.byteRate);
        printf("block align       %d\n", wavFormat.blockAlign);
        printf("bits per sample   %d\n", wavFormat.bitsPerSample);
        printf("segment size      %d\n", segmetSize);
    }

    
    if (wavFormat.bitsPerSample<1) {
        printf("bits per sample must be greater than zero, found %d\n", wavFormat.bitsPerSample);
        return;
    }

    if (wavFormat.audioFormat != 1) {
        printf("unknown audio format, expected 1, found %d\n", wavFormat.audioFormat);
        return;
    
    }

    _snd_pcm_format sndFormat;

    switch (wavFormat.bitsPerSample) {
        case 16: {
            sndFormat = SND_PCM_FORMAT_S16_LE;
            break;
        }
        case 24:
        {
            sndFormat = SND_PCM_FORMAT_S24_3LE;
            break;
        }
        case 32:
        {
            sndFormat = SND_PCM_FORMAT_S32_LE;
            break;
        }
        default:
            fprintf(stderr,"unknown alsa translation for bits per sample: %d\n", wavFormat.bitsPerSample);
            return;
    }

    _snd_pcm_access accessType = SND_PCM_ACCESS_RW_INTERLEAVED;


    if ((err = snd_pcm_set_params(soundCardHandle,
        sndFormat,
        accessType,
        wavFormat.channels,    
        wavFormat.sampleRate,  // sps
        1,                     // software resample
        500000)) < 0)          // latency 0.5sec 
    {
        printf("Playback open error: %s\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }
    

    void* data = NULL;

    while (true) {

        long rs = fread(&wavChunkHeader, sizeof(wavChunkHeader), 1, wav);

        if (rs != 1) {
            break;
        }

        if (strncmp(wavChunkHeader.chunkID, "data", 4) != 0) {
            fseek(wav, wavChunkHeader.chunkSize, SEEK_CUR);
            continue;
        }

        long dataSize = wavChunkHeader.chunkSize;
        data = realloc(data, dataSize);

        rs = fread(data, dataSize, 1, wav);

        if (rs != 1) {
            fprintf(stderr, "unexpected error, %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        snd_pcm_sframes_t bytesWritten = snd_pcm_writei(soundCardHandle, data, dataSize / segmetSize);

        if (debug) fprintf(stderr, "checking overflow\n"); fflush(stderr);
        if (bytesWritten < 0) {
            bytesWritten = snd_pcm_recover(soundCardHandle, bytesWritten, 0);
        }

        if (debug) fprintf(stderr, "verifying write status\n"); fflush(stderr);
        if (bytesWritten < 0) {
            fprintf(stderr, "snd_pcm_writei failed: %s\n", snd_strerror(bytesWritten));
            exit(EXIT_FAILURE);
        }

        if (bytesWritten > 0 && bytesWritten < dataSize / segmetSize) {
            fprintf(stderr, "data write error (expected %li, wrote %li)\n", dataSize, bytesWritten);
        }
    }

    if (debug) fprintf(stderr, "flushing soundCardHandle\n"); fflush(stderr);

    drainSound(soundCardHandle);

    free(data);
    fclose(wav);
    free(filename);
}