
/** RoadModule class header.
	@file RoadModule.h

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

#ifndef SYNTHESE_RoadModule_H__
#define SYNTHESE_RoadModule_H__

#include "GeographyModule.h"
#include "GraphModuleTemplate.h"

namespace synthese
{
	namespace geography
	{
		class NamedPlace;
	}

	/**	@defgroup m34Actions 34 Actions
		@ingroup m34

		@defgroup m34Pages 34 Pages
		@ingroup m34

		@defgroup m34Library 34 Interface library
		@ingroup m34

		@defgroup m34Functions 34 Functions
		@ingroup m34

		@defgroup m34LS 34 Table synchronizers
		@ingroup m34

		@defgroup m34Admin 34 Administration pages
		@ingroup m34

		@defgroup m34Rights 34 Rights
		@ingroup m34

		@defgroup m34Logs 34 DB Logs
		@ingroup m34

		@defgroup m34 34 Road network
		@ingroup m3

		The road module contains the graph of streets and all associated objects.

		@image html uml_road_graph.png

		The road graph is implemented by :
		<table class="table">
		<tr><th>graph interface</th><th>road implementation</th></tr>
		<tr><td>Service</td><td>PermanentService</td></tr>
		<tr><td>PathGroup</td><td>RoadPlace</td></tr>
		<tr><td>Path</td><td>Road</td></tr>
		<tr><td>Edge</td><td>RoadChunk</td></tr>
		<tr><td>Vertex</td><td>Crossing</td></tr>
		<tr><td>Hub</td><td>Crossing</td></tr>
		</table>

	@{
	*/

	/** 34 Road network module namespace.
	*/
	namespace road
	{
		class RoadPlace;
		class PublicPlace;

		/** 34 Transport network module class.
		*/
		class RoadModule:
			public graph::GraphModuleTemplate<RoadModule>
		{
		public:
			typedef lexical_matcher::LexicalMatcher<boost::shared_ptr<RoadPlace> > GeneralRoadsMatcher;
			typedef lexical_matcher::LexicalMatcher<boost::shared_ptr<PublicPlace> > GeneralPublicPlacesMatcher;

		private:
			static GeneralRoadsMatcher _generalRoadsMatcher;
			static GeneralPublicPlacesMatcher _generalPublicPlacesMatcher;

		public:
			static GeneralRoadsMatcher& GetGeneralRoadsMatcher(){ return _generalRoadsMatcher; }
			static GeneralPublicPlacesMatcher& GetGeneralPublicPlacesMatcher(){ return _generalPublicPlacesMatcher; }



			//////////////////////////////////////////////////////////////////////////
			/// Exception to throw when text fields does not permit to determinate a undetermined place.
			//////////////////////////////////////////////////////////////////////////
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.2.0
			class UndeterminedPlaceException:
				public synthese::Exception
			{
			public:
				typedef enum
				{
					EMPTY_CITY,
					NO_RESULT_FROM_CITY_SEARCH
				} Reason;

				//////////////////////////////////////////////////////////////////////////
				/// Construction from two fields.
				/// @param cityName entered city name
				/// @param placeName entered place name
				/// @param reason reason of the problem
				UndeterminedPlaceException(
					const std::string& cityName,
					const std::string& placeName,
					Reason reason
				);
			};

			struct ExtendedFetchPlaceResult
			{
				geography::GeographyModule::CitiesMatcher::MatchResult::value_type cityResult;
				lexical_matcher::LexicalMatcher<boost::shared_ptr<geography::Place> >::MatchResult::value_type placeResult;
			};

			typedef std::vector<ExtendedFetchPlaceResult> ExtendedFetchPlacesResult;

			//////////////////////////////////////////////////////////////////////////
			/// Interprets text from two fields to determinate a place.
			/// Scenarios :
			///	<table class="table">
			///	<tr><th>cityName</th><th>placeName</th><th>Returned place</th></tr>
			///	<tr><td>empty</td><td>empty</td><td>throws UndeterminedPlaceException</td></tr>
			///	<tr><td>non empty</td><td>empty</td><td>default places of the city</td></tr>
			///	<tr><td>non empty</td><td>non empty, beginning by a number</td><td>first try to find a stop which begins by the numberm then try to generate an address</td></tr>
			///	<tr><td>non empty</td><td>non empty, without number</td><td>try to find a stop or a road</td></tr>
			///	</table>
			//////////////////////////////////////////////////////////////////////////
			/// @param citiesMatcher lexical matcher to find city
			/// @param cityName name of the city to find
			/// @param placeName name of the place to find
			/// @return places in decreasing score order
			/// @author Hugues Romain
			/// @since 3.2.0
			/// @date 2010
			static ExtendedFetchPlaceResult ExtendedFetchPlace(
				const geography::GeographyModule::CitiesMatcher& citiesMatcher,
				const std::string& cityName,
				const std::string& placeName
			);



			static ExtendedFetchPlacesResult ExtendedFetchPlaces(
				boost::shared_ptr<geography::City> city,
				const std::string& placeName,
				std::size_t resultsNumber
			);



			static ExtendedFetchPlacesResult ExtendedFetchPlaces(
				const geography::GeographyModule::CitiesMatcher::MatchResult::value_type& cityResult,
				const std::string& placeName,
				std::size_t resultsNumber
			);



			//////////////////////////////////////////////////////////////////////////
			/// @param citiesMatcher lexical matcher to find city
			/// @param cityName name of the city to find
			/// @param placeName name of the place to find
			/// @param resultsNumber number of results to return
			/// @return places in decreasing score order
			/// @author Hugues Romain
			/// @since 3.2.0
			/// @date 2010
			static ExtendedFetchPlacesResult ExtendedFetchPlaces(
				const geography::GeographyModule::CitiesMatcher& citiesMatcher,
				const std::string& cityName,
				const std::string& placeName,
				std::size_t resultsNumber
			);



			static boost::shared_ptr<geography::Place> FetchPlace(
				const geography::GeographyModule::CitiesMatcher& citiesMatcher,
				const std::string& cityName,
				const std::string& placeName
			);



			//////////////////////////////////////////////////////////////////////////
			/// @param citiesMatcher lexical matcher to find city
			/// @param cityName name of the city to find
			/// @param placeName name of the place to find
			/// @return places in decreasing score order (default 1)
			/// @author Hugues Romain
			/// @since 3.2.0
			/// @date 2010
			static ExtendedFetchPlaceResult ExtendedFetchPlace(
				const std::string& cityName,
				const std::string& placeName
			);



			//////////////////////////////////////////////////////////////////////////
			/// @param citiesMatcher lexical matcher to find city
			/// @param cityName name of the city to find
			/// @param placeName name of the place to find
			/// @param resultsNumber number of results to return
			/// @return places in decreasing score order (default 1)
			/// @author Hugues Romain
			/// @since 3.2.0
			/// @date 2010
			static ExtendedFetchPlacesResult ExtendedFetchPlaces(
				const std::string& cityName,
				const std::string& placeName,
				std::size_t resultsNumber
			);

			static boost::shared_ptr<geography::Place> FetchPlace(
				const std::string& cityName,
				const std::string& placeName
			);
		};
	}
	/** @} */
}

#endif // SYNTHESE_EnvModule_H__
