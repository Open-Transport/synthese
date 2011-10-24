
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
#include "Named.h"
#include "Importable.h"

#include <boost/thread/recursive_mutex.hpp>
#include <boost/optional.hpp>

namespace synthese
{
	namespace util
	{
		class ParametersMap;
	}

	namespace calendar
	{
		class CalendarTemplate;
	}

	namespace pt
	{
		class TransportNetwork;
		class NonConcurrencyRule;
		class ReservationContact;
		class StopArea;
		class RollingStock;

		/** Commercial line.
			@ingroup m35

			TRIDENT = Line

			A commercial line is a group of @ref JourneyPattern "routes" known as a same offer unit by the customers and by the network organization. It belongs to a @ref TransportNetwork "transport network".

			The commercial line handles the denomination of the group of routes.
		*/
		class CommercialLine:
			public graph::PathGroup,
			public util::Named,
			public impex::Importable
		{
		public:
			static const std::string DATA_LINE_SHORT_NAME;
			static const std::string DATA_LINE_LONG_NAME;
			static const std::string DATA_LINE_NAME;
			static const std::string DATA_LINE_COLOR;
			static const std::string DATA_LINE_STYLE;
			static const std::string DATA_LINE_IMAGE;
			static const std::string DATA_LINE_ID;
			static const std::string DATA_LINE_CREATOR_ID;
			static const std::string DATA_LINE_MAP_URL;
			static const std::string DATA_LINE_DOC_URL;
			static const std::string DATA_LINE_TIMETABLE_ID;

			/// Chosen registry class.
			typedef util::Registry<CommercialLine>	Registry;

			typedef std::set<const pt::StopArea*> PlacesSet;

			typedef std::set<const pt::NonConcurrencyRule*> NonConcurrencyRules;

		private:
			std::string			_shortName;	//!< Name (cartouche)
			std::string			_longName;	//!< Name for schedule card

			boost::optional<util::RGBColor>		_color;		//!< JourneyPattern color
			std::string			_style;		//!< CSS style (cartouche)
			std::string			_image;		//!< Display image (cartouche)

			const pt::TransportNetwork*	_network;	//!< Network

			const pt::ReservationContact*	_reservationContact;	//!< Reservation contact
			PlacesSet	_optionalReservationPlaces;

			NonConcurrencyRules _nonConcurrencyRules;
			mutable boost::recursive_mutex _nonConcurrencyRulesMutex;

			calendar::CalendarTemplate* _calendarTemplate;	//!< List of days when the line is supposed to run

			std::string _mapURL;
			std::string _docURL;
			util::RegistryKeyType _timetableId;

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Constructor.
			///	@param key id of the object (optional)
			/// Initializes the following default use rules :
			///  - USER_PEDESTRIAN : allowed
			CommercialLine(util::RegistryKeyType key = 0);

			//! @name Getters
			//@{
				const std::string& getStyle () const { return _style; }
				const pt::TransportNetwork* getNetwork () const { return _network; }
				const std::string& getShortName () const { return _shortName; }
				const std::string& getLongName () const { return _longName; }
				const std::string& getImage () const { return _image; }
				const boost::optional<util::RGBColor>& getColor () const { return _color; }
				const pt::ReservationContact* getReservationContact() const { return _reservationContact; }
				const PlacesSet& getOptionalReservationPlaces() const { return _optionalReservationPlaces; }
				const NonConcurrencyRules& getNonConcurrencyRules() const { return _nonConcurrencyRules; }
				calendar::CalendarTemplate* getCalendarTemplate() const { return _calendarTemplate; }
				boost::recursive_mutex& getNonConcurrencyRulesMutex() const { return _nonConcurrencyRulesMutex; }
				const std::string& getMapURL() const { return _mapURL; }
				const std::string& getDocURL() const { return _docURL; }
				util::RegistryKeyType getTimetableId() const { return _timetableId; }
			//@}

			//! @name Setters
			//@{
				void setStyle (const std::string& value) { _style = value; }
				void setNetwork (const pt::TransportNetwork* value) { _network = value; }
				void setShortName (const std::string& shortName) { _shortName = shortName; }
				void setLongName (const std::string& longName) { _longName = longName; }
				void setImage (const std::string& image) { _image = image; }
				void setColor (const boost::optional<util::RGBColor>& color) { _color = color; }
				void setReservationContact(const pt::ReservationContact* value) { _reservationContact = value; }
				void setCalendarTemplate(calendar::CalendarTemplate* value) { _calendarTemplate = value;}
				void setNonConcurrencyRules(const NonConcurrencyRules& value) { _nonConcurrencyRules = value; }
				void setOpionalReservationPlaces(const PlacesSet& value) { _optionalReservationPlaces = value; }
				void setMapURL(const std::string& value){ _mapURL = value; }
				void setDocURL(const std::string& value){ _docURL = value; }
				void setTimetableId(util::RegistryKeyType value){ _timetableId = value; }
			//@}

			//! @name Services
			//@{
				virtual std::string getRuleUserName() const { return "Ligne " + getName(); }

				//////////////////////////////////////////////////////////////////////////
				/// Tests if a place belongs to the list of optional reservation places of the line.
				/// @param place place to search
				/// @return true if the place belongs to the list of optional reservation places of the line.
				/// @author Hugues Romain
				bool isOptionalReservationPlace(
					const pt::StopArea* place
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


				//////////////////////////////////////////////////////////////////////////
				/// Line CMS exporter.
				/// See https://extranet-rcsmobility.com/projects/synthese/wiki/Lines_in_CMS
				//////////////////////////////////////////////////////////////////////////
				/// @param pm parameters map to populate
				/// @param prefix prefix to add to the field names
				/// @author Hugues Romain
				/// @since 3.3.0
				/// @date 2011
				void toParametersMap(
					util::ParametersMap& pm,
					std::string prefix = std::string()
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Tests if the line has route using a specific transport mode.
				//////////////////////////////////////////////////////////////////////////
				/// @param transportMode the transport mode
				/// @return true if the line has at least a route using the specified
				/// transport mode
				/// @author Hugues Romain
				/// @since 3.3.0
				/// @date 2011
				bool usesTransportMode(
					const RollingStock& transportMode
				) const;

			//@}

			//! @name Modifiers
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Adds a non concurrency rule.
				/// @param rule the rule to add
				/// This method cleans non concurrency cache of all the services of the line.
				void addConcurrencyRule( const pt::NonConcurrencyRule* rule );


				//////////////////////////////////////////////////////////////////////////
				/// Removes a non concurrency rule.
				/// @param rule the rule to remove
				/// This method cleans non concurrency cache of all the services of the line.
				void removeConcurrencyRule( const pt::NonConcurrencyRule* rule );
			//@}
		};
	}
}

#endif // SYNTHESE_CommercialLine_h__
