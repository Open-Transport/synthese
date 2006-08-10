#ifndef SYNTHESE_TIME_HOURPERIOD_H
#define SYNTHESE_TIME_HOURPERIOD_H


#include "Hour.h"

#include <string>

namespace synthese
{
namespace time
{

  class DateTime;

/** Hour period in a day.
    @ingroup m04

    These objects describe a period of time in a day.
*/
class HourPeriod
{
  Hour _startHour; //!< Period start hour
  Hour _endHour; //!< Period end hour

  const std::string _caption;  //!< Period caption

  int _id; //!< Period id

 public:
  HourPeriod ( const std::string caption, 
	       const Hour& startHour, 
	       const Hour& endHour );
  ~HourPeriod ();

  //! @name Getters/Setters
  //@{
  const std::string& getCaption () const;
  int getId() const;
  //@}
  

  //! @name Query methods
  //@{

  /** Apply this period to given dates.
    @param startTime The DateTime object to be modified.
    @param endTime The DateTime object to be modified.
    @param calculationTime Time of calculation.
    @param pastSolutions Past solutions filter (true = past solutions kept)
    @return true If the applied period of time is valid (ie if the
    required period is not anterior to current time or if the
    pastSolutions filter is active).
    
    This method does the following :
    - Period validity checking
    - Sets startTime to period start hour or to calculationTime if
    pastSolutions filter is active.
    - If this period end hour is inferior to this period start hour, one day is added to endTime.
    - Sets endTime to period endHour.
  */
  bool applyPeriod ( DateTime& startTime, 
		     DateTime& endTime, 
		     const DateTime& calculationTime, 
		     bool pastSolutions ) const;
  //@}


      
};





}
}
#endif
