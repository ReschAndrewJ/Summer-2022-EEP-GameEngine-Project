#include "Sound_Engine.h"

#include "SFML/Audio.hpp"

#include "Loaded_Audio.h"
#include "Streamed_Audio.h"

Sound_Engine::~Sound_Engine() {
	clearEngine();
}


void Sound_Engine::clearEngine() {
	for (auto & item : audioContainer) {
		if (item.second != nullptr) delete item.second;
	}
	audioContainer.clear();
}


void Sound_Engine::setGlobalVolume(float volume) {
	sf::Listener::setGlobalVolume(volume);
}

float Sound_Engine::getGlobalVolume() {
	return sf::Listener::getGlobalVolume();
}


bool Sound_Engine::loadAudio(std::string filepath, std::string audioIdentifier, std::string audioType) {
	if (audioContainer.size() > maxSoundCount) {
		std::string err = "out of audio space, max sound count: " + std::to_string(maxSoundCount) + "\n";
		printf(err.c_str());
		return false;
	}
	if (audioContainer.count(audioIdentifier)) return true;

	Audio* sound;
	if (audioType == AUDIO_TYPE_LOADED) sound = new Loaded_Audio();
	else if (audioType == AUDIO_TYPE_STREAMED) sound = new Streamed_Audio();
	else {
		std::string err = "invalid audio type: " + audioType;
		printf(err.c_str());
		return false;
	}

	if (!sound->load(filepath)) {
		std::string err = "failed to load audio file: " + filepath;
		printf(err.c_str());
		delete sound;
		return false;
	}

	audioContainer.insert({ audioIdentifier, sound });
	return true;
}


void Sound_Engine::removeAudio(std::string audioIdentifier) {
	if (audioContainer.count(audioIdentifier)) {
		if (audioContainer.at(audioIdentifier)->isPlaying()) audioContainer.at(audioIdentifier)->stop();
		delete audioContainer.at(audioIdentifier); 
		audioContainer.erase(audioIdentifier);
	}
}


bool Sound_Engine::playAudio(std::string audioIdentifier) {
	if (!audioContainer.count(audioIdentifier)) {
		std::string err = "audio not loaded, audio: " + audioIdentifier + "\n";
		printf(err.c_str());
		return false;
	}
	audioContainer.at(audioIdentifier)->play();
	if (!audioContainer.at(audioIdentifier)->isPlaying()) {
		std::string err = "audio mapped to container but failed to play, audio: " + audioIdentifier + "\n";
		printf(err.c_str());
		return false;
	}
	return true;
}

bool Sound_Engine::isAudioPlaying(std::string audioIdentifier) {
	if (!audioContainer.count(audioIdentifier)) return false;
	return audioContainer.at(audioIdentifier)->isPlaying();
}


void Sound_Engine::stopAudio(std::string audioIdentifier) {
	if (!audioContainer.count(audioIdentifier)) return;
	audioContainer.at(audioIdentifier)->stop();
}


void Sound_Engine::pauseAudio(std::string audioIdentifier) {
	if (!audioContainer.count(audioIdentifier)) return;
	audioContainer.at(audioIdentifier)->pause();
}


void Sound_Engine::setAudioVolume(std::string audioIdentifier, float volume) {
	if (!audioContainer.count(audioIdentifier)) return;
	audioContainer.at(audioIdentifier)->setVolume(volume);
}


float Sound_Engine::getAudioVolume(std::string audioIdentifier) {
	if (!audioContainer.count(audioIdentifier)) return -1;
	return audioContainer.at(audioIdentifier)->getVolume();
}


void Sound_Engine::setAudioOffset(std::string audioIdentifier, float offset) {
	if (audioContainer.count(audioIdentifier)) audioContainer.at(audioIdentifier)->setPlayingOffset(offset);
}

float Sound_Engine::getAudioOffset(std::string audioIdentifier) {
	if (!audioContainer.count(audioIdentifier)) return -1;
	return audioContainer.at(audioIdentifier)->getPlayingOffset();
}