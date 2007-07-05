
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

#include "11_interfaces/Interface.h"

#include "04_time/DateTime.h"

#include <vector>
#include <sstream>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace routeplanner;
	using namespace time;
	using namespace env;
	using namespace interfaces;
	
	namespace routeplanner
	{
		string SchedulesTableInterfaceElement::display(
			std::ostream& stream
			, const ParametersVector& parameters
			, VariablesMap& variables
			, const void* object /*= NULL*/
			, const server::Request* request /*= NULL*/ ) const
		{
			const Journeys* jv = static_cast<const Journeys*>(object);

			if ( jv == NULL || jv->empty())  // No solution or type error
			{
				shared_ptr<const RoutePlannerNoSolutionInterfacePage> noSolutionPage = _page->getInterface()->getPage<RoutePlannerNoSolutionInterfacePage>();
				noSolutionPage->display(stream, request);
			}
			else
			{
				size_t __Ligne;
				const PlaceList placesList = getStopsListForScheduleTable( *jv );
				DateTime lastDepartureDateTime;
				DateTime lastArrivalDateTime;
				Hour unknownTime( TIME_UNKNOWN );
				shared_ptr<const RoutePlannerSheetColumnInterfacePage> columnInterfacePage = _page->getInterface()->getPage<RoutePlannerSheetColumnInterfacePage>();
				shared_ptr<const RoutePlannerSheetLineInterfacePage> lineInterfacePage = _page->getInterface()->getPage<RoutePlannerSheetLineInterfacePage>();

				// Initialization of text lines
				vector<ostringstream*> __Tampons(placesList.size());
				for (vector<ostringstream*>::iterator it = __Tampons.begin(); it != __Tampons.end(); ++it)
					*it = new ostringstream;

				// Loop on each journey
				int i=1;
				for (Journeys::const_iterator it = jv->begin(); it != jv->end(); ++it, ++i )
				{
					// Loop on each leg
					__Ligne=0;
					for (int l=0; l< it->getJourneyLegCount(); ++l)
					{
						const ServiceUse& curET(it->getJourneyLeg (l));
						
						// Saving of the columns on each lines
						columnInterfacePage->display( *__Tampons[__Ligne]
							, __Ligne == 0, true, i, dynamic_cast<const Road*> (curET.getService()->getPath ()) != NULL
							, curET.getDepartureDateTime().getHour(), lastDepartureDateTime.getHour(), it->getContinuousServiceRange() > 0
							, request );
						
						for ( __Ligne++; placesList[ __Ligne ] != curET.getArrivalEdge()->getFromVertex ()->getConnectionPlace(); __Ligne++ )
							columnInterfacePage->display( *__Tampons[ __Ligne ]
								, true, true, i, false, unknownTime, unknownTime, false
								, request );
						
						columnInterfacePage->display( *__Tampons[ __Ligne ] 
							, true, l == it->getJourneyLegCount ()-1, i, dynamic_cast<const Road*> (curET.getService()->getPath ()) != NULL
							, curET.getArrivalDateTime().getHour (), lastArrivalDateTime.getHour(), it->getContinuousServiceRange() > 0
							, request );
					}
				}

				// Initialization of text lines
				bool __Couleur = false;
				for ( __Ligne = 0; __Ligne < placesList.size(); __Ligne++ )
				{
					lineInterfacePage->display(
						stream
						, __Tampons[ __Ligne ]->str()
						, __Couleur
						, variables
						, placesList[ __Ligne ]
						, request );
				}

				// Cleaning the string vector
				for (vector<ostringstream*>::iterator it = __Tampons.begin(); it != __Tampons.end(); ++it)
					delete *it;


				/*
				// GESTION DES ALERTES
				// Gestion des alertes : 3 cas possibles :
				// Alerte sur arrï¿½t de dï¿½part
				// Circulation ï¿½ rï¿½servation
				// Alerte sur circulation
				// Alerte sur arrï¿½t d'arrivï¿½e
				synthese::time::DateTime __debutAlerte, __finAlerte;

				// Alerte sur arrï¿½t de dï¿½part
				// Dï¿½but alerte = premier dï¿½part
				// Fin alerte = dernier dï¿½part
				synthese::time::DateTime debutPrem = curET->getDepartureTime();
				synthese::time::DateTime finPrem = debutPrem;
				if (__Trajet->getContinuousServiceRange ().Valeur())
				finPrem += __Trajet->getContinuousServiceRange ();
				if (curET->getGareDepart()->getAlarm().showMessage(__debutAlerte, __finAlerte)
				&& __NiveauRenvoiColonne < curET->getGareDepart()->getAlarm().Niveau())
				__NiveauRenvoiColonne = curET->getGareDepart()->getAlarm().Niveau();

				// Circulation ï¿½ rï¿½servation obligatoire
				synthese::time::DateTime maintenant;
				maintenant.setMoment();
				if (curET->getLigne()->GetResa()->TypeResa() == Obligatoire
				&& curET->getLigne()->GetResa()->reservationPossible(curET->getLigne()->GetTrain(curET->getService()), maintenant, curET->getDepartureTime())
				&& __NiveauRenvoiColonne < ALERTE_ATTENTION)
				__NiveauRenvoiColonne = ALERTE_ATTENTION;

				// Circulation ï¿½ rï¿½servation possible
				maintenant.setMoment();
				if (curET->getLigne()->GetResa()->TypeResa() == Facultative
				&& curET->getLigne()->GetResa()->reservationPossible(curET->getLigne()->GetTrain(curET->getService()), maintenant, curET->getDepartureTime())
				&& __NiveauRenvoiColonne < ALERTE_INFO)
				__NiveauRenvoiColonne = ALERTE_INFO;

				// Alerte sur circulation
				// Dï¿½but alerte = premier dï¿½part
				// Fin alerte = derniï¿½re arrivï¿½e
				debutPrem = curET->getDepartureTime();
				finPrem = curET->getArrivalTime ();
				if (__Trajet->getContinuousServiceRange ().Valeur())
				finPrem += __Trajet->getContinuousServiceRange ();
				if (curET->getLigne()->getAlarm().showMessage(__debutAlerte, __finAlerte)
				&& __NiveauRenvoiColonne < curET->getLigne()->getAlarm().Niveau())
				__NiveauRenvoiColonne = curET->getLigne()->getAlarm().Niveau();

				// Alerte sur arrï¿½t d'arrivï¿½e
				// Dï¿½but alerte = premiï¿½re arrivï¿½e
				// Fin alerte = dernier dï¿½part de l'arrï¿½t si correspondnce, derniï¿½re arrivï¿½e sinon
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

		size_t SchedulesTableInterfaceElement::OrdrePAEchangeSiPossible( const Journeys& jv, PlaceList& pl, const LockedLinesList& lll, size_t PositionActuelle, size_t PositionGareSouhaitee )
		{
			vector<bool> LignesAPermuter(PositionActuelle + 1, false);
			bool Echangeable = true;
			const ConnectionPlace* tempGare;
			size_t i;
			size_t j;

			// Construction de l'ensemble des lignes a permuter
			LignesAPermuter[ PositionActuelle ] = true;
			size_t __i=0;
			for ( Journeys::const_iterator it = jv.begin(); it != jv.end(); ++it, ++__i )
			{
				vector<bool> curLignesET = OrdrePAConstruitLignesAPermuter( pl, *it, PositionActuelle );
				for ( i = PositionActuelle; i > PositionGareSouhaitee; i-- )
					if ( curLignesET[ i ] && LignesAPermuter[ i ] )
						break;
				for ( ; i > PositionGareSouhaitee; i-- )
					if ( curLignesET[ i ] )
						LignesAPermuter[ i ] = true;
			}

			// Tests d'ï¿½changeabilitï¿½ binaire
			// A la premiere contradiction on s'arrete
			__i=0;
			for ( Journeys::const_iterator it = jv.begin(); it != jv.end(); ++it, ++__i )
			{
				vector<bool> curLignesET = OrdrePAConstruitLignesAPermuter( pl, *it, PositionActuelle );
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
/// @todo Write on lll too !
					tempGare = pl[ i ];
					for ( ; i > PositionGareSouhaitee + j; i-- )
						pl[i] = pl[i-1];
					pl[ i ] = tempGare;
				}
				return PositionGareSouhaitee + j;
			}
			else
				return OrdrePAInsere( pl, lll, pl[ PositionGareSouhaitee ], PositionActuelle + 1 );

		}

		size_t SchedulesTableInterfaceElement::OrdrePAInsere(PlaceList& pl, const LockedLinesList& lll, const synthese::env::ConnectionPlace* place, size_t position )
		{
			// Saut de ligne vérouillée par un cheminement piéton
			for (; position < lll.size() && lll[position]; ++position);

			// Insertion
			pl.insert(pl.begin() + position, place); /// @todo why not insert a false in ll ???

			// Retour de la position choisie
			return position;

			/// @todo update lll too !
		}

		vector<bool> SchedulesTableInterfaceElement::OrdrePAConstruitLignesAPermuter( const PlaceList& pl, const Journey& __TrajetATester, size_t LigneMax )
		{
			vector<bool> result;
			int l = 0;
			const ServiceUse* curET((l >= __TrajetATester.getJourneyLegCount ()) ? NULL : &__TrajetATester.getJourneyLeg (l));
			for (size_t i = 0; pl[ i ] != NULL && i <= LigneMax; i++ )
			{
				if ( curET != NULL && pl[ i ] == curET->getDepartureEdge() ->getConnectionPlace() )
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

		bool SchedulesTableInterfaceElement::OrdrePARechercheGare( const PlaceList& pl, size_t& i, const synthese::env::ConnectionPlace* GareAChercher )
		{
			// Recherche de la gare en suivant ï¿½ partir de la position i
			for ( ; i < pl.size() && pl[ i ] != NULL && pl[ i ] != GareAChercher; ++i );

			// Gare trouvï¿½e en suivant avant la fin du tableau
			if ( i < pl.size() && pl[ i ] != NULL )
				return true;

			// Recherche de position antï¿½rieure ï¿½ i
			for ( i = 0; i < pl.size() && pl[ i ] != NULL && pl[ i ] != GareAChercher; ++i );

			return i < pl.size() && pl[ i ] != NULL;

		}

		/** Build of the places list of a future schedule sheet corresponding to a journey vector.
			@author Hugues Romain
			@date 2001-2006

			Le but de la mï¿½thode est de fournir une liste ordonnï¿½e de points d'arrï¿½t de taille minimale dï¿½terminant les lignes du tableau de fiche horaire.

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

			Pas 5 : Service AED (E insï¿½rï¿½ avant B pour ne pas rompre la continuitï¿½ BC)

			<table class="Tableau" cellspacing="0" cellpadding="5">
			<tr><td>A</td><td>X</td><td>X</td><td>X</td><td>X</td><td>X</td><td>X</td></tr>
			<tr><td>C</td><td>|</td><td>X</td><td>X</td><td>|</td><td>|</td><td>|</td></tr>
			<tr><td>E</td><td>|</td><td>|</td><td>|</td><td>|</td><td>|</td><td>X</td></tr>
			<tr><td>B</td><td>X</td><td>|</td><td>X</td><td>X</td><td>V</td><td>|</td></tr>
			<tr><td>C</td><td>|</td><td>|</td><td>|</td><td>X</td><td>V</td><td>|</td></tr>
			<tr><td>D</td><td>X</td><td>X</td><td>X</td><td>X</td><td>X</td><td>X</td></tr>
			</table>

			Pour chaque trajet, on procï¿½de donc par balayage dans l'ordre des gares existantes. Si la gare ï¿½ relier nï¿½est pas trouvï¿½e entre la position de la gare prï¿½cï¿½dente et la fin, deux solutions :
			- soit la gare nï¿½est prï¿½sente nulle part (balayage avant la position de la prï¿½cï¿½dente) auquel cas elle est crï¿½ï¿½e et rajoutï¿½e ï¿½ la position de la gare prï¿½cï¿½dente + 1
			- soit la gare est prï¿½sente avant la gare prï¿½cï¿½dente. Dans ce cas, on tente de descendre la ligne de la gare recherchï¿½e au niveau de la position de la gare prï¿½cï¿½dente + 1. On contrï¿½le sur chacun des trajets prï¿½cï¿½dents que la chronologie n'en serait pas affectï¿½e. Si elle ne l'est pas, alors la ligne est descendue. Sinon une nouvelle ligne est crï¿½ï¿½e.

			Contrôle de l'échangeabilité :

			Soit \f$ \delta_{l,c}:(l,c)\mapsto\{{1\mbox{~si~le~trajet~}c\mbox{~dessert~la~ligne~}l\atop 0~sinon} \f$

			Deux lignes l et m sont échangeables si et seulement si l'ordre des lignes dont \f$ \delta_{l,c}=1 \f$ pour chaque colonne est respecté.

			Cet ordre s'exprime par la propriété suivante : Si \f$ \Phi \f$ est la permutation pévue, alors

			<img width=283 height=27 src="interface.doxygen_fichiers/image008.gif">

			Il est donc nécessaire à la fois de contrôler la possibilité de permutation, et de la déterminer éventuellement.

			Si <sub><img width=25 height=24
			src="interface.doxygen_fichiers/image009.gif"></sub>est la ligne de la gare
			précédemment trouvée, et <sub><img width=24 height=24
			src="interface.doxygen_fichiers/image010.gif"></sub>ï¿½lï¿½emplacement de la gare
			souhaitï¿½e pour permuter, alors les permutations ï¿½ opï¿½rer ne peuvent concerner
			que des lignes comprises entre <sub><img width=24 height=24
			src="interface.doxygen_fichiers/image010.gif"></sub>ï¿½et <sub><img width=25
			height=24 src="interface.doxygen_fichiers/image009.gif"></sub>. En effet, les
			autres lignes nï¿½influent pas.</p>


			En premier lieu il est nï¿½cessaire de dï¿½terminer lï¿½ensemble
			des lignes ï¿½ permuter. Cet ensemble est construit en explorant chaque colonne.
			Si <sub><img width=16 height=24 src="interface.doxygen_fichiers/image011.gif"></sub>ï¿½est
			lï¿½ensemble des lignes ï¿½ permuter pour assurer lï¿½intï¿½gritï¿½ des colonnes <sub><img
			width=36 height=27 src="interface.doxygen_fichiers/image012.gif"></sub>, on
			peut dï¿½finir cet ensemble en fonction du prï¿½cï¿½dent <sub><img width=25
			height=24 src="interface.doxygen_fichiers/image013.gif"></sub>&nbsp;: <sub><img
			width=308 height=35 src="interface.doxygen_fichiers/image014.gif"></sub>

			Le but ï¿½tant de faire descendre la ligne <sub><img width=24
			height=24 src="interface.doxygen_fichiers/image010.gif"></sub>ï¿½vers <sub><img
			width=25 height=24 src="interface.doxygen_fichiers/image009.gif"></sub>, les
			lignes appartenant ï¿½ L doivent ï¿½tre ï¿½changeables avecï¿½ les positions <sub><img
			width=216 height=27 src="interface.doxygen_fichiers/image015.gif"></sub>.
			Lï¿½ensemble de ces tests doit ï¿½tre rï¿½alisï¿½. Au moindre ï¿½chec, lï¿½ensemble de la
			permutation est rendu impossible.

			Lï¿½ï¿½changeabilitï¿½ binaire entre deux lignes l et m revient ï¿½
			contrï¿½ler la propriï¿½tï¿½&nbsp;<sub><img width=89 height=28
			src="interface.doxygen_fichiers/image016.gif"></sub>.

			Lï¿½ï¿½changeabilitï¿½ totale sï¿½ï¿½crit donc <sub><img width=145
			height=28 src="interface.doxygen_fichiers/image017.gif"></sub>

			Lï¿½algorithme est donc le suivant&nbsp;:

			- Construction de L
			- Contrï¿½le dï¿½ï¿½changeabilitï¿½ binaire pour chaque ï¿½lï¿½ment de L avec
			sa future position
			- Permutation

			<b>Echange</b>&nbsp;:

			Exemple dï¿½ï¿½change&nbsp;:

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
		SchedulesTableInterfaceElement::PlaceList SchedulesTableInterfaceElement::getStopsListForScheduleTable( const Journeys& jv )
		{
			// Variables locales
			size_t i;
			size_t dernieri;

			// Allocation
			LockedLinesList lll;
			PlaceList pl;

			// Horizontal loop
			for ( Journeys::const_iterator it = jv.begin(); it != jv.end(); ++it )
			{
				i = 0;
				dernieri = -1;

				// Vertical loop
				for (int l = 0; l < it->getJourneyLegCount (); ++l)
				{
					const ServiceUse& curET(it->getJourneyLeg(l));

					// Search of the place from the preceding one
					if ( OrdrePARechercheGare( pl, i, curET.getDepartureEdge()->getConnectionPlace() ) )
					{
						if ( i < dernieri )
							i = OrdrePAEchangeSiPossible( jv, pl, lll, dernieri, i );
					}
					else
					{
						i = OrdrePAInsere( pl, lll, curET.getDepartureEdge()->getConnectionPlace(), dernieri + 1 );
					}

					dernieri = i;
					i++;

					// Controle gare suivante pour trajet a pied
					if ( /* MJ que deviennent les lignes à pied ??? curET->getLigne() ->Materiel() ->Code() == MATERIELPied && */   
						pl[ i ] != curET.getArrivalEdge()->getConnectionPlace() && (l != it->getJourneyLegCount ()-1) )
					{
						if ( OrdrePARechercheGare( pl, i, curET.getArrivalEdge()->getConnectionPlace() ) )
						{
							OrdrePAEchangeSiPossible(jv, pl, lll, dernieri, i );
							i = dernieri + 1;
						}
						else
						{
							i = dernieri + 1;
							OrdrePAInsere( pl, lll, curET.getArrivalEdge()->getConnectionPlace(), i );
						}
						lll.insert( lll.begin() + i, true );
					}
				}
			}

			// Ajout de la destination finale en fin de tableau
			if ( jv.size() > 0 )
				pl.push_back( jv[0].getDestination()->getConnectionPlace() );

			return pl;
		}




	
		/*! \brief Crï¿½ation des niveaux d'alerte des trajets en fonction des donnï¿½es lignes et arrï¿½ts
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
