#ifndef SOUND_HPP
#define SOUND_HPP

#include <SFML/Audio.hpp>
#include <unistd.h>

static sf::SoundBuffer beep;

int loadBeep();
int playSavedSuccessfully();
int generateBeep(int, int, int);

#endif