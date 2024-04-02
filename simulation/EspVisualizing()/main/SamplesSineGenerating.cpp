/*
  This file is part of the DSP-Crowd project
  https://www.dsp-crowd.com

  Author(s):
      - Johannes Natter, office@dsp-crowd.com

  File created on 26.03.2024

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

#include <cinttypes>
#include <cmath>

#include "SamplesSineGenerating.h"

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

SamplesSineGenerating::SamplesSineGenerating()
	: Processing("SamplesSineGenerating")
	, mStartMs(0)
	// Config
	, mFreqSignalHz(1051)
	, mFreqSampleHz(10000)
	, mNumPkts(37)
	, mNumSamplesPerPkt(89)
	// Process states
	, mDx(0.0)
	, mDy(0.0)
	, mX(1.0)
	, mY(0.0)
	, mpSamplesWork(NULL)
	, mSamplesWritten(0)
	, mGenActive(false)
{
	mState = StStart;
}

/* member functions */

Success SamplesSineGenerating::process()
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

		// Parameter check

		if (mFreqSampleHz < 1000 || mFreqSampleHz > 30000)
			return procErrLog(-1, "sample frequency out of range");

		if (mNumPkts < 1 || mNumPkts > 1000)
			return procErrLog(-1, "max number of sample packets out of range");

		if (mNumSamplesPerPkt < 1 || mNumSamplesPerPkt > 1000)
			return procErrLog(-1, "size of sample packets out of range");

		ppPktSamples.sizeMaxSet(mNumPkts);

		coeffUpdate();

		mGenActive = true;

		mState = StMain;

		break;
	case StMain:

		if (ppPktSamples.isFull())
			break;

		if (!mpSamplesWork)
		{
			mpSamplesWork = new (nothrow) vector<int16_t>();
			//procWrnLog("created new sample packet  %p", mpSamplesWork);
		}

		if (!mpSamplesWork)
			return procErrLog(-1, "could not create sample buffer");

		sampleCreate();

		break;
	case StTmp:

		break;
	default:
		break;
	}

	return Pending;
}

Success SamplesSineGenerating::shutdown()
{
	if (mpSamplesWork)
	{
		//procWrnLog("deleted working buffer     %p", mpSamplesWork);
		delete mpSamplesWork;
		mpSamplesWork = NULL;
	}

	PipeEntry<vector<int16_t> *> entry;

	while (!ppPktSamples.isEmpty())
	{
		ppPktSamples.get(entry);
		//procWrnLog("deleted buffer in pipe     %p", entry.particle);
		delete entry.particle;
		entry.particle = NULL;
	}

	return Positive;
}

void SamplesSineGenerating::sampleCreate()
{
	if (!mSamplesWritten)
		mpSamplesWork->reserve(mNumSamplesPerPkt);

	float xNew = mX * mDx - mY * mDy;
	float yNew = mX * mDy + mY * mDx;

	int16_t sample = INT16_MAX * mY;
#if 0
	procWrnLog("sample                     %f", mY);
	procWrnLog("sample                     %7" PRIi16, sample);
#endif
	mpSamplesWork->push_back(sample);
	mX = xNew, mY = yNew;

	++mSamplesWritten;
	if (mSamplesWritten < mNumSamplesPerPkt)
		return;

	//procWrnLog("commited sample packet     %p", mpSamplesWork);
	//procWrnLog("commited sample packet     %p", mpSamplesWork->data());

	ppPktSamples.commit(mpSamplesWork);
	mpSamplesWork = NULL;
	mSamplesWritten = 0;

	ppPktSamples.toPushTry();
}

void SamplesSineGenerating::amplitudeNormalize()
{
}

void SamplesSineGenerating::frequenciesSet(uint32_t freqSignalHz, uint32_t freqSampleHz)
{
	if (mFreqSignalHz < 1000 || mFreqSignalHz > 30000)
	{
		procErrLog(-1, "signal frequency out of range");
		return;
	}

	mFreqSignalHz = freqSignalHz;

	if (mFreqSampleHz)
		mFreqSampleHz = freqSampleHz;

	coeffUpdate();
}

void SamplesSineGenerating::coeffUpdate()
{
	float a = 2 * M_PI * mFreqSignalHz / mFreqSampleHz;

	mDx = cosf(a);
	mDy = sinf(a);
}

void SamplesSineGenerating::bufferSizeSet(uint16_t numPkts, uint16_t numSamplesPerPkt)
{
	mNumPkts = numPkts;
	mNumSamplesPerPkt = numSamplesPerPkt;
}

void SamplesSineGenerating::processInfo(char *pBuf, char *pBufEnd)
{
#if 1
	dInfo("State\t\t\t%s\n", ProcStateString[mState]);
#endif
	if (!mGenActive)
		return;

	dInfo("Signal frequency\t%" PRIu32 "Hz\n", mFreqSignalHz);
	dInfo("Sample frequency\t%" PRIu32 "Hz\n", mFreqSampleHz);
	dInfo("Number of packets\t%" PRIu16 "\n", mNumPkts);
	dInfo("Number of samples/pkt\t%" PRIu16 "\n", mNumSamplesPerPkt);

	dInfo("Send buffer\t");
	pBuf += progressStr(pBuf, pBufEnd,
		ppPktSamples.size(),
		ppPktSamples.sizeMax());
	dInfo("\n");
}

/* static functions */

