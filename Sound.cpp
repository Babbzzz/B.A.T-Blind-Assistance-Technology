#include "Sound.hpp"

int loadBeep() {
	sf::Listener::setGlobalVolume(10000);
	if (!beep.loadFromFile("beep-07.wav")) {
        return -1;
    }
    return 0;
}

int playSavedSuccessfully() {
	//load saved successfully file
	sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("beep-07.wav"))
        return -1;
    sf::Sound sound;
	sound.setBuffer(buffer);
	sound.setPosition(0,0,0);
	sound.play();
	while (sound.getStatus() == 2) {}
	return 0;
}

int generateBeep(int x, int y, int z)
{
    sf::Sound sound;
    sound.setBuffer(beep);
    sound.setAttenuation(1);
    sound.setPosition(x,y,z);
    sound.play();
	while (sound.getStatus() == 2) {}
	return 0;
}