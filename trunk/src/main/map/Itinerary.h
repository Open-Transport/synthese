#ifndef ITINERARY_H_
#define ITINERARY_H_

#include "Referrant.h"
#include <vector>


namespace synmap
{



class PhysicalStop;
class Edge;
class Vertex;


// bus lines...
class Itinerary : public Referrant
{
private:

	const std::vector<const Vertex*> 		_vertices;
	const std::vector<const Edge*> 			_edges;
	const std::vector<const PhysicalStop*> 	_physicalStops;
	const std::vector<bool>					_isPhysicalStop;


	static const std::vector<const Edge*> createEdges (Topography* topography, 
													    const std::vector<const Vertex*>& vertices);
													    
	static const std::vector<const PhysicalStop*> createPhysicalStops (Topography* topography, 
														const Itinerary* itinerary, 
													    const std::vector<const Vertex*>& vertices,
													    const std::vector<bool>& physicalStop);

public:

	Itinerary(Topography* topography, 
			  int key,
			  const std::vector<const Vertex*>& vertices, 
			  const std::vector<bool>& physicalStop);
					
	virtual ~Itinerary();

	
	int getNbPhysicalStops () const { return _physicalStops.size (); }
	const PhysicalStop* getPhysicalStop (int position) const { return _physicalStops[position]; }
	bool isPhysicalStop (int vertexIndex);
	
	const std::vector<const Edge*>& 			getEdges () const { return _edges; }
	const std::vector<const Vertex*>& 			getVertices () const { return _vertices; }

    const Vertex* getVertex (int index) const;
    const Vertex* getFirstVertex () const;
    const Vertex* getLastVertex () const;
	
	bool hasVertex (const Vertex* vertex) const;
	bool hasEdge (const Edge* edge) const;

	int firstIndexOf (const Vertex* vertex) const;
	int firstIndexOf (const Edge* edge) const;
	
};

}

#endif /*ITINERARY_H_*/
