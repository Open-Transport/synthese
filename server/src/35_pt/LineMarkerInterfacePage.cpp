////////////////////////////////////////////////////////////////////////////////
/// LineMarkerInterfacePage class implementation.
///	@file LineMarkerInterfacePage.cpp
///	@author Hugues Romain
///	@date 2008-12-27 20:18
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
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
#include "TransportNetwork.h"
#include "Request.h"
#include "Webpage.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace cms;
	using namespace server;


	namespace pt
	{
		const string LineMarkerInterfacePage::DATA_STYLE_NAME("style");
		const string LineMarkerInterfacePage::DATA_IMAGE_URL("image");
		const string LineMarkerInterfacePage::DATA_SHORT_NAME("short_name");
		const string LineMarkerInterfacePage::DATA_NAME("name");
		const string LineMarkerInterfacePage::DATA_COLOR("color");
		const string LineMarkerInterfacePage::DATA_FOREGROUND_COLOR("foreground_color");
		const string LineMarkerInterfacePage::DATA_RANK("rank");
		const string LineMarkerInterfacePage::DATA_RANK_IS_ODD("rank_is_odd");
		const string LineMarkerInterfacePage::DATA_NETWORK_ID("network_id");
		const string LineMarkerInterfacePage::DATA_NETWORK_NAME("network_name");



		void LineMarkerInterfacePage::Display(
			std::ostream& stream,
			boost::shared_ptr<const cms::Webpage> page,
			const server::Request& request,
			const pt::CommercialLine& commercialLine,
			ParametersMap pm,
			optional<size_t> rank
		){
			pm.merge(request.getFunction()->getTemplateParameters());

			pm.insert(DATA_STYLE_NAME, commercialLine.getStyle());
			pm.insert(DATA_IMAGE_URL, commercialLine.getImage());
			pm.insert(DATA_SHORT_NAME, commercialLine.getShortName());
			pm.insert(Request::PARAMETER_OBJECT_ID, commercialLine.getKey());
			pm.insert(DATA_NAME, commercialLine.getName());
			if(commercialLine.getColor())
			{
				pm.insert(DATA_COLOR, commercialLine.getColor()->toXMLColor());
			}
			if(commercialLine.getFgColor())
			{
				pm.insert(DATA_FOREGROUND_COLOR, commercialLine.getFgColor()->toXMLColor());
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

			page->display(stream, request, pm);
		}
}	}
