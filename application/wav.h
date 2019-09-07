#ifndef _WAV_H_
#define _WAV_H_

#include "stm32f4xx_hal.h"
#include "stdint.h"
#include "core.h"

typedef struct 
{															// [BE] = Big Endian, [LE] = Little Endian													
		// The "RIFF" chunk descriptor: header
	uint8_t ChunkID[4]; 						// 0x52 49 46 46 == "RIFF" [BE]
	uint32_t ChunkSize;						// 36 + Subchunk2Size [LE]
	uint8_t Format[4];							// 0x57 41 56 45 == "WAVE" [BE]
		// The "fmt" sub-chunk: sound data's format
	uint8_t Subchunk1ID[4];					// 0x66 6d 74 20 = "fmt " [BE]
	uint32_t Subchunk1Size;				// 16 for PCM, size of the rest of this subchunk [LE]
	uint16_t AudioFormat;					// PCM = 1 (li near quantization) !=1 for compression [LE]
	uint16_t NumChannels;					// mono = 1, Stereo = 2 [LE]
	uint32_t SampleRate;					// 8000, 44100(most common), etc. [LE]
	uint32_t ByteRate;						// == Sample Rate * NumChannels * BitsPerSample/8 [LE]
	uint16_t BlockAlign;					// == NumChannels * BitsPerSample/8 [LE]
																// Number of bytes for one sample including all channels	
	uint16_t BitsPerSample;				// 8 bits = 8, 16 bits = 16, etc. [LE]
		// the "data" sub-chunk: size of data and actual sound
	uint8_t Subchunk2ID[4];					// 0x64 61 74 61 == "data" [BE]
	uint32_t Subchunk2Size;				// == NumSamples * NumChannels * BitsPerSample/8 [LE]
	//uint8_t data[];	// actual sound data, [Subchunk2Size] [LE]
} WavFileHeader;
	
typedef struct
{
	WavFileHeader header;
	uint8_t* data;
	uint64_t index;
	uint64_t size;
	uint64_t nSamples;
} WaveFile;


WavFileHeader generateWaveHeader(uint32_t sampleRate, uint16_t numChannels, uint16_t bitsPerSample); // .wav file header structure 

WaveFile makeWave(uint32_t sampleRate, uint16_t numChannels, uint16_t bitsPerSample); // .wav file structure

void addWaveSample(WaveFile* wavHandler); // add sample to .wav data, update some structure fields

#endif
