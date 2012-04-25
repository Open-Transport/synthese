////////////////////////////////////////////////////////////////////////////////
/// ParametersMap class header.
///	@file ParametersMap.h
///	@author Hugues Romain
///	@date 2008-12-27 22:13
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

#ifndef SYNTHESE_server_ParametersMap_h__
#define SYNTHESE_server_ParametersMap_h__

#include "Record.hpp"

#include "Registry.h"

#include <string>
#include <map>
#include <set>
#include <boost/optional.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/algorithm/string/trim.hpp>

namespace geos
{
	namespace geom
	{
		class Geometry;
	}
}

namespace synthese
{
	namespace util
	{
		//////////////////////////////////////////////////////////////////////////
		/// Request parameters map.
		///	@ingroup m01
		/// @author Hugues Romain
		class ParametersMap:
			public Record
		{
		public:
			typedef std::map<std::string, std::string> Map;
			typedef std::map<std::string, std::vector<boost::shared_ptr<ParametersMap> > > SubParametersMap;

		private:
			Map _map;
			SubParametersMap _subMap;
			boost::shared_ptr<geos::geom::Geometry> _geometry;

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Default constructor.
			/// Builds an empty parameters map.
			ParametersMap();



			//////////////////////////////////////////////////////////////////////////
			/// Constructor from a query string.
			/// @param text the query string : key=value&key=value...
			ParametersMap(const std::string& text);



			//////////////////////////////////////////////////////////////////////////
			/// Constructor from a map.
			/// @param source map to copy
			ParametersMap(const Map& source);

			//! \name Getters
			//@{
				const Map& getMap() const { return _map; }
			//@}

			//! \name Queries
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Extracts a part of the map.
				/// @param keyBegin beginning of the key of the elements to return
				/// @return a map containing the elements with specified key pattern
				/// @author Hugues Romain
				/// @date 2010
				/// @since 3.1.16
				ParametersMap getExtract(
					const std::string& keyBegin
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Gets a submap vector.
				const SubParametersMap::mapped_type& getSubMaps(
					const std::string& key
				) const;



				typedef std::set<std::string> SubMapsKeys;

				//////////////////////////////////////////////////////////////////////////
				/// Gets the list of the keys of submaps.
				/// @author Hugues Romain
				/// @date 2012
				/// @since 3.3.0
				/// @return the list of the keys of submaps.
				SubMapsKeys getSubMapsKeys() const;



				//////////////////////////////////////////////////////////////////////////
				/// Tests if a submap registered at the specified key exists.
				/// @param key the key to test
				/// @return true or false
				bool hasSubMaps(
					const std::string& key
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Outputs the content of the parameters map as an URI.
				/// @param os stream to write the result on
				/// @param prefix text to add at the beginning of each parameter name
				///	@author Hugues Romain
				///	@date 2007
				//////////////////////////////////////////////////////////////////////////
				/// Notes :
				///  - the attributes and sub tags are sorted in alphabetical order.
				void outputURI(
					std::ostream& os,
					std::string prefix = std::string()
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Outputs the content of the parameters map as a XML object.
				/// @param os stream to write the result on
				/// @param tag tag of the object
				/// @param withHeader if true the generate the <?xml header
				/// @param schemaLocation URL of the schema to add in the first tag
				/// @author Hugues Romain
				/// @date 2011
				/// @since 3.3.0
				//////////////////////////////////////////////////////////////////////////
				/// Note : the attributes and sub tags are sorted in alphabetical order.
				void outputXML(
					std::ostream& os,
					const std::string& tag,
					bool withHeader = false,
					std::string schemaLocation = std::string()
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Outputs the content of the parameters map into a JSON document.
				/// @param os stream to write the result on
				/// @param tag tag of the object
				/// @param first first call of a recursion (never define this parameter)
				/// @author Hugues Romain
				/// @date 2011
				/// @since 3.3.0
				//////////////////////////////////////////////////////////////////////////
				/// Note : the attributes and sub tags are sorted in alphabetical order.
				void outputJSON(
					std::ostream& os,
					const std::string& tag,
					bool first = true
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Outputs the content of the parameters map into a CSV row.
				/// Note : the columns are sorted in alphabetical order.
				//////////////////////////////////////////////////////////////////////////
				/// @param os stream to write the result on
				/// @param tag tag of the objects to output (must specify a sub map)
				/// @param separator column separator (default ,)
				/// @param colNamesInFirstRow outputs the column names list on the first row
				/// @author Hugues Romain
				/// @date 2011
				/// @since 3.3.0
				void outputCSV(
					std::ostream& os,
					const std::string& tag,
					std::string separator = ",",
					bool colNamesInFirstRow = true
				) const;


				//////////////////////////////////////////////////////////////////////////
				/// Tests if the specified parameter is defined in the map.
				/// @param parameterName key of the parameter to test
				/// @return true if the parameter is present in the map even if its value
				/// is empty
				/// @author Hugues Romain
				/// @since 3.2.0
				/// @date 2010
				virtual bool isDefined(
					const std::string& parameterName
				) const;


				virtual std::string getValue(
					const std::string& parameterName
				) const;
			//@}

			//! \name Modifiers
			//@{
				void insert(const std::string& parameterName, const std::string& value);
				void insert(const std::string& parameterName, int value);
				void insert(const std::string& parameterName, double value);
#ifndef _WIN64
				void insert(const std::string& parameterName, size_t value);
#endif
				void insert(const std::string& parameterName, util::RegistryKeyType value);
				void insert(const std::string& parameterName, bool value);
				void insert(const std::string& parameterName, const boost::posix_time::ptime& value);
				void insert(const std::string& parameterName, const boost::gregorian::date& value);
				void insert(const std::string& parameterName, const boost::posix_time::time_duration& value);
				void insert(const std::string& parameterName, const boost::shared_ptr<ParametersMap>& value);

				void setGeometry(const boost::shared_ptr<geos::geom::Geometry>& value){ _geometry = value; }

				//////////////////////////////////////////////////////////////////////////
				/// Merges this map with an other, with priority to the current one.
				/// @param other map to read and integrate into the current one
				/// @param prefix prefix to add before the keys coming from the map to read
				/// @author Hugues Romain
				void merge(
					const ParametersMap& other,
					std::string prefix = std::string()
				);



				//////////////////////////////////////////////////////////////////////////
				/// Removes a parameter from the map.
				/// @parameterName parameter to remove
				void remove(const std::string& parameterName);



				//////////////////////////////////////////////////////////////////////////
				/// Removes all parameters of the map.
				/// @author Hugues Romain
				/// @since 3.3.0
				/// @date 2011
				void clear();
			//@}
		};
}	}

#endif // SYNTHESE_server_ParametersMap_h__
