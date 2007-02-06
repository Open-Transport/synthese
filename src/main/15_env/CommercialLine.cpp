
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

namespace synthese
{
	namespace env
	{


		const TransportNetwork* CommercialLine::getNetwork() const
		{
			return _network;
		}

		void CommercialLine::setNetwork( TransportNetwork* network )
		{
			_network = network;
		}

		CommercialLine::CommercialLine()
			: _color (0, 0, 0)
			, _network(NULL)

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
	}
}
