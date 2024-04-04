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

#ifndef SAMPLE_SINE_GENERATING_H
#define SAMPLE_SINE_GENERATING_H

#include <vector>

#include "Processing.h"
#include "Pipe.h"

class SampleSineGenerating : public Processing
{

public:

	static SampleSineGenerating *create()
	{
		return new (std::nothrow) SampleSineGenerating;
	}

	// input

	void frequenciesSet(uint32_t freqSignal, uint32_t freqSample = 0);
	void bufferSizeSet(uint16_t numPkts, uint16_t numSamplesPerPkt);

	// Unit:
	// Max number of samples which can be created
	// per software system cycle
	void pressurePktSet(uint16_t pressurePkt);
	void probeRequest(size_t numSamples, std::vector<int16_t> *pProbeSamples);

	// output

	Pipe<std::vector<int16_t> *> mppPktSamples;

protected:

	SampleSineGenerating();
	virtual ~SampleSineGenerating() {}

private:

	SampleSineGenerating(const SampleSineGenerating &) : Processing("") {}
	SampleSineGenerating &operator=(const SampleSineGenerating &) { return *this; }

	/*
	 * Naming of functions:  objectVerb()
	 * Example:              peerAdd()
	 */

	/* member functions */
	Success process();
	Success shutdown();
	void processInfo(char *pBuf, char *pBufEnd);

	void sampleCreate();
	void probeSamplesCopy(int16_t sample);
	void amplitudeNormalize();
	void coeffUpdate();

	/* member variables */

	// Config
	uint32_t mStartMs;
	uint32_t mFreqSignalHz;
	uint32_t mFreqSampleHz;
	uint16_t mNumPkts;
	uint16_t mNumSamplesPerPkt;
	uint16_t mPressurePkt;

	// Process states
	/* Literature
	- https://www.math.utah.edu/~alfeld/math/p10000.html
	- https://stackoverflow.com/questions/69729326/endless-sine-generation-in-c
	- https://stackoverflow.com/questions/17730689/is-a-moved-from-vector-always-empty
	*/
	float mDx;
	float mDy;
	float mX;
	float mY;
	std::vector<int16_t> *mpSamplesWork;
	std::vector<int16_t> *mpSamplesProbe;
	size_t mNumSamplesProbeReq;
	uint16_t mSamplesWritten;
	bool mGenActive;
	std::mutex mProbeMtx;

	/* static functions */

	/* static variables */

	/* constants */

};

#endif

