////////////////////////////////////////////////////////////////////////////////
/// ActionException class header.
///	@file ActionException.h
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_ActionException_H__
#define SYNTHESE_ActionException_H__

#include "Exception.h"
#include "UtilTypes.h"
#include "ObjectNotFoundException.h"
#include "ParametersMap.h"
#include "Action.h"

#include <string>
#include <iostream>

namespace synthese
{
	namespace server
	{
		////////////////////////////////////////////////////////////////////////
		/// Action related exception class.
		/// @ingroup m15/exception
		class ActionException:
			public synthese::Exception
		{
			//lint --e{1712}

		public:
			////////////////////////////////////////////////////////////////////
			///	Action parameter reading exception constructor.
			///	@param field Field that has failed
			///	@param source Name of the action
			/// @param e Catch exception
			///	@author Hugues Romain
			///	@date 2008
			explicit ActionException(
				const util::ParametersMap::MissingParameterException& e,
				const Action& action
			) throw();



			////////////////////////////////////////////////////////////////////
			///	Action parameter reading exception constructor.
			///	@param field Field that has failed
			/// @param id ID of the object
			///	@param source Name of the action
			/// @param e Catch exception
			///	@author Hugues Romain
			///	@date 2008
			template<class C>
			explicit ActionException(
				const std::string& field,
				const util::ObjectNotFoundException<C>& e,
				const Action& action
			) throw():
				Exception("Specified "+ field + " " + boost::lexical_cast<std::string>(e.getKey()) +" not found in "+ action.getFactoryKey() + ":" + e.getMessage())
			{

			}



			////////////////////////////////////////////////////////////////////
			///	Simple action related exception constructor.
			///	@param message Exception message
			///	@author Hugues Romain
			explicit ActionException(
				const std::string& message
			) throw();



			////////////////////////////////////////////////////////////////////
			///	Action related exception destructor.
			///	@author Hugues Romain
			~ActionException () throw ();
		};
	}
}

#endif // SYNTHESE_ActionException_H__
