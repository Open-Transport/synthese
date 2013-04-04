////////////////////////////////////////////////////////////////////////////////
/// Security types declaration file.
///	@file Types.h
///	@author Hugues Romain
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

#ifndef SYNTHESE_security_Types_h__
#define SYNTHESE_security_Types_h__

#include <set>
#include <map>
#include <utility>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>

namespace synthese
{
	namespace security
	{
		class Right;

		/** @addtogroup m12
			@{
		*/

		/** Vector of rights defining an habilitation.
			This typedef is a map :
				- key : a pair :
					- left : Factory key of the Right subclass
					- right : parameter value (see each Right subclass for the possible values of the parameter)
				- value : a constant shared pointer to a Right object
		*/
		typedef std::map<
			std::pair<std::string, std::string>,
			boost::shared_ptr<Right>
		> RightsVector;

		/** Vector of rights defining an habilitation for a determinated Right subclass (extract of a RightsVector object).
			This typedef is a map :
				- key : parameter value (see each Right subclass for the possible values of the parameter)
				- value : a constant shared pointer to a Right object

			To build a RightOfSameClassMap object, use :
				- Profile::getRightsForModuleClass<T>() if the class is knwown at coding time
				- Profile::getRights(std::string) if the class is unknown at coding time, and must be choosen from its factory key.
		*/
		typedef std::map<
			std::string,
			boost::shared_ptr<Right>
		> RightsOfSameClassMap;

		/** Available vality areas map.
			Key : heading of areas (eg: lines, stops...)
			Value : set of allowed strings representing an area
		*/
		typedef std::map<
			std::string,
			std::set<std::string>
		> AvailableValidityAreasMap;

		/** Niveaux d'habilitation. */
		typedef enum {
			UNKNOWN_RIGHT_LEVEL = -1
			, FORBIDDEN	= 0		//!< Interdiction
			, USE		= 20	//!< Utilisation de fonction autorisée
			, READ		= 40	//!< Lecture directe de données autorisée
			, CANCEL	= 50	//!< Annulation autorisée (delete sans write)
			, WRITE		= 60	//!< Ecriture directe de donnée autorisée
			, DELETE_RIGHT	= 80	//!< Suppression de donnée autorisée
		} RightLevel;

		typedef std::vector<
			std::pair<boost::optional<std::string>, std::string>
		> ParameterLabelsVector;

		/** @} */
	}
}

#endif // SYNTHESE_security_Types_h__
