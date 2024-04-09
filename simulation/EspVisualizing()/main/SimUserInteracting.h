/*
  This file is part of the DSP-Crowd project
  https://www.dsp-crowd.com

  Author(s):
      - Johannes Natter, office@dsp-crowd.com

  File created on 20.03.2024

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

#ifndef SIM_USER_INTERACTING_H
#define SIM_USER_INTERACTING_H

#include "PhyAnimating.h"
#include "SampleSineGenerating.h"
#include "SampleSending.h"

class SimUserInteracting : public PhyAnimating
{

public:

	static SimUserInteracting *create()
	{
		return new (std::nothrow) SimUserInteracting;
	}

protected:

	SimUserInteracting();
	virtual ~SimUserInteracting() {}

private:

	SimUserInteracting(const SimUserInteracting &) : PhyAnimating("") {}
	SimUserInteracting &operator=(const SimUserInteracting &) { return *this; }

	/*
	 * Naming of functions:  objectVerb()
	 * Example:              peerAdd()
	 */

	/* member functions */
	Success animate();
	void sigGenProcess();
	void processInfo(char *pBuf, char *pBufEnd);

	Success seriesAdd();
	Success chartInit();
	void chartUpdate();

	/* member variables */
	uint32_t mStateSigGen;
	uint32_t mStartMs;
	uint32_t mStartDoneMs;
	QLineEdit *mpTxtIp;
	QCheckBox *mpSwGen;
	QProgressBar *mpPrgBuffOut;
	QProgressBar *mpPrgBuffRemote;
	QSlider *mpSlFreq;
	QPushButton *mpBtnSave;
	QLabel *mpStat;
	QChart *mpChart;
	QLineSeries *mpSeries;
	bool mSwGenCheckedOld;
	int mFreqSigHzOld;
	SampleSineGenerating *mpGen;
	SampleSending *mpSend;
	std::vector<int16_t> mSamplesProbe;

	/* static functions */

	/* static variables */

	/* constants */

};

#endif

