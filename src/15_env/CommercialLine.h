
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

#include "15_env/Types.h"
#include "PathGroup.h"
#include "RGBColor.h"
#include "GraphTypes.h"

namespace synthese
{
	namespace env
	{
		class TransportNetwork;
		class Place;
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

		private:
			std::string			_name;		//!< Name (code)
			std::string			_shortName;	//!< Name (cartouche)
			std::string			_longName;	//!< Name for schedule card

			util::RGBColor		_color;		//!< Line color
			std::string			_style;		//!< CSS style (cartouche)
			std::string			_image;		//!< Display image (cartouche)

			const TransportNetwork*	_network;	//!< Network
			const ReservationContact*	_generalReservationContact;	//!< Reservation contact
			const ReservationContact*	_bikeReservationContact;	//!< Bike user dedicated reservation contact
			const ReservationContact*	_handicappedReservationContact;	//!< Handicapped user dedicated Reservation contact

			PlacesSet	_optionalReservationPlaces;

		public:
			CommercialLine(util::RegistryKeyType key = UNKNOWN_VALUE);

			//! @name Getters
			//@{
				const std::string& getStyle () const;
				const TransportNetwork* getNetwork () const;
				const std::string& getShortName () const;
				const std::string& getLongName () const;
				const std::string& getImage () const;
				const util::RGBColor& getColor () const;
				const std::string& getName () const;
				const ReservationContact* getGeneralReservationContact() const;
				const ReservationContact* getBikeReservationContact() const;
				const ReservationContact* getHandicappedReservationContact() const;
			//@}
			
			//! @name Setters
			//@{
				void setStyle (const std::string& style);
				void setNetwork (const TransportNetwork* network);
				void setShortName (const std::string& shortName);
				void setLongName (const std::string& longName);
				void setImage (const std::string& image);
				void setColor (const util::RGBColor& color);
				void setName (const std::string& name);
				void setGeneralReservationContact(const ReservationContact* value);
				void setBikeReservationContact(const ReservationContact* value);
				void setHandicappedReservationContact(const ReservationContact* value);
			//@}

			//! @name Queries
			//@{
				const ReservationContact* getReservationContact(
					const graph::UserClassCode userClass
				) const;
			//@}
			
			//! @name Modifiers
			//@{
				void addOptionalReservationPlace(
					const Place* place
				);
			//@}
		};
	}
}

#endif // SYNTHESE_CommercialLine_h__
