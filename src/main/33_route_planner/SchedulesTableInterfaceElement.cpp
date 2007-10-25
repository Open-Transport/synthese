
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

#include "33_route_planner/SchedulesTableInterfaceElement.h"
#include "33_route_planner/RoutePlannerNoSolutionInterfacePage.h"
#include "33_route_planner/RoutePlannerSheetColumnInterfacePage.h"
#include "33_route_planner/RoutePlannerSheetLineInterfacePage.h"

#include "30_server/Request.h"

#include "15_env/Service.h"
#include "15_env/Road.h"
#include "15_env/Edge.h"
#include "15_env/Vertex.h"
#include "15_env/ServiceUse.h"
#include "15_env/Line.h"
#include "15_env/Journey.h"

#include "11_interfaces/Interface.h"

#include "04_time/DateTime.h"

#include <vector>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace routeplanner;
	using namespace time;
	using namespace env;
	using namespace interfaces;

	template<> const string util::FactorableTemplate<LibraryInterfaceElement,routeplanner::SchedulesTableInterfaceElement>::FACTORY_KEY("schedules_table");
	
	namespace routeplanner
	{
		string SchedulesTableInterfaceElement::display(
			std::ostream& stream
			, const ParametersVector& parameters
			, VariablesMap& variables
			, const void* object /*= NULL*/
			, const server::Request* request /*= NULL*/ ) const
		{
			const JourneyBoardJourneys* jv(static_cast<const JourneyBoardJourneys*>(object));

			if ( jv == NULL || jv->empty())  // No solution or type error
			{
				const RoutePlannerNoSolutionInterfacePage* noSolutionPage = _page->getInterface()->getPage<RoutePlannerNoSolutionInterfacePage>();
				noSolutionPage->display(stream, request);
			}
			else
			{
				const PlaceList placesList(getStopsListForScheduleTable(*jv));
				Hour unknownTime( TIME_UNKNOWN );
				const RoutePlannerSheetColumnInterfacePage* columnInterfacePage(_page->getInterface()->getPage<RoutePlannerSheetColumnInterfacePage>());
				const RoutePlannerSheetLineInterfacePage* lineInterfacePage(_page->getInterface()->getPage<RoutePlannerSheetLineInterfacePage>());
				bool pedestrianMode = false;
				bool lastPedestrianMode = false;
				
				// Cells
				
				// Loop on each journey
				int i=1;
				for(JourneyBoardJourneys::const_iterator it(jv->begin());
					it != jv->end();
					++it, ++i
				){
					// Loop on each leg
					int __Ligne(0);
					const Journey::ServiceUses& jl((*it)->getServiceUses());
					for (Journey::ServiceUses::const_iterator itl(jl.begin()); itl != jl.end(); ++itl)
					{
						const ServiceUse& curET(*itl);

						if (itl == jl.begin() || !curET.getEdge()->getParentPath()->isPedestrianMode() || lastPedestrianMode != curET.getEdge()->getParentPath()->isPedestrianMode())
						{
							DateTime lastDateTime(curET.getDepartureDateTime());
							lastDateTime += (*it)->getContinuousServiceRange();

							for (; placesList[ __Ligne ].place != curET.getDepartureEdge()->getFromVertex ()->getConnectionPlace(); ++__Ligne)
								columnInterfacePage->display(
									*(placesList[__Ligne].content)
									, __Ligne==0
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
								*(placesList[__Ligne].content)
								, __Ligne == 0
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
							++__Ligne;
							lastPedestrianMode = pedestrianMode;
						}
						
						if(	itl == jl.end()-1
						||	!(itl+1)->getEdge()->getParentPath()->isPedestrianMode()
						||	!curET.getEdge()->getParentPath()->isPedestrianMode()
						){
							for (; placesList[ __Ligne ].place != curET.getArrivalEdge()->getFromVertex ()->getConnectionPlace(); __Ligne++ )
								columnInterfacePage->display(
									*(placesList[__Ligne].content)
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
							
							DateTime lastDateTime(curET.getArrivalDateTime());
							lastDateTime += (*it)->getContinuousServiceRange();

							columnInterfacePage->display(
								*(placesList[__Ligne].content)
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

					for (++__Ligne; __Ligne < placesList.size(); ++__Ligne)
						columnInterfacePage->display(
							*(placesList[__Ligne].content)
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

				// Initialization of text lines
				bool color(false);
				for (PlaceList::const_iterator it(placesList.begin()); it != placesList.end(); ++it)
				{
					lineInterfacePage->display(
						stream
						, it->content->str()
						, color
						, it->isOrigin
						, it->isDestination
						, variables
						, it->place
						, request
					);
					delete it->content;
					color = !color;
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



		int SchedulesTableInterfaceElement::OrdrePAEchangeSiPossible(
			const JourneyBoardJourneys& jv
			, PlaceList& pl
			, int PositionActuelle
			, int PositionGareSouhaitee
		){
			vector<bool> LignesAPermuter(PositionActuelle + 1, false);
			bool Echangeable(true);
			PlaceInformation tempGare;
			int i;
			int j;

			// Construction de l'ensemble des lignes a permuter
			LignesAPermuter[ PositionActuelle ] = true;
			int __i(0);
			for (JourneyBoardJourneys::const_iterator it = jv.begin(); it != jv.end(); ++it, ++__i )
			{
				vector<bool> curLignesET = OrdrePAConstruitLignesAPermuter( pl, **it, PositionActuelle );
				for ( i = PositionActuelle; i > PositionGareSouhaitee; i-- )
					if ( curLignesET[ i ] && LignesAPermuter[ i ] )
						break;
				for ( ; i > PositionGareSouhaitee; i-- )
					if ( curLignesET[ i ] )
						LignesAPermuter[ i ] = true;
			}

			// Tests d'ï¿changeabilitï¿ binaire
			// A la premiere contradiction on s'arrete
			__i=0;
			for (JourneyBoardJourneys::const_iterator it = jv.begin(); it != jv.end(); ++it, ++__i )
			{
				vector<bool> curLignesET = OrdrePAConstruitLignesAPermuter( pl, **it, PositionActuelle );
				i = PositionGareSouhaitee;
				for ( j = PositionGareSouhaitee; true; j++ )
				{
					for ( ; !LignesAPermuter[ i ]; i++ )
					{ }

					if ( i > PositionActuelle )
						break;

					if ( curLignesET[ i ] && curLignesET[ j ] && !LignesAPermuter[ j ] )
					{
						Echangeable = false;
						break;
					}
					i++;
				}
				if ( !Echangeable )
					break;
			}

			// Echange ou insertion
			if ( Echangeable )
			{
				for ( j = 0; true; j++ )
				{
					for ( i = j; !LignesAPermuter[ i ] && i <= PositionActuelle; i++ )
					{ }

					if ( i > PositionActuelle )
						break;

					LignesAPermuter[ i ] = false;

					tempGare = pl[ i ];
					for ( ; i > PositionGareSouhaitee + j; i-- )
						pl[i] = pl[i-1];

					pl[ i ] = tempGare;
				}
				return PositionGareSouhaitee + j;
			}
			else
				return OrdrePAInsere( pl, pl[ PositionGareSouhaitee ].place, PositionActuelle + 1, false, false);
		}



		int SchedulesTableInterfaceElement::OrdrePAInsere(
			PlaceList& pl
			, const ConnectionPlace* place
			, int position
			, bool lockedAtTheTop
			, bool lockedAtTheEnd
		){
			if (lockedAtTheEnd)
				position = pl.size();
			else if (lockedAtTheTop)
				position = 0;
			else
				for (; position < pl.size() && pl[position].isOrigin; ++position);

			// Insertion
			PlaceInformation pi;
			pi.content = new ostringstream;
			pi.isOrigin = lockedAtTheTop;
			pi.isDestination = lockedAtTheEnd;
			pi.place = place;

			pl.insert(pl.begin() + position, pi);

			// Retour de la position choisie
			return position;
		}

		vector<bool> SchedulesTableInterfaceElement::OrdrePAConstruitLignesAPermuter(
			const PlaceList& pl
			, const Journey& __TrajetATester
			, int LigneMax
		){
			vector<bool> result;
			int l(0);
			const ServiceUse* curET((l >= __TrajetATester.getJourneyLegCount ()) ? NULL : &__TrajetATester.getJourneyLeg (l));
			for (int i(0); pl[ i ].place != NULL && i <= LigneMax; i++ )
			{
				if ( curET != NULL && pl[ i ].place == curET->getDepartureEdge() ->getConnectionPlace() )
				{
					result.push_back(true);
					++l;
					curET = (l >= __TrajetATester.getJourneyLegCount ()) ? NULL : &__TrajetATester.getJourneyLeg (l);
				}
				else
				{
					result.push_back(false);
				}
			}
			return result;
		}

		bool SchedulesTableInterfaceElement::OrdrePARechercheGare(
			const PlaceList& pl
			, int& i
			, const ConnectionPlace* GareAChercher
		){
			// Recherche de la gare en suivant ï¿ partir de la position i
			for ( ; i < pl.size() && pl[ i ].place != NULL && pl[ i ].place != GareAChercher; ++i );

			// Gare trouvï¿e en suivant avant la fin du tableau
			if ( i < pl.size() && pl[ i ].place != NULL )
				return true;

			// Recherche de position antï¿rieure ï¿ i
			for ( i = 0; i < pl.size() && pl[ i ].place != NULL && pl[ i ].place != GareAChercher; ++i );

			return i < pl.size() && pl[ i ].place != NULL;

		}

		/** Build of the places list of a future schedule sheet corresponding to a journey vector.
			@author Hugues Romain
			@date 2001-2006

			Le but de la mï¿thode est de fournir une liste ordonnï¿e de points d'arrï¿t de taille minimale dï¿terminant les lignes du tableau de fiche horaire.

			Examples of results after journeys addings :

			Pas 0 : Service ABD (adding of B)

			<table class="Tableau" cellspacing="0" cellpadding="5">
			<tr><td>A</td><td>X</td></tr>
			<tr><td>B</td><td>X</td></tr>
			<tr><td>D</td><td>X</td></tr>
			</table>

			Pas 1 : Service ACD (adding of C)

			<table class="Tableau" cellspacing="0" cellpadding="5">
			<tr><td>A</td><td>X</td><td>X</td></tr>
			<tr><td>B</td><td>X</td><td|</td></tr>
			<tr><td>C</td><td>|</td><td>X</td></tr>
			<tr><td>D</td><td>X</td><td>X</td></tr>
			</table>

			Pas 2 : Service ACBD (change of the order authorized : descente de B au rang C+1)

			<table class="Tableau" cellspacing="0" cellpadding="5">
			<tr><td>A</td><td>X</td><td>X</td><td>X</td></tr>
			<tr><td>B</td><td>X</td><td>|</td><td>|</td></tr>
			<tr><td>C</td><td>|</td><td>X</td><td>X</td></tr>
			<tr><td>D</td><td>X</td><td>X</td><td>-</td></tr>
			</table>

			(permutation)

			<table class="Tableau" cellspacing="0" cellpadding="5">
			<tr><td>A</td><td>X</td><td>X</td><td>X</td></tr>
			<tr><td>C</td><td>|</td><td>X</td><td>X</td></tr>
			<tr><td>B</td><td>X</td><td>|</td><td>X</td></tr>
			<tr><td>D</td><td>X</td><td>X</td><td>X</td></tr>
			</table>

			Pas 3 : Service ABCD (change of the order refused : adding of an other C row)

			<table class="Tableau" cellspacing="0" cellpadding="5">
			<tr><td>A</td><td>X</td><td>X</td><td>X</td><td>X</td></tr>
			<tr><td>C</td><td>|</td><td>X</td><td>X</td><td>|</td></tr>
			<tr><td>B</td><td>X</td><td>|</td><td>X</td><td>X</td></tr>
			<tr><td>C</td><td>|</td><td>|</td><td>|</td><td>X</td></tr>
			<tr><td>D</td><td>X</td><td>X</td><td>X</td><td>X</td></tr>
			</table>

			Pas 4 : Service AB->CD (service continu BC)

			<table class="Tableau" cellspacing="0" cellpadding="5">
			<tr><td>A</td><td>X</td><td>X</td><td>X</td><td>X</td><td>X</td></tr>
			<tr><td>C</td><td>|</td><td>X</td><td>X</td><td>|</td><td>|</td></tr>
			<tr><td>B</td><td>X</td><td>|</td><td>X</td><td>X</td><td>V</td></tr>
			<tr><td>C</td><td>|</td><td>|</td><td>|</td><td>X</td><td>V</td></tr>
			<tr><td>D</td><td>X</td><td>X</td><td>X</td><td>X</td><td>X</td></tr>
			</table>

			Pas 5 : Service AED (E insï¿rï¿ avant B pour ne pas rompre la continuitï¿ BC)

			<table class="Tableau" cellspacing="0" cellpadding="5">
			<tr><td>A</td><td>X</td><td>X</td><td>X</td><td>X</td><td>X</td><td>X</td></tr>
			<tr><td>C</td><td>|</td><td>X</td><td>X</td><td>|</td><td>|</td><td>|</td></tr>
			<tr><td>E</td><td>|</td><td>|</td><td>|</td><td>|</td><td>|</td><td>X</td></tr>
			<tr><td>B</td><td>X</td><td>|</td><td>X</td><td>X</td><td>V</td><td>|</td></tr>
			<tr><td>C</td><td>|</td><td>|</td><td>|</td><td>X</td><td>V</td><td>|</td></tr>
			<tr><td>D</td><td>X</td><td>X</td><td>X</td><td>X</td><td>X</td><td>X</td></tr>
			</table>

			Pour chaque trajet, on procï¿de donc par balayage dans l'ordre des gares existantes. Si la gare ï¿ relier nï¿est pas trouvï¿e entre la position de la gare prï¿cï¿dente et la fin, deux solutions :
			- soit la gare nï¿est prï¿sente nulle part (balayage avant la position de la prï¿cï¿dente) auquel cas elle est crï¿ï¿e et rajoutï¿e ï¿ la position de la gare prï¿cï¿dente + 1
			- soit la gare est prï¿sente avant la gare prï¿cï¿dente. Dans ce cas, on tente de descendre la ligne de la gare recherchï¿e au niveau de la position de la gare prï¿cï¿dente + 1. On contrï¿le sur chacun des trajets prï¿cï¿dents que la chronologie n'en serait pas affectï¿e. Si elle ne l'est pas, alors la ligne est descendue. Sinon une nouvelle ligne est crï¿ï¿e.

			Contrôle de l'échangeabilité :

			Soit \f$ \delta_{l,c}:(l,c)\mapsto\{{1\mbox{~si~le~trajet~}c\mbox{~dessert~la~ligne~}l\atop 0~sinon} \f$

			Deux lignes l et m sont échangeables si et seulement si l'ordre des lignes dont \f$ \delta_{l,c}=1 \f$ pour chaque colonne est respecté.

			Cet ordre s'exprime par la propriété suivante : Si \f$ \Phi \f$ est la permutation pévue, alors

			<img width=283 height=27 src="interface.doxygen_fichiers/image008.gif">

			Il est donc nécessaire à la fois de contrôler la possibilité de permutation, et de la déterminer éventuellement.

			Si <sub><img width=25 height=24
			src="interface.doxygen_fichiers/image009.gif"></sub>est la ligne de la gare
			précédemment trouvée, et <sub><img width=24 height=24
			src="interface.doxygen_fichiers/image010.gif"></sub>ï¿lï¿emplacement de la gare
			souhaitï¿e pour permuter, alors les permutations ï¿ opï¿rer ne peuvent concerner
			que des lignes comprises entre <sub><img width=24 height=24
			src="interface.doxygen_fichiers/image010.gif"></sub>ï¿et <sub><img width=25
			height=24 src="interface.doxygen_fichiers/image009.gif"></sub>. En effet, les
			autres lignes nï¿influent pas.</p>


			En premier lieu il est nï¿cessaire de dï¿terminer lï¿ensemble
			des lignes ï¿ permuter. Cet ensemble est construit en explorant chaque colonne.
			Si <sub><img width=16 height=24 src="interface.doxygen_fichiers/image011.gif"></sub>ï¿est
			lï¿ensemble des lignes ï¿ permuter pour assurer lï¿intï¿gritï¿ des colonnes <sub><img
			width=36 height=27 src="interface.doxygen_fichiers/image012.gif"></sub>, on
			peut dï¿finir cet ensemble en fonction du prï¿cï¿dent <sub><img width=25
			height=24 src="interface.doxygen_fichiers/image013.gif"></sub>&nbsp;: <sub><img
			width=308 height=35 src="interface.doxygen_fichiers/image014.gif"></sub>

			Le but ï¿tant de faire descendre la ligne <sub><img width=24
			height=24 src="interface.doxygen_fichiers/image010.gif"></sub>ï¿vers <sub><img
			width=25 height=24 src="interface.doxygen_fichiers/image009.gif"></sub>, les
			lignes appartenant ï¿ L doivent ï¿tre ï¿changeables avecï¿ les positions <sub><img
			width=216 height=27 src="interface.doxygen_fichiers/image015.gif"></sub>.
			Lï¿ensemble de ces tests doit ï¿tre rï¿alisï¿. Au moindre ï¿chec, lï¿ensemble de la
			permutation est rendu impossible.

			Lï¿ï¿changeabilitï¿ binaire entre deux lignes l et m revient ï¿
			contrï¿ler la propriï¿tï¿&nbsp;<sub><img width=89 height=28
			src="interface.doxygen_fichiers/image016.gif"></sub>.

			Lï¿ï¿changeabilitï¿ totale sï¿ï¿crit donc <sub><img width=145
			height=28 src="interface.doxygen_fichiers/image017.gif"></sub>

			Lï¿algorithme est donc le suivant&nbsp;:

			- Construction de L
			- Contrï¿le dï¿ï¿changeabilitï¿ binaire pour chaque ï¿lï¿ment de L avec
			sa future position
			- Permutation

			<b>Echange</b>&nbsp;:

			Exemple dï¿ï¿change&nbsp;:

			<table class=MsoNormalTable border=1 cellspacing=0 cellpadding=0 width=340
			style='width:254.95pt;margin-left:141.6pt;border-collapse:collapse;border:
			none'>
			<tr>
			<td>
			<p class=Tableau>-</p>
			</td>
			<td>
			<p class=Tableau>-</p>
			</td>
			<td>
			<p class=Tableau>-</p>
			</td>
			<td>
			<p class=Tableau>-</p>
			</td>
			<td>
			<p class=Tableau>X</p>
			</td>
			<td>
			<p class=Tableau>-</p>
			</td>
			<td>
			<p class=Tableau>-</p>
			</td>
			<td>
			<p class=Tableau>-</p>
			</td>
			<td>
			<p class=Tableau>-</p>
			</td>
			<td>
			<p class=Tableau>X</p>
			</td>
			<td>
			<p class=Tableau>-</p>
			</td>
			<td>
			<p class=Tableau>-</p>
			</td>
			<td>
			<p class=Tableau>X</p>
			</td>
			</tr>
			</table>

			<span style='position:relative;z-index:16'><span
			style='left:0px;position:absolute;left:398px;top:-1px;width:67px;height:53px'><img
			width=67 height=53 src="interface.doxygen_fichiers/image018.gif"></span></span><span
			style='position:relative;z-index:13'><span style='left:0px;position:absolute;
			left:371px;top:-1px;width:67px;height:52px'><img width=67 height=52
			src="interface.doxygen_fichiers/image019.gif"></span></span><span
			style='position:relative;z-index:12'><span style='left:0px;position:absolute;
			left:349px;top:-1px;width:62px;height:53px'><img width=62 height=53
			src="interface.doxygen_fichiers/image020.gif"></span></span><span
			style='position:relative;z-index:11'><span style='left:0px;position:absolute;
			left:322px;top:-1px;width:69px;height:52px'><img width=69 height=52
			src="interface.doxygen_fichiers/image021.gif"></span></span><span
			style='position:relative;z-index:10'><span style='left:0px;position:absolute;
			left:269px;top:-1px;width:97px;height:53px'><img width=97 height=53
			src="interface.doxygen_fichiers/image022.gif"></span></span><span
			style='position:relative;z-index:14'><span style='left:0px;position:absolute;
			left:455px;top:-1px;width:37px;height:51px'><img width=37 height=51
			src="interface.doxygen_fichiers/image023.gif"></span></span><span
			style='position:relative;z-index:15'><span style='left:0px;position:absolute;
			left:482px;top:-1px;width:33px;height:51px'><img width=33 height=51
			src="interface.doxygen_fichiers/image024.gif"></span></span><span
			style='position:relative;z-index:6'><span style='left:0px;position:absolute;
			left:248px;top:-1px;width:262px;height:53px'><img width=262 height=53
			src="interface.doxygen_fichiers/image025.gif"></span></span><span
			style='position:relative;z-index:5'><span style='left:0px;position:absolute;
			left:221px;top:-1px;width:206px;height:53px'><img width=206 height=53
			src="interface.doxygen_fichiers/image026.gif"></span></span><span
			style='position:relative;z-index:7'><span style='left:0px;position:absolute;
			left:242px;top:-1px;width:97px;height:52px'><img width=97 height=52
			src="interface.doxygen_fichiers/image027.gif"></span></span><span
			style='position:relative;z-index:9'><span style='left:0px;position:absolute;
			left:216px;top:-1px;width:96px;height:52px'><img width=96 height=52
			src="interface.doxygen_fichiers/image028.gif"></span></span><span
			style='position:relative;z-index:8'><span style='left:0px;position:absolute;
			left:193px;top:-1px;width:96px;height:52px'><img width=96 height=52
			src="interface.doxygen_fichiers/image029.gif"></span></span><span
			style='position:relative;z-index:4'><span style='left:0px;position:absolute;
			left:194px;top:-1px;width:103px;height:52px'><img width=103 height=52
			src="interface.doxygen_fichiers/image030.gif"></span></span>

			<table>
			<tr>
			<td>
			<p class=Tableau>X</p>
			</td>
			<td>
			<p class=Tableau>X</p>
			</td>
			<td>
			<p class=Tableau>X</p>
			</td>
			<td>
			<p class=Tableau>-</p>
			</td>
			<td>
			<p class=Tableau>-</p>
			</td>
			<td>
			<p class=Tableau>-</p>
			</td>
			<td>
			<p class=Tableau>-</p>
			</td>
			<td>
			<p class=Tableau>-</p>
			</td>
			<td>
			<p class=Tableau>-</p>
			</td>
			<td>
			<p class=Tableau>-</p>
			</td>
			<td>
			<p class=Tableau>-</p>
			</td>
			<td>
			<p class=Tableau>-</p>
			</td>
			<td>
			<p class=Tableau>-</p>
			</td>
			</tr>
			</table>

		*/
		SchedulesTableInterfaceElement::PlaceList SchedulesTableInterfaceElement::getStopsListForScheduleTable(
			const JourneyBoardJourneys& jv
		){
			// Variables locales
			int i;
			int dernieri;

			// Allocation
			PlaceList pl;

			// Horizontal loop
			for (JourneyBoardJourneys::const_iterator it(jv.begin()); it != jv.end(); ++it)
			{
				i = 0;
				dernieri = -1;

				// Vertical loop
				const Journey::ServiceUses& jl((*it)->getServiceUses());
				for (Journey::ServiceUses::const_iterator itl(jl.begin()); itl != jl.end(); ++itl)
				{
					const ServiceUse& curET(*itl);
					
					// Search of the place from the preceding one
					if (itl == jl.begin() || !curET.getEdge()->getParentPath()->isPedestrianMode())
					{
						if ( OrdrePARechercheGare( pl, i, curET.getDepartureEdge()->getConnectionPlace() ) )
						{
							if ( i < dernieri )
								i = OrdrePAEchangeSiPossible( jv, pl, dernieri, i );
						}
						else
						{
							i = OrdrePAInsere( pl, curET.getDepartureEdge()->getConnectionPlace(), dernieri + 1, itl == jl.begin(), false);
						}

						dernieri = i;
						++i;
					}

					if (itl == jl.end()-1 || !curET.getEdge()->getParentPath()->isPedestrianMode())
					{
						if ( OrdrePARechercheGare( pl, i, curET.getArrivalEdge()->getConnectionPlace() ) )
						{
							if ( i < dernieri )
								i=OrdrePAEchangeSiPossible(jv, pl, dernieri, i );
						}
						else
						{
							i = OrdrePAInsere( pl, curET.getArrivalEdge()->getConnectionPlace(), dernieri + 1, false, itl == jl.end()-1);
						}
						dernieri = i;
					}
				}
			}

			return pl;
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
