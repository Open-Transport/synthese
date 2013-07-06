
/** ScenarioFolder class header.
	@file ScenarioFolder.h

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

#ifndef SYNTHESE_messages_ScenarioFolder_h__
#define SYNTHESE_messages_ScenarioFolder_h__

#include "Registrable.h"
#include "Registry.h"
#include "UtilTypes.h"
#include <string>

namespace synthese
{
	namespace messages
	{
		/** Scenario Folder class.

			@ingroup m17
		*/
		class ScenarioFolder:
			public virtual util::Registrable
		{
		public:
			static const std::string ATTR_NAME;
			static const std::string ATTR_ID;

			/// Chosen registry class.
			typedef util::Registry<ScenarioFolder>	Registry;

		protected:
			ScenarioFolder* _parent;
			std::string	_name;

		public:
			ScenarioFolder(
				util::RegistryKeyType key = 0
			);

			//! @name Getters
			//@{
				ScenarioFolder*		getParent()	const;
				virtual std::string	getName()	const;
			//@}

			//! @name Setters
			//@{
				void	setName(const std::string& value);
				void	setParent(ScenarioFolder* value);
			//@}

			//! @name Queries
			//@{
				/** Full path getter.
					@return std::string The name of the folder and its parent separated by /
					@author Hugues Romain
					@date 2009
				*/
				std::string getFullName() const;



				virtual void toParametersMap(
					util::ParametersMap& map,
					bool withAdditionalParameters,
					boost::logic::tribool withFiles = boost::logic::indeterminate,
					std::string prefix = std::string()
				) const;
			//@}

		};
	}
}

#endif // SYNTHESE_messages_ScenarioFolder_h__
