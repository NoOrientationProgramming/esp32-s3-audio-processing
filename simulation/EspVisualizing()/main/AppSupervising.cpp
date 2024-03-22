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

#include <unistd.h>
#include <signal.h>

#include "AppSupervising.h"
#include "SystemDebugging.h"
#include "EspSimulating.h"
#include "LibFilesys.h"

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

static Processing *pTreeRoot = NULL;
static char nameApp[16];
static char nameFileProc[64];
static char buffProcTree[2*1024*1024];
static bool procTreeSaveInProgress = false;

AppSupervising::AppSupervising()
	: Processing("AppSupervising")
	, mStartMs(0)
	, mpSimu(NULL)
{
	mState = StStart;
}

static void procTreeSave()
{
	time_t now;
	FILE *pFile;
	int res;
	size_t lenReq, lenDone;

	if (procTreeSaveInProgress)
		return;
	procTreeSaveInProgress = true;

	now = time(NULL);

	if (!pTreeRoot)
	{
		wrnLog("process tree root not defined");
		return;
	}

	*buffProcTree = 0;

	pTreeRoot->processTreeStr(
		buffProcTree,
		buffProcTree + sizeof(buffProcTree),
		true,
		true);

	res = snprintf(nameFileProc, sizeof(nameFileProc),
			"%ld_%s_tree-proc.txt",
			now,
			nameApp);
	if (res < 0)
	{
		wrnLog("could not create process tree file name");
		return;
	}

	pFile = fopen(nameFileProc, "w");
	if (!pFile)
	{
		wrnLog("could not open process tree file");
		return;
	}

	lenReq = strlen(buffProcTree);
	lenDone = fwrite(buffProcTree, 1, lenReq, pFile);

	if (lenDone != lenReq)
	{
		wrnLog("error writing to process tree file");
		return;
	}

	fclose(pFile);
}

// - https://man7.org/linux/man-pages/man5/core.5.html
// - https://man7.org/linux/man-pages/man7/signal.7.html
// - https://man7.org/linux/man-pages/man3/abort.3.html
void coreDumpRequest(int signum)
{
	wrnLog("Got signal: %d", signum);

	if (signum != SIGABRT)
	{
		wrnLog("Requesting core dump");
		abort();

		return;
	}

	wrnLog("Creating process tree file");
	procTreeSave();
}

/* member functions */

Success AppSupervising::process()
{
	//uint32_t curTimeMs = millis();
	//uint32_t diffMs = curTimeMs - mStartMs;
	Success success;
	bool ok;
#if 0
	dStateTrace;
#endif
	switch (mState)
	{
	case StStart:

		pTreeRoot = this;
		signal(SIGABRT, coreDumpRequest);

		ok = coreDumpsEnable(coreDumpRequest);
		if (!ok)
			procWrnLog("could not enable core dumps");

		dbgStart();

		mpSimu = EspSimulating::create();
		if (!mpSimu)
			return procErrLog(-1, "could not create process");

		start(mpSimu);

		mState = StMain;

		break;
	case StMain:

		success = mpSimu->success();
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

void AppSupervising::dbgStart()
{
	SystemDebugging *pDbg;

	pDbg = SystemDebugging::create(this);
	if (!pDbg)
	{
		procErrLog(-1, "could not create process");
		return;
	}

	pDbg->procTreeDisplaySet(false);
	SystemDebugging::levelLogSet(3);

	start(pDbg);
}

void AppSupervising::processInfo(char *pBuf, char *pBufEnd)
{
#if 1
	dInfo("State\t\t\t%s\n", ProcStateString[mState]);
#endif
}

/* static functions */

