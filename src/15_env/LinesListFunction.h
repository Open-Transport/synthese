////////////////////////////////////////////////////////////////////////////////
/// LinesListFunction class header.
///	@file LinesListFunction.h
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

#ifndef SYNTHESE_LinesListFunction_H__
#define SYNTHESE_LinesListFunction_H__

#include "FactorableTemplate.h"
#include "Function.h"

#include <boost/shared_ptr.hpp>

namespace synthese
{
	namespace env
	{
		class TransportNetwork;

		////////////////////////////////////////////////////////////////////
		/// Lines list public Function class.
		/// @author Hugues Romain
		///	@date 2008
		///	@ingroup m35Functions refFunctions
		class LinesListFunction : public util::FactorableTemplate<server::Function,LinesListFunction>
		{
		 public:
			static const std::string PARAMETER_NETWORK_ID;

		protected:
			//! \name Page parameters
			//@{
				boost::shared_ptr<const TransportNetwork> _network;
			//@}


			/** Conversion from attributes to generic parameter maps.
				@return Generated parameters map
			*/
			server::ParametersMap _getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
				@param map Parameters map to interpret
			*/
			void _setFromParametersMap(const server::ParametersMap& map);

		public:
			/** Action to run, defined by each subclass.
			*/
			void _run(std::ostream& stream) const;

			void setNetworkId(
				util::RegistryKeyType id
			) throw(server::RequestException);

			virtual bool _isAuthorized() const;
		};
	}
}

#endif // SYNTHESE_LinesListFunction_H__
