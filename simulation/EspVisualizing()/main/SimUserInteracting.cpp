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
#include "LibTime.h"

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
		gen(StSgStartWait) \
		gen(StSgMain) \
		gen(StSgProbingDoneWait) \
		gen(StSgShutdown) \
		gen(StSgSendSdDoneWait) \
		gen(StSgSdDoneWait) \

#define dGenSigGenStateEnum(s) s,
dProcessStateEnum(SigGenState);

#if 1
#define dGenSigGenStateString(s) #s,
dProcessStateStr(SigGenState);
#endif

// ---------------------------------------

using namespace std;

#define LOG_LVL	0

#define dNumSamplesProbe 50

SimUserInteracting::SimUserInteracting()
	: PhyAnimating("SimUserInteracting")
	, mStateSigGen(StSgStartWait)
	, mStartMs(0)
	, mStartDoneMs(0)
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
	, mpSend(NULL)
{
	mState = StStart;
}

/* member functions */

Success SimUserInteracting::animate()
{
	//uint32_t curTimeMs = millis();
	//uint32_t diffMs = curTimeMs - mStartMs;
	Success success;
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

		mpPrgBuffOut = uiProgressAdd("Sample buffer");
		if (!mpPrgBuffOut)
			return procErrLog(-1, "could not create progress bar");

		mpPrgBuffOut->setValue(0);

		mpPrgBuffRemote = uiProgressAdd("Remote target buffer");
		if (!mpPrgBuffRemote)
			return procErrLog(-1, "could not create progress bar");

		mpPrgBuffRemote->setValue(0);

		uiLineAdd();

		mpSlFreq = uiSliderAdd(20000, 1000.0, "Frequency", "Hz");
		if (!mpSlFreq)
			return procErrLog(-1, "could not create slider");

		mpSlFreq->setMinimum(mpSlFreq->value());

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

		success = chartInit();
		if (success != Positive)
			return procErrLog(-1, "could not initialize chart");

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
	uint32_t curTimeMs = millis();
	uint32_t diffMs = curTimeMs - mStartDoneMs;
	Success success;
	bool swGenChecked;
	bool genStartReq;
#if 0
	dStateTrace;
#endif
	switch (mStateSigGen)
	{
	case StSgStartWait:

		if (!mpSwGen)
			break;

		swGenChecked = mpSwGen->isChecked();
		genStartReq = !mSwGenCheckedOld && swGenChecked;
		mSwGenCheckedOld = swGenChecked;

		if (!genStartReq)
			break;

		// Generator
		mpGen = SampleSineGenerating::create();
		if (!mpGen)
		{
			procErrLog(-1, "could not create process");
			break;
		}

		mpGen->frequenciesSet(1000, 50000);

		// Param 2: 333 Samples per pkt => 666 Bytes on network per pkt
		// Param 1: 751 Pkts => ~5s buffer for 50kHz: 250k samples
		mpGen->bufferSizeSet(751, 333);
		mpGen->pressurePktSet(2000);

		start(mpGen, DrivenByExternalDriver);
		ThreadPooling::procAdd(mpGen);

		// Sending
		mpSend = SampleSending::create();
		if (!mpSend)
		{
			procErrLog(-1, "could not create process");
			repel(mpGen);
			break;
		}

		mpSend->mppPktSamples.sizeMaxSet(10);

		start(mpSend, DrivenByExternalDriver);
		ThreadPooling::procAdd(mpSend);

		mpGen->mppPktSamples.connect(&mpSend->mppPktSamples);

		mStartDoneMs = curTimeMs;
		mStateSigGen = StSgMain;

		break;
	case StSgMain:

		mpPrgBuffOut->setValue(
			(100 * mpGen->mppPktSamples.size()) / mpGen->mppPktSamples.sizeMax());

		if (diffMs > 30)
		{
			mpGen->probeRequest(dNumSamplesProbe, &mSamplesProbe);
			mStateSigGen = StSgProbingDoneWait;
			break;
		}

		if (mpSwGen->isChecked())
			break;

		mStateSigGen = StSgShutdown;

		break;
	case StSgProbingDoneWait:

		success = mpGen->success();
		if (success != Pending)
		{
			mStateSigGen = StSgShutdown;
			break;
		}

		if (mSamplesProbe.size() < dNumSamplesProbe)
			break;
#if 0
		procWrnLog("Probing done: %u samples at address %p", dNumSamplesProbe, mSamplesProbe.data());

		{
			int16_t *pSample = mSamplesProbe.data();

			for (size_t i = 0; i < mSamplesProbe.size(); ++i, ++pSample)
				procInfLog("Sample %2zu: %6d 0x%04x %6d 0x%04x", i,
							mSamplesProbe[i], (uint16_t)mSamplesProbe[i],
							*pSample, (uint16_t)*pSample);
		}
#endif
		chartUpdate();

		mStartDoneMs = curTimeMs;
		mStateSigGen = StSgMain;

		break;
	case StSgShutdown:

		cancel(mpSend);
		mpPrgBuffRemote->setValue(0);

		mStateSigGen = StSgSendSdDoneWait;

		break;
	case StSgSendSdDoneWait:

		if (!mpSend->shutdownDone())
			break;

		repel(mpSend);
		mpSend = NULL;

		cancel(mpGen);
		mpPrgBuffOut->setValue(0);

		mStateSigGen = StSgSdDoneWait;

		break;
	case StSgSdDoneWait:

		if (!mpGen->shutdownDone())
			break;

		repel(mpGen);
		mpGen = NULL;

		mStateSigGen = StSgStartWait;

		break;
	default:
		break;
	}
}

Success SimUserInteracting::chartInit()
{
	mpChart->createDefaultAxes();

	float periodMs = 1;
	float tMaxMs = 2 * periodMs;

	QAbstractAxis *pAxisX, *pAxisY;

	pAxisX = mpChart->axes(Qt::Horizontal).first();
	pAxisY = mpChart->axes(Qt::Vertical).first();

	pAxisX->setRange(-tMaxMs, tMaxMs);
	pAxisY->setRange(-1.2, 1.2);

	return Positive;
}

void SimUserInteracting::chartUpdate()
{
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
}

void SimUserInteracting::processInfo(char *pBuf, char *pBufEnd)
{
#if 1
	dInfo("State\t\t%s\n", ProcStateString[mState]);
	dInfo("Signal Generator\t%s\n", SigGenStateString[mStateSigGen]);
#endif
}

/* static functions */

