
/** EvalService class implementation.
	@file EvalService.cpp
	@author Hugues Romain
	@date 2011

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

#include "EvalService.hpp"

#include "CMSModule.hpp"
#include "Request.h"
#include "RequestException.h"
#include "Webpage.h"
#include "WebpageContent.hpp"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<> const string util::FactorableTemplate<Function,cms::EvalService>::FACTORY_KEY("EvalService");

	namespace cms
	{
		const string EvalService::PARAMETER_TEXT("t");

		ParametersMap EvalService::_getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_TEXT, _text);
			return map;
		}



		void EvalService::_setFromParametersMap(const ParametersMap& map)
		{
			_text = map.getDefault<string>(PARAMETER_TEXT, string(), false);
		}



		util::ParametersMap EvalService::run(
			std::ostream& stream,
			const Request& request
		) const {

			Webpage p;
			p.set<WebpageContent>(WebpageContent(_text));
			p.setRoot(const_cast<Website*>(CMSModule::GetSite(request, getTemplateParameters())));
			p.display(stream, request, getTemplateParameters());

			return util::ParametersMap();
		}



		bool EvalService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string EvalService::getOutputMimeType() const
		{
			return "text/html";
		}
}	}
