/*
  This file is part of the DSP-Crowd project
  https://www.dsp-crowd.com

  Author(s):
      - Johannes Natter, office@dsp-crowd.com

  File created on 19.03.2024

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

#include "EspSimulating.h"
#include "ThreadPooling.h"

#define dForEach_ProcState(gen) \
		gen(StStart) \
		gen(StMain) \
		gen(StTmp) \

#define dGenProcStateEnum(s) s,
dProcessStateEnum(ProcState);

#if 1
#define dGenProcStateString(s) #s,
dProcessStateStr(ProcState);
#endif

using namespace std;

#define LOG_LVL	0

EspSimulating::EspSimulating()
	: Processing("EspSimulating")
	, mStartMs(0)
	, mpVisu(NULL)
{
	mState = StStart;
}

/* member functions */

Success EspSimulating::process()
{
	//uint32_t curTimeMs = millis();
	//uint32_t diffMs = curTimeMs - mStartMs;
	Success success;
	ThreadPooling *pThread;
#if 0
	dStateTrace;
#endif
	switch (mState)
	{
	case StStart:

		pThread = ThreadPooling::create();
		if (!pThread)
			return procErrLog(-1, "could not create process");

		pThread->workerCntSet(3);

		start(pThread);
#if 0
		mpVisu = SimUserInteracting::create();
		if (!mpVisu)
			return procErrLog(-1, "could not create process");

		start(mpVisu);
		whenFinishedRepel(mpVisu);
#endif
		mpVisu = SimUserInteracting::create();
		if (!mpVisu)
			return procErrLog(-1, "could not create process");

		start(mpVisu);

		mState = StMain;

		break;
	case StMain:

		success = mpVisu->success();
		if (success == Pending)
			break;

		return Positive;

		break;
	case StTmp:

		break;
	default:
		break;
	}

	return Pending;
}

void EspSimulating::processInfo(char *pBuf, char *pBufEnd)
{
#if 1
	dInfo("State\t\t\t%s\n", ProcStateString[mState]);
#endif
}

/* static functions */

