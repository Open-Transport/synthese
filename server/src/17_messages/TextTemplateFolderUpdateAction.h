
/** TextTemplateFolderUpdateAction class header.
	@file TextTemplateFolderUpdateAction.h
	@author Hugues Romain
	@date 2008

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

#ifndef SYNTHESE_TextTemplateFolderUpdateAction_H__
#define SYNTHESE_TextTemplateFolderUpdateAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace messages
	{
		class TextTemplate;

		//////////////////////////////////////////////////////////////////////////
		/// 17.15 Action : text template folder update.
		///	@ingroup m17Actions refActions
		class TextTemplateFolderUpdateAction
			: public util::FactorableTemplate<server::Action, TextTemplateFolderUpdateAction>
		{
		public:
			static const std::string PARAMETER_FOLDER_ID;
			static const std::string PARAMETER_NAME;
			static const std::string PARAMETER_PARENT_ID;

		private:
			boost::shared_ptr<TextTemplate>	_folder;
			std::string						_name;
			boost::shared_ptr<const TextTemplate>	_parent;


		protected:
			/** Conversion from attributes to generic parameter maps.
				@return Generated parameters map
			*/
			util::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
				Removes the used parameters from the map.
				@param map Parameters map to interpret
				@exception ActionException Occurs when some parameters are missing or incorrect.
			*/
			void _setFromParametersMap(const util::ParametersMap& map);

		public:
			/** Action to run, defined by each subclass.
			*/
			void run(server::Request& request);

			TextTemplateFolderUpdateAction();

			void setFolderId(util::RegistryKeyType id);

			virtual bool isAuthorized(const server::Session* session) const;
		};
	}
}

#endif // SYNTHESE_TextTemplateFolderUpdateAction_H__
