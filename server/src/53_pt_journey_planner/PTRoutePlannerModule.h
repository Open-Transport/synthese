
/** PTRoutePlannerModule class header.
	@file PTRoutePlannerModule.h
	@author Hugues Romain
	@date 2010

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#ifndef SYNTHESE_PTRoutePlannerModule_H__
#define SYNTHESE_PTRoutePlannerModule_H__

#include "ModuleClassTemplate.hpp"

namespace synthese
{
	//////////////////////////////////////////////////////////////////////////
	/// 53 Public transport journey planner module namespace.
	///	@author Hugues Romain
	///	@date 2010
	/// @ingroup m53
	namespace pt_journey_planner
	{
		/**	@defgroup m53Exceptions 53.01 Exceptions
			@ingroup m53

			@defgroup m53LS 53.10 Table synchronizers
			@ingroup m53

			@defgroup m53Pages 53.11 Interface Pages
			@ingroup m53

			@defgroup m53Library 53.11 Interface Library
			@ingroup m53

			@defgroup m53Rights 53.12 Rights
			@ingroup m53

			@defgroup m53Logs 53.13 DB Logs
			@ingroup m53

			@defgroup m53Admin 53.14 Administration pages
			@ingroup m53

			@defgroup m53Actions 53.15 Actions
			@ingroup m53

			@defgroup m53Functions 53.15 Functions
			@ingroup m53

			@defgroup m53File 53.16 File formats
			@ingroup m53

			@defgroup m53Alarm 53.17 Messages recipient
			@ingroup m53

			@defgroup m53 53 Public transport journey planner module namespace.
			@ingroup m5

			<h3>Road exploration : Transport public stops approach</h3>

			Cette fonctionnalité consiste en la détermination de l'ensemble des points d'arrêt accessibles en un temps maximal fixé avec le descriptif du trajet d'approche à effectuer à pied, à partir des éléments suivants :
			- voirie : on ne suit que des cheminements réels
			- limites de communes : permet de déterminer la commune du point géographique, ainsi que le parcours le plus court pour pénétrer aux communes proches dans le but d'y trouver un éventuel arrêt tout lieux.

			@image html uc35-source.png
			@image latex uc35-source.png "" width=14cm

			Le processus complet est le suivant, avec application sur l'exemple ci-dessus :
			- Etape 1 : Lecture de la commune du point géographique (module @ref m03), et extraction du « Tous lieux » de la commune s'il existe :
			<center><table class="tableau"><tr><th>Point d'arrêt</th><th>Durée</th></tr>
			<tr><td>PA 6</td><td>0</td></tr>
			</table></center>

			- Etape 2 : Calcul d'approche vers des communes proches (module @ref m40), et extraction de « Tous lieux » des communes s'ils existent :
			<center><table class="Tableau"><tr><th>Communes</th><th>Point d'arrêt</th><th>Durée</th></tr>
			<tr><td>A</td><td>PA 4</td><td>7</td></tr>
			<tr><td>B</td><td>PA 5</td><td>5</td></tr>
			</table></center>

			- Etape 3 : Calcul d'approche vers des points d'arrêt proches (module @ref m40) :
			<center><table class="tableau"><tr><th>Point d'arrêt</th><th>Durée</th></tr>
			<tr><td>PA 1</td><td>8</td></tr>
			<tr><td>PA 2</td><td>12</td></tr>
			</table></center>

			L'objet final construit est donc le suivant, incluant les descriptions de trajet représentées sur le schéma en suivant :

			<center><table class="tableau"><tr><th>Point d'arrêt</th><th>Durée</th></tr>
			<tr><td>PA 1</td><td>8</td></tr>
			<tr><td>PA 2</td><td>12</td></tr>
			<tr><td>PA 4</td><td>7</td></tr>
			<tr><td>PA 5</td><td>5</td></tr>
			<tr><td>PA 6</td><td>0</td></tr>
			</table></center>

			@image html uc35-resultat.png
			@image latex uc35-resultat.png "" width=14cm

			Remarques :
			Le point d'arrêt PA 3 a été ignoré car il n'est pas accessible malgré sa proximité à vol d'oiseau
			Les désignations de référence des points d'arrêts PA 1 et PA 2 n'ont pas besoin d'être copiées, puisque la distance vers les communes A et B est forcément plus courte

			Reference classes :
			- algorithm::IntegralSearch

			<h3>Multipoint Route Planning</h3>

			Reference classes :
			- algorithm::RoutePlanner

			<h3>Monopoint Approach Exploration + Route Planning</h3>

			Reference classes :
			- ApproachExplorerAndRoutePlanner


			@{
		*/


		//////////////////////////////////////////////////////////////////////////
		/// 53 pt_routeplanner Module class.
		///	@author Hugues Romain
		///	@date 2010
		class PTRoutePlannerModule:
			public server::ModuleClassTemplate<PTRoutePlannerModule>
		{
		private:


		public:
			/** Initialization of the 53 pt_routeplanner module after the automatic database loads.
				@author Hugues Romain
				@date 2010
			*/
			//void initialize();

		};
	}
	/** @} */
}

#endif // SYNTHESE_PTRoutePlannerModule_H__
