#ifndef SYNTHESE_ENV_VERTEXACCESSMAP_H
#define SYNTHESE_ENV_VERTEXACCESSMAP_H


#include "module.h"

#include "Point.h"
#include "SquareDistance.h"


#include <vector>
#include <map>



namespace synthese
{
namespace env
{


 class Vertex;
 class Path;

 typedef std::vector<const Vertex*> AccessPath;
 
 
 typedef struct {
     AccessPath path;
     int approachTime;
     double approachDistance;
 } VertexAccess;
 



/** 


 @ingroup m15
*/
class VertexAccessMap
{
public:


private:

    std::map<const Vertex*, VertexAccess> _map;
    std::map<const Path*, bool> _fineSteppingForDeparture;
    std::map<const Path*, bool> _fineSteppingForArrival;
    
    mutable bool _isobarycenterUpToDate;
    mutable bool _isobarycenterMaxSquareDistanceUpToDate;

    mutable Point _isobarycenter;   //!< Isobarycenter of all points contained in this map.
    mutable SquareDistance _isobarycenterMaxSquareDistance;   //!< Maximum square distance of one map point with the isobarycenter.

    int _minApproachTime;


// une fonction qui verifie pour une ligne donnée si elle passe par l'un des vertex
// de la vam qui n'est pas de correspondance transport.
// VertexAccessMap vam, 

 public:

    VertexAccessMap ();
    ~VertexAccessMap ();


    const std::map<const Vertex*, VertexAccess>& getMap () const;

    const VertexAccess& getVertexAccess (const Vertex* vertex) const;

    bool needFineSteppingForDeparture (const Path* path) const;
    bool needFineSteppingForArrival (const Path* path) const;
    
    bool contains (const Vertex* vertex) const;
    void insert (const Vertex* vertex, const VertexAccess& vertexAccess);
    
    const Point& getIsobarycenter () const;
    const SquareDistance& getIsobarycenterMaxSquareDistance () const;
    
    int getMinApproachTime () const;

 private:

    void updateFineSteppingVertexMap ();


};


}
}

#endif 	    




