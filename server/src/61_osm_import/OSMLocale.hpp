
/** OSMLocale class header.
	@file OSMLocale.hpp

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

#ifndef SYNTHESE_dataexchange_OSMLocale_hpp__
#define SYNTHESE_dataexchange_OSMLocale_hpp__

#include <string>
#include <map>


namespace synthese
{
namespace data_exchange
{

class OSMLocale
{
public:

	static const OSMLocale OSMLocale_FR;
	static const OSMLocale OSMLocale_CH;

	const std::string& getCityCodeTag() const;
	const std::map<std::string, unsigned int>& getImplicitSpeeds() const;
	const std::map<std::string, std::string>& getDefaultRoadNames() const;

	static const OSMLocale& getInstance(const std::string& countryCode);

private:

	OSMLocale(const std::string& cityCodeTag,
			  const std::map<std::string, std::string>& defaultRoadNames,
			  const std::map<std::string, unsigned int>& implicitSpeeds);

	virtual ~OSMLocale();

private:

	const std::string _cityCodeTag;
	const std::map<std::string, std::string> _defaultRoadNames;
	const std::map<std::string, unsigned int> _implicitSpeeds;

};


}
}

#endif // SYNTHESE_dataexchange_OSMLocale_hpp__
