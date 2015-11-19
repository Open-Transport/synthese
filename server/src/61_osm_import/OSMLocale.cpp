/** OsmLocale class implementation.
	@file OsmLocale.cpp
	@author Marc Jambert

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
#include "OSMLocale.hpp"

#include <boost/assign/list_of.hpp>

namespace synthese
{
namespace data_exchange
{


const OSMLocale OSMLocale::OSMLocale_FR(
			"ref:INSEE",
			boost::assign::map_list_of("FR:walk", 6) ("FR:urban", 50) ("FR:rural", 90) ("FR:trunk", 110) ("FR:motorway", 130)
			);
const OSMLocale OSMLocale::OSMLocale_CH(
			"swisstopo:BFS_NUMMER",
			boost::assign::map_list_of("CH:trunk", 100) ("CH:urban", 50) ("CH:rural", 80) ("CH:motorway", 120)
			);


OSMLocale::OSMLocale(const std::string& cityCodeTag,
					 const std::map<std::string, unsigned int>& implicitSpeeds)
: _cityCodeTag(cityCodeTag)
, _implicitSpeeds(implicitSpeeds)
{

}

OSMLocale::~OSMLocale()
{
}

const std::string&
OSMLocale::getCityCodeTag() const
{
	return _cityCodeTag;
}

const std::map<std::string, unsigned int>&
OSMLocale::getImplicitSpeeds() const
{
	return _implicitSpeeds;
}


}
}


