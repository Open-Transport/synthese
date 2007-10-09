
/** Interface class header.
	@file Interface.h

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

#ifndef SYNTHESE_INTERFACES_INTERFACE_H
#define SYNTHESE_INTERFACES_INTERFACE_H

#include <string>
#include <vector>
#include <iostream>
#include <map>

#include "01_util/Registrable.h"
#include "01_util/UId.h"
#include "01_util/Factory.h"

namespace synthese
{
	namespace interfaces
	{
		class InterfacePage;
		
		/** Interface.
			@author Hugues Romain
			@date 2001-2006
			@ingroup m11
		 
			Cette classe définit les différentes interfaces pour l'affichage de résultats issus de calculs SYNTHESE. Une interface est constituée de modèles d'affichage, permettant la d?finition du graphisme en sortie, fournis ? SYNTHESE en tant que donn?es. Des mod?les de fichiers de sortie doivent ?tre fournis au chargement d'apr?s un format de codage d?fini pour l'occasion.</p>
		 
			La liste des \ref InterfaceObjetsStandard d?finit les objets qui peuvent ?tre d?crits par un mod?le. La fourniture d'un mod?le pour chacun de ces objets est facultative. En cas de demande d'affichage d'un objet pour lequel aucun mod?le n'a ?t? fourni, une page vide sera retourn?e.
			 
			Les mod?les sont d?finis par la classe cElementInterface et ses d?riv?s, et sont ind?x?s dans le tableau de pointeurs vElement, selon une indexation par num?ro d'objet standard, selon la nomenclature des \ref InterfaceObjetsStandard .
			 
		*/
		class Interface : public util::Registrable<uid, Interface>
		{
			private:
				typedef std::map<std::string, InterfacePage*> PagesMap;

				std::string	_name;
				PagesMap	_pages;
				std::string	_noSessionDefaultPageCode;

			public:

				//! \name Accesseurs
				//@{
					/** Gets a stored page from the class factory key.
						@param key Key of the wanted class
						@return A pointer to the existing wanted page in the interface definition. The pointer does not know the real type of the page.
						@exception InterfacePageException The code is not available in the factory
					*/
					const InterfacePage* getPage( const std::string& key) const;

					/** Gets a stored page from its class (template).
						@return The required page, directly known as its type.
					*/
					template <class T>
					const T* const getPage() const
					{
						std::string key(util::Factory<InterfacePage>::getKey<T>());
						return static_cast<const T*>(getPage(key));
					}

					const std::string& getNoSessionDefaultPageCode() const;
					const std::string& getName() const;
				//@}

				//! \name Modifiers
				//@{
					void	addPage(InterfacePage* page );
					void	removePage( const std::string& page_code );
					void	setNoSessionDefaultPageCode(const std::string&);
					void	setName(const std::string& name);
				//@}

				Interface( const uid id = UNKNOWN_VALUE);
		};
	}
}

#endif
