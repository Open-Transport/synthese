
/** VinciContractPrintRequest class implementation.
	@file VinciContractPrintRequest.cpp

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

#include "01_util/Conversion.h"

#include "02_db/DBEmptyResultException.h"

#include "11_interfaces/InterfacePage.h"
#include "11_interfaces/Interface.h"

#include "30_server/RequestException.h"
#include "30_server/Request.h"

#include "VinciContract.h"
#include "VinciContractPrintRequest.h"
#include "VinciPrintedContractInterfacePage.h"
#include "VinciContractTableSync.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace db;
	using namespace interfaces;

	namespace vinci
	{
		const std::string VinciContractPrintRequest::PARAMETER_CONTRACT_ID = "ctr";
		
		VinciContractPrintRequest::VinciContractPrintRequest()
			: RequestWithInterfaceAndRequiredSession()
			, _contract(NULL)
		{}

		ParametersMap VinciContractPrintRequest::_getParametersMap() const
		{
			ParametersMap map(RequestWithInterfaceAndRequiredSession::_getParametersMap());

			map.insert(make_pair(PARAMETER_CONTRACT_ID, Conversion::ToString(_contract->getKey())));
			
			return map;
		}

		void VinciContractPrintRequest::_setFromParametersMap(const ParametersMap& map)
		{
			RequestWithInterfaceAndRequiredSession::_setFromParametersMap(map);

			try
			{
				ParametersMap::const_iterator it;

				it = map.find(PARAMETER_CONTRACT_ID);
				if (it == map.end())
					throw RequestException("Contract not specified");

				_contract = VinciContractTableSync::get(Conversion::ToLongLong(it->second));
			}
			catch (DBEmptyResultException e)
			{
				throw RequestException("Specified contract not found");
			}
		}

		void VinciContractPrintRequest::_run( std::ostream& stream ) const
		{
			const VinciPrintedContractInterfacePage* page = _interface->getPage<VinciPrintedContractInterfacePage>();
			page->display(stream, VariablesMap(), _contract, _request);
		}

		VinciContractPrintRequest::~VinciContractPrintRequest()
		{
			delete _contract;
		}

		void VinciContractPrintRequest::setContract(const VinciContract* contract )
		{
			_contract = contract;
		}
	}
}
