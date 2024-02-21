
#include <thread>

#include "AppSupervising.h"

using namespace std;

extern "C" void app_main()
{
	Processing *pApp = AppSupervising::create();

	levelLogSet(4);

	while (1)
	{
		pApp->treeTick();
		this_thread::sleep_for(chrono::milliseconds(2));
	}
}

