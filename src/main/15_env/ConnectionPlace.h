#ifndef SYNTHESE_ENV_CONNECTIONPLACE_H
#define SYNTHESE_ENV_CONNECTIONPLACE_H


#include <map>

#include "01_util/Registrable.h"
#include "01_util/UId.h"
#include "AddressablePlace.h"


namespace synthese
{
namespace env
{


class Address;
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

protected:


public:

    ConnectionPlace (const uid& id,
		     const std::string& name,
		     const City* city,
		     const ConnectionType& connectionType = CONNECTION_TYPE_FORBIDDEN);

    ~ConnectionPlace ();


    //! @name Getters/Setters
    //@{
    const std::vector<const PhysicalStop*>& getPhysicalStops () const;
    const ConnectionType& getConnectionType () const;
    void setConnectionType (const ConnectionType& connectionType);
    bool isConnectionAuthorized () const;

    //@}


    //! @name Query methods.
    //@{
    int getTransferDelay (int departureRank, int arrivalRank) const;
    
    //@}


    //! @name Update methods.
    //@{
    void addPhysicalStop (const PhysicalStop* physicalStop);
    void addTransferDelay (int departureRank, int arrivalRank, int transferDelay);
    //@}


};


}
}

#endif 	    
