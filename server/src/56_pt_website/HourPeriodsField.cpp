
/** HourPeriodsField class implementation.
	@file HourPeriodsField.cpp

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

#include "HourPeriodsField.hpp"

#include "SchemaMacros.hpp"

using namespace boost::posix_time;
using namespace std;

namespace synthese
{
	FIELD_DEFINITION_OF_TYPE(pt_website::HourPeriods, "periods", SQL_TEXT)

	namespace pt_website
	{
		const string HourPeriodsField::PERIODS_SEPARATOR = ",";
		const string HourPeriodsField::DATA_SEPARATOR = "|";


		HourPeriodsField::Type HourPeriodsField::UnSerialize( const std::string& text )
		{
			Type result;
			typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

			boost::char_separator<char> sep1 (PERIODS_SEPARATOR.c_str());
			boost::char_separator<char> sep2 (DATA_SEPARATOR.c_str());
			tokenizer tripletTokens(text, sep1);
			for (tokenizer::iterator tripletIter = tripletTokens.begin();
				tripletIter != tripletTokens.end (); ++tripletIter)
			{
				tokenizer valueTokens (*tripletIter, sep2);
				tokenizer::iterator valueIter = valueTokens.begin();

				// (beginHour|endHour|Caption)
				time_duration beginHour(duration_from_string(*valueIter));
				time_duration endHour(duration_from_string(*(++valueIter)));
				HourPeriod period(*(++valueIter), beginHour, endHour);

				result.push_back(period);
			}
			return result;
		}



		std::string HourPeriodsField::Serialize( const Type& value )
		{
			stringstream periodstr;
			for(Type::const_iterator it(value.begin()); it != value.end(); ++it)
			{
				if (it != value.begin())
				{
					periodstr << PERIODS_SEPARATOR;
				}
				periodstr <<
					to_simple_string(it->getBeginHour()) <<
					DATA_SEPARATOR << to_simple_string(it->getEndHour()) <<
					DATA_SEPARATOR << it->getCaption()
				;
			}
			return periodstr.str();
		}



		bool HourPeriodsField::LoadFromRecord(
			Type& fieldObject,
			ObjectBase& object,
			const Record& record,
			const util::Env& env
		){
			return SimpleObjectFieldDefinition<HourPeriodsField>::_LoadFromStringWithDefaultValue(
				fieldObject,
				record,
				UnSerialize,
				Type()
			);
		}



		void HourPeriodsField::SaveToFilesMap( const Type& fieldObject, const ObjectBase& object, FilesMap& map )
		{
			SimpleObjectFieldDefinition<HourPeriodsField>::_SaveToFilesMap(
				fieldObject,
				map,
				Serialize
			);
		}



		void HourPeriodsField::SaveToParametersMap(
			const Type& fieldObject,
			const ObjectBase& object,
			util::ParametersMap& map,
			const std::string& prefix,
			boost::logic::tribool withFiles
		){
			SimpleObjectFieldDefinition<HourPeriodsField>::_SaveToParametersMap(
				fieldObject,
				map,
				prefix,
				withFiles,
				Serialize
			);
		}



		void HourPeriodsField::SaveToDBContent( const Type& fieldObject, const ObjectBase& object, DBContent& content )
		{
			string s(Serialize(fieldObject));
			content.push_back(Cell(s));
		}



		void HourPeriodsField::GetLinkedObjectsIds( LinkedObjectsIds& list, const Record& record )
		{

		}
}	}
