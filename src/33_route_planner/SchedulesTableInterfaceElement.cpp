
/** SchedulesTableInterfaceElement class implementation.
	@file SchedulesTableInterfaceElement.cpp

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

#include "SchedulesTableInterfaceElement.h"
#include "RoutePlannerSheetColumnInterfacePage.h"
#include "RoutePlannerSheetLineInterfacePage.h"
#include "PTRoutePlannerResult.h"
#include "Request.h"
#include "Hub.h"
#include "Service.h"
#include "Road.h"
#include "Edge.h"
#include "Vertex.h"
#include "ServiceUse.h"
#include "Line.h"
#include "Journey.h"
#include "Crossing.h"
#include "AddressablePlace.h"
#include "Interface.h"
#include "NamedPlace.h"

#include "DateTime.h"

#include <vector>
#include <boost/foreach.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace routeplanner;
	using namespace time;
	using namespace geography;
	using namespace interfaces;
	using namespace graph;
	using namespace road;
	

	template<> const string util::FactorableTemplate<LibraryInterfaceElement,SchedulesTableInterfaceElement>::FACTORY_KEY(
		"schedules_table"
	);
	
	namespace routeplanner
	{
		string SchedulesTableInterfaceElement::display(
			std::ostream& stream
			, const ParametersVector& parameters
			, VariablesMap& variables
			, const void* object /*= NULL*/
			, const server::Request* request /*= NULL*/
		) const {
			const PTRoutePlannerResult* jv(static_cast<const PTRoutePlannerResult*>(object));

			if ( jv != NULL && !jv->getJourneys().empty())  // No solution or type error
			{
				const PTRoutePlannerResult::PlaceList& placesList(
					jv->getOrderedPlaces()
				);
				Hour unknownTime( TIME_UNKNOWN );
				const RoutePlannerSheetColumnInterfacePage* columnInterfacePage(
					_page->getInterface()->getPage<RoutePlannerSheetColumnInterfacePage>()
				);
				const RoutePlannerSheetLineInterfacePage* lineInterfacePage(
					_page->getInterface()->getPage<RoutePlannerSheetLineInterfacePage>()
				);
				typedef vector<ostringstream*> PlacesContentVector;
				PlacesContentVector sheetRows(placesList.size());
				BOOST_FOREACH(PlacesContentVector::value_type& stream, sheetRows)
				{
					stream = new ostringstream;
				}
				
				// Cells
				
				// Loop on each journey
				int i=1;
				for(PTRoutePlannerResult::Journeys::const_iterator it(jv->getJourneys().begin());
					it != jv->getJourneys().end();
					++it, ++i
				){

					bool pedestrianMode = false;
					bool lastPedestrianMode = false;

					PlacesContentVector::iterator itSheetRow(sheetRows.begin());
					PTRoutePlannerResult::PlaceList::const_iterator itPlaces(placesList.begin());

					// Loop on each leg
					const Journey::ServiceUses& jl((*it)->getServiceUses());
					for (Journey::ServiceUses::const_iterator itl(jl.begin()); itl != jl.end(); ++itl)
					{
						const ServiceUse& curET(*itl);

						if(	itl == jl.begin() ||
							!curET.getEdge()->getParentPath()->isPedestrianMode() ||
							lastPedestrianMode != curET.getEdge()->getParentPath()->isPedestrianMode()
						){
							const NamedPlace* placeToSearch(
 								(	itl == jl.begin() &&
 									dynamic_cast<const Crossing*>(curET.getDepartureEdge()->getHub())
 								) ?
 								dynamic_cast<const NamedPlace*>(jv->getDeparturePlace()) :
								dynamic_cast<const NamedPlace*>(curET.getDepartureEdge()->getHub())
							);

							DateTime lastDateTime(curET.getDepartureDateTime());
							lastDateTime += (*it)->getContinuousServiceRange();

							for (; itPlaces->place != placeToSearch; ++itPlaces, ++itSheetRow)
								columnInterfacePage->display(
									**itSheetRow
									, itPlaces == jv->getOrderedPlaces().begin()
									, (itl + 1) == jl.end()
									, i
									, pedestrianMode
									, unknownTime
									, unknownTime
									, false
									, true
									, true
									, false
									, request
								);

							pedestrianMode = curET.getEdge()->getParentPath()->isPedestrianMode();
							
							// Saving of the columns on each lines
							columnInterfacePage->display(
								**itSheetRow
								, itPlaces == jv->getOrderedPlaces().begin()
								, true
								, i
								, pedestrianMode
								, curET.getDepartureDateTime().getHour()
								, lastDateTime.getHour()
								, (*it)->getContinuousServiceRange() > 0
								, itl == jl.begin()
								, true
								, pedestrianMode && !lastPedestrianMode
								, request
							);
							++itPlaces; ++itSheetRow;
							lastPedestrianMode = pedestrianMode;
						}
						
						if(	itl == jl.end()-1
						||	!(itl+1)->getEdge()->getParentPath()->isPedestrianMode()
						||	!curET.getEdge()->getParentPath()->isPedestrianMode()
						){
							const NamedPlace* placeToSearch(
								itl == jl.end()-1 && dynamic_cast<const Crossing*>(curET.getArrivalEdge()->getHub()) ?
								dynamic_cast<const NamedPlace*>(jv->getArrivalPlace()) :
								dynamic_cast<const NamedPlace*>(curET.getArrivalEdge()->getHub())
							);
							
							for (; itPlaces->place != placeToSearch; ++itPlaces, ++itSheetRow )
							{
								columnInterfacePage->display(
									**itSheetRow
									, true
									, true
									, i
									, pedestrianMode
									, unknownTime
									, unknownTime
									, false
									, true
									, true
									, false
									, request
								);
							}
							
							DateTime lastDateTime(curET.getArrivalDateTime());
							lastDateTime += (*it)->getContinuousServiceRange();

							columnInterfacePage->display(
								**itSheetRow
								, true
								, (itl + 1) == jl.end()
								, i
								, pedestrianMode
								, curET.getArrivalDateTime().getHour ()
								, lastDateTime.getHour()
								, (*it)->getContinuousServiceRange() > 0
								, true
								, (itl + 1) == jl.end()
								, false
								, request
							);
						}
					}

					for (++itPlaces, ++itSheetRow; itPlaces != placesList.end(); ++itPlaces, ++itSheetRow)
					{
						columnInterfacePage->display(
							**itSheetRow
							, true
							, true
							, i
							, false
							, unknownTime
							, unknownTime
							, false
							, true
							, true
							, false
							, request
						);
					}
				}

				// Initialization of text lines
				bool color(false);
				PlacesContentVector::const_iterator it(sheetRows.begin());
				BOOST_FOREACH(const PTRoutePlannerResult::PlaceList::value_type& pi, placesList)
				{
					assert(dynamic_cast<const NamedPlace*>(pi.place));

					lineInterfacePage->display(
						stream
						, (*it)->str()
						, color
						, pi.isOrigin
						, pi.isDestination
						, variables
						, *dynamic_cast<const NamedPlace*>(pi.place)
						, request
					);
					delete pi.content;
					color = !color;
					++it;
				}


				/*
				// GESTION DES ALERTES
				// Gestion des alertes : 3 cas possibles :
				// Alerte sur arrï¿t de dï¿part
				// Circulation ï¿ rï¿servation
				// Alerte sur circulation
				// Alerte sur arrï¿t d'arrivï¿e
				synthese::time::DateTime __debutAlerte, __finAlerte;

				// Alerte sur arrï¿t de dï¿part
				// Dï¿but alerte = premier dï¿part
				// Fin alerte = dernier dï¿part
				synthese::time::DateTime debutPrem = curET->getDepartureTime();
				synthese::time::DateTime finPrem = debutPrem;
				if (__Trajet->getContinuousServiceRange ().Valeur())
				finPrem += __Trajet->getContinuousServiceRange ();
				if (curET->getGareDepart()->getAlarm().showMessage(__debutAlerte, __finAlerte)
				&& __NiveauRenvoiColonne < curET->getGareDepart()->getAlarm().Niveau())
				__NiveauRenvoiColonne = curET->getGareDepart()->getAlarm().Niveau();

				// Circulation ï¿ rï¿servation obligatoire
				synthese::time::DateTime maintenant;
				maintenant.setMoment();
				if (curET->getLigne()->GetResa()->TypeResa() == Obligatoire
				&& curET->getLigne()->GetResa()->reservationPossible(curET->getLigne()->GetTrain(curET->getService()), maintenant, curET->getDepartureTime())
				&& __NiveauRenvoiColonne < ALERTE_ATTENTION)
				__NiveauRenvoiColonne = ALERTE_ATTENTION;

				// Circulation ï¿ rï¿servation possible
				maintenant.setMoment();
				if (curET->getLigne()->GetResa()->TypeResa() == Facultative
				&& curET->getLigne()->GetResa()->reservationPossible(curET->getLigne()->GetTrain(curET->getService()), maintenant, curET->getDepartureTime())
				&& __NiveauRenvoiColonne < ALERTE_INFO)
				__NiveauRenvoiColonne = ALERTE_INFO;

				// Alerte sur circulation
				// Dï¿but alerte = premier dï¿part
				// Fin alerte = derniï¿re arrivï¿e
				debutPrem = curET->getDepartureTime();
				finPrem = curET->getArrivalTime ();
				if (__Trajet->getContinuousServiceRange ().Valeur())
				finPrem += __Trajet->getContinuousServiceRange ();
				if (curET->getLigne()->getAlarm().showMessage(__debutAlerte, __finAlerte)
				&& __NiveauRenvoiColonne < curET->getLigne()->getAlarm().Niveau())
				__NiveauRenvoiColonne = curET->getLigne()->getAlarm().Niveau();

				// Alerte sur arrï¿t d'arrivï¿e
				// Dï¿but alerte = premiï¿re arrivï¿e
				// Fin alerte = dernier dï¿part de l'arrï¿t si correspondnce, derniï¿re arrivï¿e sinon
				__debutAlerte = curET->getArrivalTime ();
				__finAlerte = __debutAlerte;
				if (curET->Suivant() != NULL)
				__finAlerte = curET->Suivant()->getDepartureTime();
				if (__Trajet->getContinuousServiceRange ().Valeur())
				__finAlerte += __Trajet->getContinuousServiceRange ();
				if (curET->getGareArrivee()->getAlarm().showMessage(__debutAlerte, __finAlerte)
				&& __NiveauRenvoiColonne < curET->getGareArrivee()->getAlarm().Niveau())
				__NiveauRenvoiColonne = curET->getGareArrivee()->getAlarm().Niveau();
				}

				// Affichage du renvoi si necessaire
				//    if (__NiveauRenvoiColonne)
				//    {
				//     TamponRenvois << "<img src=\"" << __RepBI << "img/warning.gif\" alt=\"Cliquez sur la colonne pour plus d'informations\" />";
				//     __MontrerLigneRenvois = true;
				//    }

				for (l++;l!=m;l++)
				{
				Tampons[l] << "<td class=\"tdHoraires";
				if (Couleur1)
				Tampons[l] << "1";
				else
				Tampons[l] << "2";
				Tampons[l] << "\" onclick=\"show('divFiche";
				Tampons[l].Copie(n,3);
				Tampons[l] << "')\">";
				if (Couleur1)
				Couleur1=false;
				else
				Couleur1=true;
				}

				//    pCtxt << "</tr></table></div>";
				//    TamponRenvois << "</td>";
				}
				*/

				//    delete [] Tampons;
			}
			return string();
		}



		void SchedulesTableInterfaceElement::storeParameters( interfaces::ValueElementList& vel )
		{
			
		}



	
		/*! \brief Crï¿ation des niveaux d'alerte des trajets en fonction des donnï¿es lignes et arrï¿ts
		\author Hugues Romain
		\date 2005
		*/
/*		void cTrajets::GenererNiveauxEtAuMoinsUneAlerte()
		{
			// Variables locales
			bool __AuMoinsUneAlerte = false;

			// Calcul des niveaux d'alerte pour chaque trajet et collecte du resultat
			for ( int __i = 0; __i < Taille(); __i++ )
			{
				__AuMoinsUneAlerte = getElement( __i ).getMaxAlarmLevel () || __AuMoinsUneAlerte;
			}

			// Stockage du resultat final de la liste de trajets sous format texte pour 
			// exploitabilite directe par module d'interface
			if ( __AuMoinsUneAlerte )
				_AuMoinsUneAlerte = "1";
			else
				_AuMoinsUneAlerte.clear();
		}
*/


	}
}
