
//////////////////////////////////////////////////////////////////////////////////////////
///	AlertListService class implementation.
///	@file AlertListService.cpp
///	@author mjambert
///	@date 2014
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "AlertListService.hpp"


#include "AlertTableSync.hpp"
#include "Request.h"
#include "RequestException.h"
#include "DBModule.h"

/*#include "ObjectBase.hpp"
#include "RequestException.h"
#include "Request.h"
*/

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,regulation::AlertListService>::FACTORY_KEY = "alert_list";
	
	namespace regulation
	{
		const string AlertListService::PARAMETER_ALERT_TYPE = "alert_type";
		const string AlertListService::PARAMETER_ADDITIONAL_PARAMETERS = "additional_parameters";
		

		ParametersMap AlertListService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void AlertListService::_setFromParametersMap(const ParametersMap& map)
		{
            _alertType = optional<AlertType>();
			try
			{
				_alertType = (AlertType) map.get<int>(PARAMETER_ALERT_TYPE);
			}
			catch(Record::MissingParameterException& mpe)
			{
			}
            
			_additionalParameters = map.getDefault<bool>(PARAMETER_ADDITIONAL_PARAMETERS, false);
            Function::setOutputFormatFromMap(map, string());
		}



		ParametersMap AlertListService::run(
			std::ostream& stream,
			const Request& request
		) const {

			ParametersMap alertsPM;
			AlertTableSync::SearchResult alerts(
				AlertTableSync::Search(
					Env::GetOfficialEnv(),
                    _alertType
                    ));
            
			BOOST_FOREACH(const boost::shared_ptr<Alert>& alert, alerts)
            {
                boost::shared_ptr<ParametersMap> alertPM(new ParametersMap);
                alert->toParametersMap(*alertPM);
                //alertPM->insert("kind", alert->get<Kind>());
                alertsPM.insert("alert", alertPM);                
            }

			if (_outputFormat == MimeTypes::JSON)
			{
                alertsPM.outputJSON(stream, "alerts");
            }
			
			return alertsPM;
		}
		
		
		
		bool AlertListService::isAuthorized(
			const Session* session
		) const {
			return true;
		}


		std::string AlertListService::getOutputMimeType() const
		{
			return getOutputMimeTypeFromOutputFormat();
		}



		AlertListService::AlertListService():
			_additionalParameters(false)
		{}
}	}
