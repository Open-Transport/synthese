
/** ResaRight class header.
	@file ResaRight.h
	@author Hugues Romain
	@date 2007

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef SYNTHESE_resa_ResaRight_H__
#define SYNTHESE_resa_ResaRight_H__

#include "NetworkLineBasedRight.h"

namespace synthese
{
	namespace resa
	{
		/** ResaRight Right Class.
			@ingroup m51Rights refRights

			Here are the different right levels :

			Private rights :
				- USE : Nothing
				- READ : The user read the list of his own reservations
				- WRITE : The user can do a reservation for himself
				- DELETE : The user can cancel a reservation created by himself

			Public rights :
				- USE : Nothing
				- READ : The user can read the list of all the reservations
				- WRITE : The user can do a reservation for anyone
				- DELETE : The user can cancel any reservation

			Perimeter definition :
				- CommercialLine object : the rules apply only on reservation for the specified line
				- TransportNetwork : the rules apply only on reservations for lines of the specified network
		*/
		class ResaRight : public pt::NetworkLineBasedRight<ResaRight>
		{
		public:
			ResaRight();
		};
	}
}

#endif // SYNTHESE_resa_ResaRight_H__
