
/** DeparturesTableInterfaceElement class implementation.
	@file DeparturesTableInterfaceElement.cpp

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

#include "DeparturesTableInterfaceElement.h"

#include "Interface.h"
#include "ValueElementList.h"
#include "InterfacePageException.h"
#include "Request.h"
#include "DeparturesTableTypes.h"
#include "DepartureTableRowInterfacePage.h"

#include <vector>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace interfaces;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<interfaces::LibraryInterfaceElement, departure_boards::DeparturesTableInterfaceElement>::FACTORY_KEY("departurestable");
	}

	namespace departure_boards
	{
		void DeparturesTableInterfaceElement::storeParameters( ValueElementList& vel )
		{
			if (vel.size() < 3)
				throw interfaces::InterfacePageException("Malformed departure table declaration");

			_multiplicateurRangeeVIE = vel.front();
			_pagesVIE = vel.front();
			_pageSeparator = vel.front();

			if (!vel.isEmpty())
				_departuresToHide = vel.front();

			if (!vel.isEmpty())
				_message = vel.front();

			if (!vel.isEmpty())
				_displayServiceNumber = vel.front();

			if (!vel.isEmpty())
				_displayQuai = vel.front();

			if (!vel.isEmpty())
				_numberOfIntermediatesStops = vel.front();

			if (!vel.isEmpty())
				_displayTeam = vel.front();

			if (!vel.isEmpty())
				_blinkingDelay = vel.front();
		}

		string DeparturesTableInterfaceElement::display(
			ostream& stream
			, const ParametersVector& parameters
			, VariablesMap& variables
			, const void* object /*= NULL*/
			, const server::Request* request /*= NULL*/ ) const
		{
			const ArrivalDepartureList& ptds(*static_cast<const ArrivalDepartureList*>(object));

			int __MultiplicateurRangee(_multiplicateurRangeeVIE->isFalse(parameters, variables, object, request)
				? 1
				: lexical_cast<int>(_multiplicateurRangeeVIE->getValue(parameters, variables, object, request))
			);
			int __Pages(0);
			if(_pagesVIE) try
			{
				__Pages = lexical_cast<int>(_pagesVIE->getValue(parameters, variables, object, request));
			}
			catch(bad_lexical_cast&)
			{
			}

			const string& __SeparateurPage = _pageSeparator->getValue(parameters, variables, object, request);
			int departuresToHide(
				_departuresToHide && !_departuresToHide->getValue(parameters, variables, object, request).empty() ?
				lexical_cast<int>(_departuresToHide->getValue(parameters, variables, object, request)) :
				0
			);
			const string message(
				_message ?
				_message->getValue(parameters, variables, object, request) :
				string()
			);
			bool displayServiceNumber(
				_displayServiceNumber && !_displayServiceNumber->isFalse(parameters, variables, object, request)
			);
			bool displayQuai(
				_displayQuai && !_displayQuai->isFalse(parameters, variables, object, request)
			);
			bool displayTeam(
				_displayTeam && !_displayTeam->isFalse(parameters, variables, object, request)
			);

			int numberOfIntermediatesStops(UNKNOWN_VALUE);
			if(_numberOfIntermediatesStops) try
			{
				numberOfIntermediatesStops = lexical_cast<int>(_numberOfIntermediatesStops->getValue(parameters, variables, object, request));
			}
			catch(bad_lexical_cast&)
			{
			}

			int blinkingDelay(0);
			if(_blinkingDelay) try
			{
				blinkingDelay = lexical_cast<int>(_blinkingDelay->getValue(parameters, variables, object, request));
			}
			catch(bad_lexical_cast&)
			{
			}

			const DepartureTableRowInterfacePage* page(_page->getInterface()->getPage<DepartureTableRowInterfacePage>());
			if(page == NULL)
			{
				return string();
			}

			// Gestion des pages
			size_t __NombrePages(1);
			if (__Pages != 0)
			{
				int departuresNumber = ptds.size() - departuresToHide;
				for (ArrivalDepartureList::const_iterator it = ptds.begin(); departuresNumber && (it != ptds.end()); ++it, --departuresNumber)
				{
					const ActualDisplayedArrivalsList& displayedList = it->second;
					if (displayedList.size() > __NombrePages + 2)
						__NombrePages = displayedList.size () - 2;
				}
				if (__Pages != UNKNOWN_VALUE && __NombrePages > (size_t)__Pages)
					__NombrePages = __Pages;
			}

			// Boucle sur les pages
			for ( size_t __NumeroPage = 1; __NumeroPage <= __NombrePages; __NumeroPage++ )
			{
				// Separateur de page
				if ( __NumeroPage > 1 )
					stream << __SeparateurPage;

				// Boucle sur les rangees
				int __Rangee = __MultiplicateurRangee;
				int departuresNumber = ptds.size() - departuresToHide;
				for (ArrivalDepartureList::const_iterator it = ptds.begin(); departuresNumber && (it != ptds.end()); ++it, --departuresNumber)
				{
					const ArrivalDepartureRow& ___DP = *it;

					int __NombrePagesRangee = ___DP.second.size () - 2;
					int pageNumber = ( !__NombrePagesRangee || __NumeroPage > __NombrePagesRangee * ( __NombrePages / __NombrePagesRangee ) )
						? __NumeroPage
						: (1 + __NumeroPage % __NombrePagesRangee);     // 1 : Numero de page

					// Lancement de l'affichage de la rangee
					page->display(
						stream
						, variables
						, __Rangee
						, pageNumber
						, displayQuai
						, displayServiceNumber
						, displayTeam
						, numberOfIntermediatesStops,
						blinkingDelay
						, ___DP
						, request
					);

					// Incrementation du numero de rangee
					__Rangee += __MultiplicateurRangee;
				}

				stream << message;
			}
			return string();
		}
	}
}
