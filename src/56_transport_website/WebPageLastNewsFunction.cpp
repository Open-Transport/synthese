
/** WebPageLastNewsFunction class implementation.
	@file WebPageLastNewsFunction.cpp
	@author Hugues Romain
	@date 2010

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

#include "RequestException.h"
#include "Request.h"
#include "WebPageLastNewsFunction.hpp"
#include "WebPage.h"
#include "WebPageTableSync.h"
#include "PlacesListModule.h"
#include "StaticFunctionRequest.h"
#include "WebPageDisplayFunction.h"
#include "WebPageInterfacePage.h"
#include "ServerModule.h"
#include "HTMLModule.h"

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace html;

	template<> const string util::FactorableTemplate<Function,transportwebsite::WebPageLastNewsFunction>::FACTORY_KEY("last_news");
	
	namespace transportwebsite
	{
		const string WebPageLastNewsFunction::PARAMETER_ROOT_ID("root");
		const string WebPageLastNewsFunction::PARAMETER_DISPLAY_PAGE_ID("display_page");
		const string WebPageLastNewsFunction::PARAMETER_MAX_NUMBER("number");
		
		ParametersMap WebPageLastNewsFunction::_getParametersMap() const
		{
			ParametersMap map;
			if(_root.get())
			{
				map.insert(PARAMETER_ROOT_ID, _root->getKey());
			}
			if(_number)
			{
				map.insert(PARAMETER_MAX_NUMBER, *_number);
			}
			if(_displayPage.get())
			{
				map.insert(PARAMETER_DISPLAY_PAGE_ID, _displayPage->getKey());
			}
			return map;
		}

		void WebPageLastNewsFunction::_setFromParametersMap(const ParametersMap& map)
		{
			optional<RegistryKeyType> rootId(map.get<RegistryKeyType>(PARAMETER_ROOT_ID));
			if(rootId) try
			{
				_root = Env::GetOfficialEnv().get<WebPage>(*rootId);
			}
			catch (ObjectNotFoundException<WebPage>&)
			{
				throw RequestException("No such root page");
			}

			optional<RegistryKeyType> displayId(map.getOptional<RegistryKeyType>(PARAMETER_DISPLAY_PAGE_ID));
			if(displayId) try
			{
				_displayPage = Env::GetOfficialEnv().get<WebPage>(*displayId);
			}
			catch (ObjectNotFoundException<WebPage>&)
			{
				throw RequestException("No such display page");
			}

			_number = map.getOptional<size_t>(PARAMETER_MAX_NUMBER);
		}

		void WebPageLastNewsFunction::run(
			std::ostream& stream,
			const Request& request
		) const {

			shared_ptr<const Site> site(PlacesListModule::GetSite(request));
			shared_ptr<const WebPage> curPage(PlacesListModule::GetWebPage(request));

			WebPageTableSync::SearchResult pages(
				WebPageTableSync::Search(
					Env::GetOfficialEnv(),
					_root.get() ? optional<RegistryKeyType>() : site->getKey(),
					_root.get() ? _root->getKey() : optional<RegistryKeyType>(0)
			)	);

			StaticFunctionRequest<WebPageDisplayFunction> openRequest(request, false);

			if(!_displayPage.get() && _root.get())
			{ // RSS
				openRequest.getFunction()->setPage(_root);

				stream <<
					"<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>" <<
					"<rss version=\"2.0\" xmlns:atom=\"http://www.w3.org/2005/Atom\">" <<
					"<channel>" <<
					"<title>" << _root->getName() << "</title>" <<
					"<description><![CDATA[" << _root->getAbstract() << "]]></description>" <<
					"<link>" << HTMLModule::HTMLEncode(openRequest.getURL(true, true)) << "</link>" <<
					"<generator>SYNTHESE " << ServerModule::VERSION << "</generator>";
				if(_root.get() && !_root->getImage().empty())
				{
					stream << "<image><url>" << _root->getImage() << "</url><title>" <<
						_root->getName() << "</title><link>" <<
						HTMLModule::HTMLEncode(openRequest.getURL(true, true)) << "</link></image>";
				}

				openRequest.getFunction()->setPage(curPage);
				stream << "<atom:link href=\"" << HTMLModule::HTMLEncode(openRequest.getURL(true, true)) << "\" rel=\"self\" type=\"application/rss+xml\" />";
			}

			size_t number(0);
			BOOST_FOREACH(shared_ptr<WebPage> page, pages)
			{
				if(!page->mustBeDisplayed())
				{
					continue;
				}

				if(_displayPage.get())
				{
					WebPageInterfacePage::Display(stream, *_displayPage, request, *page, false);
				}
				else if(_root.get())
				{
					openRequest.getFunction()->setPage(page);

					stream <<
						"<item>" <<
						"<title>" << page->getName() << "</title>" <<
						"<description><![CDATA[" << page->getAbstract() << "]]></description>" <<
						"<link>" << HTMLModule::HTMLEncode(openRequest.getURL(true, true)) << "</link>" <<
						"<guid isPermaLink=\"true\">" << HTMLModule::HTMLEncode(openRequest.getURL(true, true)) << "</guid>"
						;
					if(!page->getStartDate().is_not_a_date_time())
					{
						stream <<
							"<pubDate>" <<
							page->getStartDate().date().day_of_week() << ", " <<
							page->getStartDate().date().day() << " " <<
							page->getStartDate().date().month() << " " <<
							page->getStartDate().date().year() << " " <<
							page->getStartDate().time_of_day() << " " <<
							"+0100" <<
							"</pubDate>"
							;
					}
					stream << "</item>";
				}

				number++;
				if(_number && number == *_number)
				{
					break;
				}
			}

			if(!_displayPage.get() && _root.get())
			{
				stream << "</channel></rss>";
			}
		}
		
		
		
		bool WebPageLastNewsFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string WebPageLastNewsFunction::getOutputMimeType() const
		{
			return _displayPage.get() ? _displayPage->getMimeType() : "application/rss+xml";
		}
	}
}
