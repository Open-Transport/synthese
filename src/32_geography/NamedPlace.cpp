
/** NamedPlace class implementation.
	@file NamedPlace.cpp

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

#include "NamedPlace.h"

#include "City.h"
#include "Env.h"
#include "GeographyModule.h"
#include "ObjectBase.hpp"
#include "ParametersMap.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace geography;
	using namespace util;

	namespace geography
	{
		NamedPlace::NamedPlace(
		):	_city(NULL)
		{}



		const std::string& NamedPlace::getOfficialName() const
		{
			return getName();
		}



		string NamedPlace::getFullName() const
		{
			if(_city == NULL)
			{
				return getName();
			}
			else if(_getCityNameBeforePlaceName())
			{
				return _city->getName() + " " + getName();
			}
			else
			{
				return getName() + " " + _city->getName();
			}
		}



		string NamedPlace::getName13OrName() const
		{
			return _name13.empty() ? _name.substr(0, 13) : _name13;
		}



		string NamedPlace::getName26OrName() const
		{
			return _name26.empty() ? _name.substr(0, 26) : _name26;
		}
	}

	template<> const Field ComplexObjectFieldDefinition<NamedPlaceData>::FIELDS[] = { Field("name", SQL_TEXT), Field("city_id", SQL_INTEGER), Field() };
	template<> const bool ComplexObjectFieldDefinition<NamedPlaceData>::EXPORT_CONTENT_AS_FILE = false;



	template<> void ComplexObjectField<NamedPlaceData, NamedPlaceData::Type>::GetLinkedObjectsIds(
		LinkedObjectsIds& list,
		const Record& record
	){
		RegistryKeyType city_id(record.getDefault<RegistryKeyType>(FIELDS[1].name, 0));
		if(city_id > 0)
		{
			list.push_back(city_id);
		}
	}



	template<>
	void ComplexObjectField<NamedPlaceData, NamedPlaceData::Type>::LoadFromRecord(
		NamedPlaceData::Type& fieldObject,
		ObjectBase& object,
		const Record& record,
		const Env& env
	){
		assert(dynamic_cast<NamedPlace*>(&object));
		NamedPlace& place(dynamic_cast<NamedPlace&>(object));

		if(record.isDefined(FIELDS[0].name))
		{
			// Name
			place.setName(record.getDefault<string>(FIELDS[0].name));
		}

		if(record.isDefined(FIELDS[1].name))
		{
			RegistryKeyType city_id(record.getDefault<RegistryKeyType>(FIELDS[1].name, 0));
			place.setCity(NULL);
			if(city_id > 0)
			{
				try
				{
					City* city(env.getEditable<City>(city_id).get());
					place.setCity(city);
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
	}

	template<> void ComplexObjectField<NamedPlaceData, NamedPlaceData::Type>::SaveToParametersMap(
		const NamedPlaceData::Type& fieldObject,
		const ObjectBase& object,
		util::ParametersMap& map,
		const std::string& prefix
	){
		assert(dynamic_cast<const NamedPlace*>(&object));
		const NamedPlace& place(dynamic_cast<const NamedPlace&>(object));

		// Name
		string escapedName;
		switch(map.getFormat())
		{
		case util::ParametersMap::FORMAT_SQL:
			escapedName = "\""+ boost::algorithm::replace_all_copy(place.getName(), "\"", "\\\"") + "\"";

		default:
			escapedName = place.getName();
		}
		map.insert(
			prefix + FIELDS[0].name,
			ObjectField<void, string>::Serialize(
				escapedName,
				map.getFormat()
		)	);

		// City id
		map.insert(
			prefix + FIELDS[1].name,
			place.getCity() ? place.getCity()->getKey() : RegistryKeyType(0)
		);
	}
}
