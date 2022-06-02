#ifndef AUDIO
#define AUDIO

#include <string>

// Abstract audio class to be managed by the sound engine
class Audio {
public:
	// load the audio to be played from a file
	virtual void load(std::string) = 0;

	// start or resume playing the audio
	virtual void play() = 0;
	// pause the audio
	virtual void pause() = 0;
	// stop playing the audio
	virtual void stop() = 0;

};


#endif //AUDIO