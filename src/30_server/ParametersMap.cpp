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
#include "Conversion.h"

#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <sstream>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace synthese
{
	using namespace util;

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
					throw ParametersMap::MissingParameterException(parameterName);
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

		void ParametersMap::insert(
			const std::string& parameterName,
			const ptime& value
		){
			insert(parameterName, value.is_not_a_date_time() ? string() : to_iso_extended_string(value.date()) + " " + to_simple_string(value.time_of_day()));
		}

		void ParametersMap::insert(
			const std::string& parameterName,
			const time_duration& value
		){
			insert(parameterName, value.is_not_a_date_time() ? string() : to_simple_string(value));
		}

		void ParametersMap::insert(
			const std::string& parameterName,
			const date& value
		){
			insert(parameterName, value.is_not_a_date() ? string() : to_iso_extended_string(value));
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





		void ParametersMap::remove( const std::string& parameterName )
		{
			Map::iterator it(_map.find(parameterName));
			if(it != _map.end()) _map.erase(it);
		}

		
		ParametersMap::MissingParameterException::MissingParameterException( const std::string& field ):
			_field(field),
			_message("Missing parameter in request parsing : " + _field)
		{
		}

		ParametersMap::MissingParameterException::~MissingParameterException() throw()
		{
		}

		const char* ParametersMap::MissingParameterException::what() const throw()
		{
			return _message.c_str();
		}

		const std::string& ParametersMap::MissingParameterException::getField() const
		{
			return _field;
		}
	}
}
