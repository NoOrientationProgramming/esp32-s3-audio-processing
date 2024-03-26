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
#include "ThreadPooling.h"

// ---------------------------------------

#define dForEach_ProcState(gen) \
		gen(StStart) \
		gen(StMain) \

#define dGenProcStateEnum(s) s,
dProcessStateEnum(ProcState);

#if 1
#define dGenProcStateString(s) #s,
dProcessStateStr(ProcState);
#endif

// ---------------------------------------

#define dForEach_SigGenState(gen) \
		gen(StStartWait) \
		gen(StStopWait) \
		gen(StSdDoneWait) \

#define dGenSigGenStateEnum(s) s,
dProcessStateEnum(SigGenState);

#if 1
#define dGenSigGenStateString(s) #s,
dProcessStateStr(SigGenState);
#endif

// ---------------------------------------

using namespace std;

#define LOG_LVL	0

SimUserInteracting::SimUserInteracting()
	: PhyAnimating("SimUserInteracting")
	, mStateSigGen(StStartWait)
	, mStartMs(0)
	, mpTxtIp(NULL)
	, mpSwGen(NULL)
	, mpPrgBuffOut(NULL)
	, mpPrgBuffRemote(NULL)
	, mpSlFreq(NULL)
	, mpBtnSave(NULL)
	, mpStat(NULL)
	, mpChart(NULL)
	, mSwGenCheckedOld(false)
	, mpGen(NULL)
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

		mpPrgBuffOut = uiProgressAdd("Send buffer");
		if (!mpPrgBuffOut)
			return procErrLog(-1, "could not create progress bar");

		//mpPrgBuffOut->setValue(39);

		mpPrgBuffRemote = uiProgressAdd("Remote buffer");
		if (!mpPrgBuffRemote)
			return procErrLog(-1, "could not create progress bar");

		uiLineAdd();

		mpSlFreq = uiSliderAdd(20000, 1000.0, "Frequency", "Hz");
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

		mpChart = uiChartAdd();
		if (!mpChart)
			return procErrLog(-1, "could not create chart");

		seriesAdd();

		mpWindow->setWindowTitle("ESP32S3 - Simulating()");
		mpWindow->show();

		mState = StMain;

		break;
	case StMain:

		sigGenProcess();

		break;
	default:
		break;
	}

	return Pending;
}

void SimUserInteracting::sigGenProcess()
{
	//uint32_t curTimeMs = millis();
	//uint32_t diffMs = curTimeMs - mStartMs;
	//Success success;
	bool swGenChecked;
	bool genStartReq;
#if 0
	dStateTrace;
#endif
	switch (mStateSigGen)
	{
	case StStartWait:

		if (!mpSwGen)
			break;

		swGenChecked = mpSwGen->isChecked();
		genStartReq = !mSwGenCheckedOld && swGenChecked;
		mSwGenCheckedOld = swGenChecked;

		if (!genStartReq)
			break;

		mpGen = SignalGenerating::create();
		if (!mpGen)
		{
			procErrLog(-1, "could not create process");
			break;
		}
#if 0
		start(mpGen, DrivenByNewInternalDriver);
#else
		start(mpGen, DrivenByExternalDriver);
		ThreadPooling::procAdd(mpGen);
#endif
		mStateSigGen = StStopWait;

		break;
	case StStopWait:

		if (mpSwGen->isChecked())
			break;

		cancel(mpGen);

		mStateSigGen = StSdDoneWait;

		break;
	case StSdDoneWait:

		if (!mpGen->shutdownDone())
			break;

		repel(mpGen);
		mpGen = NULL;

		mStateSigGen = StStartWait;

		break;
	default:
		break;
	}
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
	dInfo("Signal Generator\t\t%s\n", SigGenStateString[mStateSigGen]);
#endif
}

/* static functions */

