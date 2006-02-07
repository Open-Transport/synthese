#ifndef TOPOGRAPHY_H_
#define TOPOGRAPHY_H_

#include <map>
#include <vector>
#include <string>
#include <set>
#include <map>


namespace synmap
{

  class Vertex;
  class Location;
  class Edge;
  class Road;
  class RoadChunk;
  class Referrant;
  class PhysicalStop;
  class Itinerary;
  class Zone;


  using namespace std;

  class Topography
    {

    private:
	
      typedef map< double, Vertex*      > VertexMap1D;
      typedef map< double, VertexMap1D* > VertexMap2D;

      typedef map< const Vertex*, Edge*      > EdgeMap1D;
      typedef map< const Vertex*, EdgeMap1D* > EdgeMap2D;

	
      struct Referrants 
      {
	set<const RoadChunk*>       roadChunks;     
	set<const PhysicalStop*>    physicalStops;  
	set<const Itinerary*>       itineraries;    
	set<const Zone*>            zones;          
      };
		
      typedef map< const Vertex*, Referrants*>  VertexRefMap;
      typedef map< const Edge*,   Referrants*>  EdgeRefMap;

		
	
      VertexMap2D _vertices;
      EdgeMap2D   _edges;

      VertexRefMap _vertexReferrants;
      EdgeRefMap   _edgeReferrants;
	

      map<int, const Road*> _roads;
      map<int, const RoadChunk*> _roadChunks;
      map<int, const PhysicalStop*> _physicalStops;
      map<int, const Itinerary*> _itineraries;
      map<int, const Zone*> _zones;
	
	
    public:

      typedef bool (*RoadChunkFilter) (const RoadChunk*);
      typedef bool (*PhysicalStopFilter) (const PhysicalStop*);
      typedef bool (*ItineraryFilter) (const Itinerary*);
      typedef bool (*ZoneFilter) (const Zone*);

      Topography();
      virtual ~Topography();
	
	
      //! Creates the unique vertex associated with x and y.
      //! If the vertex already exists it is returned.
      /*!
	\param x x coordinate.
	\param y y coordinate.
	\return The immutable vertex.
      */
      const Vertex* newVertex (double x, double y);



      const Location* newLocation (int key, double x, double y);

	
      //! Creates the unique edge associated with 2 vertices.
      //! If the edge already exists it is returned.
      /*!
	\param from The origin vertex.
	\param to The target vertex.
	\return The immutable edge.
      */
      const Edge* newEdge (const Vertex* from, const Vertex* to);
	

      bool hasEdge (const Vertex* from, const Vertex* to);

      //! Gets the unique edge associated with 2 vertices.
      /*!
	\param from The origin vertex.
	\param to The target vertex.
	\return The immutable edge.
      */
      const Edge* getEdge (const Vertex* from, const Vertex* to);

      //! Gets the unique vertex associated with x and y.
      /*!
	\param x x coordinate.
	\param y y coordinate.
	\return The immutable vertex.
      */
      const Vertex* getVertex (double x, double y);


      const Road* newRoad (int key,
			   const std::string& name,
			   const std::string& discriminant,
			   const std::vector<RoadChunk*>& chunks);


      RoadChunk* newRoadChunk (int key,
			       const std::vector<const Location*>& steps,
			       int rightStartNumber, 
			       int rightEndNumber, 
			       int leftStartNumber, 
			       int leftEndNumber);

	
      const PhysicalStop* newPhysicalStop (int key,
					   int position, 
					   const Vertex* vertex);
	
      const Itinerary* newItinerary (int key,
				     const std::vector<const Location*>& steps); 
	
      const Zone* newZone (int key,
			   const std::vector<const Vertex*>& vertices);
	
    
      const Itinerary* getItinerary (int key) const;

      void removeItinerary (int key);



      void findRoadChunks (const Vertex* vertex, 
			   std::vector<const RoadChunk*>& dst, 
			   const RoadChunkFilter filter = 0) const;
					
      void findPhysicalStops (const Vertex* vertex, 
			      std::vector<const PhysicalStop*>& dst,
			      const PhysicalStopFilter filter = 0) const;
							
      void findItineraries (const Vertex* vertex, 
			    std::vector<const Itinerary*>& dst, 
			    const ItineraryFilter filter = 0) const;
	
      void findZones (const Vertex* vertex, 
		      std::vector<const Zone*>& dst,
		      const ZoneFilter filter = 0) const;
							

      void findRoadChunks (const Edge* edge, 
			   std::vector<const RoadChunk*>& dst, 
			   const RoadChunkFilter filter = 0) const;
					

      void findItineraries (const Edge* edge, 
			    std::vector<const Itinerary*>& dst,
			    const ItineraryFilter filter = 0) const;
	
	
      const map<int, const Itinerary*>& getItineraries () const { return _itineraries; }

	
	
	
    private:

      Vertex* newVertexPtr (double x, double y);
      Edge*   newEdgePtr (const Vertex* from, const Vertex* to);
	
      void addVertexReferrant (const Vertex* vertex, const Referrant* ref);
      void addEdgeReferrant (const Edge* vertex, const Referrant* ref);
	
	

    };

}

#endif /*TOPOGRAPHY_H_*/
