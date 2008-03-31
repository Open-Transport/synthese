
/** RoutePlanningUserPrefPanel class implementation.
	@file RoutePlanningUserPrefPanel.cpp

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

#include "RoutePlanningUserPrefPanel.h"

namespace synthese
{
    namespace util
    {
	template<> const std::string 
	FactorableTemplate<security::UserPrefPanel, 
			   routeplanner::RoutePlanningUserPrefPanel>::FACTORY_KEY ("RoutePlanningUserPrefPanel"); // ?
    }	
    
    namespace security
    {
	template<> const std::string 
	UserPrefPanelTemplate<synthese::routeplanner::RoutePlanningUserPrefPanel>::NAME ("RoutePlanningUserPrefPanel"); // ?
    }

    
    namespace routeplanner
    {
	
	
	void RoutePlanningUserPrefPanel::display( std::ostream& stream, const security::User* user ) const
	{
	    
	}
    }
}
