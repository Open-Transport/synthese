
/** AlarmRecipient class header.
	@file AlarmRecipient.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#ifndef SYNTHESE_AlarmRecipient_h__
#define SYNTHESE_AlarmRecipient_h__

#include <ostream>

#include "01_util/Factorable.h"

#include "30_server/Request.h"

namespace synthese
{
	namespace messages
	{
		class Alarm;

		/** AlarmRecipient class.
			@ingroup m17
		*/
		class AlarmRecipient : public util::Factorable
		{
		private:
			const std::string _title;
			

		public:
			AlarmRecipient(const std::string& title);

			const std::string& getTitle();

			virtual void displayBroadcastListEditor(std::ostream& stream, const messages::Alarm* alarm, const server::Request* request) = 0;

		};
	}
}

#endif // SYNTHESE_AlarmRecipient_h__
