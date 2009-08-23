
/** XMLReservationFunction class implementation.
	@file XMLReservationFunction.cpp
	@author Hugues
	@date 2009

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

#include "ResaRight.h"
#include "RequestException.h"
#include "XMLReservationFunction.h"
#include "Request.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<> const string util::FactorableTemplate<Function,resa::XMLReservationFunction>::FACTORY_KEY("XMLReservationFunction");
	
	namespace resa
	{
		ParametersMap XMLReservationFunction::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}

		void XMLReservationFunction::_setFromParametersMap(const ParametersMap& map)
		{
			_request->setActionWillCreateObject();
		}

		void XMLReservationFunction::_run( std::ostream& stream ) const
		{
			stream << "<?xml version='1.0' encoding='UTF-8' ?><reservation>";
			if(_request->getActionCreatedId())
			{
				stream << *_request->getActionCreatedId();
			}
			stream << "</reservation>";
		}
		
		
		
		bool XMLReservationFunction::_isAuthorized() const
		{
			return
				_request->isAuthorized<ResaRight>(WRITE) ||
				_request->isAuthorized<ResaRight>(UNKNOWN_RIGHT_LEVEL, WRITE);
		}



		std::string XMLReservationFunction::getOutputMimeType() const
		{
			return "text/xml";
		}



		XMLReservationFunction::XMLReservationFunction():
		FactorableTemplate<server::Function,XMLReservationFunction>()
		{
			setEnv(shared_ptr<Env>(new Env));
		}
	}
}
