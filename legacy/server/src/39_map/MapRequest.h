
/** MapRequest class header.
	@file MapRequest.h

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

#ifndef SYNTHESE_MapRequest_H__
#define SYNTHESE_MapRequest_H__

#include "FactorableTemplate.h"

#include "Function.h"

#include "City.h"
#include "CommercialLine.h"
#include "JourneyPattern.hpp"
#include "LineStop.h"
#include "StopArea.hpp"
#include "StopPoint.hpp"

#include "Env.h"

namespace synthese
{
	namespace map
	{
		class Map;

		/** MapRequest class.
			@ingroup m39Functions refFunctions
		*/
		class MapRequest:
			public util::FactorableTemplate<server::Function, MapRequest>
		{

		private:
			//! @name Query
			//@{
				std::string _output;
				std::string _query;
			//@}

			//! @name Temporary environment
			//@{
				std::string _data;
				bool		_useEnvironment;
				util::Env	_temporaryEnvironment;
			//@}

			//! @name Result
			//@{
				std::auto_ptr<Map> _map;
			//@}


			/** Conversion from attributes to generic parameter maps.
			 */
			util::ParametersMap _getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
			 */
			void _setFromParametersMap(const util::ParametersMap& map);

		public:

			static const std::string OUTPUT_PARAMETER;
			static const std::string DATA_PARAMETER;
			static const std::string MAP_PARAMETER;
			static const std::string PARAMETER_USE_ENVIRONMENT;

			MapRequest();
			MapRequest(const MapRequest& value);

			//! @name Setters
			//@{


				/** Sets and parse the data to be used for drawing the map.
					The call of this setter is useless if _useEnvironment is set to true / default.
					@param value the data provided in XML format
					@author Hugues Romain
					@date 2008
				*/
				void setData(const std::string& value);


				/** Sets and parse the map XML query.
					@warning the setUseEnvironment and/or the setData method must be launched before setQuery.
					@param value the XML Query
					@author Hugues Romain
					@date 2008
				*/
				void setQuery(const std::string& value);
				void setOutput(const std::string& value);
				void setUseEnvironment(bool value);
			//@}



			/** Action to run, defined by each subclass.
			 */
			virtual util::ParametersMap run(
				std::ostream& stream,
				const server::Request& request
			) const;



			virtual bool isAuthorized(const server::Session* session) const;

			virtual std::string getOutputMimeType() const;
		};
}	}

#endif // SYNTHESE_MapRequest_H__
