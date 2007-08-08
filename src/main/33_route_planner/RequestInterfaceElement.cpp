
/** RequestInterfaceElement class implementation.
	@file RequestInterfaceElement.cpp

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

#include "RequestInterfaceElement.h"

#include "33_route_planner/RoutePlannerFunction.h"

#include "30_server/FunctionRequest.h"

#include "11_interfaces/ValueElementList.h"

#include "05_html/HTMLForm.h"

using namespace std;

namespace synthese
{
	using namespace interfaces;
	using namespace server;
	using namespace html;

	namespace routeplanner
	{
		void RequestInterfaceElement::storeParameters(ValueElementList& vel)
		{
			_formName = vel.front();
			_html = vel.front();
		}

		string RequestInterfaceElement::display(
			std::ostream& stream
			, const interfaces::ParametersVector& parameters
			, VariablesMap& variables
			, const void* object /*= NULL*/
			, const server::Request* request /*= NULL*/
		) const {

			string formName(_formName->getValue(parameters, variables, object, request));
			string html(_html->getValue(parameters, variables, object, request));
			
			FunctionRequest<RoutePlannerFunction> rprequest(request);
			rprequest.getFunction()->setMaxSolutions(request->getFunction<RoutePlannerFunction>()->getMaxSolutions());

			HTMLForm form(rprequest.getHTMLForm(formName));
			stream << form.open(html);
			stream << form.getHiddenFields();

/*			// Parametres cas validation fiche horaire
			if ( __Fonction == "timetable validation" )
			{
				request.addParameter( synthese::server::PARAMETER_FUNCTION, synthese::server::FUNCTION_SCHEDULE_SHEET_VALIDATION );

				request.addParameter( synthese::server::PARAMETER_DEPARTURE_CITY_NUMBER
					, _Parametres[ ELEMENTINTERFACEURLFormulaireVFHNumeroCommuneDepart ] ->Texte( __Parametres ) );
				request.addParameter( synthese::server::PARAMETER_DEPARTURE_STOP_NUMBER
					, _Parametres[ ELEMENTINTERFACEURLFormulaireVFHNumeroArretDepart ] ->Texte( __Parametres ) );
				request.addParameter( synthese::server::PARAMETER_DEPARTURE_WORDING_NUMBER
					, _Parametres[ ELEMENTINTERFACEURLFormulaireVFHNumeroDesignationDepart ] ->Texte( __Parametres ) );
				request.addParameter( synthese::server::PARAMETER_ARRIVAL_CITY_NUMBER
					, _Parametres[ ELEMENTINTERFACEURLFormulaireVFHNumeroCommuneArrivee ] ->Texte( __Parametres ) );
				request.addParameter( synthese::server::PARAMETER_ARRIVAL_STOP_NUMBER
					, _Parametres[ ELEMENTINTERFACEURLFormulaireVFHNumeroArretArrivee ] ->Texte( __Parametres ) );
				request.addParameter( synthese::server::PARAMETER_ARRIVAL_WORDING_NUMBER
					, _Parametres[ ELEMENTINTERFACEURLFormulaireVFHNumeroDesignationArrivee ] ->Texte( __Parametres ) );
			}
			// Pour fiche horaire seulement
			else if ( __Fonction == "timetable" )
			{
				request.addParameter( synthese::server::PARAMETER_FUNCTION, 
					synthese::server::FUNCTION_SCHEDULE_SHEET );

				request.addParameter( synthese::server::PARAMETER_DATE
					, _Parametres[ ELEMENTINTERFACEURLFormulaireFHDate ] ->Texte( __Parametres ) );
				request.addParameter( synthese::server::PARAMETER_PERIOD
					, _Parametres[ ELEMENTINTERFACEURLFormulaireFHPeriode ] ->Texte( __Parametres ) );
				request.addParameter( synthese::server::PARAMETER_BIKE
					, _Parametres[ ELEMENTINTERFACEURLFormulaireFHVelo ] ->Texte( __Parametres ) );
				request.addParameter( synthese::server::PARAMETER_HANDICAPPED
					, _Parametres[ ELEMENTINTERFACEURLFormulaireFHHandicape ] ->Texte( __Parametres ) );
				request.addParameter( synthese::server::PARAMETER_TAXIBUS
					, _Parametres[ ELEMENTINTERFACEURLFormulaireFHResa ] ->Texte( __Parametres ) );
				request.addParameter( synthese::server::PARAMETER_PRICE
					, _Parametres[ ELEMENTINTERFACEURLFormulaireFHTarif ] ->Texte( __Parametres ) );
				request.addParameter( synthese::server::PARAMETER_DEPARTURE_STOP_NUMBER
					, _Parametres[ ELEMENTINTERFACEURLFormulaireFHNumeroArretDepart ] ->Texte( __Parametres ) );
				request.addParameter( synthese::server::PARAMETER_DEPARTURE_WORDING_NUMBER
					, _Parametres[ ELEMENTINTERFACEURLFormulaireFHNumeroDesignationDepart ] ->Texte( __Parametres ) );
				request.addParameter( synthese::server::PARAMETER_ARRIVAL_STOP_NUMBER
					, _Parametres[ ELEMENTINTERFACEURLFormulaireFHNumeroArretArrivee ] ->Texte( __Parametres ) );
				request.addParameter( synthese::server::PARAMETER_ARRIVAL_WORDING_NUMBER
					, _Parametres[ ELEMENTINTERFACEURLFormulaireFHNumeroDesignationArrivee ] ->Texte( __Parametres ) );
			}
			else if ( __Fonction == ( "from city list" ) )
			{
				request.addParameter( synthese::server::PARAMETER_FUNCTION, synthese::server::FUNCTION_CITY_LIST );
				request.addParameter( synthese::server::PARAMETER_DIRECTION, 1 );
			}
			else if ( __Fonction == ( "to city list" ) )
			{
				request.addParameter( synthese::server::PARAMETER_FUNCTION, synthese::server::FUNCTION_CITY_LIST );
				request.addParameter( synthese::server::PARAMETER_DIRECTION, 0 );
			}
			else if ( __Fonction == ( "from station list" ) )
			{
				request.addParameter( synthese::server::PARAMETER_FUNCTION, synthese::server::FUNCTION_STOP_LIST );
				request.addParameter( synthese::server::PARAMETER_DIRECTION, 1 );
			}
			else if ( __Fonction == ( "to station list" ) )
			{
				request.addParameter( synthese::server::PARAMETER_FUNCTION, synthese::server::FUNCTION_STOP_LIST );
				request.addParameter( synthese::server::PARAMETER_DIRECTION, 0 );
			}

			// Affichage de la requ�te au format voulu
			if ( __TypeSortie == ( "url" ) )
			{
				// Partie ex�cutable de l'url
				pCtxt << __Site->getURLClient() << "?";

				// Champ fonction
				pCtxt << request.toInternalString ();
			}
			else if ( __TypeSortie.substr (0, 4) == "form" )
			{
				// Tag d'ouverture du formulaire
				pCtxt << "<form method=\"get\" action=\"" << __Site->getURLClient() << "\" "
					<< __TypeSortie.substr (5) << ">";

				for ( std::map<std::string, std::string>::const_iterator iter = request.getParameters().begin();
					iter != request.getParameters().end(); 
					++iter )
				{
					pCtxt << "<input type=\"hidden\" name=\"" << iter->first << "\" value=\"" << iter->second << "\" />";
				}
			}

*/


			return string();
		}

		RequestInterfaceElement::~RequestInterfaceElement()
		{
		}
	}
}
