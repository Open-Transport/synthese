
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

#include "UId.h"
#include "FactorableTemplate.h"

#include "RequestWithInterfaceAndRequiredSession.h"
#include "AdminInterfaceElement.h"

namespace synthese
{
	namespace admin
	{
		class AdminInterfaceElement;

		/** Administration console Function Class.
			@ingroup m14Functions refFunctions
		*/
		class AdminRequest : public util::FactorableTemplate<interfaces::RequestWithInterfaceAndRequiredSession, AdminRequest>
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
				server::ParametersMap						_parameters;
			//@}


			/** Conversion from generic parameters map to attributes.
			*/
			void _setFromParametersMap(const server::ParametersMap& map);

			/** Conversion from attributes to generic parameter maps.
			*/
			server::ParametersMap _getParametersMap() const;

			/** Action to run, defined by each subclass.
			*/
			void _run(std::ostream& stream) const;

			virtual bool _isAuthorized() const;



		public:
			void setPage(boost::shared_ptr<AdminInterfaceElement> aie);

			template<class T>
			void setPage();

			void setActionFailedPage(boost::shared_ptr<AdminInterfaceElement> aie);

			template<class T>
			void setActionFailedPage();

			boost::shared_ptr<AdminInterfaceElement> getPage() const;

			void setParameter(const std::string& name, const std::string value);

			virtual std::string getOutputMimeType() const;

			////////////////////////////////////////////////////////////////////
			///	Sets the function to display the same page with same parameters.
			///	@param page the current page (always use this)
			///	@author Hugues Romain
			///	@date 2008
			/// The following parameters are copied :
			///		- page key
			///		- object id
			///		- active tab = current tab of this at the method call
			///		- same env
			////////////////////////////////////////////////////////////////////
			virtual void _copy(boost::shared_ptr<const Function> other);
		};

		template<class T>
		void AdminRequest::setActionFailedPage()
		{
		    _actionFailedPage.reset(new T);
			_actionFailedPage->setRequest(static_cast<const server::FunctionRequest<AdminRequest>* >(_request));
		}

		template<class T>
		void AdminRequest::setPage()
		{
			_page.reset(new T);
			_page->setRequest(static_cast<const server::FunctionRequest<AdminRequest>* >(_request));
		}
	}
}
#endif // SYNTHESE_AdminRequest_H__
