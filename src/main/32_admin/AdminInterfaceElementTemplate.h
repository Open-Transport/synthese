
/** AdminInterfaceElementTemplate class header.
	@file AdminInterfaceElementTemplate.h

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

#ifndef SYNTHESE_admin_AdminInterfaceElementTemplate_h__
#define SYNTHESE_admin_AdminInterfaceElementTemplate_h__

#include "32_admin/AdminInterfaceElement.h"

#include "01_util/FactorableTemplate.h"

namespace synthese
{
	namespace admin
	{
		/** AdminInterfaceElementTemplate class.
			@ingroup m14
		*/
		template<class T>
		class AdminInterfaceElementTemplate : public util::FactorableTemplate<AdminInterfaceElement, T>
		{
		public:
			static const std::string ICON;
			static const std::string DEFAULT_TITLE;

			AdminInterfaceElementTemplate()
				: util::FactorableTemplate<AdminInterfaceElement, T>(AdminInterfaceElement::Args(ICON, DEFAULT_TITLE))
			{
			}

			virtual std::string getIcon() const;

			virtual std::string getTitle() const;
		};

		template<class T>
		std::string synthese::admin::AdminInterfaceElementTemplate<T>::getTitle() const
		{
			return DEFAULT_TITLE;
		}

		template<class T>
		std::string synthese::admin::AdminInterfaceElementTemplate<T>::getIcon() const
		{
			return ICON;
		}
	}
}

#endif // SYNTHESE_admin_AdminInterfaceElementTemplate_h__
