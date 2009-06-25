////////////////////////////////////////////////////////////////////////////////
/// ParametersMap class implementation.
///	@file ParametersMap.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

// At first to avoid the Windows bug "WinSock.h has already been included"
#include "ServerModule.h"

#include "ParametersMap.h"
#include "Request.h"
#include "RequestMissingParameterException.h"
#include "DateTime.h"
#include "Conversion.h"

#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include <sstream>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace time;

	namespace server
	{


		ParametersMap::ParametersMap( const std::string& text )
		{
			typedef tokenizer<char_separator<char> > _tokenizer;
			char_separator<char> sep(Request::PARAMETER_SEPARATOR.c_str ());

			// Parsing
			_tokenizer parametersTokens (text, sep);
			BOOST_FOREACH(const string& parameterToken, parametersTokens)
			{
				size_t pos = parameterToken.find (Request::PARAMETER_ASSIGNMENT);
				if (pos == string::npos) continue;

				string parameterName (parameterToken.substr (0, pos));
				string parameterValue;
				if(ServerModule::URLDecode(parameterToken.substr(pos+1), parameterValue))
				{
					_map.insert (make_pair (parameterName, parameterValue));
				}
			}
		}

		ParametersMap::ParametersMap()
		{

		}

		ParametersMap::ParametersMap( const Map& source )
			: _map(source)
		{

		}

		std::string ParametersMap::getString(
			const std::string& parameterName,
			bool neededParameter,
			const std::string& source 
		) const
		{
			Map::const_iterator it(_map.find(parameterName));
			if (it == _map.end())
			{
				if (neededParameter)
					throw RequestMissingParameterException(parameterName, source);
				return string();
			}
			return it->second;
		}

		uid ParametersMap::getUid( const std::string& parameterName , bool neededParameter , const std::string& source ) const
		{
			const string result(getString(parameterName, neededParameter, source));
			return result.empty() ? UNKNOWN_VALUE : Conversion::ToLongLong(result);
		}

		int ParametersMap::getInt( const std::string& parameterName , bool neededParameter , const std::string& source ) const
		{
			const string result(getString(parameterName, neededParameter, source));
			return result.empty() ? UNKNOWN_VALUE : Conversion::ToInt(result);
		}

		double ParametersMap::getDouble( const std::string& parameterName , bool neededParameter , const std::string& source ) const
		{
			const string result(getString(parameterName, neededParameter, source));
			return result.empty() ? UNKNOWN_VALUE : Conversion::ToDouble(result);
		}

		bool ParametersMap::getBool( const std::string& parameterName , bool neededParameter , bool defaultValue , const std::string& source ) const
		{
			const string result(getString(parameterName, neededParameter, source));
			return result.empty() ? defaultValue : Conversion::ToBool(result);
		}

		time::DateTime ParametersMap::getDateTime( const std::string& parameterName , bool neededParameter , const std::string& source ) const
		{
			const string result(getString(parameterName, neededParameter, source));
			return result.empty() ? DateTime(TIME_UNKNOWN) : DateTime::FromSQLTimestamp(result);
		}

		time::Date ParametersMap::getDate( const std::string& parameterName , bool neededParameter , const std::string& source ) const
		{
			const string result(getString(parameterName, neededParameter, source));
			return result.empty() ? Date(TIME_UNKNOWN) : Date::FromSQLDate(result);
		}

		void ParametersMap::insert( const std::string& parameterName, const std::string& value )
		{
			_map[parameterName] = value;
		}

		void ParametersMap::insert( const std::string& parameterName, int value )
		{
			insert(parameterName, Conversion::ToString(value));
		}

		void ParametersMap::insert( const std::string& parameterName, double value )
		{
			insert(parameterName, Conversion::ToString(value));
		}

		void ParametersMap::insert( const std::string& parameterName, uid value )
		{
			insert(parameterName, Conversion::ToString(value));
		}

		void ParametersMap::insert( const std::string& parameterName, bool value )
		{
			insert(parameterName, Conversion::ToString(value));
		}

		void ParametersMap::insert( const std::string& parameterName, const time::DateTime& value )
		{
			insert(parameterName, value.toSQLString(false));
		}

		void ParametersMap::insert( const std::string& parameterName, const time::Date& value )
		{
			insert(parameterName, value.toSQLString(false));
		}

		const ParametersMap::Map& ParametersMap::getMap() const
		{
			return _map;
		}

		string ParametersMap::getURI()
		{
			stringstream ss;
			for (Map::const_iterator iter = _map.begin(); 
				iter != _map.end(); 
				++iter
				){
					if (iter != _map.begin ())
						ss << Request::PARAMETER_SEPARATOR;
					ss << iter->first << Request::PARAMETER_ASSIGNMENT << iter->second;
			}

			return ss.str();
		}
		
		
		
		void ParametersMap::merge(const ParametersMap& other)
		{
			BOOST_FOREACH(ParametersMap::Map::value_type it, other._map)
			{
				if(_map.find(it.first) != _map.end()) continue;
				_map.insert(make_pair(it.first,it.second));
			}
		}



		boost::optional<time::Date> ParametersMap::getOptionalDate( const std::string& parameterName ) const
		{
			optional<string> result(getOptionalString(parameterName));
			return result ? Date::FromSQLOptionalDate(*result) : optional<Date>();
		}



		boost::optional<std::string> ParametersMap::getOptionalString( const std::string& parameterName ) const
		{
			Map::const_iterator it(_map.find(parameterName));
			return (it == _map.end()) ? optional<string>() : optional<string>(it->second);
		}
	}
}
