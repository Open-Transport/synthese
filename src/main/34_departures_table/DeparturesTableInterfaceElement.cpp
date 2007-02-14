
/** DeparturesTableInterfaceElement class implementation.
	@file DeparturesTableInterfaceElement.cpp

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

#include <vector>

#include "01_util/Conversion.h"

#include "11_interfaces/Interface.h"
#include "11_interfaces/ValueInterfaceElement.h"
#include "11_interfaces/ValueElementList.h"
#include "11_interfaces/InterfacePageException.h"

#include "30_server/Request.h"

#include "34_departures_table/Types.h"
#include "34_departures_table/DeparturesTableInterfaceElement.h"
#include "34_departures_table/DepartureTableRowInterfacePage.h"

using namespace std;

namespace synthese
{
	using namespace interfaces;
	using namespace util;

	namespace departurestable
	{
		DeparturesTableInterfaceElement::DeparturesTableInterfaceElement()
			: _multiplicateurRangeeVIE(NULL)
			, _pageSeparator(NULL)
			, _pagesVIE(NULL)
		{

		}

		DeparturesTableInterfaceElement::~DeparturesTableInterfaceElement()
		{
			delete _multiplicateurRangeeVIE;
			delete _pageSeparator;
			delete _pagesVIE;
		}

		void DeparturesTableInterfaceElement::storeParameters( ValueElementList& vel )
		{
			if (vel.size() < 3)
				throw interfaces::InterfacePageException("Malformed departure table declaration");

			_multiplicateurRangeeVIE = vel.front();
			_pagesVIE = vel.front();
			_pageSeparator = vel.front();
		}

		void DeparturesTableInterfaceElement::display(ostream& stream, const ParametersVector& parameters, const void* object /*= NULL*/, const server::Request* request /*= NULL*/ ) const
		{
			const ArrivalDepartureList* ptds = (ArrivalDepartureList*) object;
			
			int __MultiplicateurRangee = _multiplicateurRangeeVIE->isZero(parameters, object, request) ? 1 : Conversion::ToInt(_multiplicateurRangeeVIE->getValue(parameters, object, request));
			const std::string& __Pages = _pagesVIE->getValue(parameters, object, request);
			const std::string& __SeparateurPage = _pageSeparator->getValue(parameters, object, request);

			// Gestion des pages
			int __NombrePages = 1;
			if ( (__Pages == "intermediate" ) || 
			(__Pages == "destination" ) )
			{
				for (ArrivalDepartureList::const_iterator it = ptds->begin(); it != ptds->end(); ++it)
				{
					const ActualDisplayedArrivalsList& displayedList = it->second;
					if (displayedList.size () - 2 > __NombrePages )
						__NombrePages = displayedList.size () - 2;
				}
			}

			if (__Pages == "destination" )
				__NombrePages++;

			// Boucle sur les pages
			for ( int __NumeroPage = 1; __NumeroPage <= __NombrePages; __NumeroPage++ )
			{
				// Separateur de page
				if ( __NumeroPage > 1 )
					stream << __SeparateurPage;

				// Boucle sur les rangees
				int __Rangee = __MultiplicateurRangee;
				for (ArrivalDepartureList::const_iterator it = ptds->begin(); it != ptds->end(); ++it)
				{
					const ArrivalDepartureRow& ___DP = *it;

					int __NombrePagesRangee = ___DP.second.size () - 2 + ( __Pages == "destination" ? 1 : 0 );
					int pageNumber = ( !__NombrePagesRangee || __NumeroPage > __NombrePagesRangee * ( __NombrePages / __NombrePagesRangee ) )
						? __NumeroPage
						: (1 + __NumeroPage % __NombrePagesRangee);     // 1 : Numero de page

					// Lancement de l'affichage de la rangee
					const DepartureTableRowInterfacePage* page = _page->getInterface()->getPage<DepartureTableRowInterfacePage>();
					page->display(stream, __Rangee, pageNumber, &___DP, request);
					delete page;

					// Incrementation du numero de rangee
					__Rangee += __MultiplicateurRangee;
				}
			}
		}
	}
}
