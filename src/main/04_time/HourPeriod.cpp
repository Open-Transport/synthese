#include "HourPeriod.h"

#include "DateTime.h"


namespace synthese
{
namespace time 
{



HourPeriod::HourPeriod ( const std::string caption, 
	       const Hour& startHour, 
	       const Hour& endHour )
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

