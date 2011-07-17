////////////////////////////////////////////////////////////////////////////////
/// ParametersMap class header.
///	@file ParametersMap.h
///	@author Hugues Romain
///	@date 2008-12-27 22:13
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

#ifndef SYNTHESE_server_ParametersMap_h__
#define SYNTHESE_server_ParametersMap_h__

#include <string>
#include <map>

#include "Registry.h"

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
		class ParametersMap
		{
		public:
			//////////////////////////////////////////////////////////////////////////
			/// Missing parameter at parsing exception.
			/// @ingroup m01Exception refException
			class MissingParameterException:
				public std::exception
			{
			private:
				const std::string _field;
				const std::string _message;

			public:
				MissingParameterException(
					const std::string& field
				);
				~MissingParameterException() throw();

				virtual const char* what() const throw();

				const std::string& getField() const;
			};

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



				bool hasSubMaps(
					const std::string& key
				) const;



				/** Search for the value of a parameter in a ParameterMap object.
					@param parameterName Name of the searched parameter
					@param neededParameter Throw an exception if the parameter is not found and if this parameter is true
					@param source Name of the action or function that requested the parameter (for the error message only)
					@return std::string Value of the parameter (empty if parameter nor found)
					@throw ParametersMap::MissingParameterException if the parameter is not found and if it is needed
					@author Hugues Romain
					@date 2007
				*/
				std::string getString(
					const std::string& parameterName,
					bool neededParameter,
					const std::string& source
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
				/// @param os stream to write the result on
				/// @param tag tag of the object
				/// @param first first call of a recursion (never define this parameter)
				/// @author Hugues Romain
				/// @date 2011
				/// @since 3.3.0
				//////////////////////////////////////////////////////////////////////////
				/// Note : the columns are sorted in alphabetical order.
				void outputCSV(
					std::ostream& os
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Gets the value of an optional parameter and converts it into C type if
				/// available.
				/// @param parameterName key of the parameter to get
				/// @return the parameter value converted into the C type, undefined if
				/// impossible.
				/// @author Hugues Romain
				//////////////////////////////////////////////////////////////////////////
				/// If the parameter does not exist, is empty, or cannot be converted into the type
				/// specified by the template parameter C, then an undefined object is
				/// returned
				template<class C>
				boost::optional<C> getOptional(
					const std::string& parameterName
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Tests if the specified parameter is defined in the map.
				/// @param parameterName key of the parameter to test
				/// @return true if the parameter is present in the map even if its value
				/// is empty
				/// @author Hugues Romain
				/// @since 3.2.0
				/// @date 2010
				bool isDefined(
					const std::string& parameterName
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Gets the value of a compulsory parameter and converts it into C type.
				/// @param parameterName key of the parameter to get
				/// @return the parameter value converted into the C type
				/// @author Hugues Romain
				/// @throws MissingParameterException if the parameter does not exist or
				/// cannot be converted into the type specified by the template parameter C.
				template<class C>
				C get(
					const std::string& parameterName
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Gets the value of an optional parameter and converts it into C type if
				/// available.
				/// @param parameterName key of the parameter to get
				/// @param defaultValue value to return if the parameter is undefined
				/// @return the parameter value converted into the C type, a default value
				/// if impossible.
				/// @author Hugues Romain
				//////////////////////////////////////////////////////////////////////////
				/// If the parameter does not exist, is empty, or cannot be converted into
				/// the type specified by the template parameter C, then a default value
				/// is returned
				template<class C>
				C getDefault(
					const std::string& parameterName,
					const C defaultValue
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Gets the value of an optional parameter and converts it into C type if
				/// available.
				/// @param parameterName key of the parameter to get
				/// @return the parameter value converted into the C type, a default value
				/// if impossible.
				/// @author Hugues Romain
				//////////////////////////////////////////////////////////////////////////
				/// If the parameter does not exist, is empty, or cannot be converted into
				/// the type specified by the template parameter C, then a the object
				/// constructed by default constructor returned.
				template<class C>
				C getDefault(
					const std::string& parameterName
				) const;
			//@}

			//! \name Modifiers
			//@{
				void insert(const std::string& parameterName, const std::string& value);
				void insert(const std::string& parameterName, int value);
				void insert(const std::string& parameterName, double value);
				void insert(const std::string& parameterName, std::size_t value);
				void insert(const std::string& parameterName, util::RegistryKeyType value);
				void insert(const std::string& parameterName, bool value);
				void insert(const std::string& parameterName, const boost::posix_time::ptime& value);
				void insert(const std::string& parameterName, const boost::gregorian::date& value);
				void insert(const std::string& parameterName, const boost::posix_time::time_duration& value);
				void insert(const std::string& parameterName, boost::shared_ptr<ParametersMap> value);

				void setGeometry(boost::shared_ptr<geos::geom::Geometry> value){ _geometry = value; }

				//////////////////////////////////////////////////////////////////////////
				/// Merges this map with an other, with priority to the current one.
				/// @param other map to read an integrate into the current one
				/// @author Hugues Romain
				void merge(const ParametersMap& other);



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

		template<class C>
		boost::optional<C> ParametersMap::getOptional(
			const std::string& parameterName
		) const {
			try
			{
				Map::const_iterator it(_map.find(parameterName));
				return (it == _map.end() || it->second.empty()) ? boost::optional<C>() : boost::optional<C>(boost::lexical_cast<C>(it->second));
			}
			catch(...)
			{
				return boost::optional<C>();
			}
		}


		template<class C>
		C ParametersMap::getDefault(
			const std::string& parameterName,
			const C defaultValue
		) const {
			try
			{
				Map::const_iterator it(_map.find(parameterName));
				return (it == _map.end() || it->second.empty()) ? defaultValue : boost::lexical_cast<C>(it->second);
			}
			catch(...)
			{
				return defaultValue;
			}
		}



		template<class C>
		C ParametersMap::getDefault(
			const std::string& parameterName
		) const {
			return getDefault(parameterName, C());
		}



		template<class C>
		C ParametersMap::get(
			const std::string& parameterName
		) const {
			try
			{
				Map::const_iterator it(_map.find(parameterName));
				if (it == _map.end())
				{
					throw ParametersMap::MissingParameterException(parameterName);
				}
				return boost::lexical_cast<C>(boost::trim_copy(it->second));
			}
			catch(...)
			{
				throw ParametersMap::MissingParameterException(parameterName);
			}
		}



		// Specialization for retrieving strings, which we don't want to trim.
		template<>
		std::string ParametersMap::get(
			const std::string& parameterName
		) const;
	}
}

#endif // SYNTHESE_server_ParametersMap_h__
