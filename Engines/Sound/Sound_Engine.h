#ifndef SOUND_ENGINE
#define SOUND_ENGINE

#include "Audio.h"

#include <unordered_map>

const auto AUDIO_TYPE_LOADED = "AUDIO_TYPE_LOADED";
const auto AUDIO_TYPE_STREAMED = "AUDIO_TYPE_STREAMED";

class Sound_Engine
{
public:
	~Sound_Engine();

private:
	// should never be set above 256, internal limit varies by platform
	static const unsigned maxSoundCount = 50;

	std::unordered_map<std::string, Audio*> audioContainer;

public:
	void clearEngine();

	// [0, 100]
	void setGlobalVolume(float);
	float getGlobalVolume();

	bool loadAudio(std::string filepath, std::string audioIdentifier, std::string AudioType);

	void removeAudio(std::string audioIdentifier);

	// return false on failure
	bool playAudio(std::string audioIdentifier);

	void stopAudio(std::string audioIdentifier);
	void pauseAudio(std::string audioIdentifier);

	void setAudioVolume(std::string audioIdentifier, float volume);
	// return -1 if the audio is not stored
	float getAudioVolume(std::string audioIdentifier);

	float getAudioOffset(std::string audioIdentifier);
	void setAudioOffset(std::string audioIdentifier, float offset);
};


#endif //SOUND_ENGINE