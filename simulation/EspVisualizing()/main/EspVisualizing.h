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

#ifndef ESP_VISUALIZING_H
#define ESP_VISUALIZING_H

#include "PhyAnimating.h"

class EspVisualizing : public PhyAnimating
{

public:

	static EspVisualizing *create()
	{
		return new (std::nothrow) EspVisualizing;
	}

protected:

	EspVisualizing();
	virtual ~EspVisualizing() {}

private:

	EspVisualizing(const EspVisualizing &) : PhyAnimating("") {}
	EspVisualizing &operator=(const EspVisualizing &) { return *this; }

	/*
	 * Naming of functions:  objectVerb()
	 * Example:              peerAdd()
	 */

	/* member functions */
	Success animate();
	void processInfo(char *pBuf, char *pBufEnd);

	/* member variables */
	uint32_t mStartMs;
	QPushButton *mpButton;

	/* static functions */

	/* static variables */

	/* constants */

};

#endif

