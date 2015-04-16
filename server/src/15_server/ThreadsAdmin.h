
/** ThreadsAdmin class header.
	@file ThreadsAdmin.h
	@author Hugues
	@date 2009

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

#ifndef SYNTHESE_ThreadsAdmin_H__
#define SYNTHESE_ThreadsAdmin_H__

#include "AdminInterfaceElementTemplate.h"

namespace synthese
{
	namespace server
	{
		/** ThreadsAdmin Class.
			@ingroup m15Admin refAdmin
			@author Hugues
			@date 2009
		*/
		class ThreadsAdmin : public admin::AdminInterfaceElementTemplate<ThreadsAdmin>
		{
		public:
			/** Initialization of the parameters from a parameters map.
				@param map The parameters map to use for the initialization.
				@throw AdminParametersException if a parameter has incorrect value.
				@author Hugues
				@date 2009
			*/
			void setFromParametersMap(const util::ParametersMap& map);


			util::ParametersMap getParametersMap() const;

			/** Display of the content of the admin element.
				@param stream Stream to write on.
				@param request The current request
				@author Hugues
				@date 2009
			*/
			void display(
				std::ostream& stream,
				const server::Request& request
			) const;



			/** Authorization control.
				@param request The current request
				@return bool True if the displayed page can be displayed
				@author Hugues
				@date 2009
			*/
			bool isAuthorized(
				const security::User& profile
			) const;

			/** Gets sub page of the designed parent page, which are from the current class.
				@param factoryKey Key of the parent class
				@return PageLinks A link to the page if the parent is Home
				@author Hugues Romain
				@date 2008
			*/
			virtual AdminInterfaceElement::PageLinks getSubPagesOfModule(
				const server::ModuleClass& module,
				const AdminInterfaceElement& currentPage,
				const server::Request& request
			) const;
		};
	}
}

#endif // SYNTHESE_ThreadsAdmin_H__
