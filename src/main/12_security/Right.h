﻿
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

#ifndef SYNTHESE_SECURITY_RIGHT_TEMPLATE_H
#define SYNTHESE_SECURITY_RIGHT_TEMPLATE_H

#include <string>
#include <map>
#include <set>
#include <utility>
#include <vector>

#include "01_util/Factorable.h"

namespace synthese
{
	namespace security
	{
		/** Habilitation (abstraite).
			@ingroup m12

			Une habilitation est un droit d'effectuer une ou plusieurs opération(s) sur un périmètre donné.

			Une habilitation est définie par :
				-# un périmètre d'application dont l'usage est précisé par les sous classes. Exemple d'utilisations possibles :
					- Masque de code SYNTHESE d'objet (MP/TLS/*, MP/TLS/14/*...)
					- Liste d'objet(s) (MP/TLS/14;MP/TLS/41)
					- @c * = Pas de restriction de périmètre
				-# un niveau de droit sur les objets appartenant à l'utilisateur (droits privés)
				-# un niveau de droit sur les objets n'appartenant pas à l'utilisateur (droits publics)

			Les niveaux de droits sont les suivants :
				- FORBIDDEN : interdiction (utile pour annuler une habilitation héritée)
				- USE : droit d'utiliser une fonction
				- READ : accès en lecture
				- WRITE : accès en écriture, effacement non permis
				- DELETE : accès en écriture, effacement permis
				- NB : Chaque niveau de droit inclut les niveaux précédents.

			Chaque module contient des définitions d'habilitations (sous-classes enregistrées dans Factory<Right>) qui implémentent le contrôle des droits et la génération d'une liste de paramètres possibles.
		*/
		class Right : public util::Factorable
		{
		public:
			/** Available vality areas map.
				Key : heading of areas (eg: lines, stops...)
				Value : set of allowed strings representing an area
			*/
			typedef std::map<std::string, std::set<std::string> > AvailableValidityAreasMap;

			/** Niveaux d'habilitation. */
			typedef enum {
				FORBIDDEN	= 0		//!< Interdiction
				, USE		= 20	//!< Utilisation de fonction autorisée
				, READ		= 40	//!< Lecture directe de données autorisée
				, WRITE		= 60	//!< Ecriture directe de donnée autorisée
				, DELETE	= 80	//!< Suppression de donnée autorisée
			} Level;

			typedef std::vector<std::pair<std::string, std::string> > ParameterLabelsVector;

		private:
			Level _privateRightLevel;
			Level _publicRightLevel;

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
				Level				getPrivateRightLevel()	const;
				Level				getPublicRightLevel()	const;
				const std::string&	getParameter() const;
			//@}

			//! \name Setters
			//@{
				void	setPrivateLevel(const Level& level);
				void	setPublicLevel(const Level& level);
				void	setParameter(const std::string& parameter);
			//@}


			static std::string getLevelLabel(Level level);

		};
	}
}

#endif

