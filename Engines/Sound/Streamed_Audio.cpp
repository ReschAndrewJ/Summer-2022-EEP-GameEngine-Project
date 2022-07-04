#include "Streamed_Audio.h"


Streamed_Audio::~Streamed_Audio() {}

bool Streamed_Audio::load(std::string filepath) {
	return music.openFromFile(filepath);
}


void Streamed_Audio::play() {
	music.play();
}

void Streamed_Audio::pause() {
	music.pause();
}

void Streamed_Audio::stop() {
	music.stop();
}


bool Streamed_Audio::isPlaying() {
	return music.getStatus() == sf::SoundStream::Status::Playing;
}


void Streamed_Audio::setVolume(float volume) {
	music.setVolume(volume);
}

float Streamed_Audio::getVolume() {
	return music.getVolume();
}


void Streamed_Audio::setPlayingOffset(float offset) {
	music.setPlayingOffset(sf::seconds(offset));
}

float Streamed_Audio::getPlayingOffset() {
	return music.getPlayingOffset().asSeconds();
}