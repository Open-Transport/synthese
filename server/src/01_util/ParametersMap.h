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

#include "MimeType.hpp"
#include "Registry.h"

#include <string>
#include <map>
#include <set>
#include <boost/optional.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/property_tree/ptree.hpp>

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

			/// Virtual destructor for polymorphism delete
			virtual ~ParametersMap ();

			struct File
			{
				MimeType mimeType;
				std::string filename;
				std::string content;
			};

			typedef std::map<std::string, std::string> Map;
			typedef std::map<std::string, File> Files;
			typedef std::map<std::string, std::vector<boost::shared_ptr<ParametersMap> > > SubParametersMap;

			static const SubParametersMap::mapped_type EMPTY_SUBMAP;

			//////////////////////////////////////////////////////////////////////////
			///  - FORMAT_INTERNAL : in most of cases the result of streaming
			///  - FORMAT_SQL : SQL values (eg : values in quotes)
			///  - FORMAT_XML : standard XML conventions
			typedef enum
			{
				FORMAT_INTERNAL,
				FORMAT_SQL,
				FORMAT_XML,
				FORMAT_SQL_WITHOUT_QUOTES
			} SerializationFormat;

		private:
			Map _map;
			Files _files;
			SubParametersMap _subMap;
			boost::shared_ptr<geos::geom::Geometry> _geometry;
			const SerializationFormat _format;

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Default constructor.
			/// Builds an empty parameters map.
			/// @param format format of storage of values. See SerializationFormat.
			ParametersMap(
				SerializationFormat format = FORMAT_INTERNAL
			);



			//////////////////////////////////////////////////////////////////////////
			/// Constructor from a query string.
			/// @param text the query string : key=value&key=value...
			/// Serialization format is FORMAT_INTERNAL
			ParametersMap(
				const std::string& text
			);



			//////////////////////////////////////////////////////////////////////////
			/// Constructor from a multipart/form-data content.
			/// @param content the content to parse
			/// @param boundary the separator between items
			/// Serialization format is FORMAT_INTERNAL
			ParametersMap(
				const std::string& content,
				const std::string& boundary
			);



			//////////////////////////////////////////////////////////////////////////
			/// Constructor from a map.
			/// @param source map to copy
			/// @param format format of storage of values. See SerializationFormat.
			ParametersMap(
				const Map& source,
				SerializationFormat format = FORMAT_INTERNAL
			);



			//! \name Getters
			//@{
				const Map& getMap() const { return _map; }
				SerializationFormat getFormat() const { return _format; }
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



				bool operator< (const ParametersMap& rhs) const;

				//////////////////////////////////////////////////////////////////////////
				/// Gets a submap vector.
				const SubParametersMap::mapped_type& getSubMaps(
					const std::string& key
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Gets the list of the available fields
				virtual FieldNames getFieldNames() const;



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


				bool empty() const;



				//////////////////////////////////////////////////////////////////////////
				/// Gets the value of a field
				/// @param parameterName name of the parameter to read
				/// @param exceptionIfMissing throws an exception if the parameter is undefined, else returns an empty string
				/// @return the value of the parameter
				virtual std::string getValue(
					const std::string& parameterName,
					bool exceptionIfMissing = true
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// File getter.
				/// @param key the key of the file to get
				/// @return the specified file
				const File& getFile(
					const std::string& key
				) const;
			//@}

			//! \name Modifiers
			//@{
				void insert(const std::string& parameterName, const std::string& value);
				void insert(const std::string& parameterName, int value);
				void insert(const std::string& parameterName, double value);
#ifndef _WIN32
				void insert(const std::string& parameterName, size_t value);
#endif
				void insert(const std::string& parameterName, util::RegistryKeyType value);
				void insert(const std::string& parameterName, bool value);
				void insert(const std::string& parameterName, const boost::posix_time::ptime& value);
				void insert(const std::string& parameterName, const boost::gregorian::date& value);
				void insert(const std::string& parameterName, const boost::posix_time::time_duration& value);
				void insert(const std::string& parameterName, const boost::shared_ptr<ParametersMap>& value);

				//////////////////////////////////////////////////////////////////////////
				/// File inserter.
				/// @param key The key of the file
				/// @param file The file (name and content)
				void insert(
					const std::string& key,
					const File& file
				);

				void setGeometry(const boost::shared_ptr<geos::geom::Geometry>& value){ _geometry = value; }

				//////////////////////////////////////////////////////////////////////////
				/// Merges this map with an other, with priority to the current one.
				/// @param other map to read and integrate into the current one
				/// @param prefix prefix to add before the keys coming from the map to read
				/// @author Hugues Romain
				/// @pre the format of the other parameters map is the same as the current one
				void merge(
					const ParametersMap& other,
					std::string prefix = std::string(),
					bool withSubmap = false
				);



				//////////////////////////////////////////////////////////////////////////
				/// Copy operator.
				/// @param other map to copy
				/// @author Hugues Romain
				/// @pre the format of the other parameters map is the same as the current one
				void operator=(
					const ParametersMap& other
				);



				bool operator==(
					const util::ParametersMap& other
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Removes a parameter from the map.
				/// @parameterName parameter to remove
				void remove(const std::string& parameterName);



				void removeSubMap(const std::string& parameterName);



				//////////////////////////////////////////////////////////////////////////
				/// Removes all parameters of the map.
				/// @author Hugues Romain
				/// @since 3.3.0
				/// @date 2011
				void clear();
			//@}


				/// Creates a parameter map from JSON string parsing 
				static boost::shared_ptr<ParametersMap> FromJson(const std::string& json);

			private:

				/// Fills recursively a parameter map from a boost property tree
				void importPropertyTree(const boost::property_tree::basic_ptree<std::string, std::string>& ptree);
		};
}	}

#endif // SYNTHESE_server_ParametersMap_h__
