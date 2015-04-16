
/** AlarmObjectLinkException class header.
	@file AlarmObjectLinkException.h
	@author Hugues Romain
	@date 2008

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

#ifndef SYNTHESE_AlarmObjectLinkException_H
#define SYNTHESE_AlarmObjectLinkException_H

#include "Exception.h"
#include "UtilTypes.h"

#include <string>
#include <iostream>

namespace synthese
{
	namespace messages
	{
		/** Link to the alarm is broken
			@ingroup m17Exceptions refExceptions
		*/
		class AlarmObjectLinkException : public synthese::Exception
		{
		public:

			/** Constructor.
				@param message Message to display when the exception occurs.
			*/
			AlarmObjectLinkException(util::RegistryKeyType objectId, util::RegistryKeyType alarmId, const std::string& message);

			/** Destructor.
			*/
			~AlarmObjectLinkException() throw ();
		};
	}
}

#endif
