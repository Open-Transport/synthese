#ifndef PHYSICALSTOP_H_
#define PHYSICALSTOP_H_

#include "Location.h"
#include "Referrant.h"


namespace synmap
{

class Vertex;
class Road;
class Itinerary;
class Address;

class PhysicalStop : public Location, public Referrant
{
	
private:

  const int _logicalPlaceId;
  const int _rank;

  Address* _address; // corresponding address. c vraiment a chier
		     // comme implementation mais ya conflit d'interet
		     // entre carto et recherche d'adresse.
	
public:

	PhysicalStop(Topography* topography, 
		     int logicalPlaceId,
		     int rank,
		     const Vertex* vertex);
				 
	virtual ~PhysicalStop();


	int getLogicalPlaceId () const { return _logicalPlaceId; }
	int getRank () const { return _rank; }

	// bidouille infame
	void setMetricOffset (const Road* road, double metricOffset);
	
	// bidouille encore plus infame
	Address* getAddress () const;

	friend class Topography;
};

}

#endif /*PHYSICALSTOP_H_*/
