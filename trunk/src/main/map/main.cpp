
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <assert.h>

#include "Geometry.h"
#include "DBLComparator.h"
#include "Map.h"
#include "MapBackground.h"
#include "XYPoint.h"
#include "Vertex.h"
#include "Edge.h"
#include "Itinerary.h"
#include "PhysicalStop.h"
#include "MapBackgroundManager.h"



#include "Rectangle.h"
#include "DrawableBusLine.h"
#include "PostscriptCanvas.h"
#include "Topography.h"

using namespace std;
using namespace synmap;




std::vector<XYPoint> 
pattern10 () {
	std::vector<XYPoint> points;
	points.push_back (XYPoint (100.0, 100.0));
	points.push_back (XYPoint (400.0, 300.0));
	points.push_back (XYPoint (700.0, 200.0));
	points.push_back (XYPoint (900.0, 600.0));
	points.push_back (XYPoint (1200.0, 700.0));
	points.push_back (XYPoint (1400.0, 300.0));
	points.push_back (XYPoint (1500.0, 1100.0));
	points.push_back (XYPoint (1100.0, 1200.0));
	points.push_back (XYPoint (500.0, 900.0));
	points.push_back (XYPoint (400.0, 1300.0));
	points.push_back (XYPoint (800.0, 1600.0));
	points.push_back (XYPoint (1400.0, 1700.0));
	points.push_back (XYPoint (1800.0, 1100.0));
	
	return points;	
}



std::vector<XYPoint> 
pattern20 () {
	std::vector<XYPoint> points;
	points.push_back (XYPoint (700.0, 200.0));
	points.push_back (XYPoint (900.0, 600.0));
	points.push_back (XYPoint (1200.0, 700.0));
	points.push_back (XYPoint (1500.0, 1100.0));
	
	return points;	
}


std::vector<XYPoint> 
pattern1 () {
	std::vector<XYPoint> points;
	points.push_back (XYPoint (300.0, 600.0));
	points.push_back (XYPoint (700.0, 1000.0));
	points.push_back (XYPoint (1100.0, 600.0));
	points.push_back (XYPoint (700.0, 200.0));
	points.push_back (XYPoint (500.0, 400.0));
	
	return points;	
}

std::vector<XYPoint> 
pattern2 () {
	std::vector<XYPoint> points;
	points.push_back (XYPoint (100.0, 100.0));
	points.push_back (XYPoint (400.0, 300.0));
	points.push_back (XYPoint (700.0, 200.0));
	points.push_back (XYPoint (900.0, 600.0));
	points.push_back (XYPoint (1200.0, 700.0));
	points.push_back (XYPoint (1400.0, 300.0));
	points.push_back (XYPoint (1500.0, 1100.0));
	points.push_back (XYPoint (1100.0, 1200.0));
	points.push_back (XYPoint (500.0, 900.0));
	points.push_back (XYPoint (400.0, 1300.0));
	points.push_back (XYPoint (800.0, 1600.0));
	points.push_back (XYPoint (1400.0, 1700.0));
	points.push_back (XYPoint (1800.0, 1100.0));
	
	return points;	

}


std::vector<XYPoint> 
pattern3 () {
	std::vector<XYPoint> points;
	points.push_back (XYPoint (1100.0, 600.0));
	points.push_back (XYPoint (700.0, 1000.0));
	points.push_back (XYPoint (300.0, 600.0));
	points.push_back (XYPoint (700.0, 200.0));
	points.push_back (XYPoint (900.0, 400.0));
	
	return points;	
}


std::vector<XYPoint> 
pattern4 () 
{
	std::vector<XYPoint> points;
	points.push_back (XYPoint (200.0, 200.0));
	points.push_back (XYPoint (600.0, 200.0));
	points.push_back (XYPoint (600.0, 600.0));
	points.push_back (XYPoint (200.0, 600.0));
	points.push_back (XYPoint (200.0, 200.0));
	
	return points;	
}

std::vector<XYPoint> 
pattern5 () 
{
	std::vector<XYPoint> points;
	points.push_back (XYPoint (200.0, 600.0));
	points.push_back (XYPoint (700.0, 200.0));
	points.push_back (XYPoint (200.0, 200.0));
	points.push_back (XYPoint (700.0, 900.0));
	points.push_back (XYPoint (200.0, 900.0));
	points.push_back (XYPoint (700.0, 400.0));
	
	return points;	
}

std::vector<XYPoint> 
pattern6 () 
{
	std::vector<XYPoint> points;
	points.push_back (XYPoint (200.0, 600.0));
	points.push_back (XYPoint (700.0, 200.0));
	points.push_back (XYPoint (400.0, 200.0));
	points.push_back (XYPoint (300.0, 500.0));
	points.push_back (XYPoint (200.0, 900.0));
	points.push_back (XYPoint (700.0, 400.0));
	
	return points;	
}

std::vector<XYPoint> 
pattern7 () 
{
	std::vector<XYPoint> points;
	points.push_back (XYPoint (900.0, 800.0));
	points.push_back (XYPoint (600.0, 1100.0));
	points.push_back (XYPoint (1500.0, 900.0));
	points.push_back (XYPoint (1500.0, 1400.0));
	points.push_back (XYPoint (300.0, 1400.0));
	points.push_back (XYPoint (299.0, 700.0));
	points.push_back (XYPoint (1100.0, 600.0));
	points.push_back (XYPoint (700.0, 200.0));
	points.push_back (XYPoint (100.0, 300.0));
	points.push_back (XYPoint (300.0, 1800.0));
	points.push_back (XYPoint (800.0, 1600.0));
	points.push_back (XYPoint (1200.0, 1900.0));
	points.push_back (XYPoint (1900.0, 1500.0));
	points.push_back (XYPoint (1300.0, 400.0));
	
	return points;	
}


std::vector<XYPoint> 
pattern8 () 
{
	std::vector<XYPoint> points;
	points.push_back (XYPoint (200.0, 200.0));
	points.push_back (XYPoint (700.0, 500.0));
	points.push_back (XYPoint (700.0, 800.0));  // Limit problem...
	points.push_back (XYPoint (700.0, 1100.0));
	points.push_back (XYPoint (1100.0, 1300.0));
	return points;	
}




/*
double mapXmin, mapYmin, mapXmax, mapYmax;

void loadRegions (const std::string& filepath, std::vector<Region>& regions) {
	char buf[256];
	// fill regions from file
	ifstream ifs (filepath.c_str());
	bool first;
	
	int nbPoints;
	double lat, lon;
	while (!ifs.eof()) {
		ifs.getline (buf, 256);
		Region reg;
		std::string s (buf);
		if (s.find ("Region") == 0) {
			ifs >> nbPoints;
			for (int i=0; i<nbPoints; ++i) {
				ifs >> lat;
				ifs >> lon;
				
				if (first) {
					mapXmin = lat;
					mapYmin = lon; 
					mapXmax = lat; 
					mapYmax = lon;
					first = false;
				} else {
					if (lat < mapXmin) mapXmin = lat;
					if (lat > mapXmax) mapXmax = lat;
					if (lon < mapYmin) mapYmin = lon;
					if (lon > mapYmax) mapYmax = lon;
										
				}
				
				reg.push_back(XYPoint (lat, lon));
			}
			regions.push_back(reg);
		}
		
	}
		
    cout << regions.size () << " regions found." << endl;
	
}


void loadPlines (const std::string& filepath, std::vector<Street>& regions) {
	char buf[256];
	// fill regions from file
	ifstream ifs (filepath.c_str());
	bool first;
	
	int nblimits = 0;
	
	int nbPoints;
	double lat, lon;
	while (!ifs.eof()) {
		ifs.getline (buf, 256);
		Region reg;
		std::string s (buf);
		if (s.find ("Pline") == 0) {
			nbPoints = atoi (s.substr (6).c_str());
			for (int i=0; i<nbPoints; ++i) {
				ifs >> lat;
				ifs >> lon;
				
				if (first) {
					mapXmin = lat;
					mapYmin = lon; 
					mapXmax = lat; 
					mapYmax = lon;
					first = false;
				} else {
					if (lat < mapXmin) mapXmin = lat;
					if (lat > mapXmax) mapXmax = lat;
					if (lon < mapYmin) mapYmin = lon;
					if (lon > mapYmax) mapYmax = lon;
										
				}
				
				reg.push_back(XYPoint (lat, lon));
			}
			regions.push_back(reg);
			++nblimits;
		} 
		
	}
		
    cout << nblimits << " plines found." << endl;
	
}

void loadLines (const std::string& filepath, std::vector<Street>& streets) {
	char buf[256];
	// fill regions from file
	ifstream ifs (filepath.c_str());
	bool first;
	
	int nblimits = 0;
	
	int nbPoints;
	double lat, lon;
	while (!ifs.eof()) {
		ifs.getline (buf, 256);
		Street street;
		std::string s (buf);
		if (s.find ("Line") == 0) {
			s = (s.substr (5).c_str());
			stringstream ss (s);
			ss >> lat;
			ss >> lon;
			street.push_back(XYPoint (lat, lon));
			ss >> lat;
			ss >> lon;
			street.push_back(XYPoint (lat, lon));
			
			streets.push_back(street);
			++nblimits;
		} 
		
	}
		
    cout << nblimits << " lines found." << endl;
	
}
*/

/*
void
drawPoints(Region& points, PostscriptCanvas& sp) {
	if (points.size() < 1) return;
	sp.newPath();
	
	sp.moveTo(points[0].x, points[0].y);
	
	
	for (int i=1; i<points.size (); ++i) {
		double x = points[i].x;
		double y = points[i].y;
		
		sp.lineTo(x, y);		
	}
	sp.stroke();	
}

*/


/*
void
testVertex () {
	Topography topo;
	const Vertex* v0 = topo.newVertex (1.5, 2.5);
	const Vertex* v1 = topo.newVertex (1.5, 2.5);
	const Vertex* v2 = topo.newVertex (1.5, 3.5);
	
	assert (v0->getX () == 1.5);
	assert (v0->getY () == 2.5);
	
	assert (v0 = v1);
	assert (v0 != v2);
}


void
testEdge () {
	Topography topo;
	const Vertex* v0 = topo.newVertex (1.5, 2.5);
	const Vertex* v1 = topo.newVertex (1.5, 2.5);
	const Vertex* v2 = topo.newVertex (1.5, 4.5);

	const Edge* e0 = topo.newEdge (v0, v2);
	const Edge* e1 = topo.newEdge (v1, v2);
	const Edge* e2 = topo.newEdge (v2, v1);
	
	assert (e0->getFrom () == v0);
	assert (e0->getTo () == v2);
	
	assert (e0 == e1);
	assert (e0 != e2);
	
}

void
testItinerary () {
	Topography topo;
	std::vector<const Vertex*> vertices;
	
	vertices.push_back (topo.newVertex (1.5, 2.5));
	vertices.push_back (topo.newVertex (2.5, 3.5));
	vertices.push_back (topo.newVertex (3.5, 4.5));
	vertices.push_back (topo.newVertex (3.5, 5.5));
	vertices.push_back (topo.newVertex (3.5, 6.5));

	std::vector<bool> pstops;
	pstops.push_back (false);
	pstops.push_back (true);
	pstops.push_back (true);
	pstops.push_back (false);
	pstops.push_back (true);
	
	const Itinerary* iti0 = topo.newItinerary(iti_key++, vertices, pstops);
	
	assert (iti0->getNbPhysicalStops () == 3);
	assert (iti0->getVertices ().size () == 5);
	assert (iti0->getEdges ().size () == 4);

	{	
		std::vector<const PhysicalStop*> stops;
		topo.findPhysicalStops(topo.newVertex (2.5, 3.5), stops);
		assert (stops.size () == 1);	
	}
	{	
		std::vector<const PhysicalStop*> stops;
		topo.findPhysicalStops(topo.newVertex (1.5, 2.5), stops);
		assert (stops.size () == 0);	
	}
	{	
		std::vector<const Itinerary*> itis;
		topo.findItineraries(topo.newVertex (1.2, 2.5), itis);
		assert (itis.size () == 0);	
	}
	{	
		std::vector<const Itinerary*> itis;
		topo.findItineraries(topo.newVertex (1.5, 2.5), itis);
		assert (itis.size () == 1);	
	}
	
}

*/


/*
 * 
============== Pattern1 for testing line separation =============================
=== one common edge
=== four lines

	Topography topo;
	std::vector<const Vertex*> vertices;
	std::vector<bool> pstops;
	vertices.push_back (topo.newVertex (3.0, 3.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (10.0, 10.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (14.0, 14.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (23.0, 23.0)); pstops.push_back (false);
	
	Itinerary* iti1 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti1->setProperty("name", "bus1");
	iti1->setProperty("lineNumber", "1");
	iti1->setProperty("color", "red");
	
	vertices.clear ();
	pstops.clear ();
	vertices.push_back (topo.newVertex (7.0, 3.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (10.0, 10.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (14.0, 14.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (23.0, 20.0)); pstops.push_back (false);

	Itinerary* iti2 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti2->setProperty("name", "bus2");
	iti2->setProperty("lineNumber", "2");
	iti2->setProperty("color", "blue");
	
	vertices.clear ();
	pstops.clear ();
	vertices.push_back (topo.newVertex (3.0, 6.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (10.0, 10.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (14.0, 14.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (22.0, 24.0)); pstops.push_back (false);

	Itinerary* iti3 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti3->setProperty("name", "bus3");
	iti3->setProperty("lineNumber", "3");
	iti3->setProperty("color", "green");
	
	vertices.clear ();
	pstops.clear ();
	vertices.push_back (topo.newVertex (10.0, 4.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (10.0, 10.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (14.0, 14.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (22.0, 18.0)); pstops.push_back (false);

	Itinerary* iti4 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti4->setProperty("name", "bus4");
	iti4->setProperty("lineNumber", "4");
	iti4->setProperty("color", "magenta");
	
	DrawableBusLine* dbl1 = new DrawableBusLine (iti1);
	DrawableBusLine* dbl2 = new DrawableBusLine (iti2);
	DrawableBusLine* dbl3 = new DrawableBusLine (iti3);
	DrawableBusLine* dbl4 = new DrawableBusLine (iti4);
	
	Map map (&topo, Rectangle (0.0, 0.0, 25.0, 25.0), 2000, 2000);
	map.addBusLine(dbl4);
	map.addBusLine(dbl3);
	map.addBusLine(dbl1);
	map.addBusLine(dbl2);
	
	map.dump();

==================================================================================

*/




void testWithOnlyReversedLine ()
{
	Topography topo;
    int iti_key = 0;

	std::vector<const Vertex*> vertices;
	std::vector<bool> pstops;

	vertices.clear ();
	pstops.clear ();
	vertices.push_back (topo.newVertex (2.0, 2.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (4.0, 4.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (6.0, 4.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (4.0, 2.0)); pstops.push_back (false);
	
	Itinerary* iti1 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti1->setProperty("name", "bus1");
	iti1->setProperty("lineNumber", "1");
	iti1->setProperty("color", "red");

	vertices.clear ();
	pstops.clear ();
	vertices.push_back (topo.newVertex (4.0, 2.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (6.0, 4.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (4.0, 4.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (2.0, 2.0)); pstops.push_back (false);

	Itinerary* iti2 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti2->setProperty("name", "bus2");
	iti2->setProperty("lineNumber", "2");
	iti2->setProperty("color", "blue");

	vertices.clear ();
	pstops.clear ();
	vertices.push_back (topo.newVertex (2.0, 6.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (4.0, 6.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (2.0, 8.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (4.0, 8.0)); pstops.push_back (false);
		
	Itinerary* iti3 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti3->setProperty("name", "bus3");
	iti3->setProperty("lineNumber", "3");
	iti3->setProperty("color", "red");

	vertices.clear ();
	pstops.clear ();
	vertices.push_back (topo.newVertex (4.0, 8.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (2.0, 8.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (4.0, 6.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (2.0, 6.0)); pstops.push_back (false);

	Itinerary* iti4 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti4->setProperty("name", "bus4");
	iti4->setProperty("lineNumber", "4");
	iti4->setProperty("color", "blue");


	vertices.clear ();
	pstops.clear ();
	vertices.push_back (topo.newVertex (10.0, 2.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (12.0, 2.0)); pstops.push_back (false);
		
	Itinerary* iti5 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti5->setProperty("name", "bus5");
	iti5->setProperty("lineNumber", "5");
	iti5->setProperty("color", "red");

	vertices.clear ();
	pstops.clear ();
	vertices.push_back (topo.newVertex (10.0, 2.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (12.0, 2.0)); pstops.push_back (false);

	Itinerary* iti6 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti6->setProperty("name", "bus6");
	iti6->setProperty("lineNumber", "6");
	iti6->setProperty("color", "blue");

	vertices.clear ();
	pstops.clear ();
	vertices.push_back (topo.newVertex (10.0, 6.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (10.0, 8.0)); pstops.push_back (false);
		
	Itinerary* iti7 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti7->setProperty("name", "bus7");
	iti7->setProperty("lineNumber", "7");
	iti7->setProperty("color", "red");

	vertices.clear ();
	pstops.clear ();
	vertices.push_back (topo.newVertex (10.0, 8.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (10.0, 6.0)); pstops.push_back (false);

	Itinerary* iti8 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti8->setProperty("name", "bus8");
	iti8->setProperty("lineNumber", "8");
	iti8->setProperty("color", "blue");



	
	std::ofstream of ("/home/mjambert/map.ps");
	Map map (&topo, of, Rectangle (0.0, 0.0, 16.0, 16.0), 1000, 1000);

	map.dump();
	
	
	
}



void testWithPlentyOfLines () {

	Topography topo;
    int iti_key = 0;
    
	const Vertex* A = topo.newVertex (16.0, 22.0);
	const Vertex* B = topo.newVertex (16.0, 19.0);
	const Vertex* C = topo.newVertex (16.0, 16.0);
	const Vertex* D = topo.newVertex (16.0, 12.0);
	const Vertex* E = topo.newVertex (16.0, 9.0);
	const Vertex* F = topo.newVertex (13.0, 6.0);
	const Vertex* G = topo.newVertex (10.0, 3.0);
	const Vertex* H = topo.newVertex (15.0, 3.0);
	const Vertex* I = topo.newVertex (19.0, 14.0);
	const Vertex* J = topo.newVertex (24.0, 14.0);
	const Vertex* K = topo.newVertex (27.0, 17.0);
	const Vertex* L = topo.newVertex (23.0, 17.0);
	const Vertex* M = topo.newVertex (22.0, 11.0);


	std::vector<const Vertex*> vertices;
	std::vector<bool> pstops;

	vertices.push_back (A); pstops.push_back (false);
	vertices.push_back (B); pstops.push_back (false);
	vertices.push_back (C); pstops.push_back (false);
	vertices.push_back (D); pstops.push_back (false);
	vertices.push_back (E); pstops.push_back (false);
	vertices.push_back (F); pstops.push_back (false);
	vertices.push_back (G); pstops.push_back (false);
	
	Itinerary* iti1 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti1->setProperty("name", "bus1");
	iti1->setProperty("lineNumber", "1");
	iti1->setProperty("color", "red");
	
	
	vertices.clear ();
	pstops.clear ();
	vertices.push_back (A); pstops.push_back (false);
	vertices.push_back (B); pstops.push_back (false);
	vertices.push_back (C); pstops.push_back (false);
	vertices.push_back (D); pstops.push_back (false);
	vertices.push_back (E); pstops.push_back (false);
	vertices.push_back (F); pstops.push_back (false);
	vertices.push_back (H); pstops.push_back (false);

	Itinerary* iti2 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti2->setProperty("name", "bus2");
	iti2->setProperty("lineNumber", "2");
	iti2->setProperty("color", "blue");

	vertices.clear ();
	pstops.clear ();
	vertices.push_back (A); pstops.push_back (false);
	vertices.push_back (B); pstops.push_back (false);
	vertices.push_back (C); pstops.push_back (false);
	vertices.push_back (I); pstops.push_back (false);
	vertices.push_back (J); pstops.push_back (false);
	vertices.push_back (K); pstops.push_back (false);
	vertices.push_back (L); pstops.push_back (false);

	Itinerary* iti3 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti3->setProperty("name", "bus3");
	iti3->setProperty("lineNumber", "3");
	iti3->setProperty("color", "green");
	
	
	vertices.clear ();
	pstops.clear ();
	vertices.push_back (A); pstops.push_back (false);
	vertices.push_back (B); pstops.push_back (false);
	vertices.push_back (C); pstops.push_back (false);
	vertices.push_back (I); pstops.push_back (false);
	vertices.push_back (M); pstops.push_back (false);

	Itinerary* iti4 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti4->setProperty("name", "bus4");
	iti4->setProperty("lineNumber", "4");
	iti4->setProperty("color", "magenta");
	
	vertices.clear ();
	pstops.clear ();
	vertices.push_back (topo.newVertex (13.0, 25.0)); pstops.push_back (false);
	vertices.push_back (A); pstops.push_back (false);
	vertices.push_back (B); pstops.push_back (false);
	vertices.push_back (topo.newVertex (13.0, 18.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (10.0, 16.0)); pstops.push_back (false);

	Itinerary* iti5 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti5->setProperty("name", "bus5");
	iti5->setProperty("lineNumber", "5");
	iti5->setProperty("color", "cyan");

	vertices.clear ();
	pstops.clear ();
	vertices.push_back (A); pstops.push_back (false);
	vertices.push_back (B); pstops.push_back (false);
	vertices.push_back (C); pstops.push_back (false);
	vertices.push_back (D); pstops.push_back (false);
	vertices.push_back (topo.newVertex (13.0, 10.0)); pstops.push_back (false);

	Itinerary* iti6 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti6->setProperty("name", "bus6");
	iti6->setProperty("lineNumber", "6");
	iti6->setProperty("color", "yellow");

	vertices.clear ();
	pstops.clear ();
	vertices.push_back (topo.newVertex (21.0, 26.0)); pstops.push_back (false);
	vertices.push_back (A); pstops.push_back (false);
	vertices.push_back (B); pstops.push_back (false);
	vertices.push_back (C); pstops.push_back (false);
	vertices.push_back (topo.newVertex (21.0, 19.0)); pstops.push_back (false);

	Itinerary* iti7 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti7->setProperty("name", "bus7");
	iti7->setProperty("lineNumber", "7");
	iti7->setProperty("color", "magenta");
	
	// Second set of lines
	// Second set of lines
	// Second set of lines
	// Second set of lines
	
	vertices.clear ();
	pstops.clear ();
	vertices.push_back (topo.newVertex (5.0, 11.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (5.0, 8.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (3.0, 6.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (3.0, 3.0)); pstops.push_back (false);

	Itinerary* iti8 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti8->setProperty("name", "bus8");
	iti8->setProperty("lineNumber", "8");
	iti8->setProperty("color", "red");
	
	vertices.clear ();
	pstops.clear ();
	vertices.push_back (topo.newVertex (7.0, 10.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (7.0, 12.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (7.0, 14.0)); pstops.push_back (false); 
	vertices.push_back (topo.newVertex (14.0, 16.0)); pstops.push_back (false);
	vertices.push_back (C); pstops.push_back (false);
	vertices.push_back (D); pstops.push_back (false);
	vertices.push_back (topo.newVertex (7.0, 12.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (7.0, 10.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (5.0, 8.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (3.0, 6.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (3.0, 3.0)); pstops.push_back (false);

	Itinerary* iti9 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti9->setProperty("name", "bus9");
	iti9->setProperty("lineNumber", "9");
	iti9->setProperty("color", "green");


	vertices.clear ();
	pstops.clear ();
	vertices.push_back (topo.newVertex (21.0, 26.0)); pstops.push_back (false);
	vertices.push_back (A); pstops.push_back (false);
	vertices.push_back (B); pstops.push_back (false);
	vertices.push_back (topo.newVertex (25.0, 26.0)); pstops.push_back (false); 

	Itinerary* iti10 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti10->setProperty("name", "bus10");
	iti10->setProperty("lineNumber", "10");
	iti10->setProperty("color", "yellow");

	vertices.clear ();
	pstops.clear ();
	vertices.push_back (M); pstops.push_back (false);
	vertices.push_back (I); pstops.push_back (false);
	vertices.push_back (J); pstops.push_back (false);
	vertices.push_back (K); pstops.push_back (false);
	vertices.push_back (topo.newVertex (27.0, 25.0)); pstops.push_back (false); 
	vertices.push_back (topo.newVertex (24.0, 25.0)); pstops.push_back (false); 
	
	Itinerary* iti11 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti11->setProperty("name", "bus11");
	iti11->setProperty("lineNumber", "11");
	iti11->setProperty("color", "blue");

	vertices.clear ();
	pstops.clear ();   // Reverse of 8
	vertices.push_back (topo.newVertex (3.0, 3.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (3.0, 6.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (5.0, 8.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (5.0, 11.0)); pstops.push_back (false);

	Itinerary* iti12 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti12->setProperty("name", "bus12");
	iti12->setProperty("lineNumber", "12");
	iti12->setProperty("color", "blue");
	
	vertices.clear ();
	pstops.clear ();

	vertices.push_back (M); pstops.push_back (false);
	vertices.push_back (I); pstops.push_back (false);
	vertices.push_back (J); pstops.push_back (false);
	vertices.push_back (K); pstops.push_back (false);
	vertices.push_back (topo.newVertex (27.0, 25.0)); pstops.push_back (false); 
	vertices.push_back (topo.newVertex (24.0, 25.0)); pstops.push_back (false); 
	
	Itinerary* iti13 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti13->setProperty("name", "bus13");
	iti13->setProperty("lineNumber", "13");
	iti13->setProperty("color", "red");

	vertices.clear ();
	pstops.clear ();
	vertices.push_back (topo.newVertex (10.0, 16.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (13.0, 18.0)); pstops.push_back (false);
	vertices.push_back (B); pstops.push_back (false);
	vertices.push_back (A); pstops.push_back (false);
	vertices.push_back (topo.newVertex (13.0, 25.0)); pstops.push_back (false);



	Itinerary* iti14 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti14->setProperty("name", "bus14");
	iti14->setProperty("lineNumber", "14");  // reverse of the 5
	iti14->setProperty("color", "cyan");
	
	
	vertices.clear ();
	pstops.clear ();
	vertices.push_back (A); pstops.push_back (false);
	vertices.push_back (B); pstops.push_back (false);
	vertices.push_back (C); pstops.push_back (false);
	vertices.push_back (D); pstops.push_back (false);
	vertices.push_back (E); pstops.push_back (false);
	vertices.push_back (F); pstops.push_back (false);
	vertices.push_back (H); pstops.push_back (false);


	Itinerary* iti15 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti15->setProperty("name", "bus15");  // Same than line 2
	iti15->setProperty("lineNumber", "15");
	iti15->setProperty("color", "green");
	
	vertices.clear ();
	pstops.clear ();

	vertices.push_back (topo.newVertex (25.0, 26.0)); pstops.push_back (false); 
	vertices.push_back (B); pstops.push_back (false);
	vertices.push_back (A); pstops.push_back (false);
	vertices.push_back (topo.newVertex (21.0, 26.0)); pstops.push_back (false);

	Itinerary* iti16 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti16->setProperty("name", "bus16");  // reverse 10
	iti16->setProperty("lineNumber", "16");
	iti16->setProperty("color", "black");

	std::ofstream of ("/home/mjambert/map.ps");
	Map map (&topo, of, Rectangle (0.0, 0.0, 35.0, 35.0), 1000, 1000);
	
	map.dump();
}	


void testWithPlentyOfLines2 () {
	Topography topo;
    int iti_key = 0;
	const Vertex* A = topo.newVertex (2.0, 3.0);
	const Vertex* B = topo.newVertex (6.0, 6.0);
	const Vertex* C = topo.newVertex (9.0, 5.0);
	const Vertex* D = topo.newVertex (12.0, 9.0);
	const Vertex* E = topo.newVertex (12.0, 13.0);
	const Vertex* F = topo.newVertex (15.0, 15.0);
	const Vertex* G = topo.newVertex (18.0, 18.0);
	const Vertex* H = topo.newVertex (20.0, 20.0);
	const Vertex* I = topo.newVertex (18.0, 23.0);

	const Vertex* J = topo.newVertex (8.0, 14.0);
	const Vertex* K = topo.newVertex (16.0, 22.0);

	const Vertex* L = topo.newVertex (16.0, 7.0);
	const Vertex* M = topo.newVertex (26.0, 20.0);
	const Vertex* N = topo.newVertex (19.0, 8.0);

	const Vertex* O = topo.newVertex (12.0, 1.0);
	const Vertex* P = topo.newVertex (14.0, 9.0);

	const Vertex* Q = topo.newVertex (29.0, 14.0);

    const Vertex* R = topo.newVertex (10.0, 20.0);
    const Vertex* S = topo.newVertex (20.0, 10.0);


	std::vector<const Vertex*> vertices;
	std::vector<bool> pstops;

	vertices.push_back (A); pstops.push_back (false);
	vertices.push_back (B); pstops.push_back (false);
	vertices.push_back (C); pstops.push_back (false);
	vertices.push_back (D); pstops.push_back (false);
	vertices.push_back (E); pstops.push_back (false);
	vertices.push_back (F); pstops.push_back (false);
	vertices.push_back (G); pstops.push_back (false);
	vertices.push_back (H); pstops.push_back (false);
	vertices.push_back (I); pstops.push_back (false);
	
	Itinerary* iti1 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti1->setProperty("name", "bus1");
	iti1->setProperty("lineNumber", "1");
	iti1->setProperty("color", "red");
	
	vertices.clear ();
	pstops.clear ();
	vertices.push_back (J); pstops.push_back (false);
	vertices.push_back (E); pstops.push_back (false);
	vertices.push_back (F); pstops.push_back (false);
	vertices.push_back (G); pstops.push_back (false);
	vertices.push_back (K); pstops.push_back (false);

	Itinerary* iti2 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti2->setProperty("name", "bus2");
	iti2->setProperty("lineNumber", "2");
	iti2->setProperty("color", "blue");
	
	vertices.clear ();
	pstops.clear ();
	vertices.push_back (M); pstops.push_back (false);
	vertices.push_back (H); pstops.push_back (false);
	vertices.push_back (G); pstops.push_back (false);
	vertices.push_back (F); pstops.push_back (false);
	vertices.push_back (L); pstops.push_back (false);

	Itinerary* iti3 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti3->setProperty("name", "bus3");
	iti3->setProperty("lineNumber", "3");
	iti3->setProperty("color", "cyan");
	
	vertices.clear ();
	pstops.clear ();
	vertices.push_back (H); pstops.push_back (false);
	vertices.push_back (G); pstops.push_back (false);
	vertices.push_back (F); pstops.push_back (false);
	vertices.push_back (N); pstops.push_back (false);

	Itinerary* iti4 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti4->setProperty("name", "bus4");
	iti4->setProperty("lineNumber", "4");
	iti4->setProperty("color", "yellow");
	
	vertices.clear ();
	pstops.clear ();
	vertices.push_back (F); pstops.push_back (false);
	vertices.push_back (G); pstops.push_back (false);
	vertices.push_back (H); pstops.push_back (false);

	Itinerary* iti5 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti5->setProperty("name", "bus5");
	iti5->setProperty("lineNumber", "5");
	iti5->setProperty("color", "green");
	
	vertices.clear ();
	pstops.clear ();
	vertices.push_back (K); pstops.push_back (false);
	vertices.push_back (G); pstops.push_back (false);
	vertices.push_back (F); pstops.push_back (false);
	vertices.push_back (E); pstops.push_back (false);
	vertices.push_back (J); pstops.push_back (false);

	Itinerary* iti6 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti6->setProperty("name", "bus6");
	iti6->setProperty("lineNumber", "6");
	iti6->setProperty("color", "magenta");
	
	vertices.clear ();
	pstops.clear ();

	vertices.push_back (L); pstops.push_back (false);
	vertices.push_back (F); pstops.push_back (false);
	vertices.push_back (G); pstops.push_back (false);
	vertices.push_back (H); pstops.push_back (false);
	vertices.push_back (M); pstops.push_back (false);

	Itinerary* iti7 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti7->setProperty("name", "bus7");
	iti7->setProperty("lineNumber", "7");
	iti7->setProperty("color", "blue");

	vertices.clear ();
	pstops.clear ();
	vertices.push_back (O); pstops.push_back (false);
	vertices.push_back (C); pstops.push_back (false);
	vertices.push_back (D); pstops.push_back (false);
	vertices.push_back (P); pstops.push_back (false);

	Itinerary* iti8 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti8->setProperty("name", "bus8");
	iti8->setProperty("lineNumber", "8");
	iti8->setProperty("color", "green");
	
	vertices.clear ();
	pstops.clear ();
	vertices.push_back (P); pstops.push_back (false);
	vertices.push_back (D); pstops.push_back (false);
	vertices.push_back (C); pstops.push_back (false);
	vertices.push_back (O); pstops.push_back (false);

	Itinerary* iti9 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti9->setProperty("name", "bus9");
	iti9->setProperty("lineNumber", "9");
	iti9->setProperty("color", "yellow");
	
	vertices.clear ();
	pstops.clear ();
	vertices.push_back (H); pstops.push_back (false);
	vertices.push_back (G); pstops.push_back (false);
	vertices.push_back (F); pstops.push_back (false);
	vertices.push_back (Q); pstops.push_back (false);

	Itinerary* iti10 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti10->setProperty("name", "bus10");
	iti10->setProperty("lineNumber", "10");
	iti10->setProperty("color", "red");
	
    vertices.clear ();
    pstops.clear ();
    vertices.push_back (R); pstops.push_back (false);
    vertices.push_back (F); pstops.push_back (false);
    vertices.push_back (S); pstops.push_back (false);

    // A crossing line
 /*   Itinerary* iti11 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
    iti11->setProperty("name", "bus11");
    iti11->setProperty("lineNumber", "11");
    iti11->setProperty("color", "blue");
   */ 
	// Do some testing...
	DrawableBusLine* dbl1 = new DrawableBusLine (iti1);
	DrawableBusLine* dbl2 = new DrawableBusLine (iti2);
	DrawableBusLine* dbl3 = new DrawableBusLine (iti3);
	DrawableBusLine* dbl4 = new DrawableBusLine (iti4);
	DrawableBusLine* dbl5 = new DrawableBusLine (iti5);
	DrawableBusLine* dbl6 = new DrawableBusLine (iti6);
	
	
	{
		DBLComparator cmp (dbl1, F, F);
		assert (cmp (dbl2, dbl1) == 0); // Following 1, 2 is on the left of 1
		assert (cmp (dbl1, dbl2) == 1); // Following 1, 1 is on the right of 2
	}
	{
		DBLComparator cmp (dbl2, F, F);
		assert (cmp (dbl2, dbl1) == 0); // Following 2, 2 is on the left of 1
		assert (cmp (dbl1, dbl2) == 1); // Following 2, 1 is on the right of 2
	}
	{
		DBLComparator cmp (dbl3, H, H);
		assert (cmp (dbl3, dbl4) == 1); // Following 3, 3 is on the right of 4
		assert (cmp (dbl4, dbl3) == 0); // Following 3, 4 is on the left of 3
	}
	{
		DBLComparator cmp (dbl4, H, H);
		assert (cmp (dbl3, dbl4) == 1); // Following 4, 3 is on the right of 4
		assert (cmp (dbl4, dbl3) == 0); // Following 4, 4 is on the left of 3
	}
	{
		DBLComparator cmp (dbl1, H, H);
		assert (cmp (dbl3, dbl4) == 0); // Following 1, 3 is on the left of 4
		assert (cmp (dbl4, dbl3) == 1); // Following 1, 4 is on the right of 3
	}
	{
		DBLComparator cmp (dbl1, F, F);
		assert (cmp (dbl1, dbl5) == 0); // Following 1, 1 is on the left of 5
		assert (cmp (dbl5, dbl1) == 1); // Following 1, 5 is on the right of 1
	}
	{
		DBLComparator cmp (dbl1, F, F);
		assert (cmp (dbl3, dbl5) == 1); // Following 1, 3 is on the right of 5
		assert (cmp (dbl5, dbl3) == 0); // Following 1, 5 is on the left of 1
	}
	{
		DBLComparator cmp (dbl1, F, F);
		assert (cmp (dbl1, dbl6) == 1); // Following 1, 1 is on the right of 6
		assert (cmp (dbl6, dbl1) == 0); // Following 1, 6 is on the left of 1
	}
	{
		DBLComparator cmp (dbl5, F, F);
		assert (cmp (dbl3, dbl4) == 0); // Following 5, 3 is on the left of 4
		assert (cmp (dbl4, dbl3) == 1); // Following 5, 4 is on the right of 3
	}
	{
		DBLComparator cmp (dbl6, F, F);
		assert (cmp (dbl6, dbl5) == 1); // Following 6, 6 is on the right of 5
		assert (cmp (dbl5, dbl6) == 0); // Following 6, 5 is on the left of 6
	}
	
		
	
	// Finally dump a map
	std::ofstream of ("/home/mjambert/map2.ps");
	Map map (&topo, of, Rectangle (0.0, 0.0, 35.0, 35.0), 1000, 1000);
	
	map.dump();
	
}


void
testL24 () {
    Topography topo;
    int iti_key = 0;
    
    std::vector<const Vertex*> vertices;
    std::vector<bool> pstops;
    
    vertices.push_back (topo.newVertex (526908, 1845660)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (527162, 1845815)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (527460, 1845910)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (527815, 1845779)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (527961, 1845458)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (527957, 1845141)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (527952, 1844573)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (527965, 1844363)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (528187, 1844067)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (528464, 1844058)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (528510, 1843786)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (528483, 1843587)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (528407, 1843437)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (528454, 1843127)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (528554, 1842971)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (528688, 1842824)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (528657, 1842632)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (528717, 1842429)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (528891, 1842236)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (529106, 1842338)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (529180, 1842199)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (529090, 1842085)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (529029, 1841831)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (529160, 1841554)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (529332, 1841397)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (529527, 1840980)); pstops.push_back (false);
    
    Itinerary* iti1 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
    iti1->setProperty("name", "bus24");
    iti1->setProperty("lineNumber", "24");
    iti1->setProperty("color", "red");
    vertices.clear (); pstops.clear ();
    vertices.push_back (topo.newVertex (524832, 1846719)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (525024, 1846471)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (525213,  1846217)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (525377,  1846002)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (525467,  1845874)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (525944,  1845731)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (526252,  1845569)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (526772,  1845419)); pstops.push_back (false);

    vertices.push_back (topo.newVertex (526908, 1845660)); pstops.push_back (false); //
//    vertices.push_back (topo.newVertex (526902,  1845654)); pstops.push_back (false); //
    vertices.push_back (topo.newVertex (527162,  1845815)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (527460,  1845910)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (527815, 1845779)); pstops.push_back (false); //
//    vertices.push_back (topo.newVertex (527881,  1845708)); pstops.push_back (false); //
    vertices.push_back (topo.newVertex (527961,  1845458)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (527957,  1845141)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (528036,  1844685)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (528283,  1844689)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (528729,  1844525)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (528867 , 1844633)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (529011,  1844806)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (529487,  1844676)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (529734,  1844685)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (530028,  1844694)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (530263,  1844703)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (530571,  1844596)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (530866,  1844304)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (531038,  1844249)); pstops.push_back (false);
    
    Itinerary* iti3 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
    iti3->setProperty("name", "bus16");
    iti3->setProperty("lineNumber", "16");
    iti3->setProperty("color", "cyan");
    



/*    
    vertices.clear (); pstops.clear ();
    vertices.push_back (topo.newVertex (526582,  1849455)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (526692,  1849016)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (526765,  1848699)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (526806,  1848525)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (526863,  1848290)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (526929,  1848003)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (527041,  1847510)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (527072,  1847236)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (527184,  1846912)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (527244,  1846658)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (527305,  1846383)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (527460,  1845910)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (527881,  1845708)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (527961,  1845458)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (527957,  1845141)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (528036,  1844685)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (528283,  1844689)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (528463,  1844388)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (528464,  1844058)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (528510,  1843786)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (528607,  1843559)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (528862,  1843404)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (529049,  1843285)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (529274,  1843179)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (529355,  1843033)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (529402,  1842843)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (529458,  1842632)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (529611,  1842412)); pstops.push_back (false);
    vertices.push_back (topo.newVertex (529730,  1842250)); pstops.push_back (false);
    
    Itinerary* iti2 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
    iti2->setProperty("name", "bus10");
    iti2->setProperty("lineNumber", "10");
    iti2->setProperty("color", "green");
   */ 
    
    
    // Finally dump a map
    // for (int i=0; i<100; ++i) 
    {
        MapBackgroundManager mbgm ("/home/mjambert/rcs/data/processed_map_data/TILES");
        std::ofstream of ("/home/mjambert/l24.ps");
        Map map (&topo, of, Rectangle (526908-500, 1840980-500, 6000, 6000), 800, 800, &mbgm);
        
        map.dump();
    }

}    
    
    
//synmap::MapBackground mbg ("/home/mjambert/rcs/data/processed_map_data/TILES/scale_0.121037924152_0.12103792408"); // 25k 0.8
//synmap::MapBackground mbg ("/home/mjambert/rcs/data/processed_map_data/TILES/scale_0.15129740519_0.15129740519");  // 25k 1.0
//synmap::MapBackground mbg ("/home/mjambert/scratch/25k/tiles");

    
    



int main(int argc, char **argv) {

	/* testVertex ();
	testEdge ();
	testItinerary ();
	*/

//	 testWithOnlyReversedLine ();
	 testWithPlentyOfLines ();
	 testWithPlentyOfLines2 ();
     testL24 ();
    
    /*
    Topography topo;

    std::ofstream of ("/home/mjambert/testbg.ps");
    Map map (&topo, of, Rectangle (509994, 1856994, 2004, 2004*4), 303, 303*4);
    
    map.dump();
    */
    
//    tile_303_303_509994_1856994_511998_1858998
/*
    synmap::MapBackground mbg ("/home/mjambert/rcs/data/map_data/RASTER2/25k/tiles");
    mbg.dumpTileGrid ();
*/
    std::cout << "Done." << std::endl;

	return 0;
	


}



































	
	/*
	Vertex vertex (1.0, 1.0);
	vertex.getX ();
	
	for (int i=0; i<1000; ++i) {
		for (int j=0; j<100; ++j) {
			topo.newVertex(i*1.1, j*1.2);
		}
	}
	cout << "1" << endl;
	for (int k=0; k<10; ++k) {
		
	for (int i=0; i<1000; ++i) {
		for (int j=0; j<100; ++j) {
			topo.newVertex(i*1.1, j*1.2);
		}
	}
	cout << "." << endl;
		
	}
	cout << "2" << endl;
		
	*/
/*	std::vector<Region> regions;
	std::vector<Street> streets;
	loadRegions ("/home/mjambert/allsurfacesa.mif", regions);
	// loadRegions ("/home/mjambert/allsurfaces.mif", regions);
	//loadPlines ("/home/mjambert/alllimits.mif", regions);
	loadPlines ("/home/mjambert/allstreets.mif", streets);
	loadLines ("/home/mjambert/allstreets.mif", streets);
	
	double mapW = mapXmax - mapXmin;
	double mapH = mapYmax - mapYmin;

	cout << "MapX=" << mapXmin << " ; MapY=" << mapYmin	<< " ; MapW=" << mapW << " ; MapH=" << mapH << endl;
	
//	Map map (Frame (mapXmin + mapW/4.0, mapYmin + mapH/4.0, mapW / 16.0, mapH / 16.0), 1000, 1000);
//	Map map (Frame (mapXmin + mapW/4.0, mapYmin + mapH/4.0, mapW / 8.0, mapH / 8.0), 1000, 1000);
	Map map (Rectangle (mapXmin, mapYmin, mapW, mapH), 2000, 2000);
	for (unsigned int i=0; i<regions.size (); ++i) {
		map.addRegion (regions[i]);
	}
	for (unsigned int i=0; i<streets.size (); ++i) {
		map.addStreet (streets[i]);
	}
	
	map.dump();
	
	*/
	
	
	
	/*
	std::ofstream of ("/home/mjambert/regions.ps");
	PostscriptCanvas s (of);
	s.startPage (0, 0, 2000, 2000);
	s.setScale(10.0, 10.0);
	s.setTranslate(-521289.0,-1836014.0);
	// s.setLineWidth (6);
	
	cout << "Drawing " << endl;
	for (unsigned int i=0; i<regions.size (); ++i) {
		// drawPoints (regions[i], s);
	}
	
	s.showPage();
		*/

	
/*



	std::vector< std::vector<XYPoint> > patterns;

	patterns.push_back (pattern1());
	patterns.push_back (pattern2());
	patterns.push_back (pattern3());
	patterns.push_back (pattern4());
	patterns.push_back (pattern5());
	patterns.push_back (pattern6()); 
	patterns.push_back (pattern7());  
	patterns.push_back (pattern8());

	for (unsigned int i=0; i<patterns.size (); ++i) {
		ostringstream oss;
		oss << "/home/mjambert/pattern" << (i+1) << ".ps";
		
		// testPattern (oss.str(), patterns[i]);

		ostringstream ross;
		ross << "/home/mjambert/pattern" << (i+1) << "R.ps";
		
		// Reverse pattern
		std::vector<XYPoint> rpoints;
		for (int j=patterns[i].size()-1; j >= 0; --j) {
			rpoints.push_back (patterns[i][j]);	
		}
	
		// testPattern (ross.str(), rpoints);

		
	}
*/



