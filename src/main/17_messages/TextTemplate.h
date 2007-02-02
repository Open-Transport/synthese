
/** TextTemplate class header.
	@file TextTemplate.h

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

#ifndef SYNTHESE_TextTemplate_h__
#define SYNTHESE_TextTemplate_h__

#include <string>

#include "01_util/Registrable.h"
#include "01_util/Uid.h"

#include "17_messages/Types.h"

namespace synthese
{
	namespace messages
	{
		/** Text template to paste in an alarm.
			@ingroup m17
		*/
		class TextTemplate : public util::Registrable<uid, TextTemplate>
		{
		private:
			std::string	_name;
			std::string _short_message;
			std::string	_long_message;
			AlarmLevel	_level;

		public:
			TextTemplate();

			const std::string& getName() const;
			const std::string& getShortMessage() const;
			const std::string& getLongMessage() const;
			AlarmLevel getAlarmLevel() const;

			void setName(const std::string& name);
			void setShortMessage(const std::string& message);
			void setLongMessage(const std::string& message);
			void setAlarmLevel(AlarmLevel level);
		};
	}
}

#endif // SYNTHESE_TextTemplate_h__
