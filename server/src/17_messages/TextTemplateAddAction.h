
/** TextTemplateAddAction class header.
	@file TextTemplateAddAction.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#ifndef SYNTHESE_TextTemplateAddAction_H__
#define SYNTHESE_TextTemplateAddAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace messages
	{
		/** TextTemplateAddAction action class.
			@ingroup m17Actions refActions
		*/
		class TextTemplateAddAction : public util::FactorableTemplate<server::Action, TextTemplateAddAction>
		{
		public:
			static const std::string PARAMETER_NAME;
			static const std::string PARAMETER_SHORT_MESSAGE;
			static const std::string PARAMETER_LONG_MESSAGE;
			static const std::string PARAMETER_PARENT_ID;
			static const std::string PARAMETER_IS_FOLDER;

		private:
			std::string		_name;
			std::string		_shortMessage;
			std::string		_longMessage;
			util::RegistryKeyType		_parentId;
			bool			_isFolder;

			/** Authorization control.
				@return True if the action run is authorized
				@author Hugues Romain
				@date 2007
			*/
			virtual bool isAuthorized(const server::Session* session) const;

		protected:
			/** Conversion from attributes to generic parameter maps.
			*/
			util::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
				Removes the used parameters from the map.
				@exception ActionException Occurs when some parameters are missing or incorrect.
			*/
			void _setFromParametersMap(const util::ParametersMap& map);

		public:
			/** Action to run, defined by each subclass.
			*/
			void run(server::Request& request);

			//! @name Setters
			//@{
				void setParentId(util::RegistryKeyType value) { _parentId = value; }
				void setIsFolder(bool value) { _isFolder = value; }
			//@}
		};
	}
}

#endif // SYNTHESE_TextTemplateAddAction_H__
