
/** ReservationEditFunction class implementation.
	@file ReservationEditFunction.cpp
	@author Hugues
	@date 2009

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

#include "RequestException.h"
#include "Request.h"
#include "ResaRight.h"
#include "ReservationEditFunction.h"
#include "ReservationEditInterfacePage.h"
#include "ReservationTransaction.h"
#include "ReservationTransactionTableSync.h"
#include "Interface.h"
#include "ReservationTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace interfaces;

	template<> const string util::FactorableTemplate<RequestWithInterface,resa::ReservationEditFunction>::FACTORY_KEY("ReservationEditFunction");

	namespace resa
	{
		const string ReservationEditFunction::PARAMETER_RESERVATION_ID("ri");

		ParametersMap ReservationEditFunction::_getParametersMap() const
		{
			ParametersMap map(RequestWithInterface::_getParametersMap());
			if(_resa) map.insert(PARAMETER_RESERVATION_ID, _resa->getKey());
			return map;
		}

		void ReservationEditFunction::_setFromParametersMap(const ParametersMap& map)
		{
			RequestWithInterface::_setFromParametersMap(map);

			try
			{
				_resa = ReservationTransactionTableSync::Get(map.get<RegistryKeyType>(PARAMETER_RESERVATION_ID), *_env, UP_DOWN_LINKS_LOAD_LEVEL);
				ReservationTableSync::Search(*_env, _resa->getKey());
			}
			catch (ObjectNotFoundException<ReservationTransaction> e)
			{
				throw RequestException("No such reservation");
			}

			if(getInterface()->getPage<ReservationEditInterfacePage>() == NULL)
			{
				throw RequestException("Interface without reservation display");
			}
		}



		util::ParametersMap ReservationEditFunction::run( std::ostream& stream, const Request& request ) const
		{
			const ReservationEditInterfacePage* page(getInterface()->getPage<ReservationEditInterfacePage>());

			VariablesMap v;
			page->display(stream, *_resa, v, &request);

			return util::ParametersMap();
		}



		bool ReservationEditFunction::isAuthorized(const server::Session* session) const
		{
			return true;
		}



		std::string ReservationEditFunction::getOutputMimeType() const
		{
			if(	getInterface() &&
				getInterface()->getPage<ReservationEditInterfacePage>()
			){
				return getInterface()->getPage<ReservationEditInterfacePage>()->getMimeType();
			}
			return "text/html";
		}



		ReservationEditFunction::ReservationEditFunction():
			FactorableTemplate<interfaces::RequestWithInterface,ReservationEditFunction>()
		{
			setEnv(boost::shared_ptr<Env>(new Env));
		}
	}
}
