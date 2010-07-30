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

#include "ModuleClassTemplate.hpp"

#include "12_security/Types.h"

namespace synthese
{
	namespace pt
	{
		class LineStop;
		class StopArea;
		class JourneyPattern;
	}

	namespace pt
	{
		class StopPoint;
	}

	/**	@defgroup m54Actions 54 Actions
		@ingroup m54

		@defgroup m54Pages 54 Pages
		@ingroup m54

		@defgroup m54Functions 54 Functions
		@ingroup m54

		@defgroup m54LS 54 Table synchronizers
		@ingroup m54

		@defgroup m54Admin 54 Administration pages
		@ingroup m54

		@defgroup m54Rights 54 Rights
		@ingroup m54

		@defgroup m54Logs 54 DB Logs
		@ingroup m54

		@defgroup m54Alarm 54 Message recipient
		@ingroup m54

		@defgroup m54 54 Arrival/departures tables service module.
		@ingroup m5

		@subsection m54_target_structure Architecture fonctionnelle cible

		Un syst�me de t�l�affichage bas� sur SYNTHESE repose sur les principes suivants :
			- la base de donn�es de SYNTHESE constitue le point de convergence des diverses sources de donn�es. Dans le cadre du t�l�affichage, les sources majeures sont : 
			- les sources dites <i>th�oriques</i> : elles proviennent de fichiers mis � jour par saisie manuelle ou par import informatique
			- les sources dites <i>temps r�el</i> : saisies manuelles dans la console d'administration (ex : saisie de messages...) ou r�ceptions de donn�es transmises automatiquement (ex : contr�les de supervision issues du client de supervision, import modifications horaires SAE...)
			- le t�l�affichage est un service du syst�me SYNTHESE comme un autre : il exploite la base de donn�es � chaud et fabrique des objets r�sultats (une liste de d�parts ou d'arriv�e) qui sont transmis au module d'interface, charg� de les diffuser selon un langage d�fini
			- le module d'interface permet de d�finir le format des sorties sous forme de donn�es d'entr�e, dans une grande libert� de choix. Les formats de sortie peuvent �tre d�finis en HTML pour int�gration � une charte graphique, en XML pour communiquer avec d'autres applications, ou encore selon un langage propri�taire pour piloter des dispositifs de t�l�affichage mat�riels

		Le sch�ma suivant repr�sente l'architecture cible d'un syst�me de t�l�affichage bas� sur la solution SYNTHESE :

		@image latex 1195515_1_000.eps "Architecture cible d'un syst�me de t�l�affichage SYNTHESE" height=10cm
		@image html 1195515.2.000.png

	@{
	*/

	/** 54 departures table module namespace.
	*/
	namespace departurestable
	{
		class DeparturesTableModule:
			public server::ModuleClassTemplate<DeparturesTableModule>
		{
		public:
			typedef std::vector<std::pair<boost::optional<util::RegistryKeyType>, std::string> > Labels;

			static Labels getDisplayTypeLabels(
				bool withAll = false,
				bool withNone = false
			);
			static Labels	getPlacesWithBroadcastPointsLabels(
				const security::RightsOfSameClassMap& rights 
				, bool totalControl 
				, security::RightLevel neededLevel
				, bool withAll = false
			);

			static Labels	getCommercialLineWithBroadcastLabels(bool withAll = false);
		};
	}

	/** @} */

}

#endif // SYNTHESE_DeparturesTableModule_H__
