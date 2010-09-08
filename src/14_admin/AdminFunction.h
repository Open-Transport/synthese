
/** AdminRequest class header.
	@file AdminRequest.h

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

#ifndef SYNTHESE_AdminRequest_H__
#define SYNTHESE_AdminRequest_H__

#include <boost/shared_ptr.hpp>

#include "FactorableTemplate.h"
#include "RequestWithInterface.h"
#include "AdminInterfaceElement.h"

namespace synthese
{
	namespace admin
	{
		class AdminInterfaceElement;
		
		/** Administration console Function Class.
			@ingroup m14Functions refFunctions
			
			@warning Must be linked to the Request by AdminRequest or its subclasses.
		*/
		class AdminFunction:
			public util::FactorableTemplate<interfaces::RequestWithInterface, AdminFunction>
		{
		public:
			static const std::string PARAMETER_PAGE;
			static const std::string PARAMETER_TAB;
			static const std::string PARAMETER_ACTION_FAILED_PAGE;
			static const std::string PARAMETER_ACTION_FAILED_TAB;
			
		protected:
			//! \name Page parameters
			//@{
				boost::shared_ptr<AdminInterfaceElement>	_page;
				boost::shared_ptr<AdminInterfaceElement>	_actionFailedPage;
				boost::optional<std::string>	_errorMessage;
			//@}


			/** Conversion from generic parameters map to attributes.
			*/
			void _setFromParametersMap(const server::ParametersMap& map);

			/** Conversion from attributes to generic parameter maps.
			*/
			server::ParametersMap _getParametersMap() const;

			/** Action to run, defined by each subclass.
			*/
			void run(std::ostream& stream, const server::Request& request) const;

			virtual bool isAuthorized(const server::Session* session) const;

		public:
		
			//! @name Getters
			//@{
				boost::shared_ptr<AdminInterfaceElement> getPage() const { return _page; }
				boost::shared_ptr<AdminInterfaceElement> getActionFailedPage() const { return _actionFailedPage; }
			//@}
			
			//! @name Setters
			//@{
				void setPage(boost::shared_ptr<AdminInterfaceElement> aie){ _page = aie; }
				void setActionFailedPage(boost::shared_ptr<AdminInterfaceElement> aie){ _actionFailedPage = aie; }
	
				template<class T>
				void setActionFailedPage() { _actionFailedPage = _page->getNewOtherPage<T>(); }
			//@}

			virtual void _copy(boost::shared_ptr<const Function> function);

			virtual std::string getOutputMimeType() const;
		};
	}
}
#endif // SYNTHESE_AdminRequest_H__
