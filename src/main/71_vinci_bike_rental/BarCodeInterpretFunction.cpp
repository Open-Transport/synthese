
/** BarCodeInterpretFunction class implementation.
	@file BarCodeInterpretFunction.cpp

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

#include "71_vinci_bike_rental/BarCodeInterpretFunction.h"
#include "71_vinci_bike_rental/VinciCustomerAdminInterfaceElement.h"
#include "71_vinci_bike_rental/VinciCustomerSearchAdminInterfaceElement.h"
#include "71_vinci_bike_rental/VinciBikeTableSync.h"
#include "71_vinci_bike_rental/VinciContractTableSync.h"
#include "71_vinci_bike_rental/ReturnABikeAction.h"
#include "71_vinci_bike_rental/VinciBikeAdminInterfaceElement.h"
#include "71_vinci_bike_rental/VinciBike.h"

#include "01_util/Conversion.h"

#include "02_db/DBEmptyResultException.h"

#include "11_interfaces/RedirectInterfacePage.h"
#include "11_interfaces/Interface.h"

#include "30_server/RequestException.h"
#include "30_server/ActionFunctionRequest.h"

#include "32_admin/AdminRequest.h"

#include "57_accounting/TransactionPart.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace admin;
	using namespace accounts;
	using namespace interfaces;
	using namespace db;

	namespace vinci
	{
		const string BarCodeInterpretFunction::PARAMETER_READED_CODE = "rco";
		const string BarCodeInterpretFunction::PARAMETER_LAST_PAGE = "lp";
		const string BarCodeInterpretFunction::PARAMETER_LAST_OBJECT_ID = "loi";
		
		ParametersMap BarCodeInterpretFunction::_getParametersMap() const
		{
			ParametersMap map(RequestWithInterfaceAndRequiredSession::_getParametersMap());

			map.insert(make_pair(PARAMETER_LAST_PAGE, _lastPage));
			if (_lastId)
				map.insert(make_pair(PARAMETER_LAST_OBJECT_ID, Conversion::ToString(_lastId)));
			return map;
		}

		void BarCodeInterpretFunction::_setFromParametersMap(const ParametersMap& map)
		{
			RequestWithInterfaceAndRequiredSession::_setFromParametersMap(map);

			ParametersMap::const_iterator it;

			// Read code
			it = map.find(PARAMETER_READED_CODE);
			if (it == map.end())
				throw RequestException("Readed code not found");
			uid uidCode = Conversion::ToLongLong(it->second);
			_strCode = it->second;

			// Last page
			it = map.find(PARAMETER_LAST_PAGE);
			if (it != map.end())
				_lastPage = it->second;

			// Last object
			it = map.find(PARAMETER_LAST_OBJECT_ID);
			if (it != map.end())
				_lastId = Conversion::ToLongLong(it->second);

			// Interface control
			try
			{
				getInterface()->getPage<RedirectInterfacePage>();
			}
			catch(...)
			{
				throw RequestException("Interface redirect page not implemented");
			}

			_tableId = decodeTableId(uidCode);


			if (_tableId == VinciBikeTableSync::TABLE_ID)
			{
				try
				{
					_bike = VinciBikeTableSync::get(uidCode);
				}
				catch (DBEmptyResultException<VinciBike>)
				{
					throw RequestException("Specified bike not found");
				}
			}
			else if (_tableId == VinciContractTableSync::TABLE_ID)
			{
				try
				{
					_contract = VinciContractTableSync::get(uidCode);
				}
				catch (DBEmptyResultException<VinciContract>)
				{
					throw RequestException("Specified contract not found");
				}
			}
		}

		void BarCodeInterpretFunction::_run( std::ostream& stream ) const
		{
			if (_tableId == VinciBikeTableSync::TABLE_ID)
			{
				shared_ptr<VinciContract> contract = VinciBikeTableSync::getRentContract(_bike);
				if (contract.get())
				{
					if (_lastPage == Factory<AdminInterfaceElement>::getKey<VinciCustomerAdminInterfaceElement>()
						&& _lastId == contract->getKey()
					){
						shared_ptr<TransactionPart> transactionPart = VinciBikeTableSync::getRentTransactionPart(_bike);

						// Action : return the bike(s)
						// Function : display the contract
						ActionFunctionRequest<ReturnABikeAction, AdminRequest> redirRequest(_request);
						redirRequest.getAction()->setTransactionPart(transactionPart);
						redirRequest.getFunction()->setPage<VinciCustomerAdminInterfaceElement>();
						redirRequest.setObjectId(contract->getKey());
						redirRequest.getFunction()->getInterface()->getPage<RedirectInterfacePage>()->display(stream, VariablesMap(), &redirRequest);
						return;
					}
					else
					{
						// Function : display the contract
						FunctionRequest<AdminRequest> redirRequest(_request);
						redirRequest.getFunction()->setPage<VinciCustomerAdminInterfaceElement>();
						redirRequest.setObjectId(contract->getKey());
						redirRequest.getFunction()->getInterface()->getPage<RedirectInterfacePage>()->display(stream, VariablesMap(), &redirRequest);
						return;
					}
				}
				else
				{
					if (_lastPage == Factory<AdminInterfaceElement>::getKey<VinciCustomerAdminInterfaceElement>())
					{
						// Action : rent the bike (what about the lock ? do not implement this part now)
						// Function : keep the current display
					}
					else
					{
						// Action : nothing
						// Function : go to the bike page
						FunctionRequest<AdminRequest> redirRequest(_request);
						redirRequest.getFunction()->setPage<VinciBikeAdminInterfaceElement>();
						redirRequest.setObjectId(_bike->getKey());
						redirRequest.getFunction()->getInterface()->getPage<RedirectInterfacePage>()->display(stream, VariablesMap(), &redirRequest);
						return;
					}
				}
			}
			else if (_tableId == VinciContractTableSync::TABLE_ID)
			{
				if (_lastPage == Factory<AdminInterfaceElement>::getKey<VinciCustomerAdminInterfaceElement>()
					&& _lastId == _contract->getKey()
				){
					// Action : return all the bikes
					// Function same display
				}
				else
				{
					// Action : nothing
					// Function : go to the contract
					// Function : go to the bike page
					FunctionRequest<AdminRequest> redirRequest(_request);
					redirRequest.getFunction()->setPage<VinciCustomerAdminInterfaceElement>();
					redirRequest.setObjectId(_contract->getKey());
					redirRequest.getFunction()->getInterface()->getPage<RedirectInterfacePage>()->display(stream, VariablesMap(), &redirRequest);
					return;
				}
			}
			else
			{
				// Function : go to the customer search page
				FunctionRequest<AdminRequest> redirRequest(_request);
				shared_ptr<VinciCustomerSearchAdminInterfaceElement> page = Factory<AdminInterfaceElement>::create<VinciCustomerSearchAdminInterfaceElement>();
				page->setSearchName(_strCode);
				redirRequest.getFunction()->setPage(page);
				redirRequest.getFunction()->getInterface()->getPage<RedirectInterfacePage>()->display(stream, VariablesMap(), &redirRequest);
				return;
			}
		}
	}
}
