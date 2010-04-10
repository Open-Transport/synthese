
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

#include "Registrable.h"
#include "Registry.h"
#include "Factory.h"
#include "InterfacePage.h"

namespace synthese
{
	namespace interfaces
	{
		class InterfacePage;
		
		/** Interface.
			@author Hugues Romain
			@date 2001-2006
			@ingroup m11
		 
			Cette classe définit les différentes interfaces pour l'affichage de résultats issus de calculs SYNTHESE. Une interface est constituée de mod�šles d'affichage, permettant la d?finition du graphisme en sortie, fournis ? SYNTHESE en tant que donn?es. Des mod?les de fichiers de sortie doivent ?tre fournis au chargement d'apr?s un format de codage d?fini pour l'occasion.</p>
		 
			La liste des \ref InterfaceObjetsStandard d?finit les objets qui peuvent ?tre d?crits par un mod?le. La fourniture d'un mod?le pour chacun de ces objets est facultative. En cas de demande d'affichage d'un objet pour lequel aucun mod?le n'a ?t? fourni, une page vide sera retourn?e.
			 
			Les mod?les sont d?finis par la classe cElementInterface et ses d?riv?s, et sont ind?x?s dans le tableau de pointeurs vElement, selon une indexation par num?ro d'objet standard, selon la nomenclature des \ref InterfaceObjetsStandard .
			 
		*/
		class Interface
		:	public virtual util::Registrable
		{
		public:

			/// Chosen registry class.
			typedef util::Registry<Interface>	Registry;

		private:
			typedef std::map<std::string, std::map<std::string, InterfacePage*> >	PagesMap;
			
			std::string	_name;
			PagesMap	_pages;
			std::string	_noSessionDefaultPageCode;
			std::string	_defaultClientURL;

		public:
			//! \name Setters
			//@{
				void	setName(const std::string& name);
				void	setDefaultClientURL(const std::string& value);
			//@}

			//! \name Getters
			//@{
				/** Gets a stored page from the class factory key.
					@param key Key of the wanted class (pair of strings : if the second element of the pair is empty, and no element has empty page key, then the first element of the class is returned)
					@return A pointer to the existing wanted page in the interface definition. The pointer does not know the real type of the page.
					@exception InterfacePageException The code is not available in the factory
				*/
				const InterfacePage* getPage(
					const std::string& classCode,
					std::string pageCode = std::string()
				) const;

				/** Gets a stored page from its class (template).
					@return The required page, directly known as its type.
				*/
				template <class T>
				const T* getPage(std::string pageKey = std::string()) const
				{
					return static_cast<const T*>(getPage(T::FACTORY_KEY, pageKey));
				}



				//////////////////////////////////////////////////////////////////////////
				/// Tests if a page exists in the interface.
				/// @param classCode Factory key of the page
				/// @param pageCode Variation code of the page
				/// @return true if the page exists in the interface
				bool hasPage(
					const std::string& classCode,
					std::string pageCode = std::string()
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Tests if a page exists in the interface.
				/// @param pageKey Variation code of the page
				/// @return true if the page exists in the interface
				template <class T>
				bool hasPage(std::string pageKey = std::string()) const
				{
					return hasPage(T::FACTORY_KEY, pageKey);
				}


				const std::string& getNoSessionDefaultPageCode() const;
				const std::string& getName() const;
				const std::string& getDefaultClientURL() const;
			//@}

			//! \name Modifiers
			//@{
				void	addPage(InterfacePage* page);
				void	removePage(const std::string& classCode, const std::string& pageCode);
				void	setNoSessionDefaultPageCode(const std::string& classCode);
			//@}

			Interface(
				util::RegistryKeyType id = 0
			);
		};
	}
}

#endif
