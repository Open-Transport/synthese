
//////////////////////////////////////////////////////////////////////////////////////////
///	InterSYNTHESEPackagesService class implementation.
///	@file InterSYNTHESEPackagesService.cpp
///	@author hromain
///	@date 2013
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

#include "InterSYNTHESEPackagesService.hpp"

#include "InterSYNTHESEPackage.hpp"
#include "RequestException.h"
#include "Request.h"

using namespace std;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,inter_synthese::InterSYNTHESEPackagesService>::FACTORY_KEY = "inter_synthese_packages";
	
	namespace inter_synthese
	{
		const string InterSYNTHESEPackagesService::PARAMETER_TABLE_FILTER = "table_filter";
		const string InterSYNTHESEPackagesService::PARAMETER_OBJECT_FILTER = "object_filter";

		const string InterSYNTHESEPackagesService::TAG_PACKAGES = "packages";
		const string InterSYNTHESEPackagesService::TAG_PACKAGE = "package";
		


		ParametersMap InterSYNTHESEPackagesService::_getParametersMap() const
		{
			ParametersMap map;
			if(!_outputFormat.empty())
			{
				map.insert(PARAMETER_OUTPUT_FORMAT, _outputFormat);
			}
			return map;
		}



		void InterSYNTHESEPackagesService::_setFromParametersMap(const ParametersMap& map)
		{
			// Table filter
			RegistryTableType tableFilter(map.getDefault<RegistryTableType>(PARAMETER_TABLE_FILTER, 0));
			if(tableFilter)
			{
				_tableFilter = tableFilter;
			}

			// Object filter
			RegistryKeyType objectFilter(map.getDefault<RegistryKeyType>(PARAMETER_OBJECT_FILTER, 0));
			if(objectFilter)
			{
				_objectFilter = objectFilter;
			}

			setOutputFormatFromMap(map, string());
		}



		ParametersMap InterSYNTHESEPackagesService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap map;
			
			// Loop on packages
			BOOST_FOREACH(const InterSYNTHESEPackage::Registry::value_type& it, Env::GetOfficialEnv().getRegistry<InterSYNTHESEPackage>())
			{
				// Variable
				const InterSYNTHESEPackage& package(*it.second);

				// Jump over table filtered packages
				if(_tableFilter || _objectFilter)
				{
					bool ok(false);
					BOOST_FOREACH(const TableOrObject& item, package.get<Objects>())
					{
						if(	(_objectFilter && item.getKey() == *_objectFilter) ||
							(_tableFilter && (decodeTableId(item.getKey()) == *_tableFilter || item.getKey() == *_tableFilter))
						){
							ok = true;
							break;
						}
					}
					if(!ok)
					{
						continue;
					}
				}

				// Export
				boost::shared_ptr<ParametersMap> packagePM(new ParametersMap);
				package.toParametersMap(*packagePM, true);
				map.insert(TAG_PACKAGE, packagePM);
			}

			// Output
			if(_outputFormat == MimeTypes::XML)
			{
				map.outputXML(stream, TAG_PACKAGES, true);
			}
			else if(_outputFormat == MimeTypes::JSON)
			{
				map.outputJSON(stream, string());
			}

			return map;
		}
		
		
		
		bool InterSYNTHESEPackagesService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string InterSYNTHESEPackagesService::getOutputMimeType() const
		{
			return _outputFormat.empty() ? "text/plain" : _outputFormat;
		}
}	}
