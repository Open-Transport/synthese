////////////////////////////////////////////////////////////////////////////////
/// SimplePageFunction class header.
///	@file SimplePageFunction.h
///	@author Hugues Romain
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

#ifndef SYNTHESE_SimplePageRequest_H__
#define SYNTHESE_SimplePageRequest_H__

#include <boost/shared_ptr.hpp>

#include "RequestWithInterface.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace interfaces
	{
		class InterfacePage;

		////////////////////////////////////////////////////////////////////
		/// Request for direct displaying an interface page.
		///	@ingroup m11Functions refFunctions
		///
		///	Arguments :
		///		- page : page to display (empty or not provided = no display, action only)
		///
		///	NB : Only non registered pages can be displayed. To display a
		/// registered page, call a proper function directly.
		class SimplePageFunction:
			public util::FactorableTemplate<RequestWithInterface, SimplePageFunction>
		{
			static const std::string PARAMETER_PAGE;
			static const std::string PARAMETER_PAGE_CLASS;

			//! \name Request parameters
			//@{
			const interfaces::InterfacePage*	_page;
			server::ParametersMap				_parameters;
			//@}


			/** Conversion from attributes to generic parameter maps.
			*/
			server::ParametersMap _getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
			*/
			void _setFromParametersMap(const server::ParametersMap& map);

			/** Action to run, defined by each subclass.
			*/
			void _run(std::ostream& stream) const;

			/** This function can be used without any session.
				@return false
			*/
			virtual bool _runBeforeDisplayIfNoSession(std::ostream& stream);

		public:
			void setPage(const interfaces::InterfacePage* page);

			virtual bool _isAuthorized() const;

			virtual std::string getOutputMimeType() const;
		};
	}
}

#endif // SYNTHESE_SimplePageRequest_H__
