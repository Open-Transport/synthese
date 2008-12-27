////////////////////////////////////////////////////////////////////////////////
/// Right class header.
///	@file Right.h
///	@author Hugues Romain
///	@date 2008-12-26 21:16
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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

#include "12_security/Types.h"

namespace synthese
{
	namespace security
	{
		////////////////////////////////////////////////////////////////////////
		/// Habilitation (abstraite).
		///	@ingroup m12
		///
		///	Une habilitation est un droit d'effectuer une ou plusieurs
		///	op�ration(s) sur un p�rim�tre donn�.
		///
		///	Une habilitation est d�finie par :
		///		-# un p�rim�tre d'application dont l'usage est pr�cis� par les sous classes. 
		///			Exemple d'utilisations possibles :
		///			- Masque de code SYNTHESE d'objet (MP/TLS/*, MP/TLS/14/*...)
		///			- Liste d'objet(s) (MP/TLS/14;MP/TLS/41)
		///			- @c * = Pas de restriction de p�rim�tre
		///		-# un niveau de droit sur les objets appartenant �l'utilisateur (droits priv�s)
		///		-# un niveau de droit sur les objets n'appartenant pas � l'utilisateur (droits publics)
		///
		///	Les niveaux de droits sont les suivants :
		///		- FORBIDDEN : interdiction (utile pour annuler une habilitation h�rit�e)
		///		- USE : droit d'utiliser une fonction
		///		- READ : acc�s en lecture
		///		- WRITE : acc�s en �criture, effacement non permis
		///		- DELETE : acc�s en �criture, effacement permis
		///		- NB : Chaque niveau de droit inclut les niveaux pr�c�dents.
		///
		///	Chaque module contient des d�finitions d'habilitations (sous-classes enregistr�es 
		///	dans Factory<Right>) 
		/// qui impl�mentent le contr�e des droits et la g�n�ration d'une liste de param�tres possibles.
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
					@param perimeter Perimeter to include
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

			virtual const std::string& getName() const = 0;
			virtual bool getUsePrivateRights() const = 0;
		};
	}
}

#endif

