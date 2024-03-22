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
	, mpBtnSave(NULL)
	, mpSlFreq(NULL)
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

		mpTxtIp = uiLineEditAdd("Heloooo");
		if (!mpTxtIp)
			return procErrLog(-1, "could not create text edit");

		mpTxtIp->setPlaceholderText("Device IP");

		mpSlFreq = uiSliderAdd(7, 0.0, "Frequency", "Hz", true);
		if (!mpSlFreq)
			return procErrLog(-1, "could not create slider");

		mpOpt->addStretch(1);

		mpBtnSave = uiPushButtonAdd("Save Plot");
		if (!mpBtnSave)
			return procErrLog(-1, "could not create button");

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
#if 0
	QLineSeries *pSeries;

	pSeries = new (nothrow) QLineSeries();
	if (!pSeries)
	{
		procErrLog(-1, "could not create series");
		return;
	}

	pSeries->append(0, 6);
	pSeries->append(2, 4);
#else
	auto set0 = new QBarSet("Jane");
	auto set1 = new QBarSet("John");
	auto set2 = new QBarSet("Axel");
	auto set3 = new QBarSet("Mary");
	auto set4 = new QBarSet("Samantha");

	*set0 << 1 << 2 << 3 << 4 << 5 << 6;
	*set1 << 5 << 0 << 0 << 4 << 0 << 7;
	*set2 << 3 << 5 << 8 << 13 << 8 << 5;
	*set3 << 5 << 6 << 7 << 3 << 4 << 5;
	*set4 << 9 << 7 << 5 << 3 << 1 << 2;

	QBarSeries *pSeries;

	pSeries = new (nothrow) QBarSeries();
	if (!pSeries)
	{
		procErrLog(-1, "could not create series");
		return;
	}

	pSeries->append(set0);
	pSeries->append(set1);
	pSeries->append(set2);
	pSeries->append(set3);
	pSeries->append(set4);
#endif
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

