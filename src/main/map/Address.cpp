
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


	const 	Address::AddressNumber Address::UNKNOWN_ADDRESS_NUMBER = -1;

/** Constructeur.
 @param road Route sur laquelle se trouve l'adresse
 @param number Numéro d'adresse (rien = numéro inconnu)
 @param logicalPlace Lieu logique auquel appartient l'adresse (NULL si aucun)
 @param rankInLogicalPlace Rang de l'adresse au sein du lieu logique
 
 @todo Gérer ici immédiatement le PM : soit il est fourni, soit il est extrapolé d'après le numéro, soit on prend le milieu
*/
Address::Address(LogicalPlace* logicalPlace,
		 size_t rankInLogicalPlace,
		 Road* road, 
		 double metricOffset,
		 AddressNumber number)
  : NetworkAccessPoint (logicalPlace, rankInLogicalPlace)
  , _road(road)
  , _metricOffset (metricOffset)
  , _number(number)

{}


/** Destructeur.
*/
Address::~Address()
{}



std::set
    < std::pair<double, const Address*> >
Address::findDistancesToAddresses (double maxDistance) const
{
  std::set
    < Address::PathToAddress > paths = findPathsToAddresses (maxDistance);

  std::set
    < std::pair<double, const Address*> > result;
  
  for (std::set < Address::PathToAddress >::const_iterator iter (paths.begin ());
       iter != paths.end ();
       ++iter) {
    result.insert (make_pair (computePathLength (*iter), iter->second));

  }
  return result;

}



/** Recherche des adresses situés à proximité de l'adresse.
 
 @param maxDistance Distance maximale à parcourir
 
 L'algorithme consiste en un parcours intégral du graphe des segments de route, borné par une distance maximale parcourue.
 
 @todo Remplacer findmostplausiblechunk par un findChunkByPM Voir le positionnement de cette question.
 
 @author Marc Jambert
*/
std::set
< Address::PathToAddress >
Address::findPathsToAddresses (double maxDistance) const
    {
        const RoadChunk* chunk = _road->findMostPlausibleChunkForNumber (_number);
        // TODO : add an algorithm to find more precisely the vertex to
        // start from ?
        const Vertex* start = chunk->getStep (0)->getVertex ();
        std::set
            < std::vector<const Vertex*> > paths =
                start->findPathsToCloseNeighbors (maxDistance);

        std::set
            < PathToAddress > result;

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

                        result.insert (PathToAddress (pathChunks, pstop->getAddress ()));


                    }

                }
            }


        return result;

    }



double 
Address::computePathLength (const PathToAddress& path) {
  double pathLength = 0.0;
  for (std::vector<const RoadChunk*>::const_iterator iterChunk (path.first.begin ());
       iterChunk != path.first.end ();
       ++iterChunk) {
    pathLength += (*iterChunk)->getLength ();
  }

  return pathLength;

}




Address::PathToAddress
Address::findShortestPath (std::set<Address*> originAddresses,
			   std::set<Address*> destinationAddresses,
			   double maxDistance) 
{
  
  double minPathLength = 100000.0;
  PathToAddress shortestPath;
  
  // Algo mega-bourrin pour determiner le plus court chemin entre
  // un choix d'adresses de départ et d'arrivée
  for (std::set<Address*>::const_iterator from = originAddresses.begin ();
       from != originAddresses.end ();
       ++from) {
    
    std::set< Address::PathToAddress > paths = (*from)->findPathsToAddresses (maxDistance);
    for (std::set<PathToAddress>::const_iterator path = paths.begin ();
	 path != paths.end ();
	 ++path) {
      if (destinationAddresses.find ((Address*) path->second) != destinationAddresses.end ()) 
	{
	  double pathLength = computePathLength (*path);
	  if (pathLength < minPathLength) {
	    shortestPath = *path;
	  }
	}
    }
  }
	
  return shortestPath;
}





	
NetworkAccessPoint::AddressList 
Address::getAddresses(bool forDeparture)
{
  AddressList result;
  AddressWithAccessDuration addressAndDuration = make_pair (this, 0);
  result.push_back(addressAndDuration);
  return result;
}



}
