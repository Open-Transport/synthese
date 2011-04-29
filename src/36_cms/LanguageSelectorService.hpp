
//////////////////////////////////////////////////////////////////////////////////////////
/// LanguageSelectorService class header.
///	@file LanguageSelectorService.hpp
///	@author RCSobility
///	@date 2011
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_LanguageSelectorService_H__
#define SYNTHESE_LanguageSelectorService_H__

#include "FactorableTemplate.h"
#include "Function.h"

namespace synthese
{
	class Language;

	namespace cms
	{
		//////////////////////////////////////////////////////////////////////////
		///	36.15 Function : LanguageSelectorService.
		/// See https://extranet-rcsmobility.com/projects/synthese/wiki/https://extranet-rcsmobility.com/projects/synthese/wiki/Language_selector
		//////////////////////////////////////////////////////////////////////////
		///	@ingroup m36Functions refFunctions
		///	@author RCSobility
		///	@date 2011
		/// @since 3.2.1
		class LanguageSelectorService:
			public util::FactorableTemplate<server::Function,LanguageSelectorService>
		{
		public:
			static const std::string PARAMETER_LANGUAGE;
			static const std::string PARAMETER_NAME;
			static const std::string PARAMETER_VALUE;

		protected:
			//! \name Page parameters
			//@{
				const Language* _language;
				const Language* _value;
				std::string _name;
			//@}


			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			/// See https://extranet-rcsmobility.com/projects/synthese/wiki/https://extranet-rcsmobility.com/projects/synthese/wiki/Language_selector#Request
			//////////////////////////////////////////////////////////////////////////
			///	@return Generated parameters map
			/// @author RCSobility
			/// @date 2011
			/// @since 3.2.1
			server::ParametersMap _getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			/// See https://extranet-rcsmobility.com/projects/synthese/wiki/https://extranet-rcsmobility.com/projects/synthese/wiki/Language_selector#Request
			//////////////////////////////////////////////////////////////////////////
			///	@param map Parameters map to interpret
			/// @author RCSobility
			/// @date 2011
			/// @since 3.2.1
			virtual void _setFromParametersMap(
				const server::ParametersMap& map
			);


		public:
			LanguageSelectorService();

			//! @name Setters
			//@{
			//	void setObject(boost::shared_ptr<const Object> value) { _object = value; }
			//@}



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content generated by the function.
			/// @param stream Stream to display the content on.
			/// @param request the current request
			/// @author RCSobility
			/// @date 2011
			virtual void run(std::ostream& stream, const server::Request& request) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets if the function can be run according to the user of the session.
			/// @param session the current session
			/// @return true if the function can be run
			/// @author RCSobility
			/// @date 2011
			virtual bool isAuthorized(const server::Session* session) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets the Mime type of the content generated by the function.
			/// @return the Mime type of the content generated by the function
			/// @author RCSobility
			/// @date 2011
			virtual std::string getOutputMimeType() const;
		};
	}
}

#endif // SYNTHESE_LanguageSelectorService_H__
