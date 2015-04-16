////////////////////////////////////////////////////////////////////////////////
/// Right class header.
///	@file Right.h
///	@author Hugues Romain
///	@date 2008-12-26 21:16
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_SECURITY_RIGHT_H
#define SYNTHESE_SECURITY_RIGHT_H

#include <string>
#include <map>
#include <set>
#include <utility>
#include <vector>

#include "FactoryBase.h"
#include "Env.h"

#include "SecurityTypes.hpp"

namespace synthese
{
	namespace security
	{
		////////////////////////////////////////////////////////////////////////
		/// Habilitation (abstraite).
		///	@ingroup m12
		///
		///	Une habilitation est un droit d'effectuer une ou plusieurs
		///	opération(s) sur un périmètre donné.
		///
		///	Une habilitation est définie par :
		///		-# un périmètre d'application dont l'usage est précisé par les sous classes.
		///			Exemple d'utilisations possibles :
		///			- Masque de code SYNTHESE d'objet (MP/TLS/*, MP/TLS/14/*...)
		///			- Liste d'objet(s) (MP/TLS/14;MP/TLS/41)
		///			- @c * = Pas de restriction de périmètre
		///			- vide = Au moins un périmètre (utilisé uniquement pour la validation d'habilitation)
		///		-# un niveau de droit sur les objets appartenant à l'utilisateur (droits privés)
		///		-# un niveau de droit sur les objets n'appartenant pas à l'utilisateur (droits publics)
		///
		///	Les niveaux de droits sont les suivants :
		///		- FORBIDDEN : interdiction (utile pour annuler une habilitation héritée)
		///		- USE : droit d'utiliser une fonction
		///		- READ : accès en lecture
		///		- WRITE : accès en écriture, effacement non permis
		///		- DELETE : accès en écriture, effacement permis
		///		- NB : Chaque niveau de droit inclut les niveaux précédents.
		///
		///	Chaque module contient des définitions d'habilitations (sous-classes enregistrées
		///	dans Factory<Right>)
		/// qui implémentent le contrôe des droits et la génération d'une liste de paramètres possibles.
		class Right
		:	public util::FactoryBase<Right>
		{
		public:

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
				virtual std::string	displayParameter(
					util::Env& env = util::Env::GetOfficialEnv()
				) const = 0;
				virtual ParameterLabelsVector	getParametersLabels()	const = 0;


				/** Perimeter inclusion test.
					@param env Environment to read
					@param perimeter Perimeter to test
					@return bool True if the current perimeter includes the compared one
					@author Hugues Romain
					@date 2007
				*/
				virtual bool perimeterIncludes(
					const std::string& perimeter,
					util::Env& env = util::Env::GetOfficialEnv()
				) const = 0;
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
			static RightLevel getRightLevel(const std::string level);

			virtual const std::string& getName() const = 0;
			virtual bool getUsePrivateRights() const = 0;
		};
	}
}

#endif

