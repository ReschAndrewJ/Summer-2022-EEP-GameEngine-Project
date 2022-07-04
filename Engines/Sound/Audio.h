#ifndef AUDIO
#define AUDIO

#include <string>

// Abstract audio class to be managed by the sound engine
class Audio {
public:
	virtual ~Audio();

	// load the audio to be played from a file
	virtual bool load(std::string) = 0;

	// start or resume playing the audio
	virtual void play() = 0;
	// pause the audio
	virtual void pause() = 0;
	// stop playing the audio
	virtual void stop() = 0;

	virtual bool isPlaying();

	virtual void setVolume(float) = 0;
	virtual float getVolume() = 0;

	virtual void setPlayingOffset(float) = 0;
	virtual float getPlayingOffset() = 0;

};


#endif //AUDIO