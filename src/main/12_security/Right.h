/** Right class header.
	@file Right.h

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

#ifndef SYNTHESE_SECURITY_RIGHT_H
#define SYNTHESE_SECURITY_RIGHT_H

#include <string>
#include <map>
#include <set>
#include <utility>
#include <vector>

#include "01_util/Factorable.h"

#include "12_security/Types.h"

namespace synthese
{
	namespace security
	{
		/** Habilitation (abstraite).
			@ingroup m12

			Une habilitation est un droit d'effectuer une ou plusieurs opération(s) sur un périmÃštre donné.

			Une habilitation est définie par :
				-# un périmÃštre d'application dont l'usage est précisé par les sous classes. Exemple d'utilisations possibles :
					- Masque de code SYNTHESE d'objet (MP/TLS/*, MP/TLS/14/*...)
					- Liste d'objet(s) (MP/TLS/14;MP/TLS/41)
					- @c * = Pas de restriction de périmÃštre
				-# un niveau de droit sur les objets appartenant Ã  l'utilisateur (droits privés)
				-# un niveau de droit sur les objets n'appartenant pas Ã  l'utilisateur (droits publics)

			Les niveaux de droits sont les suivants :
				- FORBIDDEN : interdiction (utile pour annuler une habilitation héritée)
				- USE : droit d'utiliser une fonction
				- READ : accÃšs en lecture
				- WRITE : accÃšs en écriture, effacement non permis
				- DELETE : accÃšs en écriture, effacement permis
				- NB : Chaque niveau de droit inclut les niveaux précédents.

			Chaque module contient des définitions d'habilitations (sous-classes enregistrées dans Factory<Right>) qui implémentent le contrÃŽle des droits et la génération d'une liste de paramÃštres possibles.
		*/
		class Right : public util::Factorable<Right>
		{
		private:
			RightLevel _privateRightLevel;
			RightLevel _publicRightLevel;

		protected:
			std::string _parameter;

			/** Constructor without parameters.
				Used to build temporary right objects for factory iteration purposes.
			*/
			Right();

		public:
			// Will be activated when factory key will be static
			//Right(std::string parameter, Level privateLevel, Level publicLevel);
			~Right();


			//! \name Right definition
			//@{
				virtual std::string			displayParameter()		const = 0;
				virtual ParameterLabelsVector	getParametersLabels()	const = 0;
				
				
				/** Perimeter inclusion test.
					@param perimeter Perimeter to include
					@return bool True if the current perimeter includes the compared one
					@author Hugues Romain
					@date 2007				
				*/
				virtual bool perimeterIncludes(const std::string& perimeter) const = 0;
			//@}

			//! \name Getters
			//@{
				RightLevel			getPrivateRightLevel()	const;
				RightLevel			getPublicRightLevel()	const;
				const std::string&	getParameter() const;
			//@}

			//! \name Setters
			//@{
				void	setPrivateLevel(const RightLevel& level);
				void	setPublicLevel(const RightLevel& level);
				void	setParameter(const std::string& parameter);
			//@}


			static std::string getLevelLabel(RightLevel level);

			virtual const std::string& getName() const = 0;
			virtual bool getUsePrivateRights() const = 0;
		};
	}
}

#endif

