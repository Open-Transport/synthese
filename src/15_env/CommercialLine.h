
/** CommercialLine class header.
	@file CommercialLine.h

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

#ifndef SYNTHESE_CommercialLine_h__
#define SYNTHESE_CommercialLine_h__

#include <string>
#include <set>

#include "PathGroup.h"
#include "RGBColor.h"
#include "GraphTypes.h"
#include "Calendar.h"

#include <boost/thread/recursive_mutex.hpp>
#include <boost/optional.hpp>

namespace synthese
{
	namespace pt
	{
		class TransportNetwork;
	}
	
	namespace env
	{
		class PublicTransportStopZoneConnectionPlace;
		class NonConcurrencyRule;
	}

	namespace calendar
	{
		class CalendarTemplate;
	}

	namespace env
	{
		class ReservationContact;

		/** Commercial line.
			@ingroup m35

			TRIDENT = Line

			A commercial line is a group of @ref Line "routes" known as a same offer unit by the customers and by the network organization. It belongs to a @ref TransportNetwork "transport network".

			The commercial line handles the denomination of the group of routes.
		*/
		class CommercialLine
		:	public graph::PathGroup
		{
		public:

			/// Chosen registry class.
			typedef util::Registry<CommercialLine>	Registry;

			typedef std::set<const env::PublicTransportStopZoneConnectionPlace*> PlacesSet;

			typedef std::set<const NonConcurrencyRule*> NonConcurrencyRules;

		private:
			std::string			_name;		//!< Name (code)
			std::string			_shortName;	//!< Name (cartouche)
			std::string			_longName;	//!< Name for schedule card

			boost::optional<util::RGBColor>		_color;		//!< Line color
			std::string			_style;		//!< CSS style (cartouche)
			std::string			_image;		//!< Display image (cartouche)

			const pt::TransportNetwork*	_network;	//!< Network

			const ReservationContact*	_reservationContact;	//!< Reservation contact
			PlacesSet	_optionalReservationPlaces;
			
			std::string _creatorId;

			NonConcurrencyRules _nonConcurrencyRules;
			mutable boost::recursive_mutex _nonConcurrencyRulesMutex;

			calendar::CalendarTemplate* _calendarTemplate;	//!< List of days when the line is supposed to run

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Constructor.
			///	@param key id of the object (optional)
			/// Initializes the following default use rules :
			///  - USER_PEDESTRIAN : allowed
			CommercialLine(util::RegistryKeyType key = UNKNOWN_VALUE);

			//! @name Getters
			//@{
				const std::string& getStyle () const { return _style; }
				const pt::TransportNetwork* getNetwork () const { return _network; }
				const std::string& getShortName () const { return _shortName; }
				const std::string& getLongName () const { return _longName; }
				const std::string& getImage () const { return _image; }
				const boost::optional<util::RGBColor>& getColor () const { return _color; }
				const std::string& getName () const { return _name; }
				const ReservationContact* getReservationContact() const { return _reservationContact; }
				const std::string& getCreatorId() const { return _creatorId; }
				const PlacesSet& getOptionalReservationPlaces() const { return _optionalReservationPlaces; }
				const NonConcurrencyRules& getNonConcurrencyRules() const { return _nonConcurrencyRules; }
				calendar::CalendarTemplate* getCalendarTemplate() const { return _calendarTemplate; }
				boost::recursive_mutex& getNonConcurrencyRulesMutex() const { return _nonConcurrencyRulesMutex; }
			//@}
			
			//! @name Setters
			//@{
				void setStyle (const std::string& value) { _style = value; }
				void setNetwork (const pt::TransportNetwork* value) { _network = value; }
				void setShortName (const std::string& shortName) { _shortName = shortName; }
				void setLongName (const std::string& longName) { _longName = longName; }
				void setImage (const std::string& image) { _image = image; }
				void setColor (const boost::optional<util::RGBColor>& color) { _color = color; }
				void setName (const std::string& name) { _name = name; }
				void setReservationContact(const ReservationContact* value) { _reservationContact = value; }
				void setCreatorId(const std::string& value) { _creatorId = value; }
				void setCalendarTemplate(calendar::CalendarTemplate* value) { _calendarTemplate = value;}
				void setNonConcurrencyRules(const NonConcurrencyRules& value) { _nonConcurrencyRules = value; }
				void setOpionalReservationPlaces(const PlacesSet& value) { _optionalReservationPlaces = value; }
			//@}

			//! @name Services
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Tests if a place belongs to the list of optional reservation places of the line.
				/// @param place place to search
				/// @return true if the place belongs to the list of optional reservation places of the line.
				/// @author Hugues Romain
				bool isOptionalReservationPlace(
					const env::PublicTransportStopZoneConnectionPlace* place
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Controls if the line runs every day of its calendar template.
				/// @param duration period to analyze (from now)
				/// @author Hugues Romain
				/// @date 2010
				/// @since 3.1.16
				bool respectsCalendarTemplate(boost::gregorian::date_duration duration) const;



				//////////////////////////////////////////////////////////////////////////
				/// Get the days when the line runs.
				/// @param mask Calendar defining which days the check must be done
				/// @author Hugues Romain
				/// @date 2010
				/// @since 3.1.16
				calendar::Calendar getRunDays(const calendar::Calendar& mask) const;
			//@}

			//! @name Modifiers
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Adds a non concurrency rule.
				/// @param rule the rule to add
				/// This method cleans non concurrency cache of all the services of the line.
				void addConcurrencyRule( const NonConcurrencyRule* rule );


				//////////////////////////////////////////////////////////////////////////
				/// Removes a non concurrency rule.
				/// @param rule the rule to remove
				/// This method cleans non concurrency cache of all the services of the line.
				void removeConcurrencyRule( const NonConcurrencyRule* rule );
			//@}
		};
	}
}

#endif // SYNTHESE_CommercialLine_h__
