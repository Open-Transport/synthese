
/** TextTemplate class header.
	@file TextTemplate.h

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

#ifndef SYNTHESE_TextTemplate_h__
#define SYNTHESE_TextTemplate_h__

#include <string>

#include "Registrable.h"
#include "Registry.h"

#include "MessagesTypes.h"

namespace synthese
{
	namespace messages
	{
		/** Text template to paste in an alarm.
			@ingroup m17
		*/
		class TextTemplate
		:	public virtual util::Registrable
		{
		public:

			/// Chosen registry class.
			typedef util::Registry<TextTemplate>	Registry;

		private:
			std::string	_name;
			std::string _short_message;
			std::string	_long_message;
			util::RegistryKeyType			_parentId;
			bool		_isFolder;
			AlarmLevel	_level;

		public:
			TextTemplate(util::RegistryKeyType key = 0);

			//! @name Getters
			//@{
				virtual std::string	getName()			const;
				const std::string&	getShortMessage()	const;
				const std::string&	getLongMessage()	const;
				AlarmLevel			getAlarmLevel()		const;
				bool				getIsFolder()		const;
				util::RegistryKeyType	getParentId()		const { return _parentId; }
			//@}

			//! @name Setters
			//@{
				void setName(const std::string& name);
				void setShortMessage(const std::string& message);
				void setLongMessage(const std::string& message);
				void setAlarmLevel(AlarmLevel level);
				void setIsFolder(bool value);
				void setParentId(util::RegistryKeyType value) { _parentId = value; }
			//@}
		};
	}
}

#endif // SYNTHESE_TextTemplate_h__
