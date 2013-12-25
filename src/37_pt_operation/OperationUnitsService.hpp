
//////////////////////////////////////////////////////////////////////////////////////////
///	OperationUnitsService class header.
///	@file OperationUnitsService.hpp
///	@author hromain
///	@date 2013
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_OperationUnitsService_H__
#define SYNTHESE_OperationUnitsService_H__

#include "FactorableTemplate.h"
#include "Function.h"

namespace synthese
{
	namespace pt_operation
	{
		//////////////////////////////////////////////////////////////////////////
		///	37.15 Function : OperationUnitsService.
		/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Operation_units
		//////////////////////////////////////////////////////////////////////////
		///	@ingroup m37Functions refFunctions
		///	@author hromain
		///	@date 2013
		/// @since 
		class OperationUnitsService:
			public util::FactorableTemplate<server::Function,OperationUnitsService>
		{
		public:
			static const std::string TAG_OPERATION_UNIT;
			
		protected:
			//! \name Page parameters
			//@{
			//@}
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Operation_units#Request
			//////////////////////////////////////////////////////////////////////////
			///	@return Generated parameters map
			/// @author hromain
			/// @date 2013
			/// @since 
			util::ParametersMap _getParametersMap() const;
			
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Operation_units#Request
			//////////////////////////////////////////////////////////////////////////
			///	@param map Parameters map to interpret
			/// @author hromain
			/// @date 2013
			/// @since 
			virtual void _setFromParametersMap(
				const util::ParametersMap& map
			);
			
			
		public:
			//////////////////////////////////////////////////////////////////////////
			/// Display of the content generated by the function.
			/// @param stream Stream to display the content on.
			/// @param request the current request
			/// @author hromain
			/// @date 2013
			virtual util::ParametersMap run(std::ostream& stream, const server::Request& request) const;
			
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Gets if the function can be run according to the user of the session.
			/// @param session the current session
			/// @return true if the function can be run
			/// @author hromain
			/// @date 2013
			virtual bool isAuthorized(const server::Session* session) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets the Mime type of the content generated by the function.
			/// @return the Mime type of the content generated by the function
			/// @author hromain
			/// @date 2013
			virtual std::string getOutputMimeType() const;
		};
}	}

#endif // SYNTHESE_OperationUnitsService_H__
