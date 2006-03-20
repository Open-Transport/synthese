#ifndef SYNTHESE_ENV_CONNECTIONPLACE_H
#define SYNTHESE_ENV_CONNECTIONPLACE_H


#include <map>

#include "AddressablePlace.h"


namespace synthese
{
namespace env
{


class Address;

/** A connection place indicates that there are possible
connections between different network vertices.

Each connection is associated with a type (authorized, 
forbidden, recommended...) and a transfer delay.

 @ingroup m15
*/
class ConnectionPlace : public AddressablePlace
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
    std::map< std::pair<int, int>, int > _transferDelays; //!< Transfer delays between vertices

protected:

    ConnectionPlace (const std::string& name,
		     const City* city,
		     const ConnectionType& connectionType = CONNECTION_TYPE_FORBIDDEN);


public:

    virtual ~ConnectionPlace ();


    //! @name Getters/Setters
    //@{
    const ConnectionType& getConnectionType () const;
    void setConnectionType (const ConnectionType& connectionType);
    bool isConnectionAuthorized () const;

    int getTransferDelay (int departureRank, int arrivalRank) const;
    void setTransferDelay (int departureRank, int arrivalRank, int transferDelay);
    
    //@}


    //! @name Query methods.
    //@{
    //@}


    //! @name Update methods.
    //@{

    //@}


};


}
}

#endif 	    
