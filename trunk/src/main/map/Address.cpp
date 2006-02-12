
#include "Address.h"
#include "RoadChunk.h"
#include "Vertex.h"
#include "PhysicalStop.h"
#include "Topography.h"
#include "Road.h"
#include <cmath>
#include <assert.h>


namespace synmap
{


/** Constructeur.
 @param road Route sur laquelle se trouve l'adresse
 @param number Numéro d'adresse (rien = numéro inconnu)
 
 @todo Gérer ici immédiatement le PM : soit il est fourni, soit il est extrapolé d'après le numéro, soit on prend le milieu
*/
Address::Address(Road* road, AddressNumber number)
        : _road(road)
        , _number(number)
{}


/** Destructeur.
*/
Address::~Address()
{}



std::set
    < std::pair<double, const PhysicalStop*> >
Address::findDistancesToPhysicalStops (double maxDistance) const
{
  std::set
    < Address::PathToPhysicalStop > paths = findPathsToPhysicalStops (maxDistance);

  std::set
    < std::pair<double, const PhysicalStop*> > result;
  
  double pathLength = 0.0;
  for (std::set < Address::PathToPhysicalStop >::const_iterator iter (paths.begin ());
       iter != paths.end ();
       ++iter) {
    for (std::vector<const RoadChunk*>::const_iterator iterChunk (iter->first.begin ());
	 iterChunk != iter->first.end ();
	 ++iterChunk) {
      pathLength += (*iterChunk)->getLength ();
      
    }
    result.insert (make_pair (pathLength, iter->second));

  }
  return result;

}



/** Recherche des arrêts physiques situés à proximité de l'adresse.
 
 @param maxDistance Distance maximale à parcourir
 
 L'algorithme consiste en un parcours intégral du graphe des segments de route, borné par une distance maximale parcourue.
 
 @todo Remplacer findmostplausiblechunk par un findChunkByPM Voir le positionnement de cette question.
 
 @author Marc Jambert
*/
std::set
    < Address::PathToPhysicalStop >
Address::findPathsToPhysicalStops (double maxDistance) const
    {
        const RoadChunk* chunk = _road->findMostPlausibleChunkForNumber (_number);
        // TODO : add an algorithm to find more precisely the vertex to
        // start from ?
        const Vertex* start = chunk->getStep (0)->getVertex ();
        std::set
            < std::vector<const Vertex*> > paths =
                start->findPathsToCloseNeighbors (maxDistance);

        std::set
            < PathToPhysicalStop > result;

        Road::RoadChunkVector tmpChunks;
        std::vector<const PhysicalStop*> tmpPhysicalStops;

        for (std::set
                    < std::vector<const Vertex*> >::iterator path = paths.begin ();
                    path != paths.end ();
                    ++path)
            {

                for (int i=0; i<path->size (); ++i)
                {
                    const Vertex* v = path->at (i);

                    // Is there any physical stop located at this vertex ?
                    tmpPhysicalStops.clear ();
                    _road->getTopography()->findPhysicalStops (v, tmpPhysicalStops);

                    for (std::vector<const PhysicalStop*>::const_iterator
                            itPstop = tmpPhysicalStops.begin ();
                            itPstop != tmpPhysicalStops.end ();
                            ++itPstop)
                    {
                        const PhysicalStop* pstop = *itPstop;

                        // Create an entry in result which is a path from start
                        // to the found physical stop
                        Road::RoadChunkVector pathChunks;

                        for (int j=0; j+1<=i; ++j)
                        {
                            const Edge* edge = _road->getTopography()->getEdge (path->at (j), path->at (j+1));

                            // Normally, there can be only one road chunk associated
                            // with an edge.
                            tmpChunks.clear ();
                            _road->getTopography ()->findRoadChunks (edge, tmpChunks);

                            assert (tmpChunks.size () == 1);

                            // If the last chunk inserted is the same than the one
                            // we just found, do nothing.
                            if ((pathChunks.size () > 0) &&
                                    (tmpChunks.at(0) == pathChunks[pathChunks.size ()-1]))
                                continue;

                            pathChunks.push_back (tmpChunks.at (0));

                        }

                        result.insert (PathToPhysicalStop (pathChunks, pstop));


                    }

                }
            }


        return result;

    }









}
