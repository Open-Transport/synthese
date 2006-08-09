#ifndef SYNTHESE_ENV_CONNECTIONPLACE_H
#define SYNTHESE_ENV_CONNECTIONPLACE_H


#include <map>

#include "01_util/Registrable.h"
#include "01_util/UId.h"
#include "AddressablePlace.h"


namespace synthese
{

namespace time
{
    class DateTime;
}


namespace env
{


class Address;
class Alarm;
class PhysicalStop;


/** A connection place indicates that there are possible
connections between different network vertices.

Each connection is associated with a type (authorized, 
forbidden, recommended...) and a transfer delay.

 @ingroup m15
*/
class ConnectionPlace : 
    public synthese::util::Registrable<uid,ConnectionPlace>, 
    public AddressablePlace
{
public:

    typedef enum { 
	CONNECTION_TYPE_FORBIDDEN,
	CONNECTION_TYPE_AUTHORIZED,
	CONNECTION_TYPE_RECOMMENDED_SHORT,
	CONNECTION_TYPE_RECOMMENDED
    } ConnectionType;

    static const int UNKNOWN_TRANSFER_DELAY;
    static const int FORBIDDEN_TRANSFER_DELAY;


private:

    ConnectionType _connectionType;
    std::vector<const PhysicalStop*> _physicalStops; 

    std::map< std::pair<int, int>, int > _transferDelays; //!< Transfer delays between vertices
    int _defaultTransferDelay;

    const Alarm* _alarm; //!< Current valid alarm

protected:


public:

    ConnectionPlace (const uid& id,
		     const std::string& name,
		     const City* city,
		     const ConnectionType& connectionType = CONNECTION_TYPE_FORBIDDEN,
		     int defaultTransferDelay = FORBIDDEN_TRANSFER_DELAY);

    ~ConnectionPlace ();


    //! @name Getters/Setters
    //@{
    int getDefaultTransferDelay () const;
    void setDefaultTransferDelay (int defaultTransferDelay);

    const ConnectionType& getConnectionType () const;
    void setConnectionType (const ConnectionType& connectionType);

    const std::vector<const PhysicalStop*>& getPhysicalStops () const;

    bool hasApplicableAlarm (const synthese::time::DateTime& start, 
			     const synthese::time::DateTime& end) const;
    const Alarm* getAlarm () const;
    void setAlarm (const Alarm* alarm);
    //@}


    //! @name Query methods.
    //@{
    bool isConnectionAuthorized () const;
    int getTransferDelay (int departureRank, int arrivalRank) const;
    
    //@}


    //! @name Update methods.
    //@{
    void addPhysicalStop (const PhysicalStop* physicalStop);
    void addTransferDelay (int departureRank, int arrivalRank, int transferDelay);
    void clearTransferDelays ();
    //@}


};


}
}

#endif 	    
