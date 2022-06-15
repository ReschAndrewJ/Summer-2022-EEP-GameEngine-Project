#include "Loaded_Audio.h"


Loaded_Audio::~Loaded_Audio() {}


bool Loaded_Audio::load(std::string filepath) {
	sound.setBuffer(audioBuffer);
	return audioBuffer.loadFromFile(filepath);
}



void Loaded_Audio::play() {
	sound.play();
}

void Loaded_Audio::stop() {
	sound.stop();
}

void Loaded_Audio::pause() {
	sound.pause();
}


bool Loaded_Audio::isPlaying() {
	return sound.getStatus() == sf::SoundSource::Status::Playing;
}


void Loaded_Audio::setVolume(float volume) {
	sound.setVolume(volume);
}


float Loaded_Audio::getVolume() {
	return sound.getVolume();
}

