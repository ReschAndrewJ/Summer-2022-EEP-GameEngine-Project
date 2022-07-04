#include "Object_SoundOutput.h"


Object_SoundOutput::Object_SoundOutput() {
	addClassIdentifier(OBJECT_CLASS_SOUNDOUTPUT);

	createAttribute(ATTRIBUTE_SOUND_FILE, Attribute::types::STRING);
	createAttribute(ATTRIBUTE_SOUND_VOLUME, Attribute::types::DOUBLE);
	createAttribute(ATTRIBUTE_SOUND_ISMUSIC, Attribute::types::BOOLEAN);
	createAttribute(ATTRIBUTE_SOUND_DOES_LOOP, Attribute::types::BOOLEAN);
	createAttribute(ATTRIBUTE_SOUND_LOOP_START, Attribute::types::DOUBLE);
	createAttribute(ATTRIBUTE_SOUND_LOOP_END, Attribute::types::DOUBLE);

	setAttribute(ATTRIBUTE_SOUND_VOLUME, 50);

	addRequestedPointer(PTR_IDENTIFIER::SOUND_ENGINE_PTR, &soundEnginePtr);

	addAfterCreationFunction(&afterCreationFunc);
	addProcessFunction(&processFunction, 20);
}

Object_SoundOutput::~Object_SoundOutput(){}

void Object_SoundOutput::afterCreationFunc(Object* selfPtr) {
	dynamic_cast<Object_SoundOutput*>(selfPtr)->loadSound();
}

void Object_SoundOutput::beforeDestructionFunc(Object* selfPtr) {
	dynamic_cast<Object_SoundOutput*>(selfPtr)->soundEnginePtr->removeAudio(selfPtr->getIdentifier());
}

void Object_SoundOutput::loadSound() {
	soundEnginePtr->loadAudio((std::string)getAttribute(ATTRIBUTE_SOUND_FILE), getIdentifier(),
		(bool)getAttribute(ATTRIBUTE_SOUND_ISMUSIC) ? AUDIO_TYPE_STREAMED : AUDIO_TYPE_LOADED);
}

void Object_SoundOutput::play() {
	if (!soundEnginePtr->playAudio(getIdentifier())) {
		loadSound();
		soundEnginePtr->playAudio(getIdentifier());
	}
}

void Object_SoundOutput::pause() {
	soundEnginePtr->pauseAudio(getIdentifier());
}

void Object_SoundOutput::stop() {
	soundEnginePtr->stopAudio(getIdentifier());
}


void Object_SoundOutput::processFunction(Object* selfPtr, float delta) {
	Object_SoundOutput& self = *dynamic_cast<Object_SoundOutput*>(selfPtr);
	float vol = self.getAttribute(ATTRIBUTE_SOUND_VOLUME);
	if (self.soundEnginePtr->getAudioVolume(self.getIdentifier()) != vol) self.soundEnginePtr->setAudioVolume(self.getIdentifier(), vol);
	if (!(bool)self.getAttribute(ATTRIBUTE_SOUND_DOES_LOOP)) return; // return if sound does not loop

	float loopStart = self.getAttribute(ATTRIBUTE_SOUND_LOOP_START);
	float loopEnd = self.getAttribute(ATTRIBUTE_SOUND_LOOP_END);

	float current = self.soundEnginePtr->getAudioOffset(self.getIdentifier());
	if (current >= loopEnd) self.soundEnginePtr->setAudioOffset(self.getIdentifier(), loopStart);

}