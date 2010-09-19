
/** TransportWebsiteclass header.
	@file TransportWebsite.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#ifndef SYNTHESE_CSITE_H
#define SYNTHESE_CSITE_H

#include "HourPeriod.h"
#include "LexicalMatcher.h"
#include "UtilConstants.h"
#include "GraphTypes.h"
#include "City.h"
#include "AccessParameters.h"
#include "RollingStockFilter.h"
#include "Website.hpp"
#include "GeographyModule.h"
#include "RoadModule.h"

#include <set>
#include <boost/date_time/gregorian/greg_duration.hpp>

namespace synthese
{
	namespace geography
	{
		class Place;
	}
	
	namespace graph
	{
		class AccessParameters;
	}

	namespace pt
	{
		class CommercialLine;
	}

	namespace pt_website
	{
		//////////////////////////////////////////////////////////////////////////
		/// Transport website.
		///	@ingroup m56
		///	@author Hugues Romain
		///	@date 2005-2006
		//////////////////////////////////////////////////////////////////////////
		/// Defines a  web site content, with :
		///  - content management system
		///  - transport objects selection
		///  - algorithms parameters
		class TransportWebsite:
			public virtual util::Registrable,
			public cms::Website
		{
		public:

			//////////////////////////////////////////////////////////////////////////
			/// Exception to throw when a route planning cannot be launched at the
			/// specified dates according to the site rules.
			/// @ingroup m36
			class ForbiddenDateException:
				public synthese::Exception
			{
			public:
				ForbiddenDateException();
			};

			/// Chosen registry class.
			typedef util::Registry<TransportWebsite>	Registry;

			typedef std::vector<HourPeriod> Periods;
			
			typedef std::map<std::size_t,RollingStockFilter*> RollingStockFilters;
			
		private:
			//! \name Environment
			//@{
				std::set<pt::CommercialLine*>	_lines;
				geography::GeographyModule::CitiesMatcher _citiesMatcher;
				RollingStockFilters _rollingStockFilters;
			//@}

			//! \name Parameters
			//@{
				bool _onlineBookingAllowed;
				bool _pastSolutionsDisplayed;
				int		_maxTransportConnectionsCount;
				bool _displayRoadApproachDetail;
			//@}

			//! \name Cached used days
			//@{
				bool	_useOldData;
				boost::gregorian::date_duration		_useDateRange;
				Periods	_periods;
			//@}


		public:
			static const std::string TEMPS_MIN_CIRCULATIONS;
			static const std::string TEMPS_MAX_CIRCULATIONS;


			//! \name Constructeur
			//@{
				TransportWebsite(util::RegistryKeyType id = 0);
			//@}

			//! \name Setters
			//@{
				void setOnlineBookingAllowed(const bool value){ _onlineBookingAllowed = value; }
				void setPastSolutionsDisplayed(bool value){ _pastSolutionsDisplayed = value; }
				void setMaxTransportConnectionsCount(int value){ _maxTransportConnectionsCount = value; }
				void setUseDateRange(boost::gregorian::date_duration value) { _useDateRange = value; }
				void setDisplayRoadApproachDetail(bool value) { _displayRoadApproachDetail = value; }
			//@}

			//! \name Getters
			//@{
				bool							getOnlineBookingAllowed() const { return _onlineBookingAllowed; }
				bool							getPastSolutionsDisplayed() const { return _pastSolutionsDisplayed; }
				int								getMaxTransportConnectionsCount() const { return _maxTransportConnectionsCount; }
				const Periods&					getPeriods() const { return _periods; }
				boost::gregorian::date_duration	getUseDatesRange() const { return _useDateRange; }
				const RollingStockFilters&		getRollingStockFilters() const { return _rollingStockFilters; }
				bool							getDisplayRoadApproachDetail() const { return _displayRoadApproachDetail; }
			//@}

			// \name Modifiers
			//@{
				void addHourPeriod(const HourPeriod& hourPeriod);
				void clearHourPeriods();
				void addCity(boost::shared_ptr<geography::City> value);
				void addRollingStockFilter(RollingStockFilter& value);
				void removeRollingStockFilter(RollingStockFilter& value);
				void clearRollingStockFilters();
			//@}

			//! \name Services
			//@{
				/** Access parameter generator.
					@param parameter Access profile
					@return AccessParameters
					@author Hugues Romain
					@date 2007
					@todo Modify the generated object to avoid memory leaks due to the use of new operator

					Default values :
					<ul>
					<li>Handicapped : 0.556 m/s = 2 km/h</li>
					<li>Bike : 4.167 m/s = 15 km/h</li>
					<li>Pedestrian : 1.111 m/s = 4 km/h</li>
				*/
				graph::AccessParameters	getAccessParameters(
					graph::UserClassCode parameter,
					const graph::AccessParameters::AllowedPathClasses& allowedPathClasses
				) const;
	
				const geography::GeographyModule::CitiesMatcher&			getCitiesMatcher() const;

				const boost::gregorian::date				getMinUseDate() const;
				const boost::gregorian::date				getMaxUseDate() const;
				

				/** Interprets date from text and environment data.
					@param text Text to interpret
					@return Interpreted date
					@author Hugues Romain
					@date 2005-2006
					@warning The parameters are not verified

					The returned date depends on the text :
						- date au format texte interne : date transcrite (no control) (ex : 20070201 => 1/2/2007)
						- commande de date classique (time::TIME_MIN ('m'), time::TIME_MAX ('M'), time::TIME_CURRENT ('A'), time::TIME_UNKNOWN ('?')) : la date correspondante (voir time::Date::setDate())
						- texte vide : identical to time::TIME_CURRENT
						- time::TIME_MIN_CIRCULATIONS ('r') : First date where at least one service runs (see Environment::getMinDateInUse())
						- TEMPS_MAX_CIRCULATIONS ('R') : Last date where at least one service runs (see Environment::getMaxDateInUse())

					The following assertion is always assumed : \f$ TEMPS_{INCONNU}<=TEMPS_{MIN}<=TEMPS_{MIN ENVIRONNEMENT}<=TEMPS_{MIN CIRCULATIONS}<=TEMPS_{ACTUEL}<=TEMPS_{MAX CIRCULATIONS}<=TEMPS_{MAX ENVIRONNEMENT}<=TEMPS_{MAX} \f$.
				*/
				boost::gregorian::date interpretDate( const std::string& text ) const;

					
				/** Apply this period to given dates.
					@param startTime The start time object to update.
					@param endTime The end time object to update.
					@param calculationTime Time of calculation.
					@param pastSolutions Past solutions filter (true = past solutions kept)
					@return true if the applied period of time is valid (ie if the required period is not anterior 
						to current time or if the pastSolutions filter is active).

					This method does the following :
						- Period validity checking
						- Sets startTime to period start hour or to calculationTime if pastSolutions filter is active.
						- If this period end hour is inferior to this period start hour, one day is added to endTime.
						- Sets endTime to period endHour.

					@throws Site::ForbiddenDateException if end time is in the past ans if the pastSolutions filter is active
				*/
				void applyPeriod(
					const HourPeriod& period
					, boost::posix_time::ptime& startTime
					, boost::posix_time::ptime& endTime
				) const;				

				/** Find the best place corresponding to a city name and a place name.
					@param cityName City name
					@param placeName Place name
					@return const pt::Place* best place found
					@author Hugues Romain
					@date 2007
					@throw Exception if no place can be found
					@todo Implement a true place fetcher which takes into account the place selection of the site
				*/
				const boost::shared_ptr<geography::Place> fetchPlace(
					const std::string& cityName
					, const std::string& placeName
				) const;



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
				road::RoadModule::ExtendedFetchPlaceResult extendedFetchPlace(
					const std::string& cityName,
					const std::string& placeName
				) const;

				typedef std::map<boost::optional<std::size_t>, std::string> Labels;
				Labels getRollingStockFiltersList() const;
			//@}

		};
	}
}

#endif
