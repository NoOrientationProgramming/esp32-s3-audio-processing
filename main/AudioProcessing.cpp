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

#ifdef __xtensa__
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#endif
#include <cinttypes>

#include "AudioProcessing.h"
#include "SystemCommanding.h"
#include "LibDspc.h"
#ifdef __xtensa__
#include "EspWifiConnecting.h"
#endif

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

#ifdef __xtensa__
extern "C" void *dummyCalc(void *pA, int b);
extern "C" void simdCpy(void *pSrc, void *pDst);
extern "C" void simdClear(void *pDst);
extern "C" void simdLdQr(void *pSrc, void *pDst);

static char bufSimdA[57];
static char bufSimdB[57];
static char bufSimdC[57];
#endif

AudioProcessing::AudioProcessing()
	: Processing("AudioProcessing")
	, mStartMs(0)
	, mDiffLoopMs(0)
	, mLastTimeLoopMs(0)
	, mpPool(NULL)
#ifdef __xtensa__
	, mpLed(NULL)
	, mOkWifiOld(false)
#endif
{
	mState = StStart;
}

/* member functions */

Success AudioProcessing::process()
{
	uint32_t curTimeMs = millis();
	//uint32_t diffMs = curTimeMs - mStartMs;
	//Success success;
#ifdef __xtensa__
	TaskHandle_t pTask;
	UBaseType_t prio;
#endif
#if 0
	dStateTrace;
#endif
	mDiffLoopMs = curTimeMs - mLastTimeLoopMs;
	mLastTimeLoopMs = curTimeMs;

	switch (mState)
	{
	case StStart:

		procInfLog("Starting main process");
#ifdef __xtensa__
		pTask = xTaskGetCurrentTaskHandle();
		prio = uxTaskPriorityGet(pTask);

		procDbgLog(LOG_LVL, "Priority of main process is %u", prio);

		mpLed = EspLedPulsing::create();
		if (!mpLed)
			return procErrLog(-1, "could not create process");

		mpLed->pinSet(GPIO_NUM_2);
		mpLed->paramSet(50, 200, 1, 800);

		mpLed->procTreeDisplaySet(false);
		start(mpLed);

		bufSimdInit();

		cmdReg(
			"ldqr",
			BIND_MEMBER_FN(cmdSimdLdQr),
			"", "Load QR. Usage: ldqr [offset]",
			"Assembler Instructions");

		cmdReg(
			"clear",
			BIND_MEMBER_FN(cmdSimdClear),
			"", "Clear SIMD style",
			"Assembler Instructions");

		cmdReg(
			"cpy",
			BIND_MEMBER_FN(cmdSimdCpy),
			"", "Copy SIMD style",
			"Assembler Instructions");

		cmdReg(
			"calc",
			&AudioProcessing::cmdDummyCalc,
			"", "Make dummy calculation",
			"Assembler Instructions");

		cmdReg(
			"print",
			BIND_MEMBER_FN(cmdBufSimdPrint),
			"", "Print SIMD buffer",
			"SIMD Buffer");

		cmdReg(
			"init",
			BIND_MEMBER_FN(cmdBufSimdInit),
			"", "Initialize SIMD buffer",
			"SIMD Buffer");
#endif
		mpPool = ThreadPooling::create();
		if (!mpPool)
		{
			procErrLog(-1, "could not create process");

			mState = StMain;
			break;
		}

		mpPool->workerCntSet(2);
#ifdef __xtensa__
		mpPool->driverCreateSet(poolDriverCreate);
#endif
		mpPool->procTreeDisplaySet(false);
		start(mpPool);

		mState = StMain;

		break;
	case StMain:

#ifdef __xtensa__
		wifiCheck();
#endif
		break;
	case StTmp:

		break;
	default:
		break;
	}

	return Pending;
}

#ifdef __xtensa__
void AudioProcessing::wifiCheck()
{
	bool ok = EspWifiConnecting::ok();

	if (ok == mOkWifiOld)
		return;
	mOkWifiOld = ok;

	if (ok)
		mpLed->paramSet(50, 200, 1, 800);
	else
		mpLed->paramSet(50, 200, 2, 600);
}

void AudioProcessing::cmdSimdLdQr(char *pArgs, char *pBuf, char *pBufEnd)
{
	const char *pA = bufSimdA;
	void *pC = addrAlign(bufSimdC);
	int offset = 0;

	if (pArgs)
		offset = strtol(pArgs, NULL, 0);

	pA += offset;

	dInfo("bufSimdA = %p, pA = %p\n", bufSimdA, pA);
	dInfo("bufSimdC = %p, pC = %p\n", bufSimdC, pC);

	simdLdQr((void *)pA, pC);
}

void AudioProcessing::cmdSimdClear(char *pArgs, char *pBuf, char *pBufEnd)
{
	void *pC = addrAlign(bufSimdC);

	(void)pArgs;

	dInfo("bufSimdC = %p, pC = %p\n", bufSimdC, pC);

	simdClear(pC);
}

void AudioProcessing::cmdSimdCpy(char *pArgs, char *pBuf, char *pBufEnd)
{
	void *pA = addrAlign(bufSimdA);
	void *pC = addrAlign(bufSimdC);

	(void)pArgs;

	dInfo("bufSimdA = %p, pA = %p\n", bufSimdA, pA);
	dInfo("bufSimdC = %p, pC = %p\n", bufSimdC, pC);

	simdCpy(pA, pC);
}

void AudioProcessing::cmdBufSimdPrint(char *pArgs, char *pBuf, char *pBufEnd)
{
	pBuf += hexDumpPrint(pBuf, pBufEnd, bufSimdA, sizeof(bufSimdA), "Buffer A");
	pBuf += hexDumpPrint(pBuf, pBufEnd, bufSimdB, sizeof(bufSimdB), "Buffer B");
	pBuf += hexDumpPrint(pBuf, pBufEnd, bufSimdC, sizeof(bufSimdC), "Buffer C");
}

void AudioProcessing::cmdBufSimdInit(char *pArgs, char *pBuf, char *pBufEnd)
{
	(void)pArgs;
	bufSimdInit();
	dInfo("SIMD buffer initialized");
}

void AudioProcessing::bufSimdInit()
{
	size_t i;

	for (i = 0; i < sizeof(bufSimdA); ++i)
	{
		bufSimdA[i] = i;
		bufSimdB[i] = i;
		bufSimdC[i] = 0;
	}
}

void *AudioProcessing::addrAlign(void *pData)
{
	int tmp = (intptr_t)pData;
	tmp &= ~0xf;
	tmp += 16;
	return (void *)tmp;
}
#endif

/*
 * Literature
 * - https://en.cppreference.com/w/c/types/integer#Format_macro_constants
 */
void AudioProcessing::processInfo(char *pBuf, char *pBufEnd)
{
#if 1
	dInfo("State\t\t\t%s\n", ProcStateString[mState]);
#endif
	dInfo("Loop duration\t\t%" PRIu32 "ms\n", mDiffLoopMs);
}

/* static functions */

#ifdef __xtensa__
/*
 * Literature
 * - https://docs.espressif.com/projects/esp-idf/en/v4.3/esp32/api-reference/system/freertos.html
 */
void AudioProcessing::poolDriverCreate(Processing *pDrv, uint16_t idDrv)
{
	xTaskCreatePinnedToCore(
		cpuBoundDrive,					// function
		!idDrv ? "Primary" : "Secondary",	// name
		4096,		// stack
		pDrv,		// parameter
		1,			// priority
		NULL,		// handle
		idDrv);		// core ID
}

void AudioProcessing::cpuBoundDrive(void *arg)
{
	Processing *pDrv = (Processing *)arg;

	dbgLog(LOG_LVL, "entered CPU bound process for driver %p", pDrv);

	while (1)
	{
		pDrv->treeTick();
		this_thread::sleep_for(chrono::milliseconds(2));
	}
}

void AudioProcessing::cmdDummyCalc(char *pArgs, char *pBuf, char *pBufEnd)
{
	int offset = 0;
	void *pBase = &offset;
	void *pRes;

	if (pArgs)
		offset = strtol(pArgs, NULL, 0);

	pRes = dummyCalc(pBase, offset);

	dInfo("Base %p, Offset %d, Result %p\n", pBase, offset, pRes);
}
#endif

