#ifndef SYNTHESE_ENV_ALARM_H
#define SYNTHESE_ENV_ALARM_H

#include <string>
#include "04_time/DateTime.h"


namespace synthese
{
namespace env
{



/** Alarm message.
 @ingroup m15
*/
class Alarm
{
public:

    /** Alarm level.

	- INFO : the solution is available, but some information is 
	provided for better comfort

	- WARNING : the solution could not be available, or requires action 
	from the passenger (reservation...)
     */
    typedef enum { ALARM_LEVEL_INFO, ALARM_LEVEL_WARNING } AlarmLevel; 

private:

    std::string _message;  //!< Alarm message
    synthese::time::DateTime _periodStart; //!< Alarm applicability period start
    synthese::time::DateTime _periodEnd;   //!< Alarm applicability period end
    AlarmLevel _level; 
    
public:

    Alarm ();
    
    //! @name Getters/Setters
    //@{

    const std::string& getMessage () const;
    void setMessage( const std::string& message);

    const AlarmLevel& getLevel () const;
    void setLevel (const AlarmLevel& level);

    void setPeriodStart ( const synthese::time::DateTime& periodStart);
    void setPeriodEnd ( const synthese::time::DateTime& periodEnd);

    //@}
    
    
    //! \name Calculateurs
    //@{

    /** Applicability test.
	@param start Start of applicability period
	@param end End of applicability period
	@return true if the message is not empty and 
	is valid for the whole period given as argument.
    */
    bool isMessageShowable ( const synthese::time::DateTime& start, 
			     const synthese::time::DateTime& end ) const;
    //@}
    
};




}
}


#endif
