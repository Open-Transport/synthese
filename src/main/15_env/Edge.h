#ifndef SYNTHESE_ENV_EDGE_H
#define SYNTHESE_ENV_EDGE_H



#include <vector>

#include "Point.h"

#include "04_time/DateTime.h"
#include "04_time/Schedule.h"


namespace synthese
{
namespace env
{

    class Path;
    class Vertex;
    class AddressablePlace;
    class ConnectionPlace;
    


/** Edge abstract base class.

An edge necessarily belongs to a path.

For instance :
- if the edge is a road chunk, its parent path is a road.
- if the edge is a line stop, its parent path is a line.

An edge holds links between one vertex (the "from" vertex),
and several others edges sharing the same path.

This way, a path can be traversed following different views :
- departure/arrival view
- departure/arrival connection view

An edge is said to be :
- a departure edge if it is possible to start a move from it;
- an arrival edge if it is possible to end a move to it;
- a passage edge if it can be considered both as a departure edge 
  and as an arrival edge.


 @ingroup m15
*/
class Edge
{
public:


private:


    bool _isDeparture;
    bool _isArrival;

    const Path* _parentPath;      //!< Parent path
    int _rankInPath;  //!< Rank in path.

    const Edge* _nextInPath; //!< Next edge in path.

    const Edge* _previousConnectionDeparture; //!< Previous connection departure edge along path.
    const Edge* _previousDepartureForFineSteppingOnly; //!< 

    const Edge* _followingConnectionArrival; //!< Next connection arrival edge along path.
    const Edge* _followingArrivalForFineSteppingOnly; //!< 

    std::vector<const Point*> _viaPoints; //!< Intermediate points along the edge.

    std::vector<synthese::time::Schedule> _departureBeginSchedule;  //!< 
    std::vector<synthese::time::Schedule> _departureEndSchedule;  //!< 
    std::vector<synthese::time::Schedule> _arrivalBeginSchedule; //!< 
    std::vector<synthese::time::Schedule> _arrivalEndSchedule;  //!< 

    int _departureIndex[24];     //!< First line service index by departure hour of day
    int _arrivalIndex[24];  //!< First line service index by arrival hour of day


protected:

    Edge (bool isDeparture,
	  bool isArrival,
	  const Path* parentPath,
	  int rankInPath);

public:

    virtual ~Edge ();


    //! @name Getters/Setters
    //@{

    const Path* getParentPath () const;

    /** Returns this edge origin vertex.
     */
    virtual const Vertex* getFromVertex () const = 0;

    /** Returns metric offset of this edge from
	parent path origin vertex.
    */
    virtual double getMetricOffset () const = 0;


    /** Returns length of this edge, in meters. from
    */
    double getLength () const;

    const Edge* getNextInPath () const;
    void setNextInPath (const Edge* nextInPath);

    const Edge* getPreviousDeparture () const;
    void setPreviousDeparture ( const Edge* previousDeparture);

    const Edge* getPreviousConnectionDeparture () const;
    void setPreviousConnectionDeparture( const Edge* previousConnectionDeparture);

    const Edge* getPreviousDepartureForFineSteppingOnly () const;
    void setPreviousDepartureForFineSteppingOnly ( const Edge* previousDeparture);

    const Edge* getFollowingConnectionArrival () const;
    void setFollowingConnectionArrival( const Edge* followingConnectionArrival);
    
    const Edge* getFollowingArrivalForFineSteppingOnly () const;
    void setFollowingArrivalForFineSteppingOnly ( const Edge* followingArrival);
    
    /** Gets intermediate points 
     * between this line stop and the next in path.
     */
    const std::vector<const Point*>& getViaPoints () const;

    const synthese::time::Schedule& 
	getDepartureBeginSchedule (int serviceIndex) const;

    const synthese::time::Schedule& 
	getDepartureEndSchedule (int serviceIndex) const;

    const synthese::time::Schedule& 
	getArrivalBeginSchedule (int serviceIndex) const;

    const synthese::time::Schedule& 
	getArrivalEndSchedule (int serviceIndex) const;

    


    //@}


    //! @name Query methods
    //@{
    const AddressablePlace* getPlace () const;
    const ConnectionPlace* getConnectionPlace () const;
	const ConnectionPlace* getToConnectionPlace() const;

    int getRankInPath () const;

    bool isArrival () const;
    bool isDeparture () const;
    
    bool isRunning( const synthese::time::DateTime& startMoment, 
		    const synthese::time::DateTime& endMoment ) const;

    void calculateArrival (const Edge& departureEdge, 
			    int serviceIndex,
			    const synthese::time::DateTime& departureMoment, 
			    synthese::time::DateTime& arrivalMoment ) const;

    void calculateDeparture (const Edge& arrivalEdge, 
			     int serviceIndex,
			     const synthese::time::DateTime& arrivalMoment, 
			     synthese::time::DateTime& departureMoment ) const;

    int getBestRunTime (const Edge& other ) const;
    
    /** Checks consistency of input schedules.
      @param edgeWithPreviousSchedule Previous edge with schedule
      @return true if no problem detected, false otherwise
    */
    bool checkSchedule (const Edge* edgeWithPreviousSchedule ) const;



    // TODO : initialisation des filtres de compliance de la ligne pour tenir compte
    // de ceux des services et eviter le parcour total de tous les services

    
    /** Provides next departure service number (method 1)
	@param departureMoment Presence hour at departure place
	@param maxDepartureMoment Maximum departure hour
	@param calculationMoment Calculation moment for reservation delay checking
	@return Found service index or -1 if none was found.
	@retval departureMoment Accurate departure moment. Meaningless if -1 returned.
	0 means scheduled service.
    */
    int getNextService (synthese::time::DateTime& departureMoment, 
			const synthese::time::DateTime& maxDepartureMoment,
			const synthese::time::DateTime& calculationMoment,
			int minNextServiceIndex = UNKNOWN_VALUE ) const;

    
 

    int getPreviousService ( synthese::time::DateTime& arrivalMoment, 
			     const synthese::time::DateTime& minArrivalMoment,
			     int maxPreviousServiceIndex = UNKNOWN_VALUE) const;
    


    //@}

    //! @name Update methods
    //@{
    void clearViaPoints ();
    void addViaPoint (const Point& viaPoint);
    
    void insertDepartureSchedule (int index, const synthese::time::Schedule& schedule);
    void insertArrivalSchedule (int index, const synthese::time::Schedule& schedule);

    void updateDepartureIndex ();
    void updateArrivalIndex ();


 private:

    //@}



    
};


}
}

#endif 	    

