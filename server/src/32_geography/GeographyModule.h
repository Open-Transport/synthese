

/** GeographyModule class header.
	@file GeographyModule.h
	@author Hugues Romain
	@date 2008

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

#ifndef SYNTHESE_GeographyModule_H__
#define SYNTHESE_GeographyModule_H__

#include "ModuleClassTemplate.hpp"
#include "LexicalMatcher.h"
#include "City.h"

#include <boost/shared_ptr.hpp>

namespace synthese
{
	/**	@defgroup m32Exceptions 32.01 Exceptions
		@ingroup m32

		@defgroup m32LS 32.10 Table synchronizers
		@ingroup m32

		@defgroup m32Pages 32.11 Interface Pages
		@ingroup m32

		@defgroup m32Library 32.11 Interface Library
		@ingroup m32

		@defgroup m32Rights 32.12 Rights
		@ingroup m32

		@defgroup m32Logs 32.13 DB Logs
		@ingroup m32

		@defgroup m32Admin 32.14 Administration pages
		@ingroup m32

		@defgroup m32Actions 32.15 Actions
		@ingroup m32

		@defgroup m32Functions 32.15 Functions
		@ingroup m32

		@defgroup m32File 32.16 File formats
		@ingroup m32

		@defgroup m32Alarm 32.17 Messages recipient
		@ingroup m32

		@defgroup m32 32 Geography
		@ingroup m3

		The geography module contains the objects related to the locations on earth.

		@image html uml_generic_place.png

		It defines the concepts of :
		<ul>
		<li>@ref Place</li>
		<li>@ref City</li>
		</ul>

		@{
	*/

	/** 32 Geography Module namespace.
		@author Hugues Romain
		@date 2008
	*/
	namespace geography
	{
		/** 09 Geography Module class.
			@author Hugues Romain
			@date 2008
		*/
		class GeographyModule:
			public server::ModuleClassTemplate<GeographyModule>
		{
		public:
			static const std::string MODULE_PARAM_CITY_NAME_BEFORE_PLACE_NAME;

			typedef lexical_matcher::LexicalMatcher<boost::shared_ptr<City> > CitiesMatcher;
			typedef lexical_matcher::LexicalMatcher<boost::shared_ptr<geography::Place> > GeneralAllPlacesMatcher;
			typedef std::vector<CitiesMatcher::Content> CityList;

		private:
			static GeneralAllPlacesMatcher _generalAllPlacesMatcher;
			static CitiesMatcher _citiesMatcher;
			static CitiesMatcher _citiesT9Matcher;
			static bool _cityNameBeforePlaceName;

		public:
			static GeneralAllPlacesMatcher& GetGeneralAllPlacesMatcher(){ return _generalAllPlacesMatcher; }
			static const CitiesMatcher& GetCitiesMatcher(){ return _citiesMatcher; }
			static bool GetCityNameBeforePlaceName(){ return _cityNameBeforePlaceName; }



			/** Find the best matches in the city list comparing to a text entry.
				@param fuzzyName The text entry to compare
				@param nbMatches The number of matches to return
				@param t9 true indicates that the text entry follows the t9 format (optional : default = false)
				@return synthese::pt::CityList The list of results
				@author Hugues Romain
				@date 2008
			*/
			static CityList GuessCity(
				const std::string& fuzzyName
				, int nbMatches = 10
				, bool t9 = false
			);

			static void AddToCitiesMatchers(
				CitiesMatcher::Content city
			);

			static void RemoveFromCitiesMatchers(
				CitiesMatcher::Content city
			);



			/** Called whenever a parameter registered by this module is changed.
			*/
			static void ParameterCallback(
				const std::string& name,
				const std::string& value
			);
		};
	}
	/** @} */
}

#endif // SYNTHESE_GeographyModule_H__
