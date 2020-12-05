//
// Created by Roninkoi on 19.12.2015.
//

#ifndef CORIUM_RANDOM_H
#define CORIUM_RANDOM_H

#include <random>

inline int random_range(int min, int max)
{
	int returns = -1;

	//std::random_device rd; // obtain a random number from hardware
	max = std::max(0, max);
	min = std::max(0, min);

	returns = min + (std::rand() % (max - min + 1)); // this thing isn't really random

	/*
	   std::random_device rd; // obtain a random number from hardware
	   std::mt19937 eng(rd()); // seed the generator
	   std::uniform_int_distribution<> distr(min, max); // define the range

	   returns = distr(eng);*/

	return returns;
}

#endif //CORIUM_RANDOM_H
