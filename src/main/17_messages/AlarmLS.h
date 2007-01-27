
/** AlarmLS class header.
	@file AlarmLS.h

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

#ifndef SYNTHESE_ENVLSXML_ALARMLS_H
#define SYNTHESE_ENVLSXML_ALARMLS_H


#include <string>

struct XMLNode;


namespace synthese
{
	namespace messages
	{
		class Alarm;


		/** Alarm XML loading/saving service class.
			@ingroup m17

			Sample XML format :

			@code
			<alarm id="1"
				name="A1"
				free="true"
				authorized="false"/>
			@endcode

		*/
		class AlarmLS
		{
		public:

			static const std::string ALARM_TAG;
			static const std::string ALARM_ID_ATTR;
			static const std::string ALARM_MESSAGE_ATTR;
			static const std::string ALARM_PERIODSTART_ATTR;
			static const std::string ALARM_PERIODEND_ATTR;

			static const std::string ALARM_LEVEL_ATTR;
			static const std::string ALARM_LEVEL_ATTR_INFO;
			static const std::string ALARM_LEVEL_ATTR_WARNING;

		private:

			AlarmLS ();
			~AlarmLS();

		public:
	    
			//! @name Query methods.
			//@{
				static void Load (XMLNode& node);

				/** @todo Not implemented.
				**/
				static XMLNode* Save (const Alarm* alarm);
			//@}


		};
	}
}

#endif
