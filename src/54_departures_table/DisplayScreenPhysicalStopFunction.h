
/** DisplayScreenPhysicalStopFunction class header.
	@file DisplayScreenPhysicalStopFunction.h
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

#ifndef SYNTHESE_DisplayScreenPhysicalStopFunction_H__
#define SYNTHESE_DisplayScreenPhysicalStopFunction_H__

#include "01_util/FactorableTemplate.h"

#include "30_server/Function.h"

namespace synthese
{
	namespace departurestable
	{
		class DisplayScreen;
		class DisplayType;

		/** DisplayScreenPhysicalStopFunction Function class.
			@todo Integrate this function intoDisplayScreenContentRequest when IGN Geoportail will not link to "fonction=td" anymore
			@author Hugues Romain
			@date 2008
			@ingroup m54Functions refFunctions
		*/
		class DisplayScreenPhysicalStopFunction : public util::FactorableTemplate<server::Function,DisplayScreenPhysicalStopFunction>
		{
		public:
			static const std::string PARAMETER_INTERFACE_ID;
			static const std::string PARAMETER_OPERATOR_CODE;
			
		protected:
			//! \name Page parameters
			//@{
				boost::shared_ptr<const DisplayScreen>	_screen;
				boost::shared_ptr<DisplayType>			_type;
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
			*/
			void run(std::ostream& stream, const server::Request& request) const;

			virtual bool isAuthorized(const security::Profile& profile) const;

			virtual std::string getOutputMimeType() const;
		};
	}
}

#endif // SYNTHESE_DisplayScreenPhysicalStopFunction_H__
