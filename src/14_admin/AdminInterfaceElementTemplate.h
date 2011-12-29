
/** AdminInterfaceElementTemplate class header.
	@file AdminInterfaceElementTemplate.h

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

#ifndef SYNTHESE_admin_AdminInterfaceElementTemplate_h__
#define SYNTHESE_admin_AdminInterfaceElementTemplate_h__

#include "AdminInterfaceElement.h"
#include "FactorableTemplate.h"

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
				: util::FactorableTemplate<AdminInterfaceElement, T>()
			{
			}

			virtual std::string getIcon() const { return ICON; }
			virtual std::string getTitle() const { return DEFAULT_TITLE; }

			boost::shared_ptr<T> getNewCopiedPage() const;

			virtual boost::shared_ptr<AdminInterfaceElement> getNewBaseCopiedPage() const;

			virtual AdminInterfaceElement::PageLinks _getCurrentTreeBranch() const;
		};



		template<class T>
		AdminInterfaceElement::PageLinks synthese::admin::AdminInterfaceElementTemplate<T>::_getCurrentTreeBranch() const
		{
			AdminInterfaceElement::PageLinks links;
			links.push_back(getNewCopiedPage());
			return links;
		}



		template<class T>
		boost::shared_ptr<AdminInterfaceElement> synthese::admin::AdminInterfaceElementTemplate<T>::getNewBaseCopiedPage() const
		{
			return boost::static_pointer_cast<AdminInterfaceElement, T>(getNewCopiedPage());
		}



		template<class T>
		boost::shared_ptr<T> AdminInterfaceElementTemplate<T>::getNewCopiedPage() const
		{
			return boost::shared_ptr<T>(new T(dynamic_cast<const T&>(*this)));
		}
}	}

#endif // SYNTHESE_admin_AdminInterfaceElementTemplate_h__
