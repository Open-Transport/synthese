/** DeparturesTableModule class header.
	@file DeparturesTableModule.h

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

#ifndef SYNTHESE_DeparturesTableModule_H__
#define SYNTHESE_DeparturesTableModule_H__

#include <vector>
#include <utility>
#include <string>

#include "01_util/ModuleClass.h"
#include "01_util/FactorableTemplate.h"

#include "12_security/Types.h"

namespace synthese
{
	namespace env
	{
		class LineStop;
		class PublicTransportStopZoneConnectionPlace;
		class PhysicalStop;
		class Line;
	}

	/**	@defgroup m34Actions 54 Actions
		@ingroup m34

		@defgroup m34Pages 54 Pages
		@ingroup m34

		@defgroup m34Functions 54 Functions
		@ingroup m34

		@defgroup m34LS 54 Table synchronizers
		@ingroup m34

		@defgroup m34Admin 54 Administration pages
		@ingroup m34

		@defgroup m34Rights 54 Rights
		@ingroup m34

		@defgroup m34Logs 54 DB Logs
		@ingroup m34

		@defgroup m34 54 Arrival/departures tables service module.
		@ingroup m5

		@subsection m34_target_structure Architecture fonctionnelle cible

		Un système de téléaffichage basé sur SYNTHESE repose sur les principes suivants :
			- la base de données de SYNTHESE constitue le point de convergence des diverses sources de données. Dans le cadre du téléaffichage, les sources majeures sont : 
			- les sources dites <i>théoriques</i> : elles proviennent de fichiers mis à jour par saisie manuelle ou par import informatique
			- les sources dites <i>temps réel</i> : saisies manuelles dans la console d'administration (ex : saisie de messages...) ou réceptions de données transmises automatiquement (ex : contrôles de supervision issues du client de supervision, import modifications horaires SAE...)
			- le téléaffichage est un service du système SYNTHESE comme un autre : il exploite la base de données à chaud et fabrique des objets résultats (une liste de départs ou d'arrivée) qui sont transmis au module d'interface, chargé de les diffuser selon un langage défini
			- le module d'interface permet de définir le format des sorties sous forme de données d'entrée, dans une grande liberté de choix. Les formats de sortie peuvent être définis en HTML pour intégration à une charte graphique, en XML pour communiquer avec d'autres applications, ou encore selon un langage propriétaire pour piloter des dispositifs de téléaffichage matériels

		Le schéma suivant représente l'architecture cible d'un système de téléaffichage basé sur la solution SYNTHESE :

		@image latex 1195515_1_000.eps "Architecture cible d'un système de téléaffichage SYNTHESE" height=10cm
		@image html 1195515.2.000.png

	@{
	*/

	/** 34 departures table module namespace.
	*/
	namespace departurestable
	{
		class DeparturesTableModule : public util::FactorableTemplate<util::ModuleClass, DeparturesTableModule>
		{
		public:
			static std::vector<std::pair<uid, std::string> >	getDisplayTypeLabels(bool withAll = false);
			static std::vector<std::pair<uid, std::string> >	getPlacesWithBroadcastPointsLabels(
				const security::RightsOfSameClassMap& rights 
				, bool totalControl 
				, security::RightLevel neededLevel
				, bool withAll = false
				);
			static std::vector<std::pair<uid, std::string> >	getCommercialLineWithBroadcastLabels(bool withAll = false);

			void initialize();

			virtual std::string getName() const;
		};
	}

	/** @} */

}

#endif // SYNTHESE_DeparturesTableModule_H__
