
/** Site class header.
	@file Site.h

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
#include "Registrable.h"
#include "Registry.h"
#include "01_util/Constants.h"
#include "GraphTypes.h"
#include "City.h"
#include "AccessParameters.h"
#include "RollingStockFilter.h"
#include "Named.h"

#include <string>
#include <set>
#include <boost/date_time/gregorian/greg_date.hpp>
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

	namespace transportwebsite
	{
		class WebPage;

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
		class Site:
			public virtual util::Registrable,
			public util::Named
		{
		public:

			//////////////////////////////////////////////////////////////////////////
			/// Exception to throw when a route planning cannot be launched at the
			/// specified dates according to the site rules.
			/// @ingroup m36
			class ForbiddenDateException:
				public util::Exception
			{
			public:
				ForbiddenDateException();
			};

			/// Chosen registry class.
			typedef util::Registry<Site>	Registry;

			typedef std::vector<HourPeriod> Periods;
			
			typedef lexmatcher::LexicalMatcher<geography::City*> CitiesMatcher;

			typedef std::map<std::size_t,RollingStockFilter*> RollingStockFilters;
			
		private:
			//! \name Properties
			//@{
				boost::gregorian::date		_startValidityDate;
				boost::gregorian::date		_endValidityDate;
				std::string					_clientURL;
				WebPage*					_defaultTemplate;
			//@}

			//! \name Environment
			//@{
				std::set<pt::CommercialLine*>	_lines;
				CitiesMatcher _citiesMatcher;
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
				Site(util::RegistryKeyType id = 0);
			//@}

			//! \name Setters
			//@{
				void setStartDate ( const boost::gregorian::date& dateDebut );
				void setEndDate ( const boost::gregorian::date& dateFin );
				void setOnlineBookingAllowed ( const bool valeur );
				void setPastSolutionsDisplayed ( bool );
				void setMaxTransportConnectionsCount(int number);
				void setUseDateRange(boost::gregorian::date_duration range) { _useDateRange = range; }
				void setDisplayRoadApproachDetail(bool value) { _displayRoadApproachDetail = value; }
				void setClientURL(const std::string& value) { _clientURL = value; }
				void setDefaultTemplate(WebPage* value){ _defaultTemplate = value; }
			//@}

			//! \name Getters
			//@{
				bool							getOnlineBookingAllowed() const { return _onlineBookingAllowed; }
				bool							getPastSolutionsDisplayed() const;
				int								getMaxTransportConnectionsCount()	const;
				const Periods&					getPeriods()	const;
				boost::gregorian::date_duration	getUseDatesRange()					const;
				const boost::gregorian::date&	getStartDate()						const;
				const boost::gregorian::date&	getEndDate()						const;
				const CitiesMatcher&			getCitiesMatcher () const;
				const RollingStockFilters&		getRollingStockFilters() const;
				bool							getDisplayRoadApproachDetail() const { return _displayRoadApproachDetail; }
				const std::string& getClientURL() const { return _clientURL; }
				WebPage* getDefaultTemplate() const { return _defaultTemplate; }
			//@}

			// \name Modifiers
			//@{
				void addHourPeriod(const HourPeriod& hourPeriod);
				void clearHourPeriods();
				void addCity(geography::City* value);
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
				*/
				graph::AccessParameters	getAccessParameters(
					graph::UserClassCode parameter,
					const graph::AccessParameters::AllowedPathClasses& allowedPathClasses
				) const;
	
				bool dateControl() const;

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
				const geography::Place* fetchPlace(
					const std::string& cityName
					, const std::string& placeName
				) const;


				struct ExtendedFetchPlaceResult
				{
					CitiesMatcher::MatchResult::value_type cityResult;
					lexmatcher::LexicalMatcher<const geography::Place*>::MatchResult::value_type placeResult;

					ExtendedFetchPlaceResult();
				};

				ExtendedFetchPlaceResult extendedFetchPlace(
					const std::string& cityName
					, const std::string& placeName
				) const;

				typedef std::map<boost::optional<std::size_t>, std::string> Labels;
				Labels getRollingStockFiltersList() const;
			//@}

		};
	}
}

#endif
