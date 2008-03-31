
/** XmlBuilder class header.
	@file XmlBuilder.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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


#include "Axis.h"
#include "City.h"
#include "CommercialLine.h"
#include "Line.h"
#include "LineStop.h"
#include "15_env/PhysicalStop.h"
#include "15_env/PublicTransportStopZoneConnectionPlace.h"


struct XMLNode;


namespace synthese
{




namespace env
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

 <axes>
   <axis id="a1" free="true" authorized="false">  
   <axis id="a2" free="true" authorized="false">  
   <!-- ... -->
 </axes>

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
   <line id="l1" axisId="a1" firstYear="2006" lastYear="2007">

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


	 static boost::shared_ptr<Axis> CreateAxis (XMLNode& node);

	 static boost::shared_ptr<City> CreateCity (XMLNode& node);

    
	 static boost::shared_ptr<PublicTransportStopZoneConnectionPlace> CreateConnectionPlace (XMLNode& node, 
						   const City::Registry& cities);

	 static boost::shared_ptr<CommercialLine> CreateCommercialLine (XMLNode& node);

	 static boost::shared_ptr<Line> CreateLine (XMLNode& node, 
			     const Axis::Registry& axes,
			     const CommercialLine::Registry& commercialLines);

	 static boost::shared_ptr<LineStop> CreateLineStop (XMLNode& node, 
				     Line::Registry& lines,
				     const PhysicalStop::Registry& physicalStops);

	 static boost::shared_ptr<PhysicalStop> CreatePhysicalStop (XMLNode& node, 
					     const PublicTransportStopZoneConnectionPlace::Registry& connectionPlaces);
    
	 static geometry::Point2D CreatePoint (XMLNode& node);


};



}
}


#endif

