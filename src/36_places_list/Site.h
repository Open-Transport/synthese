
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
#include "DateTime.h"
#include "LexicalMatcher.h"
#include "Registrable.h"
#include "Registry.h"
#include "01_util/Constants.h"
#include "GraphTypes.h"
#include "City.h"

#include <string>
#include <set>

namespace synthese
{
	namespace time
	{
		class DateTime;
	}

	namespace geography
	{
		class Place;
	}
	
	namespace graph
	{
		class AccessParameters;
	}

	namespace env
	{
		class CommercialLine;
	}

	namespace interfaces
	{
		class Interface;
	}

	namespace transportwebsite
	{
		/** Transport website.
			@ingroup m36
			@author Hugues Romain
			@date 2005-2006

			Includes :
				- transport sub-environment definition

		*/
		class Site
		:	public virtual util::Registrable
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
			
			typedef lexmatcher::LexicalMatcher<const geography::City*> CitiesMatcher;
			
		private:
			//! \name Properties
			//@{
				const interfaces::Interface*	_interface;
				std::string						_name;  //!< Name of the site
				time::Date						_startValidityDate;
				time::Date						_endValidityDate;
			//@}

			//! \name Environment
			//@{
				std::set<env::CommercialLine*>	_lines;
				CitiesMatcher _citiesMatcher;
			//@}

			//! \name Parameters
			//@{
				bool _onlineBookingAllowed;
				bool _pastSolutionsDisplayed;
				int		_maxTransportConnectionsCount;
			//@}

			//! \name Cached used days
			//@{
				bool	_useOldData;
				int		_useDateRange;
				Periods	_periods;
			//@}


		public:
			static const std::string TEMPS_MIN_CIRCULATIONS;
			static const std::string TEMPS_MAX_CIRCULATIONS;


			//! \name Constructeur
			//@{
				Site(util::RegistryKeyType uid = UNKNOWN_VALUE);
			//@}

			//! \name Setters
			//@{
				void setInterface (const interfaces::Interface* interf);
				void setStartDate ( const time::Date& dateDebut );
				void setEndDate ( const time::Date& dateFin );
				void setOnlineBookingAllowed ( const bool valeur );
				void setPastSolutionsDisplayed ( bool );
				void setName(const std::string& name);
				void setMaxTransportConnectionsCount(int number);
				void setUseDateRange(int range);
			//@}

			//! \name Getters
			//@{
				const interfaces::Interface*	getInterface() const;
				bool							getOnlineBookingAllowed() const;
				bool							getPastSolutionsDisplayed() const;
				int								getMaxTransportConnectionsCount()	const;
				const Periods&					getPeriods()	const;
				const std::string&				getName()							const;
				int								getUseDatesRange()					const;
				const time::Date&				getStartDate()						const;
				const time::Date&				getEndDate()						const;
				const CitiesMatcher&			getCitiesMatcher () const;
			//@}

			// \name Modifiers
			//@{
				void addHourPeriod(const HourPeriod& hourPeriod);
				void addCity(const geography::City* value);
			//@}

			//! \name Queries
			//@{
				/** Access parameter generator.
					@param parameter Access profile
					@return AccessParameters
					@author Hugues Romain
					@date 2007
					@todo Modify the generated object to avoid memory leaks due to the use of new operator
				*/
				graph::AccessParameters	getAccessParameters(graph::UserClassCode parameter)	const;
	
				bool dateControl() const;

				const time::Date				getMinUseDate() const;
				const time::Date				getMaxUseDate() const;

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
				time::Date interpretDate( const std::string& text ) const;

					
				/** Apply this period to given dates.
					@param startTime The DateTime object to be modified.
					@param endTime The DateTime object to be modified.
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
					, time::DateTime& startTime
					, time::DateTime& endTime
				) const;				

				/** Find the best place corresponding to a city name and a place name.
					@param cityName City name
					@param placeName Place name
					@return const env::Place* best place found
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
			//@}

		};
	}
}

#endif
