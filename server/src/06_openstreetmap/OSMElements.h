/*
 * Elements.h
 *
 *  Created on: Jan 26, 2010
 *      Author: tbonfort
 */

#ifndef OSMELEMENTS_H_
#define OSMELEMENTS_H_

#include <boost/shared_ptr.hpp>
#include <list>
#include "AttributeMap.h"
#include <geos/geom/Geometry.h>
#include <geos/geom/prep/PreparedPolygon.h>
#include <proj_api.h>
#include "RoadTypes.hpp"
#include "OSMException.h"

namespace synthese {
namespace osm {


/**
 * \brief Abstract Base Class representing an OSM element along with its tags
 */
class Element {
public:
   /**
    * \param attributes key/value pairs included in osm xml tag
    * \exception Exception if the attributes don't contain a valid "id" key
    */
   Element(AttributeMap &attributes) throw(Exception);
   virtual ~Element();

   /**
    * \return the id of the element in the osm database
    */
   unsigned long long int getId();

   /**
    * \brief add a key/value pair to the element
    * \param key The key to add
    * \param value The value to associate with the key
    */
   void addTag(const std::string &key, const std::string &value);

   /**
    * \brief delete a tag from the element
    * \param key The key to delete
    */
   void delTag(const std::string &key);

   /**
    * \brief check if the element contains a tag
    * \param key The key to lookup
    */
   bool hasTag(const std::string &key);

   /**
    * \return the tag value corresponding to the key
    * \exception Exception if the map doesn't contain the given key
    */
   std::string getTag(const std::string &key) throw(Exception);

   /**
    * \brief builds the WKT (see http://en.wikipedia.org/wiki/Well-known_text)representation of the element
    * \return the std::string containing the WKT representation of the element
    */
   virtual std::string toWKT()=0;

   bool isOfType(const std::string &tag);

   /**
    * \return the geos geometry corresponding to the element
    */
   virtual boost::shared_ptr<const geos::geom::Geometry> toGeometry();
   virtual boost::shared_ptr<const geos::geom::prep::PreparedGeometry> toPreparedGeometry();


protected:
   virtual void resetGeometry();
   boost::shared_ptr<geos::geom::Geometry> geometry;
   boost::shared_ptr<geos::geom::prep::PreparedGeometry> preparedGeometry;
   std::map<std::string,std::string> tags;
   unsigned long long int id;

public:
   static const std::string TAG_HIGHWAY;
   static const std::string TAG_RAILWAY;
   static const std::string TAG_AMENITY;
   static const std::string TAG_JUNCTION;
   static const std::string TAG_SERVICE;
   static const std::string TAG_FOOT;
   static const std::string TAG_BICYCLE;
   static const std::string TAG_MOTOR_VEHICLE;
   static const std::string TAG_MOTORCAR;
   static const std::string TAG_BOUNDARY;
   static const std::string TAG_ADMINLEVEL;
   static const std::string TAG_NAME;
   static const std::string TAG_ACCESS;
   static const std::string TAG_AREA;
   static const std::string TAG_BRIDGE;
   static const std::string TAG_TUNNEL;


   static const std::string ATTR_LONGITUDE;
   static const std::string ATTR_LATITUDE;
   static const std::string ATTR_TYPE;
   static const std::string ATTR_ROLE;
   static const std::string ATTR_ID;
   static const std::string ATTR_KEY;
   static const std::string ATTR_VALUE;
   static const std::string ATTR_REF;

   static const projPJ originalProjection;

};

class Relation;
class Node;
class Way;
class Network;

typedef boost::shared_ptr<Network> NetworkPtr;
typedef boost::shared_ptr<Relation> RelationPtr;
typedef boost::shared_ptr<Element> ElementPtr;
typedef boost::shared_ptr<Way> WayPtr;
typedef boost::shared_ptr<Node> NodePtr;
//typedef Relation* RelationPtr;
//typedef Element* ElementPtr;
//typedef Way* WayPtr;
//typedef Node* NodePtr;


/**
 * \brief represent a single OSM node, which represents a point given by its longitude and latitude
 */
class Node: public synthese::osm::Element {
public:
   /**
    * \param attrs key/value pairs of attributes from the osm xml file. should contain at least id, lon and lat.
    */
   Node(AttributeMap &attrs) throw(Exception);
   std::string toWKT();
   /**
    * \return node longitude
    */
   double getLon();

   /**
    * \return node latitude
    */
   double getLat();

   /**
    * \return is the node a a transport stop
    */
   bool isStop();

   /**
    * \return the number of ways that pass through this node
    */
   int numConnectedWay();

   double distance(NodePtr &other);

   //std::list<Way*> getWays() { return ways; }

protected:
   double lon,lat;

private:
   /**
    * Ways that reference this node.
    * accessed by the owning network for reference counting
    */
   std::list<Way*> ways;

   friend class Way;
};

/**
 * \brief Represents an OSM way, which is an ordered list of \ref Node "nodes"
 */
class Way: public Element {
public:

   static std::map<std::string, road::RoadType> highwayTypes;
   static std::map<road::RoadType, double> defaultSpeed;
   static std::map<std::string, double> implicitSpeed;
   static std::map<std::string, std::string> defaultName;

   Way(AttributeMap &attrs) throw(Exception);

   /**
    * \brief Add a reference to a \ref Node to the end of the way
    * \param id The id of the referenced \ref Node, this should be the same as the id of the given \param node
    * \param node The node to add, or NULL
    */
   void pushNode(unsigned long long int id, NodePtr node);

   /**
    * \brief Add a node to the end of the way
    * \param node The node to add
    */
   void pushNode(NodePtr node);

   /**
    * \brief Returns the list of nodes referenced by the way
    */
   const std::list<std::pair<unsigned long long int, NodePtr> > *getNodes();
   std::string toWKT();

   /**
    * \brief Searches the nodes of the given network to compute unresolved references
    * \exception RefNotFoundException if the way references a \ref Node that isn't in the \ref Network
    */
   void consolidate(Network *network) throw(RefNotFoundException);

   /**
    * \brief inspects the way's tags and tells if it can correspond to a pedestrian route.
    *
    * A \ref Way is said to be walkable if it is not a highway or trunk section, and if it
    * is not specifically marked as restricted to pedestrians.
    */
   bool isWalkable();
   bool isDrivable();
   bool isBikable();

   /**
    * loop through this way's nodes, and add a reference to ourself to each node.
    *
    */
   void referenceWithNodes();

   road::RoadType getRoadType();

   road::RoadType getAssociatedRoadType();

   double getAssociatedSpeed();

   std::string getName();

protected:
   /// the \ref Node "nodes" referenced by the way
   std::list<std::pair<unsigned long long int, NodePtr> > nodes;

};

/**
 * \brief class representing all the elements of an OSM extract: \ref Node "nodes", \ref Way "ways" and \ref Relation "relations"
 */
class Network {
public:
   Network();
   virtual ~Network();
   /**
    * \brief add a \ref Node to the network
    * \param node The node to add
    */
   virtual void addNode(NodePtr node);

   /**
    * \brief add a \ref Way to the network
    * \param way The way to add
    */
   virtual void addWay(WayPtr way);


   /**
    * \brief add a \ref Relation to the network
    * \param relation The relation to add
    */
   virtual void addRelation(RelationPtr relation);

   virtual const std::map<unsigned long long int, NodePtr > *getNodes();
   virtual const std::map<unsigned long long int, WayPtr > *getWays();
   virtual const std::map<unsigned long long int, RelationPtr > *getRelations();
   virtual NodePtr getNode(unsigned long long int id) throw(RefNotFoundException);
   virtual WayPtr getWay(unsigned long long int id) throw(RefNotFoundException);
   virtual RelationPtr getRelation(unsigned long long int id) throw(RefNotFoundException);

   /**
    * \brief compute missing references in the network's ways and relations
    *
    * loop through the created network and find missing references:
    * \li ways referencing unknown nodes (this isn't usually correctable even in a second pass,
      as all the nodes are written to the osm file before the ways, so if a way references
      an unknown node this is most likely definitive
    * \li relations referencing unknown nodes: ditto as for ways (see above)
    * \li relations referencing unknown ways: ditto (relations are written to file after all ways)
    * \li relations referencing unknown relations: likely to happen
    */
   void consolidate(bool discard_if_missing_reference);

   /**
    * \brief compute missing references in the network's ways
    */
   void consolidateWays(bool discard_if_missing_reference);

   /**
    * \brief compute missing references in the network's relations
    */
   void consolidateRelations(bool discard_if_missing_reference);


   /**
    * \brief returns the list of ways that represent a physical route accessible to a pedestrian
    */
   std::map<unsigned long long int,WayPtr> getWalkableWays();


   /**
    * \brief build polygonal geometries corresponding to all the relations of
    * the given administrative level
    *
    * the administrative levels as defined by openstreetmap:
    * \li municipalities: 8
    * \li departements: 6
    * \li countries: 2
    */
   std::map<unsigned long long int,RelationPtr> getAdministrativeBoundaries(int admin_level);

   /**
    * \brief return the walkable ways of the network, associated with their containing
    * administrative area
    */
   std::map<unsigned long long int,std::pair<RelationPtr,std::map<unsigned long long int, WayPtr> > > getWalkableWaysByAdminBoundary(int admin_level);

   // Return the valid ways of the network
   std::map<unsigned long long int, std::pair<RelationPtr, std::map<unsigned long long int, WayPtr> > > getWaysByAdminBoundary(int admin_level);

protected:
   std::map<unsigned long long int, NodePtr > nodes;
   std::map<unsigned long long int, WayPtr > ways;
   std::map<unsigned long long int, RelationPtr > relations;
};

/**
 * \brief class representing the relations in an osm network
 */
class Relation: public synthese::osm::Element {
public:
   Relation(AttributeMap &attrs);
   std::string toWKT();
   /**
    * \brief add a related node
    */
   void add(AttributeMap &attrs, NodePtr node);
   /**
    * \brief add a related way
    */
   void add(AttributeMap &attrs, WayPtr way);

   /**
    * \brief add a related relation
    */
   void add(AttributeMap &attrs, RelationPtr relation);

   /**
    * \brief add a related node by id
    */
   void addUnresolvedNode(AttributeMap &attrs, unsigned long long int id);

   /**
    * \brief add a related way by id
    */
   void addUnresolvedWay(AttributeMap &attrs, unsigned long long int id);

   /**
    * \brief add a related relation by id
    */
   void addUnresolvedRelation(AttributeMap &attrs, unsigned long long int id);

   /**
    * \brief return list of ways matching given role
    */
   std::list<WayPtr> getWays(std::string &role);

   /**
    * \brief return list of ways matching given role, recursively looking into the related relations
    */
   std::list<WayPtr> getRelatedWays(std::string &role);

   /**
    * \brief return list of ways, recursively looking into the related relations
    */
   std::list<WayPtr> getRelatedWays();
   std::list<NodePtr> getNodes(std::string &role);
   std::list<NodePtr> getNodes();
   std::list<RelationPtr> getRelations(std::string &role);
   std::list<RelationPtr> getRelations();

   /**
    * \brief check if relation contains given way
    * \param way the way to lookup
    */
   bool contains(WayPtr &way);

   /**
    * \brief check if relation contains given node
    * \param node the node to lookup
    */
   bool contains(NodePtr &node);

   /**
    * \brief check if relation contains given relation
    * \param relation the relation to lookup
    */
   bool contains(RelationPtr &relation);

   /**
    * \brief search network for missing related elements
    * \exception RefNotFoundException if a related element isn't contained in the network
    */
   void consolidate(Network *network) throw(RefNotFoundException);

   virtual boost::shared_ptr<const geos::geom::Geometry> toGeometry();

protected:
   void add(AttributeMap &attrs, unsigned long long int id, NodePtr node);
   void add(AttributeMap &attrs, unsigned long long int id, WayPtr way);
   void add(AttributeMap &attrs, unsigned long long int id, RelationPtr relation);
   void resetGeometry();
   std::map<std::string, std::map<unsigned long long int, NodePtr> >  nodes;
   std::map<std::string, std::map<unsigned long long int, WayPtr> >  ways;
   std::map<std::string, std::map<unsigned long long int, RelationPtr> >  relations;

private:
   ///has the geometry extraction process already run
   bool geometry_computed;

   /**
    * \brief compute the geometry (polygon or multipolygon) made by this relation's ways
    * \return geos::geom::Polygon or geos::geom::Multipolygon if the ways form a closed
    * surface, or NULL if no closed surface could be constructed.
    */
   geos::geom::Geometry* extractBoundary();

};

}}
#endif /* ELEMENTS_H_ */
