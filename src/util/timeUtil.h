//
// Created by Roninkoi on 31.3.2016.
//

#ifndef CORIUM_CURRENTTIME_H
#define CORIUM_CURRENTTIME_H

#include <iostream>
#include <string>
#include <stdio.h>
#include <time.h>

const std::string currentDateTime()
{
	time_t now = time(0);
	struct tm tstruct;
	char buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

	return buf;
}

#endif //CORIUM_CURRENTTIME_H
