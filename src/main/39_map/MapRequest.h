
/** MapRequest class header.
	@file MapRequest.h

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

#ifndef SYNTHESE_MapRequest_H__
#define SYNTHESE_MapRequest_H__

#include "30_server/Function.h"

#include "15_env/Axis.h"
#include "15_env/City.h"
#include "15_env/CommercialLine.h"
#include "15_env/Line.h"
#include "15_env/LineStop.h"
#include "15_env/ConnectionPlace.h"
#include "15_env/PhysicalStop.h"


namespace synthese
{
	namespace map
	{
	    class Map;

	    /** MapRequest class.
		@ingroup m39
	    */
	    class MapRequest : public server::Function
	    {
		
	    private:
		std::string _output;

		// @todo use shared_ptr in registry so that memory is not a pb anymore 
		// and that we can copy (and complete static module regitries here)
		synthese::env::City::Registry _cities;
		synthese::env::Axis::Registry _axes;
		synthese::env::ConnectionPlace::Registry _connectionPlaces;
		synthese::env::PhysicalStop::Registry _physicalStops;
		synthese::env::CommercialLine::Registry _commercialLines;
		synthese::env::Line::Registry _lines;
		synthese::env::LineStop::Registry _lineStops;

		Map* _map;

		    
		/** Conversion from attributes to generic parameter maps.
		 */
		server::ParametersMap _getParametersMap() const;
		
		/** Conversion from generic parameters map to attributes.
		 */
		void _setFromParametersMap(const server::ParametersMap& map);

	    public:
			
		static const std::string OUTPUT_PARAMETER;
		static const std::string DATA_PARAMETER;
		static const std::string MAP_PARAMETER;

		MapRequest();
		~MapRequest();
		
		/// @todo Getters/Setters for parsed parameters
		
		/** Action to run, defined by each subclass.
		 */
		void _run(std::ostream& stream) const;
	    };
	}
}
#endif // SYNTHESE_MapRequest_H__
