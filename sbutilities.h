/*
    This file is part of Soundbench.

    Soundbench is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Soundbench is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Soundbench.  If not, see <http://www.gnu.org/licenses/>.

    Copyright 2012  Amaya S.
*/

#ifndef SBUTILITIES_H
#define SBUTILITIES_H

/*
  This file is for anything that a large number of files in Soundbench might need.

  If you are a project developer, DO NOT ADD ANYTHING TO THIS FILE WITHOUT CONSULTING THE PROJECT HEAD!
*/

#include <utility>
#include <map>
#include <string>
#include <cmath>

#include "common.h"
#include "err/problemfix.h"

#ifndef IS_SOUNDBENCH
    #define IS_SOUNDBENCH
#endif

#if defined(__linux__)
    #define SB_ENV_LINUX
    #define SB_ENV_POSIX
#elif defined (__APPLE__) & defined (__MACH__)
    #define SB_ENV_MACOS
    #define SB_ENV_POSIX
#elif defined(__CYGWIN__)
    #define SB_ENV_WNDOS
    #define SB_ENV_POSIX
#elif defined(_WIN32)
    #define SB_ENV_WNDOS
#elif defined(_POSIX_VERSION)
    #define SB_ENV_POSIX
#endif

#define SBVERSION "Development Version"
#define SBVERSION_CODENAME "Phoenix"
#define SBVERSION_MAJOR 0
#define SBVERSION_MINOR 5
#define SBVERSION_PATCH 0

inline void AbortSoundbench() {
    try {
        std::cerr << "Abort requested.\n";
        throw;
    }
    catch(...) {
        throw;
    }
}

typedef float SbSample;
const SbSample SbSample_MIN = -1.0;
const SbSample SbSample_MAX = 1.0;
const SbSample SbSample_ZERO = 0.0;

#if defined(NO_RTMIDI)
#define NO_MIDIFRONTEND
#endif

#if defined(NO_RTAUDIO) & defined(NO_PORTAUDIO) & defined(NO_JACK)
#define NO_AUDIOBACKEND
#endif

class sbException {
public:
    sbException(std::string type, std::string info = "",
            sb::errs::fixes::ProblemFix* fix1 = nullptr,
            sb::errs::fixes::ProblemFix* fix2 = nullptr,
            sb::errs::fixes::ProblemFix* fix3 = nullptr,
            sb::errs::fixes::ProblemFix* fix4 = nullptr,
            sb::errs::fixes::ProblemFix* fix5 = nullptr,
            sb::errs::fixes::ProblemFix* fix6 = nullptr,
            sb::errs::fixes::ProblemFix* fix7 = nullptr) {
        errdata.first = type;
        errdata.second = info;
        fixlist.push_back(fix1);
        fixlist.push_back(fix2);
        fixlist.push_back(fix3);
        fixlist.push_back(fix4);
        fixlist.push_back(fix5);
        fixlist.push_back(fix6);
        fixlist.push_back(fix7);
    }
    std::pair<std::string, std::string> errdata;
    std::vector<sb::errs::fixes::ProblemFix*> fixlist;
};

namespace sb {

    enum EmitterType {
		NO_EMITTER,
		PORTAUDIO_O,
		RTAUDIO_O,
        JACK_O
    };

    enum FrontendType {
		NO_MIDI,
		RTMIDI_I,
        JACK_I //Not implemented.
    };

	const size_t OUTPUT_CHANS = 2;
	const float PI = 3.14159265358979323846264338327950288f;

	const size_t DEFAULT_POLYPHONY = 12;

	const size_t SAMPLING_RATES[] = {44100,48000,88200,96000,176400,192000};
	const size_t SAMPLING_RATES_COUNT = 6;

    extern size_t global_srate;
    inline float getFrequencyFromNote(int8_t delta, float A4 = 440.00) {
        return A4*std::pow(pow(2.0,1.0/12),delta);
    }
}

#endif // COMMON_H
