
/** BaseAdminFunctionRequest class header.
	@file BaseAdminFunctionRequest.hpp

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

#ifndef SYNTHESE_admin_BaseAdminFunctionRequest_hpp__
#define SYNTHESE_admin_BaseAdminFunctionRequest_hpp__

#include "StaticFunctionRequest.h"

#include "AdminInterfaceElement.h"
#include "WebPageDisplayFunction.h"

namespace synthese
{
	namespace admin
	{
		class AdminInterfaceElement;

		/** BaseAdminFunctionRequest class.
			@ingroup m14
		*/
		class BaseAdminFunctionRequest:
			public server::StaticFunctionRequest<cms::WebPageDisplayFunction>
		{
		protected:
			boost::shared_ptr<AdminInterfaceElement> _page;
			boost::shared_ptr<AdminInterfaceElement> _actionFailedPage;
			bool _noCMS;

		public:
			BaseAdminFunctionRequest(
				const server::Request& request,
				boost::shared_ptr<AdminInterfaceElement> page
			);



			BaseAdminFunctionRequest(
				const server::Request& request
			);



			/// @name Virtual services
			//@{
				virtual util::ParametersMap _getParametersMap() const;
			//@}



			/// @name Getters
			//@{
				boost::shared_ptr<AdminInterfaceElement> getPage() const {	return _page; }
				boost::shared_ptr<AdminInterfaceElement> getActionFailedPage() const {	return _actionFailedPage; }
			//@}

			/// @name Setters
			//@{
				void setActionFailedPage(boost::shared_ptr<AdminInterfaceElement> value){ _actionFailedPage = value; }
				void setPage(boost::shared_ptr<AdminInterfaceElement> value){ _page = value; }
			//@}



			template<class T>
			void setActionFailedPage()
			{
				T* tpage(dynamic_cast<T*>(_page.get()));
				if(tpage)
				{
					_actionFailedPage = tpage->getNewCopiedPage();
				}
				else
				{
					_actionFailedPage = _page->getNewPage<T>();
				}
			}
		};
	}
}

#endif // SYNTHESE_admin_BaseAdminFunctionRequest_hpp__

