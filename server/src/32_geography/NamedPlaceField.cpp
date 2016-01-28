
/** NamedPlaceField class implementation.
	@file NamedPlaceField.cpp

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

#include "NamedPlaceField.hpp"

#include "City.h"
#include "CityTableSync.h"
#include "Env.h"
#include "NamedPlace.h"
#include "ObjectBase.hpp"
#include "ParametersMap.h"

using namespace boost;
using namespace std;

namespace synthese
{
	template<>
	const Field ComplexObjectFieldDefinition<geography::NamedPlaceField>::FIELDS[] = {
		Field("name", SQL_TEXT),
		Field("city_id", SQL_INTEGER),
	Field() };

	namespace geography
	{
		using namespace graph;
		using namespace util;

		bool NamedPlaceField::LoadFromRecord(
			Type& fieldObject,
			ObjectBase& object,
			const Record& record,
			const util::Env& env
		){
			assert(dynamic_cast<NamedPlace*>(&object));
			NamedPlace& place(dynamic_cast<NamedPlace&>(object));
			bool result(false);

			if(record.isDefined(FIELDS[0].name))
			{
				// Name
				string value(record.getDefault<string>(FIELDS[0].name));
				if(place.getName() != value)
				{
					place.setName(value);
					result = true;
				}
			}

			if(record.isDefined(FIELDS[1].name))
			{
				RegistryKeyType city_id(record.getDefault<RegistryKeyType>(FIELDS[1].name, 0));
				place.setCity(NULL);
				if(city_id > 0)
				{
					try
					{
						City* city(CityTableSync::GetEditable(city_id, const_cast<util::Env&>(env)).get());
						if(place.getCity() != city)
						{
							place.setCity(city);
							result = true;
						}
					}
					catch(ObjectNotFoundException<City>&)
					{
						Log::GetInstance().warn(
							"Data corrupted in on place " + lexical_cast<string>(place.getKey()) +" : city " +
							lexical_cast<string>(city_id) + " not found"
						);
					}
				}
			}

			return result;
		}



		void NamedPlaceField::SaveToFilesMap( const Type& fieldObject, const ObjectBase& object, FilesMap& map )
		{

		}



		void NamedPlaceField::SaveToParametersMap( const Type& fieldObject, const ObjectBase& object, util::ParametersMap& map, const std::string& prefix, boost::logic::tribool withFiles )
		{
			if(withFiles == true)
			{
				return;
			}

			assert(dynamic_cast<const NamedPlace*>(&object));
			const NamedPlace& place(dynamic_cast<const NamedPlace&>(object));

			// Name
			map.insert(
				prefix + FIELDS[0].name,
				place.getName()
			);

			// City id
			map.insert(
				prefix + FIELDS[1].name,
				place.getCity() ? place.getCity()->getKey() : RegistryKeyType(0)
			);
		}



		void NamedPlaceField::SaveToDBContent( const Type& fieldObject, const ObjectBase& object, DBContent& content )
		{
			assert(dynamic_cast<const NamedPlace*>(&object));
			const NamedPlace& place(dynamic_cast<const NamedPlace&>(object));

			// Name
			content.push_back(Cell(place.getName()));

			// City id
			content.push_back(Cell(place.getCity() ? place.getCity()->getKey() : RegistryKeyType(0)));
		}



		void NamedPlaceField::GetLinkedObjectsIds( LinkedObjectsIds& list, const Record& record )
		{
			RegistryKeyType city_id(record.getDefault<RegistryKeyType>(FIELDS[1].name, 0));
			if(city_id > 0)
			{
				list.push_back(city_id);
			}
		}
}	}
