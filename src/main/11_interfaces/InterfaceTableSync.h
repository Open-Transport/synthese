
/** InterfaceTableSync class header.
	@file InterfaceTableSync.h

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

#ifndef SYNTHESE_InterfaceTableSync_H__
#define SYNTHESE_InterfaceTableSync_H__

#include <string>
#include <iostream>

#include "02_db/SQLiteTableSyncTemplate.h"

namespace synthese
{
	namespace interfaces
	{
		class Interface;

		/** InterfaceTableSync SQLite table synchronizer.
			@ingroup m11LS refLS
		*/
		class InterfaceTableSync : public db::SQLiteTableSyncTemplate<Interface>
		{
		public:
			static const std::string TABLE_COL_NO_SESSION_DEFAULT_PAGE;
			static const std::string TABLE_COL_NAME;


			/** Interface page SQLite table constructor.
			*/
			InterfaceTableSync();

		protected:

			void rowsAdded (db::SQLite* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResultSPtr& rows, bool isFirstSync = false);

			void rowsUpdated (db::SQLite* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResultSPtr& rows);

			void rowsRemoved (db::SQLite* sqlite, 
				db::SQLiteSync* sync,
				const db::SQLiteResultSPtr& rows);

		};

	}
}




/** D�claration d'une interface
@code [code @endcode
@param code Code de l'interface (voir @ref base)
*/
#define INTERFACESFORMATLIGNEInterface			TYPESousSection


/** D�claration d'une description d'un �l�ment
@code E numero chemin @endcode
@param numero Num�ro de l��l�ment d�crit (trois chiffres). Voir @ref interface pour le codage
@param chemin chemin est au format habituel : relatif � l'emplacement du fichier courant, et sans extension.
*/
#define INTERFACESFORMATLIGNEElement			0


/** Fixation de la valeur d'un param�tre
@code P numero valeur @endcode
@param numero Num�ro du param�tre fix� (3 chiffres)
@param valeur Valeur donn�e au param�tre
@deprecated V�rifier l'inutilit� de cette commande et la supprimer
*/
#define INTERFACESFORMATLIGNEParametre			1

/** D�claration d'un p�riode de la journ�e
@code PERIODE heure_debut heure_fin nom @endcode
@param heure_debut Heure de d�but de la p�riode (4 chiffres format HHMM)
@param heure_fin Heure de fin de la p�riode (4 chiffres format HHMM) : peut �tre inf�rieure � l'heure de d�but, la p�riode finira alors le lendemain � l'heure indiqu�e
@param nom Nom de la p�riode
*/
#define INTERFACESFORMATLIGNEPeriode			2


/** Traduction d'un num�ro de jour en mot
@code JOUR code mot @endcode
@param code Code du jour (1 chiffre) :
- @c 0 : dimanche
- @c 1 : lundi
- @c 2 : mardi
- ...
- @c 6 : samedi
@param mot Mot dans la langue de l'interface traduisant le jour
*/
#define INTERFACESFORMATLIGNELibelleJour		3


/** Traduction d'un num�ro de mois en mot
@code MOIS code mot @endcode
@param code Code du mois (1 chiffre) :
- @c 1 : janvier
- @c 2 : f�vrier
- ...
- @c 12 : d�cembre
@param mot Mot dans la langue de l'interface traduisant le mois
*/
#define INTERFACESFORMATLIGNELibelleMois		4


/** Traduction d'un num�ro d'alerte en mot
@code ALERTE niveau mmot @endcode
@param niveau Niveau d'alerte :
- @c 99 : attention
- @c 50 : information
@param mot Mot dans la langue de l'interface traduisant le niveau d'alerte
*/
#define INTERFACESFORMATLIGNEPrefixeAlerte		5
#define INTERFACESFORMATLIGNENombreFormats		6

#define INTERFACESFORMATCOLONNENumero				0
#define INTERFACESFORMATCOLONNEChemin				1
#define INTERFACESFORMATCOLONNEPeriodeDebut			2
#define INTERFACESFORMATCOLONNEPeriodeFin			3
#define INTERFACESFORMATCOLONNEPeriodeLibelle		4
#define INTERFACESFORMATCOLONNELibelleJourIndex		5
#define INTERFACESFORMATCOLONNELibelleJourLibelle	6
#define INTERFACESFORMATCOLONNELibelleMoisIndex		7
#define INTERFACESFORMATCOLONNELibelleMoisLibelle	8
#define INTERFACESFORMATCOLONNEPrefixeAlerteNiveau	9
#define INTERFACESFORMATCOLONNEPrefixeAlerteTexte	10
#define INTERFACESFORMATCOLONNESNombreFormats		11
//@}
#endif // SYNTHESE_InterfaceTableSync_H__

