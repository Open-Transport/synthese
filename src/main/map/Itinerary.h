#ifndef ITINERARY_H_
#define ITINERARY_H_

#include "Referrant.h"
#include <vector>


namespace synmap
{



  class PhysicalStop;
  class Edge;
  class Location;
  class Vertex;


  // bus lines...
  class Itinerary : public Referrant
    {
    private:

      std::vector<const Location*> 	_steps;
      std::vector<const Vertex*> 	_vertices;
      std::vector<const Edge*> 		_edges;
      std::vector<const PhysicalStop*> 	_physicalStops;


      static const std::vector<const Edge*> createEdges (Topography* topography, 
							 const std::vector<const Location*>& steps);
													    

    public:

      Itinerary(Topography* topography, 
		int key,
		const std::vector<const Location*>& steps);
					
      virtual ~Itinerary();

	
      int getNbPhysicalStops () const { return _physicalStops.size (); }
      const PhysicalStop* getPhysicalStop (int position) const { return _physicalStops[position]; }
	
      const std::vector<const Location*>&    getSteps () const { return _steps; }
      const std::vector<const Vertex*>&    getVertices () const { return _vertices; }
      const std::vector<const Edge*>& 	     getEdges () const { return _edges; }

      const Location* getStep (int index) const;
      const Location* getFirstStep () const;
      const Location* getLastStep () const;
	
      bool hasVertex (const Vertex* vertex) const;
      bool hasEdge (const Edge* edge) const;

      int firstIndexOf (const Vertex* vertex) const;
      int firstIndexOf (const Edge* edge) const;
	
    };

}

#endif /*ITINERARY_H_*/
