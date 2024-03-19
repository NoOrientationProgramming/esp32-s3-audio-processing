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

#ifndef APP_SUPERVISING_H
#define APP_SUPERVISING_H

#include "Processing.h"

class AppSupervising : public Processing
{

public:

	static AppSupervising *create()
	{
		return new (std::nothrow) AppSupervising;
	}

protected:

	AppSupervising();
	virtual ~AppSupervising() {}

private:

	AppSupervising(const AppSupervising &) : Processing("") {}
	AppSupervising &operator=(const AppSupervising &) { return *this; }

	/*
	 * Naming of functions:  objectVerb()
	 * Example:              peerAdd()
	 */

	/* member functions */
	Success process();
	void processInfo(char *pBuf, char *pBufEnd);

	void dbgStart();

	/* member variables */
	uint32_t mStartMs;
	Processing *mpSimu;

	/* static functions */

	/* static variables */

	/* constants */

};

#endif

