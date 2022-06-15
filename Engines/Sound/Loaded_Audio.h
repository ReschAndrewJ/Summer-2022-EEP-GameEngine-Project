#ifndef LOADED_AUDIO
#define LOADED_AUDIO

#include "Audio.h"

#include "SFML/Audio.hpp"

const auto AUDIO_TYPE_LOADED = "AUDIO_TYPE_LOADED";

class Loaded_Audio : public Audio {

private:
	sf::SoundBuffer audioBuffer;
	sf::Sound sound;

public:
	virtual ~Loaded_Audio();

	bool load(std::string) override;

	void play() override;
	void pause() override;
	void stop() override;

	bool isPlaying() override;

	void setVolume(float) override;
	float getVolume() override;


};


#endif //LOADED_AUDIO