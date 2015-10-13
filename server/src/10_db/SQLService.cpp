
//////////////////////////////////////////////////////////////////////////////////////////
///	SQLService class implementation.
///	@file SQLService.cpp
///	@author Hugues Romain
///	@date 2012
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

#include "SQLService.hpp"

#include "DBModule.h"
#include "DBResult.hpp"
#include "RequestException.h"
#include "Request.h"
#include "Session.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,db::SQLService>::FACTORY_KEY = "SQLService";
	
	namespace db
	{
		const string SQLService::PARAMETER_QUERY = "query";
		const string SQLService::TAG_RECORD = "record";
		


		ParametersMap SQLService::_getParametersMap() const
		{
			ParametersMap map;
			if(!_query.empty())
			{
				map.insert(PARAMETER_QUERY, _query);
			}
			return map;
		}



		void SQLService::_setFromParametersMap(const ParametersMap& map)
		{
			_query = map.getDefault<string>(PARAMETER_QUERY);
		}



		ParametersMap SQLService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap pm;

			DBResultSPtr rows = DBModule::GetDB()->execQuery(_query);

			while(rows->next ())
			{
				boost::shared_ptr<ParametersMap> rowPM(new ParametersMap);

				Record::FieldNames fieldNames(rows->getFieldNames());
				BOOST_FOREACH(const string& fieldName, fieldNames)
				{
					rowPM->insert(fieldName, rows->getText(fieldName));
				}

				pm.insert(TAG_RECORD, rowPM);
			}

			return pm;
		}
		
		
		std::string SQLService::getOutputMimeType() const
		{
			return "text/html";
		}
}	}
