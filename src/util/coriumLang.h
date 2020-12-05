//
// Created by Roninkoi on 23.01.2018.
//

#ifndef CORIUMLANG_H
#define CORIUMLANG_H

#include <game/game.h>
#include "lexer.h"
#include "mapParser.h"
#include "timeUtil.h"

std::vector<std::string> loopReplace(std::vector<std::string> words, int n, int nmax)
{
	std::vector<std::string> loopwords = words;

	for (int i = 0; i < loopwords.size(); ++i) {
		for (int j = 0; j < loopwords[i].length(); ++j) {
			if (loopwords[i][j] == '@') {
				std::string before = "";
				std::string after = "";

				for (int b = 0; b < j; ++b) {
					before += loopwords[i][b];
				}

				for (int a = j + 1; a < loopwords[i].length(); ++a) {
					if (a < loopwords[i].length())
						after += loopwords[i][a];
				}

				loopwords[i] = before + toString(n) + after;
			}
			if (loopwords[i][j] == '%') {
				std::string before = "";
				std::string after = "";

				for (int b = 0; b < j; ++b) {
					before += loopwords[i][b];
				}

				for (int a = j + 1; a < loopwords[i].length(); ++a) {
					if (a < loopwords[i].length())
						after += loopwords[i][a];
				}

				std::string zeroes = "";

				if (n > 0) {
					for (int k = 0; k < (int) log10(nmax) - (int) log10(n); ++k)
						zeroes += "0";
				} else if (nmax > 0) {
					for (int k = 0; k < (int) log10(nmax); ++k)
						zeroes += "0";
				}
				loopwords[i] = before + zeroes + toString(n) + after;
			}
		}
	}
	return loopwords;
}

std::string getStringVal(std::string s, Game *game)
{
	if (s == "_MAPNAME") {
		return game->map.mapName;
	} else if (s == "_TIME") {
		return toString(currentDateTime());
	} else {
		return s;
	}
}

bool getBoolVal(std::string s, Game *game)
{
	if (s == "_RUNNING") {
		return game->running;
	} else if (s == "_NRUNNING") {
		return !game->running;
	} else if (s == "_TRUE") {
		return true;
	} else if (s == "_FALSE") {
		return false;
	} else {
		return (bool) atoi(s.c_str());
	}
}

void getCmd(std::vector<std::string> words, int *i, Game *game)
{
	if (expr(words, *i, 2, "_IF")) {
		bool iftrue = false;
		std::string ifstring = words[*i + 1];

		iftrue = getBoolVal(ifstring, game);

		*i += 1;

		for (; wordIsNot(words, *i, "_ENDIF"); ++*i) {
			if (iftrue)
				getCmd(words, i, game);
		}
	}
	if (expr(words, *i, 2, "_LOOP")) {
		bool iftrue = false;
		std::string s = words[*i + 1];
		int n = atoi(s.c_str());

		int loopstart = *i;

		*i += 1;

		int loopi = 0;

		for (; loopi < n; ++*i) {
			std::vector<std::string> loopwords = loopReplace(words, loopi, n);
			getCmd(loopwords, i, game);

			if (wordIs(words, *i, "_ENDLOOP")) {
				++loopi;
				if (loopi < n)
					*i = loopstart;
			}
		}
		--*i; // correction
	}
	if (expr(words, *i, 2, "_SET_MAP")) {
		game->map.mapPath = words[*i + 1].c_str();
	}
	if (expr(words, *i, 2, "_LOAD_MAP")) {
		game->map.mapPath = words[*i + 1].c_str();
		game->map.load(&game->renderer);
	}
	if (expr(words, *i, 1, "_MAP")) {
		for (; wordIsNot(words, *i, "_ENDMAP"); ++*i) {
			mapCmd(words, *i, &game->map);
		}
	}
	if (expr(words, *i, 2, "_LOAD_SETTINGS")) {
		loadSettings(words[*i + 1], game);
	}
	if (expr(words, *i, 2, "_RUN")) {
		runScript(words[*i + 1], game);
	}
	if (expr(words, *i, 2, "_PRINT")) {
		std::string p = getStringVal(words[*i + 1], game);
		gamePrint(p + "\n");
	}
}

void runScript(std::string path, Game *game)
{
	gamePrint("running script " + path + "...\n");

	std::vector<std::string> words = fileLexer(path);

	for (int i = 0; i < words.size(); ++i) {
		if (words[i] == "_BEGIN") {
			for (; wordIsNot(words, i, "_END"); ++i) {
				getCmd(words, &i, game);
			}
		}
	}
}

#endif
