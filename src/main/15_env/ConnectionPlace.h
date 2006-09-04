#ifndef SYNTHESE_ENV_CONNECTIONPLACE_H
#define SYNTHESE_ENV_CONNECTIONPLACE_H


#include <map>
#include <set>

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
    class Edge;
    class Path; 
    class PhysicalStop;
    class SquareDistance; 


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

    static const int UNKNOWN_TRANSFER_DELAY;
    static const int FORBIDDEN_TRANSFER_DELAY;
    static const int SQUAREDISTANCE_SHORT_LONG;


private:

    std::vector<const PhysicalStop*> _physicalStops; 

    std::map< std::pair<int, int>, int > _transferDelays; //!< Transfer delays between vertices
    int _defaultTransferDelay;
    int _minTransferDelay;
    int _maxTransferDelay;

    ConnectionType _connectionType;

    const Alarm* _alarm; //!< Current valid alarm

protected:


public:

    ConnectionPlace (const uid& id,
		     const std::string& name,
		     const City* city,
		     const ConnectionType& connectionType,
		     int defaultTransferDelay = FORBIDDEN_TRANSFER_DELAY);

    ~ConnectionPlace ();


    //! @name Getters/Setters
    //@{
    int getDefaultTransferDelay () const;
    void setDefaultTransferDelay (int defaultTransferDelay);

    int getMinTransferDelay () const;
    int getMaxTransferDelay () const;

    const std::vector<const PhysicalStop*>& getPhysicalStops () const;

    bool hasApplicableAlarm (const synthese::time::DateTime& start, 
			     const synthese::time::DateTime& end) const;
    const Alarm* getAlarm () const;
    void setAlarm (const Alarm* alarm);

    const ConnectionType getConnectionType () const;
    void setConnectionType (const ConnectionType& connectionType);

    //@}


    //! @name Query methods.
    //@{
    
    bool isConnectionAllowed (const Edge* fromEdge, 
			      const Edge* toEdge) const;

    ConnectionType getRecommendedConnectionType (const SquareDistance& squareDistance) const;


    int getTransferDelay (const Vertex* fromVertex, 
			  const Vertex* toVertex) const;


    VertexAccess getVertexAccess (const AccessDirection& accessDirection,
				  const AccessParameters& accessParameters,
				  const Vertex* destination,
				  const Vertex* origin = 0) const;
    
    void getImmediateVertices (VertexAccessMap& result, 
			       const AccessDirection& accessDirection,
			       const AccessParameters& accessParameters,
			       const Vertex* origin = 0,
			       bool returnAddresses = true,
			       bool returnPhysicalStops = true) const;
    
    
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
