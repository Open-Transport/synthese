
/** HourPeriodsField class header.
	@file HourPeriodsField.hpp

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

#ifndef SYNTHESE_pt_website_HourPeriodsField_hpp__
#define SYNTHESE_pt_website_HourPeriodsField_hpp__

#include "SimpleObjectFieldDefinition.hpp"

#include "FrameworkTypes.hpp"
#include "HourPeriod.h"

namespace synthese
{
	class ObjectBase;

	namespace util
	{
		class Env;
	}

	namespace pt_website
	{
		/** HourPeriodsField class.
			@ingroup m56
		*/
		class HourPeriodsField:
			public SimpleObjectFieldDefinition<HourPeriodsField>
		{
			static const std::string PERIODS_SEPARATOR;
			static const std::string DATA_SEPARATOR;

		public:
			typedef std::vector<HourPeriod> Type;
			
			static Type UnSerialize(const std::string& text);
			static std::string Serialize(const Type& value);

			static bool LoadFromRecord(
				Type& fieldObject,
				ObjectBase& object,
				const Record& record,
				const util::Env& env
			);



			static void SaveToFilesMap(
				const Type& fieldObject,
				const ObjectBase& object,
				FilesMap& map
			);



			static void SaveToParametersMap(
				const Type& fieldObject,
				const ObjectBase& object,
				util::ParametersMap& map,
				const std::string& prefix,
				boost::logic::tribool withFiles
			);



			static void SaveToDBContent(
				const Type& fieldObject,
				const ObjectBase& object,
				DBContent& content
			);


			static void GetLinkedObjectsIds(
				LinkedObjectsIds& list, 
				const Record& record
			);
		};

		typedef HourPeriodsField HourPeriods;
	}
}

#endif // SYNTHESE_pt_website_HourPeriodsField_hpp__

