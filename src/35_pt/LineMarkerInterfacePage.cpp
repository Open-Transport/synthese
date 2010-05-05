////////////////////////////////////////////////////////////////////////////////
/// LineMarkerInterfacePage class implementation.
///	@file LineMarkerInterfacePage.cpp
///	@author Hugues Romain
///	@date 2008-12-27 20:18
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "CommercialLine.h"
#include "LineMarkerInterfacePage.h"
#include "StaticFunctionRequest.h"
#include "WebPageDisplayFunction.h"
#include "TransportNetwork.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace interfaces;
	using namespace server;
	using namespace transportwebsite;
	

	template<> const string util::FactorableTemplate<InterfacePage,pt::LineMarkerInterfacePage>::FACTORY_KEY("line_marker");

	namespace pt
	{
		const string LineMarkerInterfacePage::DATA_STYLE_NAME("style");
		const string LineMarkerInterfacePage::DATA_IMAGE_URL("image");
		const string LineMarkerInterfacePage::DATA_SHORT_NAME("short_name");
		const string LineMarkerInterfacePage::DATA_NAME("name");
		const string LineMarkerInterfacePage::DATA_COLOR("color");
		const string LineMarkerInterfacePage::DATA_RANK("rank");
		const string LineMarkerInterfacePage::DATA_RANK_IS_ODD("rank_is_odd");
		const string LineMarkerInterfacePage::DATA_NETWORK_ID("network_id");
		const string LineMarkerInterfacePage::DATA_NETWORK_NAME("network_name");

		void LineMarkerInterfacePage::display(
			ostream& stream
			, VariablesMap& vars
			, const string& tableOpeningHTML
			, const string& tableClosingHTML
			, int pixelWidth
			, int pixelHeight
			, const CommercialLine& line
			, const Request* request /*= NULL */
		) const {
			ParametersVector pv;
			pv.push_back(tableOpeningHTML);
			pv.push_back(tableClosingHTML);
			pv.push_back(lexical_cast<string>(pixelWidth));
			pv.push_back(lexical_cast<string>(pixelHeight));
			pv.push_back(line.getStyle());
			pv.push_back(line.getImage());
			pv.push_back(line.getShortName());
			pv.push_back(lexical_cast<string>(line.getKey()));

			InterfacePage::_display(stream, pv, vars, static_cast<const void*>(&line), request);
		}



		LineMarkerInterfacePage::LineMarkerInterfacePage(
		):	Registrable(0)
		{

		}



		void LineMarkerInterfacePage::Display(
			std::ostream& stream,
			boost::shared_ptr<const transportwebsite::WebPage> page,
			const server::Request& request,
			const pt::CommercialLine& commercialLine,
			optional<size_t> rank
		){
			StaticFunctionRequest<WebPageDisplayFunction> displayRequest(request, false);
			displayRequest.getFunction()->setPage(page);
			displayRequest.getFunction()->setUseTemplate(false);
			ParametersMap pm;

			pm.insert(DATA_STYLE_NAME, commercialLine.getStyle());
			pm.insert(DATA_IMAGE_URL, commercialLine.getImage());
			pm.insert(DATA_SHORT_NAME, commercialLine.getShortName());
			pm.insert(Request::PARAMETER_OBJECT_ID, commercialLine.getKey());
			pm.insert(DATA_NAME, commercialLine.getName());
			if(commercialLine.getColor())
			{
				pm.insert(DATA_COLOR, commercialLine.getColor()->toString());
			}
			if(commercialLine.getNetwork())
			{
				pm.insert(DATA_NETWORK_ID, commercialLine.getNetwork()->getKey());
				pm.insert(DATA_NETWORK_NAME, commercialLine.getNetwork()->getName());
			}
			if(rank)
			{
				pm.insert(DATA_RANK, *rank);
				pm.insert(DATA_RANK_IS_ODD, *rank % 2);
			}

			displayRequest.getFunction()->setAditionnalParametersMap(pm);
			displayRequest.run(stream);
		}
	}
}
