
/** TridentExportFunction class header.
	@file TridentExportFunction.h
	@author Hugues Romain
	@date 2007

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

#ifndef SYNTHESE_TridentExportFunction_H__
#define SYNTHESE_TridentExportFunction_H__

#include "Function.h"
#include "FactorableTemplate.h"


namespace synthese
{
	namespace pt
	{
		/** TridentExportFunction Function class.
			@author Hugues Romain
			@date 2007
			@ingroup m15Functions refFunctions
		*/
		class TridentExportFunction
		: 	public util::FactorableTemplate<server::Function, TridentExportFunction>
		{
		public:
			static const std::string PARAMETER_LINE_ID;	//!< Commercial line id parameter
			static const std::string PARAMETER_WITH_TISSEO_EXTENSION;	//!< With tisseo extension parameter
			
		protected:
			//! \name Page parameters
			//@{
				boost::shared_ptr<const env::CommercialLine> _line; //!< Commercial line to export
				bool										_withTisseoExtension;
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
			TridentExportFunction();

			/** Action to run, defined by each subclass.
			*/
			void run(std::ostream& stream, const server::Request& request) const;

			virtual bool isAuthorized(const security::Profile& profile) const;

			virtual std::string getOutputMimeType() const;

			void setCommercialLine(boost::shared_ptr<const env::CommercialLine> line);
			void setWithTisseoExtension(bool value);
		};
	}
}

#endif // SYNTHESE_TridentExportFunction_H__
