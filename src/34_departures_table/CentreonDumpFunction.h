
/** CentreonDumpFunction class header.
	@file CentreonDumpFunction.h
	@author Hugues Romain
	@date 2008

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

#ifndef SYNTHESE_CentreonDumpFunction_H__
#define SYNTHESE_CentreonDumpFunction_H__

// Util
#include "FactorableTemplate.h"

// Server
#include "Function.h"

namespace synthese
{
	namespace departurestable
	{
		/** CentreonDumpFunction Function class.
			@author Hugues Romain
			@date 2008
			@ingroup m54Functions refFunctions

			Tables correspondences :
				- Host = DisplayScreen
				- Hostgroup = DisplayType
				- Hostgroup = PublicTransportStopZoneCommercialPlace
				- Contact = User of Profile with supervision
				- Contactgroup = Profile
		*/
		class CentreonDumpFunction : public util::FactorableTemplate<server::Function,CentreonDumpFunction>
		{
		public:
			/// @todo request parameter names declaration
			// eg : static const std::string PARAMETER_xxx;
			
		protected:
			//! \name Page parameters
			//@{
				/// @todo Parsed parameters declaration
				// eg : const void*	_object;
				// eg : ParametersMap			_parameters;
			//@}
			
			
			/** Conversion from attributes to generic parameter maps.
				@return Generated parameters map
			*/
			server::ParametersMap _getParametersMap() const;
			
			/** Conversion from generic parameters map to attributes.
				@param map Parameters map to interpret
			*/
			void _setFromParametersMap(const server::ParametersMap& map);
			
			/// @todo Getters/Setters for parsed parameters
			
		public:
			/** Action to run, defined by each subclass.
				@param stream Stream to write the output on
			*/
			void _run(std::ostream& stream) const;

			virtual bool _isAuthorized() const;
		};
	}
}

#endif // SYNTHESE_CentreonDumpFunction_H__
