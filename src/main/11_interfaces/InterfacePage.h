
/** InterfacePage class header.
	@file InterfacePage.h

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

#ifndef SYNTHESE_INTERFACES_PAGE_H
#define SYNTHESE_INTERFACES_PAGE_H

#include <vector>
#include <utility>
#include <string>
#include <boost/shared_ptr.hpp>

#include "01_util/Registrable.h"
#include "01_util/UId.h"
#include "01_util/Factorable.h"

#include "11_interfaces/Types.h"
#include "11_interfaces/LibraryInterfaceElement.h"

using synthese::util::Factory;

/** @defgroup refPages Interface Pages
	@ingroup refInt
*/

namespace synthese
{
	namespace server
	{
		class Request;
	}
	   
	namespace interfaces
	{
		class Interface;

		/** Definition of a page, coming from the database. Page are elements of an interface.

			Two types of pages can be defined :
				- standard pages : the page is registered in the Factory<InterfacePage>. Its key in database corresponds to the key of registration.
				- additional pages : the page is not registerd in the Factory<InterfacePage>. It is saved directly as a InterfacePage object.
			@ingroup m11
		*/
		class InterfacePage
			: public util::Factorable
			, public util::Registrable<uid, InterfacePage>
		{
			
		private:
			boost::shared_ptr<const Interface>	_interface;
			LibraryInterfaceElement::Registry	_components;
			std::string							_code;
			
		public:

			InterfacePage();
			virtual ~InterfacePage();

			void parse( const std::string& text );

			void clear();

			/** Display method.
				@param stream Stream to write the output in
				@param parameters Parameters vector
				@return Name of the next line to display (empty = next line)
			*/
			void display(
				std::ostream& stream
				, const ParametersVector& parameters
				, VariablesMap& variables
				, const void* object = NULL
				, const server::Request* request = NULL) const;

			void				setInterface(boost::shared_ptr<const Interface>);
			boost::shared_ptr<const Interface>	getInterface()	const;

			void setCode(const std::string& );

			const std::string& getCode() const;
		};
	}
}




/** Formulaire d'entr?e recherche d'itin?raires
@code >02 @endcode
@param 0 Texte par d?faut dans le champ commune de depart
@param 1 Numero de commune de depart par defaut
@param 2 Texte par d?faut dans le champ arret de depart
@param 3 Numero d'arret de depart par defaut
@param 4 Texte par d?faut dans le champ commune d'arriv?e
@param 5 Numero de commune d'arriv?e par defaut
@param 6 Texte par d?faut dans le champ arret d'arriv?e
@param 7 Numero d'arret d'arriv?e par defaut
@param 8 Numero de d?signation d?part par defaut
@param 9 Numero de d?signation d'arriv?e par defaut
@param 10 P?riode de la journ?e par d?faut
@param 11 Etat par d?faut filtre v?lo
@param 12 Etat par d?faut filtre handicap?
@param 13 Etat par d?faut filtre lignes ? r?server
@param 14 Valeur par d?faut filtre tarif
@param 15 Date par d?faut
@return Formulaire d'entr?e recherche d'itin?raires
*/
#define INTERFACEFormulaireEntree    2


/** Nom de point d'arr?t
@code >03 @endcode
@param Objet LogicalPlace * : Point d'arr?t ? nommer
@param 0 Num?ro de d?signation
@return Nom de point d'arr?t
*/
#define INTERFACENomArret      3


/** D?signation compl?te de ligne
@code >04 @endcode
@param Objet cLigne * : Ligne ? afficher
@return D?signation compl?te de ligne
*/
#define INTERFACEDesignationLigne    5



/** Liste de communes d'origine
@code >07 @endcode
@param 0 Texte d?entr?e pour recherche commune
@return Liste de communes d'origine
*/
#define INTERFACEListeCommunesDepart   7


/** Liste de communes de destination
@code >08 @endcode
@param 0 Texte d?entr?e pour recherche commune
@return Liste de communes de destination
*/
#define INTERFACEListeCommunesArrivee   8


/** Liste de points d'arr?t d'origine
@code >09 @endcode
@param 0 Texte d?entr?e pour recherche arret
@param 1 Num?ro de la commune
@return Liste de points d'arr?t d'origine
*/
#define INTERFACEListeArretsDepart    9


/** Liste de points d'arr?t de destination
@code >10 @endcode
@param 0 Texte d?entr?e pour recherche arret
@param 1 Num?ro de la commune
@return Liste de points d'arr?t de destination
*/
#define INTERFACEListeArretsArrivee    10


/** Page d'erreur de validation des donn?es du formulaire de recherche d'itin?raire
@code >11 @endcode
<table class="Tableau" cellspacing="0" cellpadding="0">
<tr><th>Num?ro</th><th>Nom</th><th>Description</th><th>Valeur</th></tr>
<tr><td>0</td><td></td><td>Texte entr? dans le champ commune d?part</td><td></td></tr>
<tr><td>1</td><td></td><td>Num?ro commune d?part</td><td></td></tr>
<tr><td>2</td><td></td><td>Texte entr? dans le champ arr?t d?part</td><td></td></tr>
<tr><td>3</td><td></td><td>Num?ro arret de d?part</td><td></td></tr>
<tr><td>4</td><td></td><td>Texte entr? dans le champ commune arriv?e</td><td></td></tr>
<tr><td>5</td><td></td><td>Num?ro commune arriv?e</td><td></td></tr>
<tr><td>6</td><td></td><td>Texte entr? dans le champ arr?t arriv?e</td><td></td></tr>
<tr><td>7</td><td></td><td>Num?ro arret d'arriv?e</td><td></td></tr>
<tr><td>8</td><td></td><td>Num?ro d?signation de d?part</td><td></td></tr>
<tr><td>9</td><td></td><td>Num?ro d?signation d'arriv?e</td><td></td></tr>
<tr><td>10</td><td></td><td>Code p?riode de la journ?e</td><td></td></tr>
<tr><td>11</td><td></td><td>Filtre prise en charge des v?los</td><td></td></tr>
<tr><td>12</td><td></td><td>Filtre prise en charge handicap?s</td><td></td></tr>
<tr><td>13</td><td></td><td>Filtre lignes r?sa possible</td><td></td></tr>
<tr><td>14</td><td></td><td>Filtre tarif</td><td>-1 = tout tarif</td></tr>
<tr><td>15</td><td></td><td>Date du d?part</td><td></td></tr>
<tr><td>16</td><td></td><td>Message d'erreur</td><td></td></tr>
</table>

Ce message s?affiche si l?utilisateur a lanc? le calcul sans avoir valid? ses arr?ts de d?part ou arriv?e, et si une ambigu?t? rend impossible le choix des arr?ts par les m?thodes cEnvironnement::TextToCommune()
et cCommune::TextToPADe().

De ce fait, ce message va afficher, pour le d?part et l?arriv?e&nbsp;:
- un message explicitant l?erreur commise
- rien s?il obtient num?ro de commune et num?ro d'arr?t coh?rents
- une erreur de requ?te sinon
*/
#define INTERFACEErreurArretsFicheHoraire  11



/** Ecran d'attente
@code >12 @endcode
<table class="Tableau" cellspacing="0" cellpadding="0">
<tr><th>Num?ro</th><th>Nom</th><th>Description</th><th>Valeur</th></tr>
<tr><td>0</td><td></td><td>Num?ro arret de d?part</td><td></td></tr>
<tr><td>1</td><td></td><td>Num?ro arret d'arriv?e</td><td></td></tr>
<tr><td>2</td><td></td><td>Date du d?part</td><td></td></tr>
<tr><td>3</td><td></td><td>Num?ro d?signation de d?part</td><td></td></tr>
<tr><td>4</td><td></td><td>Num?ro d?signation d'arriv?e</td><td></td></tr>
<tr><td>5</td><td></td><td>Code p?riode de la journ?e</td><td></td></tr>
<tr><td>6</td><td></td><td>Filtre prise en charge des v?los</td><td></td></tr>
<tr><td>7</td><td></td><td>Filtre prise en charge handicap?s</td><td></td></tr>
<tr><td>8</td><td></td><td>Filtre lignes r?sa possible</td><td></td></tr>
<tr><td>9</td><td></td><td>Filtre tarif</td><td>-1 = tout tarif</td></tr>
</table> 
*/
#define INTERFACEAttente      12

/** Tableau de d?part de t?l?affichage
@code >13 @endcode
*/
#define INTERFACETbDepGare      13

/** Tableau de d?part
@code >14 @endcode
*/
#define INTERFACETbDep       14


/** Formulaire de r?servation
@code >15 @endcode
@param 0 Cl? du site (inutile)
@param 1 Code de la ligne
@param 2 Num?ro du service emprunt?
@param 3 Code point arr?t de d?part
@param 4 Code point arr?t d'arriv?e
@param 5 Moment du d?part (format interne)
@return Formulaire de r?servation
*/
#define INTERFACEFormResa      15



/** Ecran de validation de r?servation
@code >16 @endcode
*/
#define INTERFACEValidResa      16




/** Feuille de route
@code >18 @endcode
<table class="Tableau" cellspacing="0" cellpadding="0">
<tr><th>Num?ro</th><th>Description</th><th>Valeur</th></tr>
<tr><td>0</td><td>Index du trajet dans la fiche horaire</td><td><i>1..infini</i></td></tr>
<tr><td>1</td><td>Etat du filtre handicap?</td><td>1|0</td></tr>
<tr><td>2</td><td>Etat du filtre v?lo</td><td>1|0</td></tr>
<tr><td>3</td><td>Index du trajet pr?c?dent dans la fiche horaire</td><td><i>RIEN</i> si premier trajet<br><i>1..infini</i> sinon</td></tr>
<tr><td>4</td><td>Index du trajet pr?c?dent dans la fiche horaire</td><td><i>RIEN</i> si dernier trajet<br><i>1..infini</i> sinon</td></tr>
<tr><td>5</td><td>Date du d?part</td><td><i>AAAAMMJJ</i><br>NB : Peut ?tre sup?rieure ? la date de la fiche horaire</td></tr>
</table>
*/
#define INTERFACEFeuilleRoute     18







#define INTERFACEFicheArret      23



/** Case dur?e pour fiche horaire
@code >26 @endcode
*/
#define INTERFACECaseDuree      26



/** Case renvoi pour fiche horaire
@code >27 @endcode
@param 0 Num?ro de la colonne
@param 1 Niveau maximal d'alerte
@return Case renvoi pour fiche horaire
*/
#define INTERFACECaseRenvoi      27



#define INTERFACEDuree       29



/** Mini tableau de d?part de gare
@code >30 @endcode
<table class="Tableau" cellspacing="0" cellpadding="0">
<tr><th>Num?ro</th><th>Description</th><th>Valeur</th></tr>
<tr><td>NPA</td><td>1</td><td>Num?ro du point d?arr?t</td></tr>
<tr><td>NPropositions</td><td>2</td><td>Nombre de propositions (-1 = illimit?)</td></tr>
</table>
*/
#define INTERFACEMiniTbDepGare     30


/** Page d'accueil
@code >33 @endcode
@return Page d'accueil
*/
#define INTERFACEPageAccueil     33


/** Fiche horaire
@code >34 @endcode
@param 0 Texte par d?faut dans le champ commune de depart
@param 1 Numero de commune de depart par defaut
@param 2 Texte par d?faut dans le champ arret de depart
@param 3 Numero d'arret de depart par defaut
@param 4 Texte par d?faut dans le champ commune d'arriv?e
@param 5 Numero de commune d'arriv?e par defaut
@param 6 Texte par d?faut dans le champ arret d'arriv?e
@param 7 Numero d'arret d'arriv?e par defaut
@param 8 Numero de d?signation d?part par defaut
@param 9 Numero de d?signation d'arriv?e par defaut
@param 10 P?riode de la journ?e par d?faut
@param 11 Etat par d?faut filtre v?lo
@param 12 Etat par d?faut filtre handicap?
@param 13 Etat par d?faut filtre lignes ? r?server
@param 14 Valeur par d?faut filtre tarif
@param 15 Date par d?faut
@return Fiche horaire
*/
#define INTERFACEFicheHoraire     34


/** Ligne de tableau de d?part
@code >35 @endcode
@param 0 Num?ro de rang?e
@param 1 Num?ro de page de la rang?e
@param (projet) Situation perturb?e ?
@param (projet) Heure de passage r?elle
*/
#define INTERFACELigneTableauDepart    35

#define INTERFACECaseParticularite    36


#endif

