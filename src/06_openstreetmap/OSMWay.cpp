/*
 * Way.cpp
 *
 *  Created on: Jan 25, 2010
 *      Author: tbonfort
 */

#include "OSMElements.h"
#include "Road.h"

#include <sstream>
#include <boost/assign/list_of.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

namespace synthese 
{
	namespace osm 
	{
		using namespace road;

		std::map<std::string,Road::RoadType> Way::highwayTypes = boost::assign::map_list_of
			("motorway", Road::ROAD_TYPE_MOTORWAY)
			("motorway_link", Road::ROAD_TYPE_ACCESSROAD)
			("trunk", Road::ROAD_TYPE_PRINCIPLEAXIS)
			("trunk_link", Road::ROAD_TYPE_ACCESSROAD)
			("primary", Road::ROAD_TYPE_PRINCIPLEAXIS)
			("secondary", Road::ROAD_TYPE_SECONDARYAXIS)
			("tertiary", Road::ROAD_TYPE_HIGHWAY)
			("primary_link", Road::ROAD_TYPE_ACCESSROAD)
			("secondary_link", Road::ROAD_TYPE_ACCESSROAD)
			("tertiary_link", Road::ROAD_TYPE_ACCESSROAD)
			("unclassified", Road::ROAD_TYPE_STREET)
			("road", Road::ROAD_TYPE_STREET)
			("residential", Road::ROAD_TYPE_STREET)
			("living_street", Road::ROAD_TYPE_STREET)
			("service", Road::ROAD_TYPE_SERVICE)
			("track", Road::ROAD_TYPE_PEDESTRIANPATH)
			("pedestrian", Road::ROAD_TYPE_PEDESTRIANSTREET)
			("path", Road::ROAD_TYPE_PEDESTRIANPATH)
			("cycleway", Road::ROAD_TYPE_PEDESTRIANPATH)
			("footway", Road::ROAD_TYPE_PEDESTRIANPATH)
			("bridleway", Road::ROAD_TYPE_PEDESTRIANPATH)
			("byway", Road::ROAD_TYPE_UNKNOWN)
			("steps", Road::ROAD_TYPE_STEPS)
			("unclassified", Road::ROAD_TYPE_UNKNOWN);

		std::map<Road::RoadType, double> Way::defaultSpeed = boost::assign::map_list_of
			(Road::ROAD_TYPE_MOTORWAY, 110 / 3.6)
			(Road::ROAD_TYPE_ACCESSROAD, 50 / 3.6)
			(Road::ROAD_TYPE_PRINCIPLEAXIS, 90 / 3.6)
			(Road::ROAD_TYPE_SECONDARYAXIS, 50 / 3.6)
			(Road::ROAD_TYPE_PRIVATEWAY, 30 / 3.6)
			(Road::ROAD_TYPE_PEDESTRIANPATH, 30 / 3.6)
			(Road::ROAD_TYPE_PEDESTRIANSTREET, 30 / 3.6)
			(Road::ROAD_TYPE_UNKNOWN, 50 / 3.6)
			(Road::ROAD_TYPE_STEPS, 10 / 3.6)
			(Road::ROAD_TYPE_STREET, 50 / 3.6)
			(Road::ROAD_TYPE_HIGHWAY, 50 / 3.6)
			(Road::ROAD_TYPE_SERVICE, 50 / 3.6);

		Way::Way(AttributeMap &attrs) throw(Exception): Element(attrs)
		{
			// TODO Auto-generated constructor stub
		}

		void Way::pushNode(unsigned long long int id ,NodePtr node)
		{
			std::pair<unsigned long long int, NodePtr> pair(id, node);
			nodes.push_back(pair);
			resetGeometry();
		}

		void Way::pushNode(NodePtr node)
		{
			pushNode(node->getId(), node);
		}

		std::string Way::toWKT()
		{
			std::stringstream wkt;
			std::list<std::pair<unsigned long long int,NodePtr> >::iterator it = nodes.begin();
			wkt << "LINESTRING(";

			if (it != nodes.end())
				wkt << boost::lexical_cast<std::string>((*it).second->getLon()) << " " << boost::lexical_cast<std::string>((*it).second->getLat());

			while (++it != nodes.end()) 
			{
				wkt << "," << boost::lexical_cast<std::string>((*it).second->getLon()) << " " << boost::lexical_cast<std::string>((*it).second->getLat());
			}

			wkt << ")";
			return wkt.str();
		}

		const std::list<std::pair<unsigned long long int, NodePtr> >* Way::getNodes()
		{
			return &nodes;
		}

		void Way::consolidate(Network* network) throw(RefNotFoundException)
		{
			std::list<std::pair<unsigned long long int,NodePtr> >::iterator it = nodes.begin();
			while(it != nodes.end())
			{
				if(!(it->second))
				{
					NodePtr node = network->getNode(it->first);
					it->second = node;
				}
				it++;
			}
		}

		bool Way::isWalkable()
		{
			bool isWalkable(true);

			if(hasTag(TAG_HIGHWAY))
			{
				std::string highway = getTag(TAG_HIGHWAY);
				if(highway == "motorway" || highway == "motorway_link")
					isWalkable = false;
				else if(highwayTypes.find(highway) == highwayTypes.end())
					isWalkable = false;
			}

			if(hasTag(TAG_ACCESS))
			{
				if(getTag(TAG_ACCESS) == "no")
					isWalkable = false;
			}

			if(hasTag(TAG_FOOT))
			{
				std::string access = getTag(TAG_FOOT);
				if(access == "no")
					isWalkable = false;
				else if(access == "yes")
					isWalkable = true;
			}

			return isWalkable;
		}

		bool Way::isDrivable()
		{
			bool isDrivable(true);

			if(hasTag(TAG_HIGHWAY))
			{
				std::string highway = getTag(TAG_HIGHWAY);
				if(highway == "track" || highway == "pedestrian" || highway == "path" || highway == "cycleway" || highway == "footway" || highway == "bridleway" || highway == "steps")
					isDrivable = false;
				else if(highwayTypes.find(highway) == highwayTypes.end())
					isDrivable = false;
			}

			if(hasTag(TAG_ACCESS))
			{
				if(getTag(TAG_ACCESS) == "no")
					isDrivable = false;
			}

			if(hasTag(TAG_MOTOR_VEHICLE))
			{
				std::string access = getTag(TAG_MOTOR_VEHICLE);
				if(access == "no")
					isDrivable = false;
				else if(access == "yes")
					isDrivable = true;
			}

			if(hasTag(TAG_MOTORCAR))
			{
				std::string access = getTag(TAG_MOTORCAR);
				if(access == "no")
					isDrivable = false;
				else if(access == "yes")
					isDrivable = true;
			}

			return isDrivable;
		}

		bool Way::isBikable() 
		{
			bool isBikable(true);

			if(hasTag(TAG_HIGHWAY))
			{
				std::string highway = getTag(TAG_HIGHWAY);
				if(highway == "motorway" || highway == "motorway_link")
					isBikable = false;
				else if(highwayTypes.find(highway) == highwayTypes.end())
					isBikable = false;
			}

			if(hasTag(TAG_ACCESS))
			{
				if(getTag(TAG_ACCESS) == "no")
					isBikable = false;
			}

			if(hasTag(TAG_BICYCLE))
			{
				std::string access = getTag(TAG_BICYCLE);
				if(access == "no")
					isBikable = false;
				else if(access == "yes")
					isBikable = true;
			}

			return isBikable;
		}

		Road::RoadType Way::getRoadType()
		{
			Road::RoadType type = Road::ROAD_TYPE_UNKNOWN;
			if(!isWalkable())
				return type;

			try 
			{
				if(isOfType(TAG_BRIDGE))
					type = Road::ROAD_TYPE_BRIDGE;
				else if(isOfType(TAG_TUNNEL))
					type = Road::ROAD_TYPE_TUNNEL;
				else
				{
					std::string highway = getTag(TAG_HIGHWAY);
					std::map<std::string,Road::RoadType>::iterator it = highwayTypes.find(highway);
					if(it != highwayTypes.end())
						type = it->second;
				}
			}
			catch(Exception e) {}

			return type;
		}

		Road::RoadType Way::getAssociatedRoadType()
		{
			Road::RoadType type = Road::ROAD_TYPE_UNKNOWN;

			if(hasTag(TAG_HIGHWAY))
			{
				std::string highway = getTag(TAG_HIGHWAY);
				std::map<std::string, Road::RoadType>::iterator it = highwayTypes.find(highway);

				if(it != highwayTypes.end())
					type = it->second;
			}

			return type;
		}

		double Way::getAssociatedSpeed()
		{
			double maxSpeed = 50 / 3.6;
			if(hasTag("maxspeed"))
			{
				try
				{
					maxSpeed = boost::lexical_cast<double>(getTag("maxspeed")) / 3.6;
					return maxSpeed;
				}
				catch(boost::bad_lexical_cast &)
				{
				}
			}

			Road::RoadType type = getAssociatedRoadType();
			std::map<Road::RoadType, double>::iterator it = defaultSpeed.find(type);

			if(it != defaultSpeed.end())
				maxSpeed = it->second;

			return maxSpeed;
		}

		void Way::referenceWithNodes() 
		{
			typedef std::pair<unsigned long long int, NodePtr> nodeType;
			BOOST_FOREACH(nodeType node, nodes) 
			{
				node.second->ways.push_back(this);
			}
		}
	}
}
