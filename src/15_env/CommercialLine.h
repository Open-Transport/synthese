
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

	namespace env
	{
		class ReservationContact;

		/** Commercial line class.
			TRIDENT = Line
			@ingroup m35
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
			mutable boost::recursive_mutex _optionalReservationPlacesMutex;
			mutable boost::recursive_mutex _nonConcurrencyRulesMutex;

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

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Constructor.
			/// Initializes the following default use rules :
			///  - USER_PEDESTRIAN : allowed
			CommercialLine(util::RegistryKeyType key = UNKNOWN_VALUE);

			//! @name Getters
			//@{
				const std::string& getStyle () const;
				const pt::TransportNetwork* getNetwork () const;
				const std::string& getShortName () const;
				const std::string& getLongName () const;
				const std::string& getImage () const;
				const boost::optional<util::RGBColor>& getColor () const;
				const std::string& getName () const;
				const ReservationContact* getReservationContact() const;
				const std::string& getCreatorId() const;
				const PlacesSet& getOptionalReservationPlaces() const;
				boost::recursive_mutex& getOptionalReservationPlacesMutex() const;
				const NonConcurrencyRules& getNonConcurrencyRules() const;
				boost::recursive_mutex& getNonConcurrencyRulesMutex() const;
			//@}
			
			//! @name Setters
			//@{
				void setStyle (const std::string& style);
				void setNetwork (const pt::TransportNetwork* network);
				void setShortName (const std::string& shortName);
				void setLongName (const std::string& longName);
				void setImage (const std::string& image);
				void setColor (const boost::optional<util::RGBColor>& color);
				void setName (const std::string& name);
				void setReservationContact(const ReservationContact* value);
				void setCreatorId(const std::string& value);
				void addConcurrencyRule(const NonConcurrencyRule* rule);
				void removeConcurrencyRule(const NonConcurrencyRule* rule);
			//@}

			//! @name Queries
			//@{
				bool isOptionalReservationPlace(
					const env::PublicTransportStopZoneConnectionPlace* place
				) const;
			//@}
			
			//! @name Modifiers
			//@{
				void cleanOptionalReservationPlaces();
				void addOptionalReservationPlace(
					const PublicTransportStopZoneConnectionPlace* place
				);
			//@}
		};
	}
}

#endif // SYNTHESE_CommercialLine_h__
