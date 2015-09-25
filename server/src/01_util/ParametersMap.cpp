////////////////////////////////////////////////////////////////////////////////
/// ParametersMap class implementation.
///	@file ParametersMap.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
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
#include "Conversion.h"

#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <sstream>
#include <geos/geom/Point.h>
#include <geos/geom/LineString.h>
#include <geos/geom/Coordinate.h>
#include <geos/geom/CoordinateSequence.h>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace boost::algorithm;
using namespace boost::property_tree;
using namespace geos::geom;

namespace synthese
{
	namespace util
	{
		const ParametersMap::SubParametersMap::mapped_type ParametersMap::EMPTY_SUBMAP;



		ParametersMap::ParametersMap(
			const std::string& text
		):	_format(FORMAT_INTERNAL)
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



		ParametersMap::ParametersMap(
			const std::string& content,
			const std::string& boundary
		):	_format(FORMAT_INTERNAL)
		{
			typedef split_iterator<string::iterator> string_split_iterator;
			const string fullBoundary("--"+boundary);
			for(string_split_iterator its=make_split_iterator(const_cast<string&>(content), first_finder(fullBoundary, is_iequal()));
				its != string_split_iterator();
				++its
			){
				string param(copy_range<std::string>(*its));

				enum State
				{
					undefined,
					header,
					rn1,
					n1,
					rn2,
					in_content
				};

				State state(undefined);
				string line;
				string key;
				string fieldContent;
				string fileName;
				MimeType mimeType;
				for(string::const_iterator it(param.begin()); it != param.end(); ++it)
				{
					switch(state)
					{
					case undefined:
						if(*it == 'C')
						{
							line.clear();
							line.push_back(*it);
							state = header;
						}
						break;

					case header:
						if(*it == '\r')
						{
							state = rn1;
						}
						else if(*it == '\n')
						{
							state = n1;
						}
						else
						{
							line.push_back(*it);
						}
						if(state != header)
						{
							vector<string> parts;
							split(parts, line, is_any_of(":"));
							if(parts[0] == "Content-Disposition")
							{
								vector<string> parts1;
								split(parts1, parts[1], is_any_of(";"));
								if(	parts1.size() >= 2 &&
									parts1[1].size() > 8 &&
									parts1[1].substr(0, 7) == " name=\""
								){
									key = parts1[1].substr(7, parts1[1].size() - 8);
								}
								if(	parts1.size() >= 3 &&
									parts1[2].size() > 12 &&
									parts1[2].substr(0, 11) == " filename=\""
								){
									fileName = parts1[2].substr(11, parts1[2].size() - 12);
								}
							}
							else if(parts[0] == "Content-Type")
							{
								mimeType = MimeType(trim_copy(parts[1]));
							}
						}
						break;

					case rn1:
						if(*it == '\n')
						{
							state = n1;
						}
						break;

					case n1:
						if(*it == '\r')
						{
							state = rn2;
						}
						else if(*it == '\n')
						{
							state = in_content;
						}
						else if(*it == 'C')
						{
							line.clear();
							line.push_back(*it);
							state = header;
						}
						break;

					case rn2:
						if(*it == '\n')
						{
							state = in_content;
						}
						break;

					case in_content:
						fieldContent.push_back(*it);
						break;
					}
				}

				if(!key.empty())
				{
					if(fieldContent[fieldContent.size()-1] == '\n')
					{
						fieldContent = fieldContent.substr(0, fieldContent.size() - 1);
					}
					if(fieldContent[fieldContent.size()-1] == '\r')
					{
						fieldContent = fieldContent.substr(0, fieldContent.size() - 1);
					}
					if(!fileName.empty())
					{
						File file;
						file.content = fieldContent;
						file.filename = fileName;
						file.mimeType = mimeType;
						insert(key, file);
					}
					else
					{
						insert(key, fieldContent);
				}	}
			}
		}



		ParametersMap::ParametersMap(
			SerializationFormat format
		):	_format(format)
		{}



		ParametersMap::ParametersMap(
			const Map& source,
			SerializationFormat format
		):	_map(source),
			_format(format)
		{}


		ParametersMap::~ParametersMap ()
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


#ifndef _WIN32

		void ParametersMap::insert( const std::string& parameterName, std::size_t value )
		{
			insert(parameterName, lexical_cast<string>(value));
		}

#endif

		void ParametersMap::insert(
			const std::string& parameterName,
			const boost::shared_ptr<ParametersMap>& value )
		{
			SubParametersMap::iterator it(
				_subMap.insert(make_pair(parameterName, SubParametersMap::mapped_type())).first
			);
			it->second.push_back(value);
		}



		void ParametersMap::insert( const std::string& key, const File& file )
		{
			_files[key] = file;
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
				if (iter != _map.begin () || !prefix.empty())
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
				os << " " << itMap.first << "=\"" << Conversion::ToXMLAttrString(itMap.second) << "\"";
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
			if(!tag.empty())
			{
				if(first)
				{
					os << "{";
				}
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
				os << "\"" << itMap.first << "\":\"";
				string value(itMap.second);
				algorithm::replace_all(value, "\\", "\\\\");
				algorithm::replace_all(value, "\"", "\\\"");
				os << value << "\"";
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
				// FIXME: see https://extranet.rcsmobility.com/issues/10786
				//bool is_array(itSubMap.second.size() > 1);
				bool is_array(true);
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
					Point* point(dynamic_cast<Point*>(_geometry.get()));
					os << "\"type\":\"Point\",\"coordinates\":[" << fixed << point->getCoordinate()->x << "," << fixed << point->getCoordinate()->y << "]";
				}
				else if(dynamic_cast<LineString*>(_geometry.get()))
				{
					LineString* lineString(dynamic_cast<LineString*>(_geometry.get()));
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
			if(first && !tag.empty())
			{
				os << "}";
			}
		}



		void ParametersMap::outputCSV(
			std::ostream& os,
			const string& tag,
			string separator,
			bool colNamesInFirstRow
		) const {

			// Get sub maps
			const ParametersMap::SubParametersMap::mapped_type& items(getSubMaps(tag));

			// Building field names list
			set<string> colNames;
			BOOST_FOREACH(boost::shared_ptr<ParametersMap> item, items)
			{
				BOOST_FOREACH(const Map::value_type& itMap, item->getMap())
				{
					colNames.insert(itMap.first);
				}
			}

			// First row with col names
			if(colNamesInFirstRow)
			{
				bool firstItem(true);
				BOOST_FOREACH(const string& colName, colNames)
				{
					if(firstItem)
					{
						firstItem = false;
					}
					else
					{
						os << separator;
					}
					os << "\"" << colName << "\"";
				}
				os << endl;
			}

			// Data output
			BOOST_FOREACH(boost::shared_ptr<ParametersMap> item, items)
			{
				bool firstItem(true);
				BOOST_FOREACH(const Map::value_type& itMap, item->getMap())
				{
					if(firstItem)
					{
						firstItem = false;
					}
					else
					{
						os << separator;
					}
					os << "\"" << itMap.second << "\"";
				}
				os << endl;
			}
		}



		void ParametersMap::merge(
			const ParametersMap& other,
			string prefix,
			bool withSubmap
		){
			assert(_format == other._format);

			BOOST_FOREACH(ParametersMap::Map::value_type it, other._map)
			{
				// Declaration
				string key(prefix + it.first);

				// Check if the item already exists
				if(_map.find(key) != _map.end())
				{
					continue;
				}

				// Insertion of the item in the current map
				_map.insert(
					make_pair(key, other.getValue(it.first))
				);
			}

			// Merge the submap
			if(withSubmap)
			{
				BOOST_FOREACH(const SubParametersMap::value_type& it, other._subMap)
				{
					string key(prefix + it.first);

					BOOST_FOREACH(const boost::shared_ptr<ParametersMap>& item, it.second)
					{
						insert(key, item);
					}
				}
			}
		}



		void ParametersMap::remove( const std::string& parameterName )
		{
			Map::iterator it(_map.find(parameterName));
			if(it != _map.end()) _map.erase(it);
		}



		ParametersMap ParametersMap::getExtract(
			const std::string& keyBegin
		) const	{
			ParametersMap result(_format);
			BOOST_FOREACH(const Map::value_type& element, _map)
			{
				if(	element.first.size() > keyBegin.size() &&
					element.first.substr(0, keyBegin.size()) == keyBegin
				){
					result.insert(element.first.substr(keyBegin.size()), element.second);
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
				return EMPTY_SUBMAP;
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



		ParametersMap::SubMapsKeys ParametersMap::getSubMapsKeys() const
		{
			SubMapsKeys result;
			BOOST_FOREACH(const SubParametersMap::value_type& item, _subMap)
			{
				result.insert(item.first);
			}
			return result;
		}



		std::string ParametersMap::getValue(
			const std::string& parameterName,
			bool exceptionIfMissing
		) const	{
			Map::const_iterator it(_map.find(parameterName));
			if(it == _map.end())
			{
				if(exceptionIfMissing)
				{
					throw MissingParameterException(parameterName);
				}
				else
				{
					return string();
				}
			}
			return it->second;
		}



		void ParametersMap::operator=( const ParametersMap& other )
		{
			assert(_format == other._format);

			_map = other._map;
			_files = other._files;
			_subMap = other._subMap;
			_geometry = other._geometry;
		}



		const ParametersMap::File& ParametersMap::getFile(
			const std::string& key
		) const {
			Files::const_iterator it(_files.find(key));
			if(it == _files.end())
			{
				throw MissingParameterException(key);
			}
			return it->second;
		}



		Record::FieldNames ParametersMap::getFieldNames() const
		{
			FieldNames result;
			BOOST_FOREACH(const Map::value_type& item, _map)
			{
				result.push_back(item.first);
			}
			return result;
		}



		bool ParametersMap::operator<( const ParametersMap& rhs ) const
		{
			return _map < rhs._map;
		}


		//////////////////////////////////////////////////////////////////////////
		/// Removes a submap from a map.
		/// @param parameterName the submap to remove
		void ParametersMap::removeSubMap(
			const std::string& parameterName
		){
			SubParametersMap::iterator it(_subMap.find(parameterName));
			if(it != _subMap.end()) _subMap.erase(it);
		}


		bool ParametersMap::empty() const
		{
			return _map.empty() && _files.empty() && _subMap.empty();
		}



		bool ParametersMap::operator==(
			const util::ParametersMap& other
		) const {
			return _map == other._map;
		}


		boost::shared_ptr<ParametersMap>
		ParametersMap::FromJson(const std::string& json)
		{
			boost::shared_ptr<ParametersMap> parametersMap(new ParametersMap());
			stringstream jsonStream;
			jsonStream << json;
			basic_ptree<std::string, std::string> ptree;
			json_parser::read_json(jsonStream, ptree);
			parametersMap->importPropertyTree(ptree);
			return parametersMap;
		}


		void
		ParametersMap::importPropertyTree(const property_tree::basic_ptree<std::string, std::string>& ptree)
		{
			BOOST_FOREACH( boost::property_tree::ptree::value_type const& child, ptree)
			{
				if (!child.second.empty())
				{
					boost::shared_ptr<ParametersMap> childParametersMap(new ParametersMap());
					childParametersMap->importPropertyTree(child.second);
					insert(child.first, childParametersMap);
				}
				else
				{
					insert(child.first, child.second.data());
				}
			}
		}
}	}
