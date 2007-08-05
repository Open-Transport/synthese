
/** HourPeriod class implementation.
	@file HourPeriod.cpp

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

#include "HourPeriod.h"

#include "DateTime.h"


namespace synthese
{
namespace time 
{



HourPeriod::HourPeriod ( const std::string caption, 
	       const Hour& startHour, 
	       const Hour& endHour )
		   : _startHour(startHour)
		   , _endHour(endHour)
		   , _caption(caption)
{

}



HourPeriod::~HourPeriod ()
{

}




const std::string& 
HourPeriod::getCaption () const
{
  return _caption;
}




int 
HourPeriod::getId() const
{
  return _id;
}





bool 
HourPeriod::applyPeriod ( DateTime& startTime, 
			  DateTime& endTime, 
			  const DateTime& calculationTime, 
			  bool pastSolutions ) const
{
    // Updates
    if ( _startHour <= _endHour )
    {
	endTime.addDaysDuration ( 1 );
    }
  
     
    endTime = _endHour;
    startTime = _startHour;
    
    
    // Checks
    if ( pastSolutions == false )
    {
	if ( endTime < calculationTime ) return false;
	
	if ( startTime < calculationTime ) 
	{
	    startTime = calculationTime;
	}
    }
    
  return true;
  
}





}
}

