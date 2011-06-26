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

#include "URI.hpp"
#include "ParametersMap.h"
#include "Request.h"

#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <sstream>
#include <geos/geom/Point.h>
#include <geos/geom/LineString.h>
#include <geos/geom/Coordinate.h>
#include <geos/geom/CoordinateSequence.h>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace geos::geom;

namespace synthese
{
	namespace util
	{
		ParametersMap::ParametersMap( const std::string& text )
		{
			typedef tokenizer<char_separator<char> > _tokenizer;
			char_separator<char> sep(URI::PARAMETER_SEPARATOR.c_str ());

			// Parsing
			_tokenizer parametersTokens (text, sep);
			BOOST_FOREACH(const string& parameterToken, parametersTokens)
			{
				size_t pos = parameterToken.find(URI::PARAMETER_ASSIGNMENT);
				if (pos == string::npos) continue;

				string parameterName (parameterToken.substr (0, pos));
				string parameterValue;
				if(URI::Decode(parameterToken.substr(pos+1), parameterValue))
				{
					_map.insert (make_pair (parameterName, parameterValue));
				}
			}
		}



		ParametersMap::ParametersMap()
		{}



		ParametersMap::ParametersMap(
			const Map& source
		):	_map(source)
		{}



		void ParametersMap::insert( const std::string& parameterName, const std::string& value )
		{
			_map[parameterName] = value;
		}



		void ParametersMap::insert( const std::string& parameterName, int value )
		{
			insert(parameterName, lexical_cast<string>(value));
		}



		void ParametersMap::insert( const std::string& parameterName, double value )
		{
			insert(parameterName, lexical_cast<string>(value));
		}



		void ParametersMap::insert( const std::string& parameterName, RegistryKeyType value )
		{
			insert(parameterName, lexical_cast<string>(value));
		}



		void ParametersMap::insert( const std::string& parameterName, bool value )
		{
			insert(parameterName, lexical_cast<string>(value));
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



		void ParametersMap::insert( const std::string& parameterName, std::size_t value )
		{
			insert(parameterName, lexical_cast<string>(value));
		}



		void ParametersMap::insert( const std::string& parameterName, boost::shared_ptr<ParametersMap> value )
		{
			SubParametersMap::iterator it(
				_subMap.insert(make_pair(parameterName, SubParametersMap::mapped_type())).first
			);
			it->second.push_back(value);
		}



		void ParametersMap::outputURI(
			ostream& os,
			string prefix
		) const {
			// Values stored in the root
			for (Map::const_iterator iter = _map.begin();
				iter != _map.end();
				++iter
			){
				if (iter != _map.begin ())
				{
					os << URI::PARAMETER_SEPARATOR;
				}
				os << prefix << iter->first << URI::PARAMETER_ASSIGNMENT << URI::Encode(iter->second);
			}

			// Sub maps
			BOOST_FOREACH(const SubParametersMap::value_type& itSubMap, _subMap)
			{
				BOOST_FOREACH(const SubParametersMap::mapped_type::value_type& itVecSubMap, itSubMap.second)
				{
					itVecSubMap->outputURI(os, itSubMap.first + "_");
				}
			}
		}



		void ParametersMap::outputXML(
			std::ostream& os,
			const std::string& tag,
			bool withHeader,
			std::string schemaLocation
		) const {
			if(withHeader)
			{
				os << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
			}
			os << "<" << tag;
			if(!schemaLocation.empty())
			{
				os << " xsi:noNamespaceSchemaLocation=\"" << schemaLocation << "\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"";
			}
			BOOST_FOREACH(const Map::value_type& itMap, _map)
			{
				os << " " << itMap.first << "=\"" << itMap.second << "\"";
			}
			bool hasSubTag(false);
			BOOST_FOREACH(const SubParametersMap::value_type& itSubMap, _subMap)
			{
				BOOST_FOREACH(const SubParametersMap::mapped_type::value_type& itVecSubMap, itSubMap.second)
				{
					if(!hasSubTag)
					{
						os << ">";
						hasSubTag = true;
					}
					itVecSubMap->outputXML(os, itSubMap.first);
				}
			}
			if(hasSubTag)
			{
				os << "</" << tag << ">";
			}
			else
			{
				os << " />";
			}
		}



		void ParametersMap::outputJSON(
			std::ostream& os,
			const std::string& tag,
			bool first
		) const {

			// Head
			if(first)
			{
				os << "{";
			}
			if(!tag.empty())
			{
				os << "\"" << tag << "\":";
			}
			os << "{";

			// Tags
			bool firstItem(true);
			BOOST_FOREACH(const Map::value_type& itMap, _map)
			{
				if(firstItem)
				{
					firstItem = false;
				}
				else
				{
					os << ",";
				}
				os << "\"" << itMap.first << "\":\"" << itMap.second << "\"";
			}

			// Child objects
			BOOST_FOREACH(const SubParametersMap::value_type& itSubMap, _subMap)
			{
				if(firstItem)
				{
					firstItem = false;
				}
				else
				{
					os << ",";
				}
				bool is_array(itSubMap.second.size() > 1);
				if(is_array)
				{
					os << "\"" << itSubMap.first << "\":[";
				}
				bool first_in_array(true);
				BOOST_FOREACH(const SubParametersMap::mapped_type::value_type& itVecSubMap, itSubMap.second)
				{
					if(is_array)
					{
						if(first_in_array)
						{
							first_in_array = false;
						}
						else
						{
							os << ",";
						}
					}
					itVecSubMap->outputJSON(os, is_array ? string() : itSubMap.first, false);
				}
				if(is_array)
				{
					os << "]";
				}
			}

			// GeoJSON
			if(_geometry.get() && !_geometry->isEmpty())
			{
				if(firstItem)
				{
					firstItem = false;
				}
				else
				{
					os << ",";
				}
				if(dynamic_cast<Point*>(_geometry.get()))
				{
					Point* point(static_cast<Point*>(_geometry.get()));
					os << "\"type\":\"Point\",\"coordinates\":[" << fixed << point->getCoordinate()->x << "," << fixed << point->getCoordinate()->y << "]";
				}
				else if(dynamic_cast<LineString*>(_geometry.get()))
				{
					LineString* lineString(static_cast<LineString*>(_geometry.get()));
					os << "\"type\":\"LineString\",\"coordinates\":[";
					for(size_t i(0); i<lineString->getCoordinatesRO()->getSize(); ++i)
					{
						if(i)
						{
							os << ",";
						}
						os << "[" << fixed << lineString->getCoordinatesRO()->getAt(i).x << "," << fixed << lineString->getCoordinatesRO()->getAt(i).y << "]";
					}
					os << "]";
				}
			}

			// Foot
			os << "}";
			if(first)
			{
				os << "}";
			}
		}



		void ParametersMap::outputCSV(
			std::ostream& os
		) const {
			bool firstItem(true);
			BOOST_FOREACH(const Map::value_type& itMap, _map)
			{
				if(firstItem)
				{
					firstItem = false;
				}
				else
				{
					os << ",";
				}
				os << "\"" << itMap.second << "\"";
			}
			os << endl;
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



		ParametersMap ParametersMap::getExtract( const std::string& keyBegin ) const
		{
			ParametersMap result;
			BOOST_FOREACH(const Map::value_type& element, _map)
			{
				if(element.first.substr(0, keyBegin.size()) == keyBegin)
				{
					result.insert(element.first, element.second);
				}
			}
			return result;
		}



		bool ParametersMap::isDefined( const std::string& parameterName ) const
		{
			return _map.find(parameterName) != _map.end();
		}



		const ParametersMap::SubParametersMap::mapped_type& ParametersMap::getSubMaps( const std::string& key ) const
		{
			SubParametersMap::const_iterator it(_subMap.find(key));
			if(it == _subMap.end())
			{
				throw MissingParameterException(key);
			}
			return it->second;
		}



		bool ParametersMap::hasSubMaps( const std::string& key ) const
		{
			SubParametersMap::const_iterator it(_subMap.find(key));
			return it != _subMap.end() && !it->second.empty();
		}



		void ParametersMap::clear()
		{
			_subMap.clear();
			_map.clear();
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
