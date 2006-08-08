#ifndef SYNTHESE_DEPARTURESTABLE_PASSTHROUGHDESCRIPTION_H
#define SYNTHESE_DEPARTURESTABLE_PASSTHROUGHDESCRIPTION_H


#include "04_time/DateTime.h"
#include <vector>



namespace synthese
{

namespace env
{
    class ConnectionPlace;
    class Service;
    class LineStop;
}

namespace departurestable
{


/** Departure table pass through display
 @ingroup m34
*/
class PassThroughDescription
{
 private:
    
    synthese::time::DateTime _theoreticalMoment;
    synthese::time::DateTime _realMoment; //!< Same than theoretical right now...

    int _serviceNumber; //!< 

    const synthese::env::LineStop* _lineStop;
    std::vector<synthese::env::ConnectionPlace*> _displayedConnectionPlaces; //!< Line connection places to be displayed
    
 public:
    
    typedef std::vector<const PassThroughDescription*> PassThroughDescriptionList;

    PassThroughDescription (const synthese::env::LineStop* lineStop,
			    const synthese::time::DateTime& theoreticalMoment,
			    int serviceNumber);
    
    //! @name Getters/Setters
    //@{
    const std::vector<synthese::env::ConnectionPlace*>& getDisplayedConnectionPlaces () const;
    const synthese::env::LineStop* getLineStop () const;
    const synthese::time::DateTime& getTheoreticalMoment () const;
    const synthese::time::DateTime& getRealMoment () const;
    int getServiceNumber () const;
    //@}

    
    //! @name Query methods
    //@{
    synthese::time::DateTime getTerminusArrivalMoment () const;
    const synthese::env::ConnectionPlace* getDisplayedTerminus () const;
    //@}

};


}
}

#endif
