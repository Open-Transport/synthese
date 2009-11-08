
/** PTRoutePlannerResult class header.
	@file PTRoutePlannerResult.h

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

#ifndef SYNTHESE_routeplanner_PTRoutePlannerResult_h__
#define SYNTHESE_routeplanner_PTRoutePlannerResult_h__

#include <vector>

#include "Journey.h"
#include "TimeSlotRoutePlanner.h"

namespace synthese
{
	namespace geography
	{
		class NamedPlace;
		class Place;
	}

	namespace ptrouteplanner
	{
		/** Public transportation route planner result class.
			@ingroup m53
		*/
		class PTRoutePlannerResult
		{
		public:
			/** Information about a served place, displayed in a schedule sheet presentation.
			*/
			struct PlaceInformation
			{
				const geography::NamedPlace* place;
				bool isOrigin;
				bool isDestination;
			};



			/** Vector of served places, to display in a schedule sheet presentation.
				The order of the places is chosen to reduce the number of place repetitions in the sheet.
			*/
			typedef std::vector<PlaceInformation> PlaceList;

			typedef algorithm::TimeSlotRoutePlanner::Result Journeys;
			

		private:
			std::size_t _ordrePAEchangeSiPossible(
				std::size_t PositionOrigine,
				std::size_t PositionSouhaitee
			);



			/** Insertion of a transfer place into the served places list of the schedule sheet.
				@param place Place to insert
				@param position Minimal position rank to give to the place

				The insertion shifts the following places to the next row.
				If a pedestrian step (drawn as two joined arrows) is already at the wanted position, then the place is positioned after the end of the pedestrian step.
			*/
			std::size_t _ordrePAInsere(
				const geography::NamedPlace* place,
				std::size_t Position,
				bool isLockedAtTheTop,
				bool isLockedAtTheEnd
			);



			/** Control of la compatibilité entre l'ordre des arrêts dans la grille horaire et les arrêts du trajet.
			*/
			std::vector<bool> _ordrePAConstruitLignesAPermuter(
				const graph::Journey& __TrajetATester,
				std::size_t LigneMax
			) const;



			/** Recherche de point d'arrêt dans la liste des points d'arrêt.
			*/
			bool _ordrePARechercheGare(
				std::size_t& i,
				const geography::NamedPlace* GareAChercher
			);


			
			const geography::Place* const		_departurePlace;
			const geography::Place* const		_arrivalPlace;
			bool		_samePlaces;	//!< Indicates if the route planning was attempted between to identical places (in this case : empty result)
			Journeys	_journeys;		//!< List of the result journeys, ordered by departure time
			PlaceList	_orderedPlaces;	//!< List of the served places

		public:
			void operator=(const PTRoutePlannerResult& other);
		
			/** Build of the places list of a future schedule sheet corresponding to a journey vector.
				@author Hugues Romain
				@date 2001-2006

				Le but de la methode est de fournir une liste ordonnee de points d'arret de taille minimale determinant les lignes du tableau de fiche horaire.

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

				Pas 5 : Service AED (E ins�r� avant B pour ne pas rompre la continuit� BC)

				<table class="Tableau" cellspacing="0" cellpadding="5">
				<tr><td>A</td><td>X</td><td>X</td><td>X</td><td>X</td><td>X</td><td>X</td></tr>
				<tr><td>C</td><td>|</td><td>X</td><td>X</td><td>|</td><td>|</td><td>|</td></tr>
				<tr><td>E</td><td>|</td><td>|</td><td>|</td><td>|</td><td>|</td><td>X</td></tr>
				<tr><td>B</td><td>X</td><td>|</td><td>X</td><td>X</td><td>V</td><td>|</td></tr>
				<tr><td>C</td><td>|</td><td>|</td><td>|</td><td>X</td><td>V</td><td>|</td></tr>
				<tr><td>D</td><td>X</td><td>X</td><td>X</td><td>X</td><td>X</td><td>X</td></tr>
				</table>

				Pour chaque trajet, on procede donc par balayage dans l'ordre des gares existantes. Si la gare a relier n�est pas trouv�e entre la position de la gare pr�c�dente et la fin, deux solutions :
					- soit la gare n�est pr�sente nulle part (balayage avant la position de la precedente) auquel cas elle est cr��e et rajout�e � la position de la gare pr�c�dente + 1
					- soit la gare est pr�sente avant la gare pr�c�dente. Dans ce cas, on tente de descendre la ligne de la gare recherch�e au niveau de la position de la gare pr�c�dente + 1. On contr�le sur chacun des trajets pr�c�dents que la chronologie n'en serait pas affect�e. Si elle ne l'est pas, alors la ligne est descendue. Sinon une nouvelle ligne est cr��e.

				Contr�le de l'�changeabilit� :

				Soit \f$ \delta_{l,c}:(l,c)\mapsto\{{1\mbox{~si~le~trajet~}c\mbox{~dessert~la~ligne~}l\atop 0~sinon} \f$

				Deux lignes l et m sont �changeables si et seulement si l'ordre des lignes dont \f$ \delta_{l,c}=1 \f$ pour chaque colonne est respect�.

				Cet ordre s'exprime par la propri�t� suivante : Si \f$ \Phi \f$ est la permutation p�vue, alors

				<img width=283 height=27 src="interface.doxygen_fichiers/image008.gif">

				Il est donc n�cessaire � la fois de contr�ler la possibilit� de permutation, et de la d�terminer �ventuellement.

				Si <sub><img width=25 height=24
				src="interface.doxygen_fichiers/image009.gif"></sub>est la ligne de la gare
				pr�c�demment trouv�e, et <sub><img width=24 height=24
				src="interface.doxygen_fichiers/image010.gif"></sub>�l�emplacement de la gare
				souhait�e pour permuter, alors les permutations � op�rer ne peuvent concerner
				que des lignes comprises entre <sub><img width=24 height=24
				src="interface.doxygen_fichiers/image010.gif"></sub>�et <sub><img width=25
				height=24 src="interface.doxygen_fichiers/image009.gif"></sub>. En effet, les
				autres lignes n�influent pas.</p>

				En premier lieu il est n�cessaire de d�terminer l�ensemble
				des lignes � permuter. Cet ensemble est construit en explorant chaque colonne.
				Si <sub><img width=16 height=24 src="interface.doxygen_fichiers/image011.gif"></sub>�est
				l�ensemble des lignes � permuter pour assurer l�int�grit� des colonnes <sub><img
				width=36 height=27 src="interface.doxygen_fichiers/image012.gif"></sub>, on
				peut d�finir cet ensemble en fonction du pr�c�dent <sub><img width=25
				height=24 src="interface.doxygen_fichiers/image013.gif"></sub>&nbsp;: <sub><img
				width=308 height=35 src="interface.doxygen_fichiers/image014.gif"></sub>

				Le but �tant de faire descendre la ligne <sub><img width=24
				height=24 src="interface.doxygen_fichiers/image010.gif"></sub>�vers <sub><img
				width=25 height=24 src="interface.doxygen_fichiers/image009.gif"></sub>, les
				lignes appartenant � L doivent �tre �changeables avec� les positions <sub><img
				width=216 height=27 src="interface.doxygen_fichiers/image015.gif"></sub>.
				L�ensemble de ces tests doit �tre r�alis�. Au moindre �chec, l�ensemble de la
				permutation est rendu impossible.

				L��changeabilit� binaire entre deux lignes l et m revient �
				contr�ler la propri�t�&nbsp;<sub><img width=89 height=28
				src="interface.doxygen_fichiers/image016.gif"></sub>.

				L��changeabilit� totale s��crit donc <sub><img width=145
				height=28 src="interface.doxygen_fichiers/image017.gif"></sub>

				L�algorithme est donc le suivant&nbsp;:
					- Construction de L
					- Contr�le d��changeabilit� binaire pour chaque �l�ment de L avec
					sa future position
					- Permutation

				<b>Echange</b>&nbsp;:

				Exemple d��change&nbsp;:

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
			PTRoutePlannerResult(
				const geography::Place* departurePlace,
				const geography::Place* arrivalPlace,
				bool samePlaces,
				const algorithm::TimeSlotRoutePlanner::Result& journeys
			);

			bool getSamePlaces() const;
			const Journeys& getJourneys() const;
			const PlaceList& getOrderedPlaces() const;
			const geography::Place* getDeparturePlace() const;
			const geography::Place* getArrivalPlace() const;
		};
	}
}

#endif // SYNTHESE_routeplanner_PTRoutePlannerResult_h__
