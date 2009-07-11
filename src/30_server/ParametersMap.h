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

#include "UId.h"

#include <boost/optional.hpp>
#include <boost/lexical_cast.hpp>

namespace synthese
{
	namespace time
	{
		class DateTime;
		class Date;
		class Hour;
	}

	namespace server
	{
		class Request;

		/** ParametersMap class.
			@ingroup m15
		*/
		class ParametersMap
		{
		public:
			//////////////////////////////////////////////////////////////////////////
			/// Missing parameter at parsing exception.
			/// @ingroup m15Exception refException
			class MissingParameterException:
				public std::exception
			{
			private:
				const std::string _field;

			public:
				MissingParameterException(
					const std::string& field
				);
				~MissingParameterException() throw();

				virtual const char* what() const throw();

				const std::string& getField() const;
			};

			typedef std::map<std::string, std::string> Map;

		private:
			Map _map;

		public:
			ParametersMap();
			ParametersMap(const std::string& text);
			ParametersMap(const Map& source);

			//! \name Getters
			//@{
				const Map& getMap() const;
			//@}

			//! \name Queries
			//@{

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
					const std::string& parameterName
					, bool neededParameter
					, const std::string& source
				) const;


				boost::optional<std::string> getOptionalString(
					const std::string& parameterName
				) const;

				/** Search for the value of a parameter in a ParameterMap object and converts into an uid.
					@param parameterName Name of the searched parameter
					@param neededParameter Throw an exception if the parameter is not found and if this parameter is true
					@param source Name of the action or function that requested the parameter (for the error message only)
					@return uid Value of the parameter (UNKNOWN_VALUE/-1 if parameter nor found)
					@throw ParametersMap::MissingParameterException if the parameter is not found and if it is needed
					@author Hugues Romain
					@date 2007
				*/
				uid getUid(
					const std::string& parameterName
					, bool neededParameter
					, const std::string& source
				) const;

				/** Search for the value of a parameter in a ParameterMap object and converts into an integer.
					@param parameterName Name of the searched parameter
					@param neededParameter Throw an exception if the parameter is not found and if this parameter is true
					@param source Name of the action or function that requested the parameter (for the error message only)
					@return int Value of the parameter (UNKNOWN_VALUE/-1 if parameter nor found)
					@throw ParametersMap::MissingParameterException if the parameter is not found and if it is needed
					@author Hugues Romain
					@date 2007
				*/
				int getInt(
					const std::string& parameterName
					, bool neededParameter
					, const std::string& source
				) const;


				/** Search for the value of a parameter in a ParameterMap object and converts into an integer.
					@param parameterName Name of the searched parameter
					@param neededParameter Throw an exception if the parameter is not found and if this parameter is true
					@param source Name of the action or function that requested the parameter (for the error message only)
					@return int Value of the parameter (UNKNOWN_VALUE/-1 if parameter nor found)
					@throw ParametersMap::MissingParameterException if the parameter is not found and if it is needed
					@author Hugues Romain
					@date 2007
				*/
				double getDouble(
					const std::string& parameterName
					, bool neededParameter
					, const std::string& source
				) const;


				/** Search for the value of a parameter in a ParameterMap object and converts into an integer.
					@param parameterName Name of the searched parameter
					@param neededParameter Throw an exception if the parameter is not found and if this parameter is true
					@param defaultValue Value to return if the parameter is not found
					@param source Name of the action or function that requested the parameter (for the error message only)
					@return int Value of the parameter (defaultValue if parameter nor found)
					@throw ParametersMap::MissingParameterException if the parameter is not found and if it is needed
					@author Hugues Romain
					@date 2007
				*/
				bool getBool(
					const std::string& parameterName
					, bool neededParameter
					, bool defaultValue
					, const std::string& source
				) const;

				/** Search for the value of a parameter in a ParameterMap object and converts into an DateTime object.
					@param parameterName Name of the searched parameter
					@param neededParameter Throw an exception if the parameter is not found and if this parameter is true
					@param source Name of the action or function that requested the parameter (for the error message only)
					@return DateTime Value of the parameter (UNKNOWN_VALUE/-1 if parameter nor found)
					@throw ParametersMap::MissingParameterException if the parameter is not found and if it is needed
					@throw TimeParseException if the parameter can not be parsed as a date time (eg : empty string)
					@author Hugues Romain
					@date 2007
				*/
				time::DateTime getDateTime(
					const std::string& parameterName
					, bool neededParameter
					, const std::string& source
				) const;

				/** Search for the value of a parameter in a ParameterMap object and converts into an Date object.
					@param parameterName Name of the searched parameter
					@param neededParameter Throw an exception if the parameter is not found and if this parameter is true
					@param source Name of the action or function that requested the parameter (for the error message only)
					@return Date Value of the parameter (UNKNOWN_VALUE/-1 if parameter nor found)
					@throw ParametersMap::MissingParameterException if the parameter is not found and if it is needed
					@throw TimeParseException if the parameter can not be parsed as a date time (eg : empty string)
					@author Hugues Romain
					@date 2007
				*/
				time::Date getDate(
					const std::string& parameterName
					, bool neededParameter
					, const std::string& source
				) const;

				time::Hour getHour(
					const std::string& parameterName
					, bool neededParameter
					, const std::string& source
				) const;

				boost::optional<time::Date> getOptionalDate(
					const std::string& parameterName
				) const;

				/** Builds a query string upon a parameters map.
					@param normalize true|default = converts the url considering it is generated by a web browser (see @ref _normalizeRequest), false = do not transform the URL.
						The second option is available if the Request object is used for building virtual urls containing scripts command (eg : synthese?id='+ document.getElementById("toto").value+'&...)
					@return the generated query string
					@author Hugues Romain
					@date 2007					
				*/
				std::string getURI();

				template<class C>
				boost::optional<C> getOptional(
					const std::string& parameterName
				) const;

				template<class C>
				C get(
					const std::string& parameterName
				) const;

				template<class C>
				C getDefault(
					const std::string& parameterName,
					const C defaultValue
				) const;

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
				void insert(const std::string& parameterName, uid value);
				void insert(const std::string& parameterName, bool value);
				void insert(const std::string& parameterName, const time::DateTime& value);
				void insert(const std::string& parameterName, const time::Date& value);
				void insert(const std::string& parameterName, const time::Hour& value);

				
				/** Merge this map with an other, with priority to the current one.
				 * 
				 * @param other map to read an integrate into the current one
				 */
				void merge(const ParametersMap& other);
			//@}
		};

		template<class C>
		boost::optional<C> ParametersMap::getOptional(
			const std::string& parameterName
		) const {
			try
			{
				Map::const_iterator it(_map.find(parameterName));
				return (it == _map.end()) ? boost::optional<C>() : boost::lexical_cast<C>(it->second);
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
				return (it == _map.end()) ? defaultValue : boost::lexical_cast<C>(it->second);
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
				return boost::lexical_cast<C>(it->second);
			}
			catch(...)
			{
				throw ParametersMap::MissingParameterException(parameterName);
			}
		}
	}
}

#endif // SYNTHESE_server_ParametersMap_h__
