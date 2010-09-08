/*
 * GeomUtil.h
 *
 *  Created on: Jan 27, 2010
 *      Author: tbonfort
 */

#ifndef GEOMUTIL_H_
#define GEOMUTIL_H_

#include "OSMElements.h"
#include <list>
#include <vector>
#include <geos/geom/Polygon.h>

namespace synthese {
namespace osm {


class GeomUtil {
public:
   /**
    * \brief compute the set of polygons formed by the given list of lines
    * \returns a vector containing extracted polygons, or empty if no closed rings could be found
    */
   static std::vector<geos::geom::Polygon*>* polygonize(std::list<WayPtr> &ways);

   /*
   static std::list<WayPtr> mergeConnected(std::list<WayPtr> &ways);
   static bool connectable(WayPtr &way1, WayPtr &way2);

   //append the nodes of "from" to "to", removing duplicate start/end node
   static void connect(WayPtr &from, WayPtr &to);
*/

};


}}

#endif /* GEOMUTIL_H_ */
