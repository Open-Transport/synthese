
/** AddressField class header.
	@file AddressField.hpp

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

#ifndef SYNTHESE_pt_website_AddressField_hpp__
#define SYNTHESE_pt_website_AddressField_hpp__

#include "ComplexObjectFieldDefinition.hpp"

#include "FrameworkTypes.hpp"

#include <boost/logic/tribool.hpp>

namespace synthese
{
	class FilesMap;
	class ObjectBase;
	class Record;

	namespace util
	{
		class Env;
		class ParametersMap;
	}

	namespace road
	{
		/** AddressField class.
			@ingroup m56
		*/
		class AddressField:
			public ComplexObjectFieldDefinition<AddressField>
		{
		public:
			typedef void* Type;
			


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

		typedef AddressField HourPeriods;
	}
}

#endif
