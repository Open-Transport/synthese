////////////////////////////////////////////////////////////////////////////////
/// Scenario class header.
///	@file Scenario.h
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

#ifndef SYNTHESE_Scenario_h__
#define SYNTHESE_Scenario_h__

#include "ImportableTemplate.hpp"
#include "Named.h"
#include "ParametersMap.h"
#include "Registrable.h"
#include "Registry.h"

namespace synthese
{
	namespace messages
	{
		class Alarm;
		class MessagesSection;

		////////////////////////////////////////////////////////////////////
		/// Scenario of alarms diffusion.
		///	@ingroup m17
		///
		///	A scenario is a collection of alarms.
		///	The type of the contained alarms defines two categories of scenario :
		///		- the template scenario
		///		- the sent scenario
		class Scenario:
			public virtual util::Registrable,
			public impex::ImportableTemplate<Scenario>,
			public util::Named
		{
		public:
			typedef std::set<const MessagesSection*> Sections;
			typedef std::set<const Alarm*> Messages;

		private:

			Sections _sections;

			mutable Messages _messages;

		protected:
			Scenario(const std::string name = std::string());

		public:
			/// Chosen registry class.
			typedef util::Registry<Scenario>	Registry;


			virtual ~Scenario();

			void addMessage(const Alarm& message) const;
			void removeMessage(const Alarm& message) const;
			const Messages& getMessages() const { return _messages; }



			const Sections& getSections() const { return _sections; }
			void setSections(const Sections& value){ _sections = value; }
		};
	}
}

#endif // SYNTHESE_Scenario_h__
