/*
  This file is part of the DSP-Crowd project
  https://www.dsp-crowd.com

  Author(s):
      - Johannes Natter, office@dsp-crowd.com

  File created on 21.02.2024

  Copyright (C) 2024-now Authors and www.dsp-crowd.com

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef AUDIO_PROCESSING_H
#define AUDIO_PROCESSING_H

#include "Processing.h"
#include "EspLedPulsing.h"
#include "ThreadPooling.h"

class AudioProcessing : public Processing
{

public:

	static AudioProcessing *create()
	{
		return new (std::nothrow) AudioProcessing;
	}

protected:

	AudioProcessing();
	virtual ~AudioProcessing() {}

private:

	AudioProcessing(const AudioProcessing &) : Processing("") {}
	AudioProcessing &operator=(const AudioProcessing &) { return *this; }

	/*
	 * Naming of functions:  objectVerb()
	 * Example:              peerAdd()
	 */

	/* member functions */
	Success process();
	void processInfo(char *pBuf, char *pBufEnd);

	void wifiCheck();
	void cmdSimdCalc(char *pArgs, char *pBuf, char *pBufEnd);

	/* member variables */
	uint32_t mStartMs;
	uint32_t mDiffLoopMs;
	uint32_t mLastTimeLoopMs;
	EspLedPulsing *mpLed;
	ThreadPooling *mpPool;
	bool mOkWifiOld;

	/* static functions */
	static void poolDriverCreate(Processing *pDrv, uint16_t idDrv);
	static void cpuBoundDrive(void *arg);
	static void cmdDummyCalc(char *pArgs, char *pBuf, char *pBufEnd);

	/* static variables */

	/* constants */

};

#endif

