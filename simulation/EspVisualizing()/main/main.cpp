
#include <signal.h>
#include <iostream>
#include <thread>

#include "AppSupervising.h"

using namespace std;

Processing *pApp = NULL;

void applicationCloseRequested(int signum)
{
	(void)signum;
	cout << endl;
	pApp->unusedSet();
}

int main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	levelLogSet(4);

	signal(SIGINT, applicationCloseRequested);
	signal(SIGTERM, applicationCloseRequested);

	pApp = AppSupervising::create();
	if (!pApp)
	{
		cerr << "could not create process" << endl;
		return 1;
	}

	while (1)
	{
		pApp->treeTick();
		this_thread::sleep_for(chrono::milliseconds(2));

		if (pApp->progress())
			continue;

		break;
	}

	Success success = pApp->success();
	Processing::destroy(pApp);

	Processing::applicationClose();

	return !(success == Positive);
}

