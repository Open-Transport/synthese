
/** ScenarioTemplate class header.
	@file ScenarioTemplate.h

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

#ifndef SYNTHESE_ScenarioTemplate_h__
#define SYNTHESE_ScenarioTemplate_h__

#include "ScenarioSubclassTemplate.h"
#include "Registry.h"

namespace synthese
{
	namespace messages
	{
		class AlarmTemplate;

		/** Scenario template class.
			@ingroup m17
		*/
		class ScenarioTemplate
		:	public ScenarioSubclassTemplate<AlarmTemplate>
		{
		public:

			/// Chosen registry class.
			typedef util::Registry<ScenarioTemplate>	Registry;

		private:
			uid	_folderId;

		public:
			ScenarioTemplate(const std::string name = std::string());
			ScenarioTemplate(const ScenarioTemplate& source, const std::string& name);
			ScenarioTemplate(util::RegistryKeyType key);
			~ScenarioTemplate();
			uid	getFolderId()	const;

			void	setFolderId(uid value);
		};
	}
}

#endif // SYNTHESE_ScenarioTemplate_h__
