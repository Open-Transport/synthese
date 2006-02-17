
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
  const Vertex* v0 = topo.newLocation (1.5, 2.5);
  const Vertex* v1 = topo.newLocation (1.5, 2.5);
  const Vertex* v2 = topo.newLocation (1.5, 3.5);
	
  assert (v0->getX () == 1.5);
  assert (v0->getY () == 2.5);
	
  assert (v0 = v1);
  assert (v0 != v2);
  }


  void
  testEdge () {
  Topography topo;
  const Vertex* v0 = topo.newLocation (1.5, 2.5);
  const Vertex* v1 = topo.newLocation (1.5, 2.5);
  const Vertex* v2 = topo.newLocation (1.5, 4.5);

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
  std::vector<const Location*> locations;
	
  locations.push_back (topo.newLocation (1.5, 2.5));
  locations.push_back (topo.newLocation (2.5, 3.5));
  locations.push_back (topo.newLocation (3.5, 4.5));
  locations.push_back (topo.newLocation (3.5, 5.5));
  locations.push_back (topo.newLocation (3.5, 6.5));

  std::vector<bool> pstops;
  
  
  
  
  
	
  const Itinerary* iti0 = topo.newItinerary(iti_key++, locations);
	
  assert (iti0->getNbPhysicalStops () == 3);
  assert (iti0->getLocations ().size () == 5);
  assert (iti0->getEdges ().size () == 4);

  {	
  std::vector<const PhysicalStop*> stops;
  topo.findPhysicalStops(topo.newLocation (2.5, 3.5), stops);
  assert (stops.size () == 1);	
  }
  {	
  std::vector<const PhysicalStop*> stops;
  topo.findPhysicalStops(topo.newLocation (1.5, 2.5), stops);
  assert (stops.size () == 0);	
  }
  {	
  std::vector<const Itinerary*> itis;
  topo.findItineraries(topo.newLocation (1.2, 2.5), itis);
  assert (itis.size () == 0);	
  }
  {	
  std::vector<const Itinerary*> itis;
  topo.findItineraries(topo.newLocation (1.5, 2.5), itis);
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
 std::vector<const Location*> locations;
 std::vector<bool> pstops;
 locations.push_back (topo.newLocation (3.0, 3.0)); 
 locations.push_back (topo.newLocation (10.0, 10.0)); 
 locations.push_back (topo.newLocation (14.0, 14.0)); 
 locations.push_back (topo.newLocation (23.0, 23.0)); 
	
 Itinerary* iti1 = (Itinerary*) topo.newItinerary(iti_key++, locations);
 iti1->setProperty("name", "bus1");
 iti1->setProperty("lineNumber", "1");
 iti1->setProperty("color", "red");
	
 locations.clear ();
 pstops.clear ();
 locations.push_back (topo.newLocation (7.0, 3.0)); 
 locations.push_back (topo.newLocation (10.0, 10.0)); 
 locations.push_back (topo.newLocation (14.0, 14.0)); 
 locations.push_back (topo.newLocation (23.0, 20.0)); 

 Itinerary* iti2 = (Itinerary*) topo.newItinerary(iti_key++, locations);
 iti2->setProperty("name", "bus2");
 iti2->setProperty("lineNumber", "2");
 iti2->setProperty("color", "blue");
	
 locations.clear ();
 pstops.clear ();
 locations.push_back (topo.newLocation (3.0, 6.0)); 
 locations.push_back (topo.newLocation (10.0, 10.0)); 
 locations.push_back (topo.newLocation (14.0, 14.0)); 
 locations.push_back (topo.newLocation (22.0, 24.0)); 

 Itinerary* iti3 = (Itinerary*) topo.newItinerary(iti_key++, locations);
 iti3->setProperty("name", "bus3");
 iti3->setProperty("lineNumber", "3");
 iti3->setProperty("color", "green");
	
 locations.clear ();
 pstops.clear ();
 locations.push_back (topo.newLocation (10.0, 4.0)); 
 locations.push_back (topo.newLocation (10.0, 10.0)); 
 locations.push_back (topo.newLocation (14.0, 14.0)); 
 locations.push_back (topo.newLocation (22.0, 18.0)); 

 Itinerary* iti4 = (Itinerary*) topo.newItinerary(iti_key++, locations);
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


/*

void testWithOnlyReversedLine ()
{
  Topography topo;
  int iti_key = 0;
  int loc_key = 0;

  std::vector<const Location*> locations;
  std::vector<bool> pstops;

  locations.clear ();
  pstops.clear ();
  locations.push_back (topo.newLocation (loc_key++, 2.0, 2.0)); 
  locations.push_back (topo.newLocation (loc_key++, 4.0, 4.0)); 
  locations.push_back (topo.newLocation (loc_key++, 6.0, 4.0)); 
  locations.push_back (topo.newLocation (loc_key++, 4.0, 2.0)); 
	
  Itinerary* iti1 = (Itinerary*) topo.newItinerary(iti_key++, locations);
  iti1->setProperty("name", "bus1");
  iti1->setProperty("lineNumber", "1");
  iti1->setProperty("color", "red");

  locations.clear ();
  pstops.clear ();
  locations.push_back (topo.newLocation (loc_key++, 4.0, 2.0)); 
  locations.push_back (topo.newLocation (loc_key++, 6.0, 4.0)); 
  locations.push_back (topo.newLocation (loc_key++, 4.0, 4.0)); 
  locations.push_back (topo.newLocation (loc_key++, 2.0, 2.0)); 

  Itinerary* iti2 = (Itinerary*) topo.newItinerary(iti_key++, locations);
  iti2->setProperty("name", "bus2");
  iti2->setProperty("lineNumber", "2");
  iti2->setProperty("color", "blue");

  locations.clear ();
  pstops.clear ();
  locations.push_back (topo.newLocation (loc_key++,2.0, 6.0)); 
  locations.push_back (topo.newLocation (loc_key++,4.0, 6.0)); 
  locations.push_back (topo.newLocation (loc_key++,2.0, 8.0)); 
  locations.push_back (topo.newLocation (loc_key++,4.0, 8.0)); 
		
  Itinerary* iti3 = (Itinerary*) topo.newItinerary(iti_key++, locations);
  iti3->setProperty("name", "bus3");
  iti3->setProperty("lineNumber", "3");
  iti3->setProperty("color", "red");

  locations.clear ();
  pstops.clear ();
  locations.push_back (topo.newLocation (loc_key++,4.0, 8.0)); 
  locations.push_back (topo.newLocation (loc_key++,2.0, 8.0)); 
  locations.push_back (topo.newLocation (loc_key++,4.0, 6.0)); 
  locations.push_back (topo.newLocation (loc_key++,2.0, 6.0)); 

  Itinerary* iti4 = (Itinerary*) topo.newItinerary(iti_key++, locations);
  iti4->setProperty("name", "bus4");
  iti4->setProperty("lineNumber", "4");
  iti4->setProperty("color", "blue");


  locations.clear ();
  pstops.clear ();
  locations.push_back (topo.newLocation (loc_key++,10.0, 2.0)); 
  locations.push_back (topo.newLocation (loc_key++,12.0, 2.0)); 
		
  Itinerary* iti5 = (Itinerary*) topo.newItinerary(iti_key++, locations);
  iti5->setProperty("name", "bus5");
  iti5->setProperty("lineNumber", "5");
  iti5->setProperty("color", "red");

  locations.clear ();
  pstops.clear ();
  locations.push_back (topo.newLocation (loc_key++,10.0, 2.0)); 
  locations.push_back (topo.newLocation (loc_key++,12.0, 2.0)); 

  Itinerary* iti6 = (Itinerary*) topo.newItinerary(iti_key++, locations);
  iti6->setProperty("name", "bus6");
  iti6->setProperty("lineNumber", "6");
  iti6->setProperty("color", "blue");

  locations.clear ();
  pstops.clear ();
  locations.push_back (topo.newLocation (loc_key++,10.0, 6.0)); 
  locations.push_back (topo.newLocation (loc_key++,10.0, 8.0)); 
		
  Itinerary* iti7 = (Itinerary*) topo.newItinerary(iti_key++, locations);
  iti7->setProperty("name", "bus7");
  iti7->setProperty("lineNumber", "7");
  iti7->setProperty("color", "red");

  locations.clear ();
  pstops.clear ();
  locations.push_back (topo.newLocation (loc_key++,10.0, 8.0)); 
  locations.push_back (topo.newLocation (loc_key++,10.0, 6.0)); 

  Itinerary* iti8 = (Itinerary*) topo.newItinerary(iti_key++, locations);
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
  int loc_key = 0;
    
  const Location* A = topo.newLocation (loc_key++,16.0, 22.0);
  const Location* B = topo.newLocation (loc_key++,16.0, 19.0);
  const Location* C = topo.newLocation (loc_key++,16.0, 16.0);
  const Location* D = topo.newLocation (loc_key++,16.0, 12.0);
  const Location* E = topo.newLocation (loc_key++,16.0, 9.0);
  const Location* F = topo.newLocation (loc_key++,13.0, 6.0);
  const Location* G = topo.newLocation (loc_key++,10.0, 3.0);
  const Location* H = topo.newLocation (loc_key++,15.0, 3.0);
  const Location* I = topo.newLocation (loc_key++,19.0, 14.0);
  const Location* J = topo.newLocation (loc_key++,24.0, 14.0);
  const Location* K = topo.newLocation (loc_key++,27.0, 17.0);
  const Location* L = topo.newLocation (loc_key++,23.0, 17.0);
  const Location* M = topo.newLocation (loc_key++,22.0, 11.0);


  std::vector<const Location*> locations;
  std::vector<bool> pstops;

  locations.push_back (A); 
  locations.push_back (B); 
  locations.push_back (C); 
  locations.push_back (D); 
  locations.push_back (E); 
  locations.push_back (F); 
  locations.push_back (G); 
	
  Itinerary* iti1 = (Itinerary*) topo.newItinerary(iti_key++, locations);
  iti1->setProperty("name", "bus1");
  iti1->setProperty("lineNumber", "1");
  iti1->setProperty("color", "red");
	
	
  locations.clear ();
  pstops.clear ();
  locations.push_back (A); 
  locations.push_back (B); 
  locations.push_back (C); 
  locations.push_back (D); 
  locations.push_back (E); 
  locations.push_back (F); 
  locations.push_back (H); 

  Itinerary* iti2 = (Itinerary*) topo.newItinerary(iti_key++, locations);
  iti2->setProperty("name", "bus2");
  iti2->setProperty("lineNumber", "2");
  iti2->setProperty("color", "blue");

  locations.clear ();
  pstops.clear ();
  locations.push_back (A); 
  locations.push_back (B); 
  locations.push_back (C); 
  locations.push_back (I); 
  locations.push_back (J); 
  locations.push_back (K); 
  locations.push_back (L); 

  Itinerary* iti3 = (Itinerary*) topo.newItinerary(iti_key++, locations);
  iti3->setProperty("name", "bus3");
  iti3->setProperty("lineNumber", "3");
  iti3->setProperty("color", "green");
	
	
  locations.clear ();
  pstops.clear ();
  locations.push_back (A); 
  locations.push_back (B); 
  locations.push_back (C); 
  locations.push_back (I); 
  locations.push_back (M); 

  Itinerary* iti4 = (Itinerary*) topo.newItinerary(iti_key++, locations);
  iti4->setProperty("name", "bus4");
  iti4->setProperty("lineNumber", "4");
  iti4->setProperty("color", "magenta");
	
  locations.clear ();
  pstops.clear ();
  locations.push_back (topo.newLocation (loc_key++,13.0, 25.0)); 
  locations.push_back (A); 
  locations.push_back (B); 
  locations.push_back (topo.newLocation (loc_key++,13.0, 18.0)); 
  locations.push_back (topo.newLocation (loc_key++,10.0, 16.0)); 

  Itinerary* iti5 = (Itinerary*) topo.newItinerary(iti_key++, locations);
  iti5->setProperty("name", "bus5");
  iti5->setProperty("lineNumber", "5");
  iti5->setProperty("color", "cyan");

  locations.clear ();
  pstops.clear ();
  locations.push_back (A); 
  locations.push_back (B); 
  locations.push_back (C); 
  locations.push_back (D); 
  locations.push_back (topo.newLocation (loc_key++,13.0, 10.0)); 

  Itinerary* iti6 = (Itinerary*) topo.newItinerary(iti_key++, locations);
  iti6->setProperty("name", "bus6");
  iti6->setProperty("lineNumber", "6");
  iti6->setProperty("color", "yellow");

  locations.clear ();
  pstops.clear ();
  locations.push_back (topo.newLocation (loc_key++,21.0, 26.0)); 
  locations.push_back (A); 
  locations.push_back (B); 
  locations.push_back (C); 
  locations.push_back (topo.newLocation (loc_key++,21.0, 19.0)); 

  Itinerary* iti7 = (Itinerary*) topo.newItinerary(iti_key++, locations);
  iti7->setProperty("name", "bus7");
  iti7->setProperty("lineNumber", "7");
  iti7->setProperty("color", "magenta");
	
  // Second set of lines
  // Second set of lines
  // Second set of lines
  // Second set of lines
	
  locations.clear ();
  pstops.clear ();
  locations.push_back (topo.newLocation (loc_key++,5.0, 11.0)); 
  locations.push_back (topo.newLocation (loc_key++,5.0, 8.0)); 
  locations.push_back (topo.newLocation (loc_key++,3.0, 6.0)); 
  locations.push_back (topo.newLocation (loc_key++,3.0, 3.0)); 

  Itinerary* iti8 = (Itinerary*) topo.newItinerary(iti_key++, locations);
  iti8->setProperty("name", "bus8");
  iti8->setProperty("lineNumber", "8");
  iti8->setProperty("color", "red");
	
  locations.clear ();
  pstops.clear ();
  locations.push_back (topo.newLocation (loc_key++,7.0, 10.0)); 
  locations.push_back (topo.newLocation (loc_key++,7.0, 12.0)); 
  locations.push_back (topo.newLocation (loc_key++,7.0, 14.0));  
  locations.push_back (topo.newLocation (loc_key++,14.0, 16.0)); 
  locations.push_back (C); 
  locations.push_back (D); 
  locations.push_back (topo.newLocation (loc_key++,7.0, 12.0)); 
  locations.push_back (topo.newLocation (loc_key++,7.0, 10.0)); 
  locations.push_back (topo.newLocation (loc_key++,5.0, 8.0)); 
  locations.push_back (topo.newLocation (loc_key++,3.0, 6.0)); 
  locations.push_back (topo.newLocation (loc_key++,3.0, 3.0)); 

  Itinerary* iti9 = (Itinerary*) topo.newItinerary(iti_key++, locations);
  iti9->setProperty("name", "bus9");
  iti9->setProperty("lineNumber", "9");
  iti9->setProperty("color", "green");


  locations.clear ();
  pstops.clear ();
  locations.push_back (topo.newLocation (loc_key++,21.0, 26.0)); 
  locations.push_back (A); 
  locations.push_back (B); 
  locations.push_back (topo.newLocation (loc_key++,25.0, 26.0));  

  Itinerary* iti10 = (Itinerary*) topo.newItinerary(iti_key++, locations);
  iti10->setProperty("name", "bus10");
  iti10->setProperty("lineNumber", "10");
  iti10->setProperty("color", "yellow");

  locations.clear ();
  pstops.clear ();
  locations.push_back (M); 
  locations.push_back (I); 
  locations.push_back (J); 
  locations.push_back (K); 
  locations.push_back (topo.newLocation (loc_key++,27.0, 25.0));  
  locations.push_back (topo.newLocation (loc_key++,24.0, 25.0));  
	
  Itinerary* iti11 = (Itinerary*) topo.newItinerary(iti_key++, locations);
  iti11->setProperty("name", "bus11");
  iti11->setProperty("lineNumber", "11");
  iti11->setProperty("color", "blue");

  locations.clear ();
  pstops.clear ();   // Reverse of 8
  locations.push_back (topo.newLocation (loc_key++,3.0, 3.0)); 
  locations.push_back (topo.newLocation (loc_key++,3.0, 6.0)); 
  locations.push_back (topo.newLocation (loc_key++,5.0, 8.0)); 
  locations.push_back (topo.newLocation (loc_key++,5.0, 11.0)); 

  Itinerary* iti12 = (Itinerary*) topo.newItinerary(iti_key++, locations);
  iti12->setProperty("name", "bus12");
  iti12->setProperty("lineNumber", "12");
  iti12->setProperty("color", "blue");
	
  locations.clear ();
  pstops.clear ();

  locations.push_back (M); 
  locations.push_back (I); 
  locations.push_back (J); 
  locations.push_back (K); 
  locations.push_back (topo.newLocation (loc_key++,27.0, 25.0));  
  locations.push_back (topo.newLocation (loc_key++,24.0, 25.0));  
	
  Itinerary* iti13 = (Itinerary*) topo.newItinerary(iti_key++, locations);
  iti13->setProperty("name", "bus13");
  iti13->setProperty("lineNumber", "13");
  iti13->setProperty("color", "red");

  locations.clear ();
  pstops.clear ();
  locations.push_back (topo.newLocation (loc_key++,10.0, 16.0)); 
  locations.push_back (topo.newLocation (loc_key++,13.0, 18.0)); 
  locations.push_back (B); 
  locations.push_back (A); 
  locations.push_back (topo.newLocation (loc_key++,13.0, 25.0)); 



  Itinerary* iti14 = (Itinerary*) topo.newItinerary(iti_key++, locations);
  iti14->setProperty("name", "bus14");
  iti14->setProperty("lineNumber", "14");  // reverse of the 5
  iti14->setProperty("color", "cyan");
	
	
  locations.clear ();
  pstops.clear ();
  locations.push_back (A); 
  locations.push_back (B); 
  locations.push_back (C); 
  locations.push_back (D); 
  locations.push_back (E); 
  locations.push_back (F); 
  locations.push_back (H); 


  Itinerary* iti15 = (Itinerary*) topo.newItinerary(iti_key++, locations);
  iti15->setProperty("name", "bus15");  // Same than line 2
  iti15->setProperty("lineNumber", "15");
  iti15->setProperty("color", "green");
	
  locations.clear ();
  pstops.clear ();

  locations.push_back (topo.newLocation (loc_key++,25.0, 26.0));  
  locations.push_back (B); 
  locations.push_back (A); 
  locations.push_back (topo.newLocation (loc_key++,21.0, 26.0)); 

  Itinerary* iti16 = (Itinerary*) topo.newItinerary(iti_key++, locations);
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
  int loc_key = 0;

  const Location* A = topo.newLocation (loc_key++,2.0, 3.0);
  const Location* B = topo.newLocation (loc_key++,6.0, 6.0);
  const Location* C = topo.newLocation (loc_key++,9.0, 5.0);
  const Location* D = topo.newLocation (loc_key++,12.0, 9.0);
  const Location* E = topo.newLocation (loc_key++,12.0, 13.0);
  const Location* F = topo.newLocation (loc_key++,15.0, 15.0);
  const Location* G = topo.newLocation (loc_key++,18.0, 18.0);
  const Location* H = topo.newLocation (loc_key++,20.0, 20.0);
  const Location* I = topo.newLocation (loc_key++,18.0, 23.0);

  const Location* J = topo.newLocation (loc_key++,8.0, 14.0);
  const Location* K = topo.newLocation (loc_key++,16.0, 22.0);

  const Location* L = topo.newLocation (loc_key++,16.0, 7.0);
  const Location* M = topo.newLocation (loc_key++,26.0, 20.0);
  const Location* N = topo.newLocation (loc_key++,19.0, 8.0);

  const Location* O = topo.newLocation (loc_key++,12.0, 1.0);
  const Location* P = topo.newLocation (loc_key++,14.0, 9.0);

  const Location* Q = topo.newLocation (loc_key++,29.0, 14.0);

  const Location* R = topo.newLocation (loc_key++,10.0, 20.0);
  const Location* S = topo.newLocation (loc_key++,20.0, 10.0);


  std::vector<const Location*> locations;
  std::vector<bool> pstops;

  locations.push_back (A); 
  locations.push_back (B); 
  locations.push_back (C); 
  locations.push_back (D); 
  locations.push_back (E); 
  locations.push_back (F); 
  locations.push_back (G); 
  locations.push_back (H); 
  locations.push_back (I); 
	
  Itinerary* iti1 = (Itinerary*) topo.newItinerary(iti_key++, locations);
  iti1->setProperty("name", "bus1");
  iti1->setProperty("lineNumber", "1");
  iti1->setProperty("color", "red");
	
  locations.clear ();
  pstops.clear ();
  locations.push_back (J); 
  locations.push_back (E); 
  locations.push_back (F); 
  locations.push_back (G); 
  locations.push_back (K); 

  Itinerary* iti2 = (Itinerary*) topo.newItinerary(iti_key++, locations);
  iti2->setProperty("name", "bus2");
  iti2->setProperty("lineNumber", "2");
  iti2->setProperty("color", "blue");
	
  locations.clear ();
  pstops.clear ();
  locations.push_back (M); 
  locations.push_back (H); 
  locations.push_back (G); 
  locations.push_back (F); 
  locations.push_back (L); 

  Itinerary* iti3 = (Itinerary*) topo.newItinerary(iti_key++, locations);
  iti3->setProperty("name", "bus3");
  iti3->setProperty("lineNumber", "3");
  iti3->setProperty("color", "cyan");
	
  locations.clear ();
  pstops.clear ();
  locations.push_back (H); 
  locations.push_back (G); 
  locations.push_back (F); 
  locations.push_back (N); 

  Itinerary* iti4 = (Itinerary*) topo.newItinerary(iti_key++, locations);
  iti4->setProperty("name", "bus4");
  iti4->setProperty("lineNumber", "4");
  iti4->setProperty("color", "yellow");
	
  locations.clear ();
  pstops.clear ();
  locations.push_back (F); 
  locations.push_back (G); 
  locations.push_back (H); 

  Itinerary* iti5 = (Itinerary*) topo.newItinerary(iti_key++, locations);
  iti5->setProperty("name", "bus5");
  iti5->setProperty("lineNumber", "5");
  iti5->setProperty("color", "green");
	
  locations.clear ();
  pstops.clear ();
  locations.push_back (K); 
  locations.push_back (G); 
  locations.push_back (F); 
  locations.push_back (E); 
  locations.push_back (J); 

  Itinerary* iti6 = (Itinerary*) topo.newItinerary(iti_key++, locations);
  iti6->setProperty("name", "bus6");
  iti6->setProperty("lineNumber", "6");
  iti6->setProperty("color", "magenta");
	
  locations.clear ();
  pstops.clear ();

  locations.push_back (L); 
  locations.push_back (F); 
  locations.push_back (G); 
  locations.push_back (H); 
  locations.push_back (M); 

  Itinerary* iti7 = (Itinerary*) topo.newItinerary(iti_key++, locations);
  iti7->setProperty("name", "bus7");
  iti7->setProperty("lineNumber", "7");
  iti7->setProperty("color", "blue");

  locations.clear ();
  pstops.clear ();
  locations.push_back (O); 
  locations.push_back (C); 
  locations.push_back (D); 
  locations.push_back (P); 

  Itinerary* iti8 = (Itinerary*) topo.newItinerary(iti_key++, locations);
  iti8->setProperty("name", "bus8");
  iti8->setProperty("lineNumber", "8");
  iti8->setProperty("color", "green");
	
  locations.clear ();
  pstops.clear ();
  locations.push_back (P); 
  locations.push_back (D); 
  locations.push_back (C); 
  locations.push_back (O); 

  Itinerary* iti9 = (Itinerary*) topo.newItinerary(iti_key++, locations);
  iti9->setProperty("name", "bus9");
  iti9->setProperty("lineNumber", "9");
  iti9->setProperty("color", "yellow");
	
  locations.clear ();
  pstops.clear ();
  locations.push_back (H); 
  locations.push_back (G); 
  locations.push_back (F); 
  locations.push_back (Q); 

  Itinerary* iti10 = (Itinerary*) topo.newItinerary(iti_key++, locations);
  iti10->setProperty("name", "bus10");
  iti10->setProperty("lineNumber", "10");
  iti10->setProperty("color", "red");
	
  locations.clear ();
  pstops.clear ();
  locations.push_back (R); 
  locations.push_back (F); 
  locations.push_back (S); 

  // Do some testing...
  DrawableBusLine* dbl1 = new DrawableBusLine (iti1);
  DrawableBusLine* dbl2 = new DrawableBusLine (iti2);
  DrawableBusLine* dbl3 = new DrawableBusLine (iti3);
  DrawableBusLine* dbl4 = new DrawableBusLine (iti4);
  DrawableBusLine* dbl5 = new DrawableBusLine (iti5);
  DrawableBusLine* dbl6 = new DrawableBusLine (iti6);
	
	
	
  // Finally dump a map
  std::ofstream of ("/home/mjambert/map2.ps");
  Map map (&topo, of, Rectangle (0.0, 0.0, 35.0, 35.0), 1000, 1000);
	
  map.dump();
	
}


void
testL24 () {
  Topography topo;
  int iti_key = 0;
  int loc_key = 0;

    
  std::vector<const Location*> locations;
  std::vector<bool> pstops;
    
  locations.push_back (topo.newLocation (loc_key++,526908, 1845660)); 
  locations.push_back (topo.newLocation (loc_key++,527162, 1845815)); 
  locations.push_back (topo.newLocation (loc_key++,527460, 1845910)); 
  locations.push_back (topo.newLocation (loc_key++,527815, 1845779)); 
  locations.push_back (topo.newLocation (loc_key++,527961, 1845458)); 
  locations.push_back (topo.newLocation (loc_key++,527957, 1845141)); 
  locations.push_back (topo.newLocation (loc_key++,527952, 1844573)); 
  locations.push_back (topo.newLocation (loc_key++,527965, 1844363)); 
  locations.push_back (topo.newLocation (loc_key++,528187, 1844067)); 
  locations.push_back (topo.newLocation (loc_key++,528464, 1844058)); 
  locations.push_back (topo.newLocation (loc_key++,528510, 1843786)); 
  locations.push_back (topo.newLocation (loc_key++,528483, 1843587)); 
  locations.push_back (topo.newLocation (loc_key++,528407, 1843437)); 
  locations.push_back (topo.newLocation (loc_key++,528454, 1843127)); 
  locations.push_back (topo.newLocation (loc_key++,528554, 1842971)); 
  locations.push_back (topo.newLocation (loc_key++,528688, 1842824)); 
  locations.push_back (topo.newLocation (loc_key++,528657, 1842632)); 
  locations.push_back (topo.newLocation (loc_key++,528717, 1842429)); 
  locations.push_back (topo.newLocation (loc_key++,528891, 1842236)); 
  locations.push_back (topo.newLocation (loc_key++,529106, 1842338)); 
  locations.push_back (topo.newLocation (loc_key++,529180, 1842199)); 
  locations.push_back (topo.newLocation (loc_key++,529090, 1842085)); 
  locations.push_back (topo.newLocation (loc_key++,529029, 1841831)); 
  locations.push_back (topo.newLocation (loc_key++,529160, 1841554)); 
  locations.push_back (topo.newLocation (loc_key++,529332, 1841397)); 
  locations.push_back (topo.newLocation (loc_key++,529527, 1840980)); 
    
  Itinerary* iti1 = (Itinerary*) topo.newItinerary(iti_key++, locations);
  iti1->setProperty("name", "bus24");
  iti1->setProperty("lineNumber", "24");
  iti1->setProperty("color", "red");
  locations.clear (); pstops.clear ();
  locations.push_back (topo.newLocation (loc_key++,524832, 1846719)); 
  locations.push_back (topo.newLocation (loc_key++,525024, 1846471)); 
  locations.push_back (topo.newLocation (loc_key++,525213,  1846217)); 
  locations.push_back (topo.newLocation (loc_key++,525377,  1846002)); 
  locations.push_back (topo.newLocation (loc_key++,525467,  1845874)); 
  locations.push_back (topo.newLocation (loc_key++,525944,  1845731)); 
  locations.push_back (topo.newLocation (loc_key++,526252,  1845569)); 
  locations.push_back (topo.newLocation (loc_key++,526772,  1845419)); 

  locations.push_back (topo.newLocation (loc_key++,526908, 1845660));  //
  //    locations.push_back (topo.newLocation (loc_key++,526902,  1845654));  //
  locations.push_back (topo.newLocation (loc_key++,527162,  1845815)); 
  locations.push_back (topo.newLocation (loc_key++,527460,  1845910)); 
  locations.push_back (topo.newLocation (loc_key++,527815, 1845779));  //
  //    locations.push_back (topo.newLocation (loc_key++,527881,  1845708));  //
  locations.push_back (topo.newLocation (loc_key++,527961,  1845458)); 
  locations.push_back (topo.newLocation (loc_key++,527957,  1845141)); 
  locations.push_back (topo.newLocation (loc_key++,528036,  1844685)); 
  locations.push_back (topo.newLocation (loc_key++,528283,  1844689)); 
  locations.push_back (topo.newLocation (loc_key++,528729,  1844525)); 
  locations.push_back (topo.newLocation (loc_key++,528867 , 1844633)); 
  locations.push_back (topo.newLocation (loc_key++,529011,  1844806)); 
  locations.push_back (topo.newLocation (loc_key++,529487,  1844676)); 
  locations.push_back (topo.newLocation (loc_key++,529734,  1844685)); 
  locations.push_back (topo.newLocation (loc_key++,530028,  1844694)); 
  locations.push_back (topo.newLocation (loc_key++,530263,  1844703)); 
  locations.push_back (topo.newLocation (loc_key++,530571,  1844596)); 
  locations.push_back (topo.newLocation (loc_key++,530866,  1844304)); 
  locations.push_back (topo.newLocation (loc_key++,531038,  1844249)); 
    
  Itinerary* iti3 = (Itinerary*) topo.newItinerary(iti_key++, locations);
  iti3->setProperty("name", "bus16");
  iti3->setProperty("lineNumber", "16");
  iti3->setProperty("color", "cyan");
    

	locations.clear (); pstops.clear ();
	locations.push_back (topo.newLocation (loc_key++,526582,  1849455)); 
	locations.push_back (topo.newLocation (loc_key++,526692,  1849016)); 
	locations.push_back (topo.newLocation (loc_key++,526765,  1848699)); 
	locations.push_back (topo.newLocation (loc_key++,526806,  1848525)); 
	locations.push_back (topo.newLocation (loc_key++,526863,  1848290)); 
	locations.push_back (topo.newLocation (loc_key++,526929,  1848003)); 
	locations.push_back (topo.newLocation (loc_key++,527041,  1847510)); 
	locations.push_back (topo.newLocation (loc_key++,527072,  1847236)); 
	locations.push_back (topo.newLocation (loc_key++,527184,  1846912)); 
	locations.push_back (topo.newLocation (loc_key++,527244,  1846658)); 
	locations.push_back (topo.newLocation (loc_key++,527305,  1846383)); 
	locations.push_back (topo.newLocation (loc_key++,527460,  1845910)); 
	locations.push_back (topo.newLocation (loc_key++,527881,  1845708)); 
	locations.push_back (topo.newLocation (loc_key++,527961,  1845458)); 
	locations.push_back (topo.newLocation (loc_key++,527957,  1845141)); 
	locations.push_back (topo.newLocation (loc_key++,528036,  1844685)); 
	locations.push_back (topo.newLocation (loc_key++,528283,  1844689)); 
	locations.push_back (topo.newLocation (loc_key++,528463,  1844388)); 
	locations.push_back (topo.newLocation (loc_key++,528464,  1844058)); 
	locations.push_back (topo.newLocation (loc_key++,528510,  1843786)); 
	locations.push_back (topo.newLocation (loc_key++,528607,  1843559)); 
	locations.push_back (topo.newLocation (loc_key++,528862,  1843404)); 
	locations.push_back (topo.newLocation (loc_key++,529049,  1843285)); 
	locations.push_back (topo.newLocation (loc_key++,529274,  1843179)); 
	locations.push_back (topo.newLocation (loc_key++,529355,  1843033)); 
	locations.push_back (topo.newLocation (loc_key++,529402,  1842843)); 
	locations.push_back (topo.newLocation (loc_key++,529458,  1842632)); 
	locations.push_back (topo.newLocation (loc_key++,529611,  1842412)); 
	locations.push_back (topo.newLocation (loc_key++,529730,  1842250)); 
    
	Itinerary* iti2 = (Itinerary*) topo.newItinerary(iti_key++, locations);
	iti2->setProperty("name", "bus10");
	iti2->setProperty("lineNumber", "10");
	iti2->setProperty("color", "green");
  */ 
    /*   
    
  // Finally dump a map
  // for (int i=0; i<100; ++i) 
  {
    MapBackgroundManager mbgm ("/home/mjambert/rcs/data/processed_map_data/TILES");
    std::ofstream of ("/home/mjambert/l24.ps");
    Map map (&topo, of, Rectangle (526908-500, 1840980-500, 6000, 6000), 800, 800, &mbgm);
        
    map.dump();
  }

}    
    */
    
//synmap::MapBackground mbg ("/home/mjambert/rcs/data/processed_map_data/TILES/scale_0.121037924152_0.12103792408"); // 25k 0.8
//synmap::MapBackground mbg ("/home/mjambert/rcs/data/processed_map_data/TILES/scale_0.15129740519_0.15129740519");  // 25k 1.0
//synmap::MapBackground mbg ("/home/mjambert/scratch/25k/tiles");

    
    



int main(int argc, char **argv) {

  /* testVertex ();
     testEdge ();
     testItinerary ();
  */

  //	 testWithOnlyReversedLine ();
  //  testWithPlentyOfLines ();
  //testWithPlentyOfLines2 ();
  // testL24 ();
    
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
  topo.newLocation(i*1.1, j*1.2);
  }
  }
  cout << "1" << endl;
  for (int k=0; k<10; ++k) {
		
  for (int i=0; i<1000; ++i) {
  for (int j=0; j<100; ++j) {
  topo.newLocation(i*1.1, j*1.2);
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



