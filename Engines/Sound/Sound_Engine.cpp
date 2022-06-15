#include "Sound_Engine.h"

#include "SFML/Audio.hpp"

#include "Loaded_Audio.h"
#include "Streamed_Audio.h"

Sound_Engine::~Sound_Engine() {
	clearEngine();
}


void Sound_Engine::clearEngine() {
	for (unsigned i = 0; i < maxSoundCount; ++i) {
		if (sounds[i] != nullptr) delete sounds[i];
	}
}


void Sound_Engine::setGlobalVolume(float volume) {
	sf::Listener::setGlobalVolume(volume);
}

float Sound_Engine::getGlobalVolume() {
	return sf::Listener::getGlobalVolume();
}


int Sound_Engine::addAudio(std::string filepath, std::string audioType) {
	unsigned index = -1;
	for (unsigned i = 0; i < maxSoundCount; ++i) {
		if (sounds[i] == nullptr) {
			index = i;
			break;
		}
	}
	if (index == -1) return -1;

	Audio* sound;
	if (audioType == AUDIO_TYPE_LOADED) sound = new Loaded_Audio();
	else if (audioType == AUDIO_TYPE_STREAMED) sound = new Streamed_Audio();
	else {
		std::string err = "invalid audio type: " + audioType;
		printf(err.c_str());
		return -1;
	}

	if (!sound->load(filepath)) {
		std::string err = "failed to load audio file: " + filepath;
		printf(err.c_str());
		delete sound;
		return -1;
	}
	return index;
}


void Sound_Engine::removeAudio(int index) {
	if (index < 0 || index >= maxSoundCount) return;
	if (sounds[index] != nullptr) delete sounds[index];
}


void Sound_Engine::playAudio(int index) {
	if (index < 0 || index >= maxSoundCount) return;
	if (sounds[index] != nullptr) sounds[index]->play();
}


void Sound_Engine::stopAudio(int index) {
	if (index < 0 || index >= maxSoundCount) return;
	if (sounds[index] != nullptr) sounds[index]->stop();
}


void Sound_Engine::pauseAudio(int index) {
	if (index < 0 || index >= maxSoundCount) return;
	if (sounds[index] != nullptr) sounds[index]->pause();
}


void Sound_Engine::setAudioVolume(int index, float volume) {
	if (index < 0 || index >= maxSoundCount) return;
	if (sounds[index] != nullptr) sounds[index]->setVolume(volume);
}


float Sound_Engine::getAudioVolume(int index) {
	if (index < 0 || index >= maxSoundCount) return -1;
	return sounds[index] != nullptr ? sounds[index]->getVolume() : -1;
}