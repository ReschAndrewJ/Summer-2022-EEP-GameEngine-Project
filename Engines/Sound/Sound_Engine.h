#ifndef SOUND_ENGINE
#define SOUND_ENGINE

#include "Audio.h"



class Sound_Engine
{
public:
	~Sound_Engine();

private:
	// should never be set above 256, internal limit varies by platform
	static const unsigned maxSoundCount = 50;

	Audio* sounds[maxSoundCount];


public:
	void clearEngine();

	// [0, 100]
	void setGlobalVolume(float);
	float getGlobalVolume();

	/* int audioIndex = addAudio(string filepath, std::string audioType)
	return -1 on failure */
	int addAudio(std::string, std::string);
	// free audio at index
	void removeAudio(int);

	// playAudio(unsigned int audioIndex) 
	void playAudio(int);
	// stopAudio(unsigned int audioIndex)
	void stopAudio(int);
	void pauseAudio(int);

	void setAudioVolume(int, float);
	// return -1 if there is no audio stored at the index
	float getAudioVolume(int);

};


#endif //SOUND_ENGINE