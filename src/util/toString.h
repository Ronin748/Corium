//
// Created by Roninkoi on 18.12.2015.
//

#ifndef CORIUM_TOSTRING_H
#define CORIUM_TOSTRING_H

#include <string>
#include <sstream>

template<typename T>
std::string toString(const T &n)
{
	std::ostringstream stm;
	stm << n;
	return stm.str();
}

std::string toString(glm::vec3 v)
{
	return "(" + toString(v[0]) + ", " + toString(v[1]) + ", " + toString(v[2]) + ")";
}

#endif //CORIUM_TOSTRING_H
