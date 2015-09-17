
/** Fare class implementation.
	@file Fare.cpp

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

#include "Fare.hpp"
#include "FareType.hpp"
#include "FareTypeFlatRate.hpp"
#include "FareTypeDistance.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace fare;
	using namespace util;

	CLASS_DEFINITION(Fare, "t008_fares", 8)
	FIELD_DEFINITION_OF_OBJECT(Fare, "fare_id", "fare_ids")

	FIELD_DEFINITION_OF_TYPE(FareTypeEnum, "fare_type", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(Currency, "currency", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(PermittedConnectionsNumber, "connections_permitted_number", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(RequiredContinuity, "continuity_required", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(ValidityPeriod, "validity_period", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(Access, "access", SQL_DOUBLE)
	FIELD_DEFINITION_OF_TYPE(Slices, "slices", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(UnitPrice, "unit_price", SQL_DOUBLE)
	FIELD_DEFINITION_OF_TYPE(Matrix, "matrix", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(SubFares, "sub_fares", SQL_TEXT)

	namespace fare
	{
		const std::string Fare::FIELDS_SEPARATOR("|");
		const std::string Fare::ROWS_SEPARATOR(",");

		Fare::Fare(RegistryKeyType key)
		:	Registrable(key),
			Object<Fare, FareSchema> (
				Schema (
					FIELD_VALUE_CONSTRUCTOR(Key, key),
					FIELD_DEFAULT_CONSTRUCTOR(Name),
					FIELD_DEFAULT_CONSTRUCTOR(FareTypeEnum),
					FIELD_DEFAULT_CONSTRUCTOR(Currency),
					FIELD_DEFAULT_CONSTRUCTOR(PermittedConnectionsNumber),
					FIELD_VALUE_CONSTRUCTOR(RequiredContinuity, false),
					FIELD_DEFAULT_CONSTRUCTOR(ValidityPeriod),
					FIELD_DEFAULT_CONSTRUCTOR(Access),
					FIELD_DEFAULT_CONSTRUCTOR(Slices),
					FIELD_DEFAULT_CONSTRUCTOR(UnitPrice),
					FIELD_DEFAULT_CONSTRUCTOR(Matrix),
					FIELD_DEFAULT_CONSTRUCTOR(SubFares)
			)	)
		{
		}



		void Fare::setTypeNumber(FareType::FareTypeNumber number)
		{
			switch(number)
			{
				case FareType::FARE_TYPE_FLAT_RATE:
					_type = boost::shared_ptr<FareTypeFlatRate>(new FareTypeFlatRate());
					break;
				case FareType::FARE_TYPE_DISTANCE:
					_type = boost::shared_ptr<FareTypeDistance>(new FareTypeDistance());
					break;
				default:
					_type = boost::shared_ptr<FareType>(new FareType(FareType::FARE_TYPE_UNKNOWN));
					break;
			}
		}

		void Fare::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			setTypeNumber(get<FareTypeEnum>());
			if (getType())
			{
				getType()->setSlices(
					Fare::GetSlicesFromSerializedString(
						get<Slices>()
				)	);
				getType()->setAccessPrice(get<Access>());
				getType()->setIsUnitPrice(get<UnitPrice>());
			}
		}

		FareType::Slices Fare::GetSlicesFromSerializedString(
			const std::string& serializedString
		){
			FareType::Slices result;

			if(!serializedString.empty())
			{
				vector<string> rows;
				split(rows, serializedString, is_any_of(ROWS_SEPARATOR));
				BOOST_FOREACH(const string& row, rows)
				{
					try
					{
						// Parsing of the string
						vector<string> fields;
						split(fields, row, is_any_of(FIELDS_SEPARATOR));
						if(fields.size()>=3)
						{
							FareType::Slice slice;
							slice.min = lexical_cast<int>(fields[0]);
							slice.max = lexical_cast<int>(fields[1]);
							slice.price = lexical_cast<double>(fields[2]);
							// Storage
							result.push_back(slice);
						}
					}
					catch(bad_lexical_cast)
					{ // If bad cast, the row is ignored
						continue;
					}
				}
			}
			return result;
		}

		std::string Fare::SerializeSlices(
			const FareType::Slices& object
		){
			stringstream s;
			bool first(true);
			BOOST_FOREACH(const FareType::Slice& it, object)
			{
				if(first)
				{
					first = false;
				}
				else
				{
					s << ROWS_SEPARATOR;
				}

				s << it.min;
				s << FIELDS_SEPARATOR;
				s << it.max;
				s << FIELDS_SEPARATOR;
				s << it.price;
			}
			return s.str();
		}
	}
}
