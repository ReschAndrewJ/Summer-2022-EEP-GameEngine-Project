#ifndef STREAMED_AUDIO
#define STREAMED_AUDIO

#include "Audio.h"

#include "SFML/Audio.hpp"

const auto AUDIO_TYPE_STREAMED = "AUDIO_TYPE_STREAMED";

class Streamed_Audio : public Audio {

private:
	sf::Music music;

public:
	virtual ~Streamed_Audio();

	bool load(std::string) override;

	void play() override;
	void pause() override;
	void stop() override;

	bool isPlaying() override;

	void setVolume(float) override;
	float getVolume() override;


};


#endif //STREAMED_AUDIO