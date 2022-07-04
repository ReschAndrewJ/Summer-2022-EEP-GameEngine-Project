#ifndef STREAMED_AUDIO
#define STREAMED_AUDIO

#include "Audio.h"

#include "SFML/Audio.hpp"


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

	void setPlayingOffset(float) override;
	float getPlayingOffset() override;

};


#endif //STREAMED_AUDIO