
/** JourneyLineListInterfaceElement class implementation.
	@file JourneyLineListInterfaceElement.cpp

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

#include "JourneyLineListInterfaceElement.h"
#include "Journey.h"
#include "JourneyLeg.h"
#include "15_env/Road.h"
#include "15_env/LineMarkerInterfacePage.h"
#include "01_util/Conversion.h"
#include "11_interfaces/Interface.h"
#include "11_interfaces/ValueElementList.h"
#include "11_interfaces/ValueInterfaceElement.h"
#include "30_server/Request.h"
#include "30_server/Site.h"



namespace synthese
{
	using namespace interfaces;
	using namespace util;
	using namespace env;

	namespace routeplanner
	{
		void JourneyLineListInterfaceElement::display( std::ostream& stream, const ParametersVector& parameters, boost::shared_ptr<const void> object /*= NULL*/, const server::Request* request /*= NULL*/ ) const
		{
		    const server::Site* site = request->getSite ();
 

			// Collecte des param�tres
			const Journey* __Trajet = ( const Journey* ) object;
			bool __AfficherLignesPied = Conversion::ToBool(_displayPedestrianLines->getValue(parameters));
			const LineMarkerInterfacePage* const lineMarkerInterfacePage = site->getInterface()->getPage<LineMarkerInterfacePage>();

			// Fabrication de l'affichage
			for (int l=0; l<__Trajet->getJourneyLegCount (); ++l)
			{
				const JourneyLeg* __ET = __Trajet->getJourneyLeg (l);
				if ( __AfficherLignesPied || !dynamic_cast<const Road*> (__ET->getService ()->getPath ()) )
					lineMarkerInterfacePage->display(stream
						, _rowStartHtml->getValue(parameters)
						, _rowEndHtml->getValue(parameters)
						, Conversion::ToInt(_pixelWidth->getValue(parameters))
						, Conversion::ToInt(_pixelHeight->getValue(parameters))
						, (const Line*) __ET->getService ()->getPath ()
						, request);
			}
		}

		void JourneyLineListInterfaceElement::storeParameters( interfaces::ValueElementList& vel )
		{
			_displayPedestrianLines = vel.front();
			_rowStartHtml = vel.front();
			_rowEndHtml = vel.front();
			_pixelWidth = vel.front();
			_pixelHeight = vel.front();
		}
	}
}
