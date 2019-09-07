#include "wav.h"

WavFileHeader generateWaveHeader(uint32_t sampleRate, uint16_t numChannels, uint16_t bitsPerSample)
{
	//RIFF WAVE Header
	WavFileHeader newHeader;
	newHeader.ChunkID[0] = 'R';
	newHeader.ChunkID[1] = 'I';
	newHeader.ChunkID[2] = 'F';
	newHeader.ChunkID[3] = 'F';
	newHeader.Format[0] = 'W';
	newHeader.Format[1] = 'A';
	newHeader.Format[2] = 'V';
	newHeader.Format[3] = 'E';
	
	//Format subchunk
	newHeader.Subchunk1ID[0] = 'f';
  newHeader.Subchunk1ID[1] = 'm';
  newHeader.Subchunk1ID[2] = 't';
  newHeader.Subchunk1ID[3] = ' ';
	newHeader.AudioFormat = 1; // PCM
	newHeader.NumChannels = numChannels;
	newHeader.SampleRate = sampleRate; // most likely 44100 hertz
	newHeader.BitsPerSample = bitsPerSample; // 
  newHeader.ByteRate = newHeader.SampleRate * newHeader.NumChannels * newHeader.BitsPerSample / 8;
  newHeader.BlockAlign = newHeader.NumChannels * newHeader.BitsPerSample/8;
	
	// Data subchunk
  newHeader.Subchunk2ID[0] = 'd';
  newHeader.Subchunk2ID[1] = 'a';
  newHeader.Subchunk2ID[2] = 't';
  newHeader.Subchunk2ID[3] = 'a';
	
	// All sizes:
  // chuckSize = 4 + (8 + subChunk1Size) + (8 + subChubk2Size)
  // subChunk1Size is constanst, i'm using 16 and staying with PCM
  // subChunk2Size = nSamples * nChannels * bitsPerSample/8
  // Whenever a sample is added:
  //    chunkSize += (nChannels * bitsPerSample/8)
  //    subChunk2Size += (nChannels * bitsPerSample/8)
	newHeader.ChunkSize = 4+8+16+8+0;
  newHeader.Subchunk1Size = 16;
  newHeader.Subchunk2Size = 0;
	
	return newHeader;
}

WaveFile makeWave(uint32_t sampleRate, uint16_t numChannels, uint16_t bitsPerSample)
{
	WaveFile newWave;
	newWave.header = generateWaveHeader(sampleRate, numChannels, bitsPerSample);
	
	return newWave;
}

void addWaveSample(WaveFile* wavHandler)
{
}



	
	