/*
  This file is part of the DSP-Crowd project
  https://www.dsp-crowd.com

  Author(s):
      - Johannes Natter, office@dsp-crowd.com

  File created on 03.04.2024

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

#include "SampleSending.h"

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

SampleSending::SampleSending()
	: Processing("SampleSending")
	, mStartMs(0)
{
	mState = StStart;
}

/* member functions */

Success SampleSending::process()
{
	//uint32_t curTimeMs = millis();
	//uint32_t diffMs = curTimeMs - mStartMs;
	//Success success;
#if 0
	dStateTrace;
#endif
	switch (mState)
	{
	case StStart:

		mState = StMain;

		break;
	case StMain:

		samplesConsume();

		break;
	case StTmp:

		break;
	default:
		break;
	}

	return Pending;
}

Success SampleSending::shutdown()
{
	mppPktSamples.parentDisconnect();

	PipeEntry<vector<int16_t> *> entry;

	while (!mppPktSamples.isEmpty())
	{
		mppPktSamples.get(entry);
		//procWrnLog("deleted buffer in pipe     %p", entry.particle);
		delete entry.particle;
		entry.particle = NULL;
	}

	// Caution!
	// <---- At this point, samples from the generator
	// could be pushed into our pipe segment.
	// parentDisconnect() above MUST NOT be removed

	return Positive;
}

void SampleSending::samplesConsume()
{
	PipeEntry<vector<int16_t> *> entry;

	for (size_t i = 0; i < 4; ++i)
	{
		if (!mppPktSamples.get(entry))
			break;

		delete entry.particle;
		entry.particle = NULL;
	}
}

void SampleSending::processInfo(char *pBuf, char *pBufEnd)
{
#if 1
	dInfo("State\t\t\t%s\n", ProcStateString[mState]);
#endif
	dInfo("Sample buffer\t");
	pBuf += progressStr(pBuf, pBufEnd,
		mppPktSamples.size(),
		mppPktSamples.sizeMax());
	dInfo("\n");
}

/* static functions */

