#ifndef SYNTHESE_ENV_CONTINUOUSSERVICE_H
#define SYNTHESE_ENV_CONTINUOUSSERVICE_H

#include "Service.h"
#include <string>

#include "01_util/Registrable.h"


namespace synthese
{
namespace env
{

    class Calendar;


/** Continuous service.


 @ingroup m15
*/
class ContinuousService : 
    public synthese::util::Registrable<uid,ContinuousService>, 
    public Service
{
private:

    int _range; //!< Continuous service range (minutes).
    int _maxWaitingTime; //!< Max waiting waiting time before next service.
    

public:

    ContinuousService (const uid& id,
		       int serviceNumber,
		       const Path* path,
		       const synthese::time::Schedule& departureSchedule,
		       int range,
		       int maxWaitingTime);

    ~ContinuousService ();

    
    //! @name Getters/Setters
    //@{
    int getMaxWaitingTime () const;
    void setMaxWaitingTime (int maxWaitingTime);

    int getRange () const;
    void setRange (int range);
    //@}

    //! @name Query methods
    //@{
    bool isContinuous () const;
    //@}

};



}
}


#endif
