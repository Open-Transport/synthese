
/** AdminPagesTreeInterfaceElement class implementation.
	@file AdminPagesTreeInterfaceElement.cpp

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

#include "AdminPagesTreeInterfaceElement.h"

#include "32_admin/AdminInterfaceElement.h"
#include "32_admin/AdminParametersException.h"

#include "05_html/HTMLModule.h"

#include "11_interfaces/ValueElementList.h"

#include "30_server/FunctionRequest.h"

#include <sstream>

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
		template<> const string FactorableTemplate<LibraryInterfaceElement, AdminPagesTreeInterfaceElement>::FACTORY_KEY("admintree");
	}

	namespace admin
	{
		void AdminPagesTreeInterfaceElement::storeParameters(ValueElementList& vel)
		{
			if (vel.size() < 5)
				throw AdminParametersException("5 arguments needed");
			_subpageIntroducerVIE = vel.front();
			_lastSubpageIntroducerVIE = vel.front();
			_levelIndenterVIE = vel.front();
			_lastLevelIndenterVIE = vel.front();
			_endingVIE = vel.front();
		}

		std::string AdminPagesTreeInterfaceElement::display(
			ostream& stream
			, const ParametersVector& parameters, interfaces::VariablesMap& variables, const void* object /* = NULL */, const server::Request* request /* = NULL */ ) const
		{
			const shared_ptr<const AdminInterfaceElement>* page = (const shared_ptr<const AdminInterfaceElement>*) object;
			_lastLevelIndenter = _lastLevelIndenterVIE->getValue(parameters, variables, object, request);
			_levelIndenter = _levelIndenterVIE->getValue(parameters, variables, object, request);
			_lastSubpageIntroducer = _lastSubpageIntroducerVIE->getValue(parameters, variables, object, request);
			_subpageIntroducer = _subpageIntroducerVIE->getValue(parameters, variables, object, request);
			_ending = _endingVIE->getValue(parameters, variables, object, request);

			stream << getSubPages(string(), *page, static_cast<const server::FunctionRequest<AdminRequest>*>(request));
			return string();
		}

		std::string AdminPagesTreeInterfaceElement::getSubPages( const std::string& page, shared_ptr<const AdminInterfaceElement> currentPage, const server::FunctionRequest<admin::AdminRequest>* request, int level, string prefix) const
		{
			stringstream str;
			vector<shared_ptr<AdminInterfaceElement> > pages;
			for (Factory<AdminInterfaceElement>::Iterator it = Factory<AdminInterfaceElement>::begin(); it != Factory<AdminInterfaceElement>::end(); ++it)
			{
				if (it->getSuperiorVirtual() == page 
					&& ((it->getDisplayMode() == AdminInterfaceElement::EVER_DISPLAYED)
						|| (it->getDisplayMode() == AdminInterfaceElement::DISPLAYED_IF_CURRENT && it.getKey() == currentPage->getFactoryKey()))
					&& it->isAuthorized(request)
				)
					pages.push_back(*it);
			}

			for (vector<shared_ptr<AdminInterfaceElement> >::const_iterator it = pages.begin(); it != pages.end(); ++it)
			{
				str << prefix;
				string curPrefix(prefix);
				if (level >= 0)
				{
					if (it == (pages.end() - 1))
					{
						curPrefix += _lastLevelIndenter;
						str << _lastSubpageIntroducer;
					}
					else
					{
						curPrefix += _levelIndenter;
						str << _subpageIntroducer;
					}
				}
				if ((*it)->getFactoryKey() == currentPage->getFactoryKey())
				{
					str << HTMLModule::getHTMLImage(currentPage->getIcon(), currentPage->getTitle())
						<< currentPage->getTitle();
				}
				else
				{
					FunctionRequest<AdminRequest> r(request);
					r.getFunction()->setPage(*it);
					str << HTMLModule::getHTMLImage((*it)->getIcon(), (*it)->getTitle())
						<< HTMLModule::getHTMLLink(r.getURL(), (*it)->getTitle());
				}
				str << _ending;

				str << getSubPages((*it)->getFactoryKey(), currentPage, request, level+1, curPrefix);
			}
			return str.str();
		}
	}
}

