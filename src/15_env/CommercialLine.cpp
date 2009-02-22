
/** CommercialLine class implementation.
	@file CommercialLine.cpp

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

#include "CommercialLine.h"
#include "Registry.h"

namespace synthese
{
	using namespace util;
	using namespace graph;
	using namespace pt;

	namespace util
	{
		template<> const std::string Registry<env::CommercialLine>::KEY("CommercialLine");
	}

	namespace env
	{


		const TransportNetwork* CommercialLine::getNetwork() const
		{
			return _network;
		}

		void CommercialLine::setNetwork(const TransportNetwork* network )
		{
			_network = network;
		}

		CommercialLine::CommercialLine(RegistryKeyType key)
		:	util::Registrable(key),
			graph::PathGroup(key)
			, _color (0, 0, 0)
			, _network(NULL),
			_generalReservationContact(NULL),
			_bikeReservationContact(NULL),
			_handicappedReservationContact(NULL)
		{
		}

		const std::string& CommercialLine::getName() const
		{
			return _name;
		}

		void CommercialLine::setName( const std::string& name )
		{
			_name = name;
		}



		const std::string& CommercialLine::getCreatorId() const
		{
			return _creatorId;
		}

		void CommercialLine::setCreatorId( const std::string& name )
		{
			_creatorId = name;
		}


		const PlacesSet& CommercialLine::getOptionalReservationPlaces() const
		{
			return _optionalReservationPlaces;
		}


		const std::string& CommercialLine::getStyle() const
		{
			return _style;
		}

		void CommercialLine::setStyle( const std::string& style )
		{
			_style = style;
		}

		const std::string& CommercialLine::getShortName() const
		{
			return _shortName;
		}

		void CommercialLine::setShortName( const std::string& shortName )
		{
			_shortName = shortName;
		}

		const std::string& CommercialLine::getLongName() const
		{
			return _longName;
		}

		void CommercialLine::setLongName( const std::string& longName )
		{
			_longName = longName;
		}

		const std::string& CommercialLine::getImage() const
		{
			return _image;
		}

		void CommercialLine::setImage( const std::string& image )
		{
			_image = image;
		}

		const util::RGBColor& CommercialLine::getColor() const
		{
			return _color;
		}

		void CommercialLine::setColor( const util::RGBColor& color )
		{
			_color = color;
		}

		void CommercialLine::addOptionalReservationPlace( const Place* place )
		{
			_optionalReservationPlaces.insert(place);
		}

		const ReservationContact* CommercialLine::getReservationContact(
			const UserClassCode userClass
		) const {
			if(userClass == USER_BIKE_IN_PT && _bikeReservationContact != NULL)
			{
				return _bikeReservationContact;
			}
			else if(userClass == USER_HANDICAPPED && _handicappedReservationContact != NULL)
			{
				return _handicappedReservationContact;
			} else {
				return _generalReservationContact;
			}
		}
		
		const ReservationContact* CommercialLine::getGeneralReservationContact() const
		{
			return _generalReservationContact;
		}
		
		const ReservationContact* CommercialLine::getBikeReservationContact() const
		{
			return _bikeReservationContact;
		}
		
		const ReservationContact* CommercialLine::getHandicappedReservationContact() const
		{
			return _handicappedReservationContact;
		}
		
		void CommercialLine::setGeneralReservationContact(
			const ReservationContact* value
		){
			_generalReservationContact = value;
		}
		
		
		void CommercialLine::setBikeReservationContact(
			const ReservationContact* value
		){
			_bikeReservationContact = value;
		}
		
		
		void CommercialLine::setHandicappedReservationContact(
			const ReservationContact* value
		){
			_handicappedReservationContact = value;
		}
	}
}
