
/** AdminPagePositionInterfaceElement class implementation.
	@file AdminPagePositionInterfaceElement.cpp

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

#include <sstream>

#include "HTMLModule.h"
#include "ValueElementList.h"
#include "StaticFunctionRequest.h"
#include "Conversion.h"
#include "AdminInterfaceElement.h"
#include "AdminPagePositionInterfaceElement.h"
#include "AdminParametersException.h"
#include "AdminFunction.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace interfaces;
	using namespace util;
	using namespace server;
	using namespace html;
	using namespace admin;

	namespace util
	{
		template<> const string FactorableTemplate<LibraryInterfaceElement, AdminPagePositionInterfaceElement>::FACTORY_KEY("adminpos");
	}

	namespace admin
	{
		void AdminPagePositionInterfaceElement::storeParameters( interfaces::ValueElementList& vel )
		{
			if (vel.size() < 6)
				throw AdminParametersException("6 arguments needed");
			_normalSeparator = vel.front();
			_lastSeparator = vel.front();
			_withImages = vel.front();
			_withLinks = vel.front();
			_withFirst = vel.front();
			_lastSeparatorIfFirst = vel.front();
		}

		std::string AdminPagePositionInterfaceElement::display(
			ostream& stream
			, const ParametersVector& parameters
			, interfaces::VariablesMap& variables,
			const void* object /* = NULL */,
			const server::Request* request /* = NULL */
		) const {
			const AdminInterfaceElement* page = static_cast<const AdminInterfaceElement*>(object);

			if(page && request)
			{
				string normalSeparator(_normalSeparator->getValue(parameters, variables, object, request));
				string lastSeparator(_lastSeparator->getValue(parameters, variables, object, request));
				bool withImages(Conversion::ToBool(_withImages->getValue(parameters, variables, object, request)));
				bool withLinks(Conversion::ToBool(_withLinks->getValue(parameters, variables, object, request)));
				bool withFirst(Conversion::ToBool(_withFirst->getValue(parameters, variables, object, request)));
				bool lastSeparatorIfFirst(Conversion::ToBool(_withFirst->getValue(parameters, variables, object, request)));

				const AdminInterfaceElement::PageLinks& links(
					page->getTreePosition(
						*dynamic_cast<const AdminRequest*>(request)
				)	);

				bool first(true);
				AdminRequest r(*request, true);
				for (AdminInterfaceElement::PageLinks::const_iterator it(links.begin()); it != links.end(); ++it)
				{
					if(!withFirst && it == links.begin() && it != links.end()-1) continue;

					if (it == links.end()-1)
					{
						if(lastSeparatorIfFirst || it != links.begin())
							stream << lastSeparator;
					}
					else if(first)
						first = false;
					else
						stream << normalSeparator;

					if(withImages)
					{
						stream << HTMLModule::getHTMLImage((*it)->getIcon(), string("-"));
					}

					r.getFunction()->setPage(const_pointer_cast<AdminInterfaceElement>(*it));
					if (withLinks && it != (links.end() -1))
						stream << HTMLModule::getHTMLLink(r.getURL(), (*it)->getTitle());
					else
						stream << (*it)->getTitle();
				}
			}
			return string();
		}
	}
}

