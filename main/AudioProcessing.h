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
#include "ThreadPooling.h"
#ifdef __xtensa__
#include "EspLedPulsing.h"
#endif

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
#ifdef __xtensa__
	void wifiCheck();
	void cmdSimdLdQr(char *pArgs, char *pBuf, char *pBufEnd);
	void cmdSimdClear(char *pArgs, char *pBuf, char *pBufEnd);
	void cmdSimdCpy(char *pArgs, char *pBuf, char *pBufEnd);
	void cmdBufSimdPrint(char *pArgs, char *pBuf, char *pBufEnd);
	void cmdBufSimdInit(char *pArgs, char *pBuf, char *pBufEnd);
	void bufSimdInit();
	void *addrAlign(void *pData);
#endif
	/* member variables */
	uint32_t mStartMs;
	uint32_t mDiffLoopMs;
	uint32_t mLastTimeLoopMs;
	ThreadPooling *mpPool;
#ifdef __xtensa__
	EspLedPulsing *mpLed;
	bool mOkWifiOld;
#endif
	/* static functions */
#ifdef __xtensa__
	static void poolDriverCreate(Processing *pDrv, uint16_t idDrv);
	static void cpuBoundDrive(void *arg);
	static void cmdDummyCalc(char *pArgs, char *pBuf, char *pBufEnd);
#endif
	/* static variables */

	/* constants */

};

#endif

