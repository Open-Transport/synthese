
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

#include "32_admin/AdminParametersException.h"
#include "32_admin/AdminRequest.h"
#include "32_admin/HomeAdmin.h"

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
			_folderSubpageIntroducerVIE = vel.size() ? vel.front() : _subpageIntroducerVIE;
			_folderLastSubpageIntroducerVIE = vel.size() ? vel.front() : _lastSubpageIntroducerVIE;
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
			_folderSubpageIntroducer = _folderSubpageIntroducerVIE->getValue(parameters, variables, object, request);
			_folderLastSubpageIntroducer = _folderLastSubpageIntroducerVIE->getValue(parameters, variables, object, request);

			stream << getSubPages(
				(*page)->getTree(static_cast<const server::FunctionRequest<AdminRequest>*>(request))
				, **page
				, static_cast<const server::FunctionRequest<AdminRequest>*>(request)
				, 0
				, string()
				, true
			);
	
			return string();
		}

		std::string AdminPagesTreeInterfaceElement::getSubPages(
			const AdminInterfaceElement::PageLinksTree& pages
			, const AdminInterfaceElement& currentPage
			, const server::FunctionRequest<admin::AdminRequest>* request
			, int level
			, string prefix
			, bool last
		) const {

			stringstream str;

			str << prefix;
			string curPrefix(prefix);
			if (level > 0)
			{
				if (last)
				{
					curPrefix += _lastLevelIndenter;
					str << (pages.subPages.empty() ? _lastSubpageIntroducer : _folderLastSubpageIntroducer);
				}
				else
				{
					curPrefix += _levelIndenter;
					str << (pages.subPages.empty() ? _subpageIntroducer : _folderSubpageIntroducer);
				}
			}

			if (pages.pageLink == currentPage.getPageLink())
			{
				str << HTMLModule::getHTMLImage(currentPage.getPageLink().icon, currentPage.getPageLink().name)
					<< currentPage.getPageLink().name;
			}
			else
			{
				str << HTMLModule::getHTMLImage(pages.pageLink.icon, pages.pageLink.name)
					<< HTMLModule::getHTMLLink(pages.pageLink.getURL(request), pages.pageLink.name);
			}
			str << _ending;

			for (vector<AdminInterfaceElement::PageLinksTree>::const_iterator it = pages.subPages.begin(); it != pages.subPages.end(); ++it)
			{
				str << getSubPages(*it, currentPage, request, level+1, curPrefix, it==(pages.subPages.end()-1) );
			}
			return str.str();
		}
	}
}

