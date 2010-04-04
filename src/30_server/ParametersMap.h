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
#include <boost/date_time/posix_time/posix_time.hpp>

namespace synthese
{
	namespace server
	{
		//////////////////////////////////////////////////////////////////////////
		/// Request parameters map.
		///	@ingroup m15
		/// @author Hugues Romain
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

		private:
			Map _map;

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



				//////////////////////////////////////////////////////////////////////////
				/// Builds a query string from a parameters map.
				///	@return the generated query string
				///	@author Hugues Romain
				///	@date 2007					
				std::string getURI();



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
				void insert(const std::string& parameterName, uid value);
				void insert(const std::string& parameterName, bool value);
				void insert(const std::string& parameterName, const boost::posix_time::ptime& value);
				void insert(const std::string& parameterName, const boost::gregorian::date& value);
				void insert(const std::string& parameterName, const boost::posix_time::time_duration& value);

	
				//////////////////////////////////////////////////////////////////////////
				/// Merges this map with an other, with priority to the current one.
				/// @param other map to read an integrate into the current one
				/// @author Hugues Romain
				void merge(const ParametersMap& other);



				//////////////////////////////////////////////////////////////////////////
				/// Removes a parameter from the map.
				/// @parameterName parameter to remove
				void remove(const std::string& parameterName);
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
