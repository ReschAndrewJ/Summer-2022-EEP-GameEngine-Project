#ifndef OBJECT_SOUNDOUTPUT
#define OBJECT_SOUNDOUTPUT

#include "../Base/Object.h"
#include "../../Engines/Sound/Sound_Engine.h"

const auto OBJECT_CLASS_SOUNDOUTPUT = "Object_SoundOutput";

// sound file filepath from program file
const auto ATTRIBUTE_SOUND_FILE = "SoundFilepath";
// range [0,100]
const auto ATTRIBUTE_SOUND_VOLUME = "SoundVolume";
// determines how the audio will be streamed
const auto ATTRIBUTE_SOUND_ISMUSIC = "SoundIsMusic";
// true if the sound is to loop
const auto ATTRIBUTE_SOUND_DOES_LOOP = "SoundLoop";
// loop start time in seconds
const auto ATTRIBUTE_SOUND_LOOP_START = "SoundStart";
// loop end time in seconds
const auto ATTRIBUTE_SOUND_LOOP_END = "SoundEnd";

const auto ATTRIBUTE_SOUND_START_OFFSET = "SoundStartOffset";

class Object_SoundOutput : virtual public Object {
private:
	Sound_Engine* soundEnginePtr;

public:
	virtual ~Object_SoundOutput();
	Object_SoundOutput();

	void play();
	void pause();
	void stop();
	float getOffset();

	static void afterCreationFunc(Object* selfPtr);
	static void beforeDestructionFunc(Object* selfPtr);
	static void processFunction(Object* selfPtr, float delta);

private:
	void loadSound();

};

#endif // OBJECT_SOUNDOUTPUT