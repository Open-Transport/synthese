
/** XmlBuilder class header.
	@file 35_pt/XmlBuilder.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/


#ifndef SYNTHESE_ENV_XMLBUILDER_H
#define SYNTHESE_ENV_XMLBUILDER_H


#include "City.h"
#include "CommercialLine.h"
#include "JourneyPattern.hpp"
#include "DesignatedLinePhysicalStop.hpp"
#include "Registry.h"
#include "StopPoint.hpp"
#include "StopArea.hpp"


struct XMLNode;


namespace synthese
{




namespace pt
{


/** Environment XML loading/saving service class.

@warning ordering of child elements is important!

Sample XML format :

@code
<environment id="0">

 <cities>
   <city id="1" name="Toulouse"/>
   <city id="2" name="Colomiers"/>
   <!-- ... -->
 </cities>

 <connectionPlaces>
   <connectionPlace id="1" name="Matabiau" cityId="1"/>
   <!-- ... -->
 </connectionPlaces>

 <physicalStops>
   <physicalStop id="1" name="Metro gare SNCF"
                 connectionPlaceId="1" rankInConnectionPlace="1"
	         x="1334.5" y="123.2"/>
   <!-- ... -->
 </physicalStops>

 <lines>
   <line id="l1" firstYear="2006" lastYear="2007">

     <lineStop id="1" metricOffset="0"
	       type="departure" physicalStopId="1" scheduleInput="true">
       <point x="120.5" y="4444.2"/>
       <point x="130.5" y="4434.4"/>
       <point x="140.2" y="4414.2"/>
     </lineStop>

     <lineStop id="2" metricOffset="100"
	       type="passage" physicalStopId="2" scheduleInput="true"/>

   </line>

   <line id="l2" axisId="a2" firstYear="2006" lastYear="2007">

   </line>

   <!-- ... -->
 </lines>


</environment>
@endcode

 @ingroup m39
*/
class XmlBuilder
{
 public:


 private:

    XmlBuilder ();
    ~XmlBuilder();


 public:


	 static boost::shared_ptr<geography::City> CreateCity (XMLNode& node);


	 static boost::shared_ptr<StopArea> CreateConnectionPlace(
		 XMLNode& node,
		 const util::Registry<geography::City>& cities);

	 static boost::shared_ptr<CommercialLine> CreateCommercialLine (XMLNode& node);

	 static boost::shared_ptr<JourneyPattern> CreateLine (XMLNode& node,
		 const util::Registry<CommercialLine>& commercialLines);

	 static boost::shared_ptr<DesignatedLinePhysicalStop> CreateLineStop (XMLNode& node,
		 util::Registry<JourneyPattern>& lines,
		 const util::Registry<pt::StopPoint>& physicalStops);

	 static boost::shared_ptr<pt::StopPoint> CreatePhysicalStop (XMLNode& node,
		 const util::Registry<StopArea>& connectionPlaces
	);

	 static geos::geom::Coordinate CreatePoint (XMLNode& node);


};



}
}


#endif

