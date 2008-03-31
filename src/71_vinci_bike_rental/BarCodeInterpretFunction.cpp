
/** BarCodeInterpretFunction class implementation.
	@file BarCodeInterpretFunction.cpp

	This file belongs to the VINCI BIKE RENTAL SYNTHESE module
	Copyright (C) 2006 Vinci Park 
	
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

	namespace util
	{
		template<> const string FactorableTemplate<RequestWithInterfaceAndRequiredSession, vinci::BarCodeInterpretFunction>::FACTORY_KEY("barcoderead");
	}

	namespace vinci
	{
		const string BarCodeInterpretFunction::PARAMETER_READED_CODE = "rco";
		const string BarCodeInterpretFunction::PARAMETER_LAST_PAGE = "lp";
		const string BarCodeInterpretFunction::PARAMETER_LAST_OBJECT_ID = "loi";
		
		ParametersMap BarCodeInterpretFunction::_getParametersMap() const
		{
			ParametersMap map(RequestWithInterfaceAndRequiredSession::_getParametersMap());

			map.insert(PARAMETER_LAST_PAGE, _lastPage);
			if (_lastId)
				map.insert(PARAMETER_LAST_OBJECT_ID, _lastId);
			return map;
		}

		void BarCodeInterpretFunction::_setFromParametersMap(const ParametersMap& map)
		{
			RequestWithInterfaceAndRequiredSession::_setFromParametersMap(map);

			// Read code
			uid uidCode = map.getUid(PARAMETER_READED_CODE, true, FACTORY_KEY);
			_strCode = map.getString(PARAMETER_READED_CODE, true, FACTORY_KEY);

			// Last page
			_lastPage = map.getString(PARAMETER_LAST_PAGE, false, FACTORY_KEY);

			// Last object
			_lastId = map.getUid(PARAMETER_LAST_OBJECT_ID, false, FACTORY_KEY);

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
					_bike = VinciBikeTableSync::Get(uidCode);
				}
				catch (VinciBike::ObjectNotFoundException& e)
				{
					throw RequestException("Specified bike not found");
				}
			}
			else if (_tableId == VinciContractTableSync::TABLE_ID)
			{
				try
				{
					_contract = VinciContractTableSync::Get(uidCode);
				}
				catch (VinciContract::ObjectNotFoundException& e)
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
					if (_lastPage == VinciCustomerAdminInterfaceElement::FACTORY_KEY && _lastId == contract->getKey())
					{
						shared_ptr<TransactionPart> transactionPart = VinciBikeTableSync::getRentTransactionPart(_bike);

						// Action : return the bike(s)
						// Function : display the contract
//						ActionFunctionRequest<ReturnABikeAction, AdminRequest> redirRequest(_request);
//						redirRequest.getAction()->setTransactionPart(transactionPart);
						FunctionRequest<AdminRequest> redirRequest;
						redirRequest.getFunction()->setPage<VinciCustomerAdminInterfaceElement>();
						redirRequest.setObjectId(contract->getKey());
						VariablesMap vm;
						redirRequest.getFunction()->getInterface()->
							getPage<RedirectInterfacePage>()->display(stream, vm, &redirRequest);
						return;
					}
					else
					{
						// Function : display the contract
						FunctionRequest<AdminRequest> redirRequest(_request);
						redirRequest.getFunction()->setPage<VinciCustomerAdminInterfaceElement>();
						redirRequest.setObjectId(contract->getKey());
						VariablesMap vm;
						redirRequest.getFunction()->getInterface()->
							getPage<RedirectInterfacePage>()->display(stream, vm, &redirRequest);
						return;
					}
				}
				else
				{
					if (_lastPage == VinciCustomerAdminInterfaceElement::FACTORY_KEY)
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
						VariablesMap vm;
						redirRequest.getFunction()->getInterface()->
						  getPage<RedirectInterfacePage>()->display(stream, vm, &redirRequest);
						return;
					}
				}
			}
			else if (_tableId == VinciContractTableSync::TABLE_ID)
			{
				if (_lastPage == VinciCustomerAdminInterfaceElement::FACTORY_KEY
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
					VariablesMap vm;
					redirRequest.getFunction()->getInterface()->
					  getPage<RedirectInterfacePage>()->display(stream, vm, &redirRequest);
					return;
				}
			}
			else
			{
				// Function : go to the customer search page
				FunctionRequest<AdminRequest> redirRequest(_request);
				shared_ptr<VinciCustomerSearchAdminInterfaceElement> page(new VinciCustomerSearchAdminInterfaceElement);
				page->setSearchName(_strCode);
				redirRequest.getFunction()->setPage(page);
				VariablesMap vm;
				redirRequest.getFunction()->getInterface()->
				  getPage<RedirectInterfacePage>()->display(stream, vm, &redirRequest);
				return;
			}
		}
	}
}
