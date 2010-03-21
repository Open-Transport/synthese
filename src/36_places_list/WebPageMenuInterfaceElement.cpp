
/** WebPageMenuInterfaceElement class implementation.
	@file WebPageMenuInterfaceElement.cpp
	@author Hugues
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

#include "WebPageMenuInterfaceElement.hpp"
#include "ValueElementList.h"
#include "WebPageLinkInterfaceElement.h"
#include "SiteTableSync.h"
#include "WebPageTableSync.h"
#include "WebPageDisplayFunction.h"
#include "Request.h"
#include "InterfaceModule.h"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace interfaces;
	using namespace util;
	
	namespace util
	{
		template<> const string FactorableTemplate<LibraryInterfaceElement, transportwebsite::WebPageMenuInterfaceElement>::FACTORY_KEY("menu");
	}

	namespace transportwebsite
	{
		void WebPageMenuInterfaceElement::storeParameters(ValueElementList& vel)
		{
			_rootVIE = vel.front();
			_minDepthVIE = vel.front();
			_maxDepthVIE = vel.front();
			_levelToMaskVIE = vel.front();
			_levelToDisplayVIE = vel.front();

			_htmlVEL = vel;
		}

		string WebPageMenuInterfaceElement::display(
			ostream& stream
			, const ParametersVector& parameters
			, VariablesMap& variables
			, const void* object /*= NULL*/
			, const server::Request* request /*= NULL*/
		) const {

			if(	!request
			){
				return string();
			}

			RegistryKeyType rootId(lexical_cast<RegistryKeyType>(_rootVIE->getValue(parameters,variables,object,request)));
			size_t depth(0);
			if(decodeTableId(rootId) == WebPageTableSync::TABLE.ID)
			{
				shared_ptr<const WebPage> page(WebPageTableSync::Get(rootId, Env::GetOfficialEnv()));
				depth = page->getDepth();
			}
			
			std::vector<MenuDefinition_> _menuDefinition;
			std::size_t	_minDepth;
			std::size_t	_maxDepth;
			boost::optional<std::size_t>	_levelToMask;
			std::size_t	_levelToDisplay;
			boost::shared_ptr<const WebPage> _currentPage;

			if(dynamic_cast<const WebPageDisplayFunction*>(request->getFunction().get()))
			{
				_currentPage = static_cast<const WebPageDisplayFunction*>(request->getFunction().get())->getPage();
			}

			_minDepth = lexical_cast<size_t>(_minDepthVIE->getValue(parameters,variables,object,request));
			_maxDepth = lexical_cast<size_t>(_maxDepthVIE->getValue(parameters,variables,object,request));
			if(!_levelToMaskVIE->getValue(parameters,variables,object,request).empty())
			{
				_levelToMask = lexical_cast<size_t>(_levelToMaskVIE->getValue(parameters,variables,object,request));
			}
			_levelToDisplay = lexical_cast<size_t>(_levelToDisplayVIE->getValue(parameters,variables,object,request));

			ValueElementList htmlVEL(_htmlVEL);
			for(size_t i(_minDepth); i<=_maxDepth; ++i)
			{
				MenuDefinition_ curMenuDefinition;
				curMenuDefinition.beginning = htmlVEL.front()->getValue(parameters, variables, object, request);
				curMenuDefinition.ending = htmlVEL.front()->getValue(parameters, variables, object, request);
				curMenuDefinition.beginningSelected = htmlVEL.front()->getValue(parameters, variables, object, request);
				curMenuDefinition.endingSelected = htmlVEL.front()->getValue(parameters, variables, object, request);
				curMenuDefinition.endingAfterSubmenu = htmlVEL.front()->getValue(parameters, variables, object, request);
				_menuDefinition.push_back(curMenuDefinition);
			}

			_getMenuContentRecursive(stream, request, rootId, depth, _currentPage, _menuDefinition, _minDepth, _maxDepth, _levelToMask, _levelToDisplay);

			return string();
		}

		WebPageMenuInterfaceElement::~WebPageMenuInterfaceElement()
		{
		}



		bool WebPageMenuInterfaceElement::_getMenuContentRecursive(
			ostream& stream,
			const server::Request* request /*= NULL*/,
			RegistryKeyType rootId,
			std::size_t depth,
			boost::shared_ptr<const WebPage> _currentPage,
			const std::vector<MenuDefinition_>& _menuDefinition,
			std::size_t	_minDepth,
			std::size_t	_maxDepth,
			boost::optional<std::size_t> _levelToMask,
			std::size_t	_levelToDisplay
		) const {
			/** - Page in branch update */
			bool returned_page_in_branch(_currentPage.get() ? (rootId == _currentPage->getKey()) : false);

			/** - Recursion attempting if :
					- the max depth is not reached
					- no masked level is defined or the masked level is superior to the current level or the current level is the submenu of the current branch
			*/
			stringstream submenu;

			WebPageTableSync::SearchResult pages(
				WebPageTableSync::Search(
					Env::GetOfficialEnv(),
					decodeTableId(rootId) == SiteTableSync::TABLE.ID ? rootId : optional<RegistryKeyType>(),
					decodeTableId(rootId) == SiteTableSync::TABLE.ID ? 0 : rootId
			)	);
			BOOST_FOREACH(shared_ptr<WebPage> page, pages)
			{
				if(!page->mustBeDisplayed())
				{
					continue;
				}

				returned_page_in_branch |= _getMenuContentRecursive(submenu, request, page->getKey(), depth + 1, _currentPage, _menuDefinition, _minDepth, _maxDepth, _levelToMask, _levelToDisplay);
			}

			/** - Preparing the output of the current level if the min depth is reached */
			if (decodeTableId(rootId) == WebPageTableSync::TABLE.ID && _minDepth <= depth)
			{
				stream <<
					(rootId == _currentPage->getKey() || returned_page_in_branch) ?
					_menuDefinition[depth].beginningSelected :
					_menuDefinition[depth].beginning;
					
				vector<string> params;
				params.push_back(lexical_cast<string>(rootId));
				params.push_back(string());
				InterfaceModule::SimpleDisplay<WebPageLinkInterfaceElement>(stream, params, request);
					
				stream <<
					(rootId == _currentPage->getKey() || returned_page_in_branch) ?
					_menuDefinition[depth].endingSelected :
					_menuDefinition[depth].ending;
			}

			/** - Submenu only if applicable */
			if( _maxDepth > depth &&
				(	!_levelToMask ||
					*_levelToMask > depth
			)	){
				stream << submenu.str();
			}

			stream << _menuDefinition[depth].endingAfterSubmenu;

			return returned_page_in_branch;
		}
	}
}
