
/** VinciPrintedContractInterfacePage class header.
	@file VinciPrintedContractInterfacePage.h

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

#ifndef SYNTHESE_VinciPrintedContractInterfacePage_H__
#define SYNTHESE_VinciPrintedContractInterfacePage_H__

#include <string>

#include <boost/shared_ptr.hpp>

#include "11_interfaces/InterfacePage.h"

namespace synthese
{
	namespace server
	{
		class Request;
	}

	namespace vinci
	{
		class VinciContract;

		/** Vinci printed contract.
			@ingroup m71Pages refPages
		*/
		class VinciPrintedContractInterfacePage : public interfaces::InterfacePage
		{
		public:
			/** Overloaded display method for specific parameter conversion.
				This function converts the parameters into a single ParametersVector object.
			*/
			void display(std::ostream& stream
				, interfaces::VariablesMap& vars
				, boost::shared_ptr<const VinciContract> contract
				, const server::Request* request = NULL) const;
		};
	}
}

#endif // SYNTHESE_VinciPrintedContractInterfacePage_H__
