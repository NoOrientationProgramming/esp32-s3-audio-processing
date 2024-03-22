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

#include "SimUserInteracting.h"

#define dForEach_ProcState(gen) \
		gen(StStart) \
		gen(StMain) \

#define dGenProcStateEnum(s) s,
dProcessStateEnum(ProcState);

#if 1
#define dGenProcStateString(s) #s,
dProcessStateStr(ProcState);
#endif

using namespace std;

#define LOG_LVL	0

SimUserInteracting::SimUserInteracting()
	: PhyAnimating("SimUserInteracting")
	, mStartMs(0)
	, mpTxtIp(NULL)
	, mpSwGen(NULL)
	, mpProgress(NULL)
	, mpSlFreq(NULL)
	, mpBtnSave(NULL)
	, mpStat(NULL)
{
	mState = StStart;
}

/* member functions */

Success SimUserInteracting::animate()
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

		uiLineAdd();

		mpTxtIp = uiLineEditAdd();
		if (!mpTxtIp)
			return procErrLog(-1, "could not create text edit");

		mpTxtIp->setPlaceholderText("Device: <ip>[:port]");

		uiLineAdd();

		mpSwGen = uiSwitchAdd("Generate signal");
		if (!mpSwGen)
			return procErrLog(-1, "could not create switch");

		mpProgress = uiProgressAdd("Send buffer");
		if (!mpProgress)
			return procErrLog(-1, "could not create progress bar");

		mpProgress->setValue(39);

		uiLineAdd();

		mpSlFreq = uiSliderAdd(7, 0.0, "Frequency", "Hz", true);
		if (!mpSlFreq)
			return procErrLog(-1, "could not create slider");

		mpOpt->addStretch(1);

		mpBtnSave = uiButtonAdd("&Save Plot");
		if (!mpBtnSave)
			return procErrLog(-1, "could not create button");

		uiLineAdd();

		mpStat = uiLabelAdd("Status");
		if (!mpStat)
			return procErrLog(-1, "could not create status label");

		mpStat->setText("OK");

		seriesAdd();

		mpWindow->setWindowTitle("ESP32S3 Simulating()");
		mpWindow->show();

		mState = StMain;

		break;
	case StMain:

		break;
	default:
		break;
	}

	return Pending;
}

void SimUserInteracting::seriesAdd()
{
	QLineSeries *pSeries;

	pSeries = new (nothrow) QLineSeries();
	if (!pSeries)
	{
		procErrLog(-1, "could not create series");
		return;
	}

	pSeries->append(0, 6);
	pSeries->append(2, 4);

	mpChart->addSeries(pSeries);
	mpChart->createDefaultAxes();
}

void SimUserInteracting::processInfo(char *pBuf, char *pBufEnd)
{
#if 1
	dInfo("State\t\t\t%s\n", ProcStateString[mState]);
#endif
}

/* static functions */

