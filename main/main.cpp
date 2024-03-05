
#ifndef __xtensa__
#include <signal.h>
#endif
#include <iostream>
#include <thread>

#include "AppSupervising.h"

using namespace std;

Processing *pApp = NULL;

#ifndef __xtensa__
void applicationCloseRequested(int signum)
{
	(void)signum;
	cout << endl;
	pApp->unusedSet();
}
#endif

extern "C" void app_main()
{
	levelLogSet(4);

	pApp = AppSupervising::create();
	if (!pApp)
	{
		cerr << "could not create process" << endl;
		return;
	}
#ifndef __xtensa__
	signal(SIGINT, applicationCloseRequested);
	signal(SIGTERM, applicationCloseRequested);
#endif
	while (1)
	{
		pApp->treeTick();
		this_thread::sleep_for(chrono::milliseconds(2));
#ifndef __xtensa__
		if (pApp->progress())
			continue;

		break;
#endif
	}
}

#ifndef __xtensa__
int main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	app_main();

	Success success = pApp->success();
	Processing::destroy(pApp);

	Processing::applicationClose();

	return !(success == Positive);
}
#endif

