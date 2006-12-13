
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
	using namespace routeplanner;
	using namespace util;
	using namespace env;

	namespace interfaces
	{
		void JourneyLineListInterfaceElement::parse( const std::string& text )
		{
			ValueElementList vel(text);
			_displayPedestrianLines = vel.front();
			_rowStartHtml = vel.front();
			_rowEndHtml = vel.front();
			_pixelWidth = vel.front();
			_pixelHeight = vel.front();
		}

		void JourneyLineListInterfaceElement::display( std::ostream& stream, const ParametersVector& parameters, const void* object /*= NULL*/, const server::Request* request /*= NULL*/ ) const
		{
		    const server::Site* site = request->getSite ();
 

			// Collecte des param�tres
			const Journey* __Trajet = ( const Journey* ) object;
			bool __AfficherLignesPied = Conversion::ToBool(_displayPedestrianLines->getValue(parameters));
			const LineMarkerInterfacePage* lineMarkerInterfacePage = site->getInterface()->getPage<LineMarkerInterfacePage>();

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
	}
}
