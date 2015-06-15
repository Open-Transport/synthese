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

#include "Object.hpp"
#include "PointersSetField.hpp"
#include "DataSourceLinksField.hpp"
#include "ImportableTemplate.hpp"

namespace synthese
{

	
	namespace messages
	{
		class MessagesSection;
		class Alarm;
		class ScenarioCalendar;

		FIELD_DATASOURCE_LINKS(DataSourceLinksWithoutUnderscore)
		FIELD_POINTERS_SET(Sections, MessagesSection)
		
		////////////////////////////////////////////////////////////////////
		/// Scenario of alarms diffusion.
		///	@ingroup m17
		///
		///	A scenario is a collection of alarms.
		///	The type of the contained alarms defines two categories of scenario :
		///		- the template scenario
		///		- the sent scenario
		class Scenario:
			public virtual util::Registrable
		{
		protected:

		public:

			Scenario(util::RegistryKeyType id=0);
			virtual ~Scenario();

			typedef std::set<const Alarm*> Messages;
			typedef std::set<ScenarioCalendar*> ScenarioCalendars;

		private:
			
			mutable Messages _messages;
			mutable ScenarioCalendars _calendars;

		public:
			
			void addMessage(const Alarm& message) const;
			void removeMessage(const Alarm& message) const;
			void addSection(const MessagesSection& section) const;

			/// @name Setters
			//@{
			void setCalendars(const ScenarioCalendars& value) const { _calendars = value; }
			//@}
			
			/// @name Getters
			//@{
			const Messages& getMessages() const { return _messages; }
			const ScenarioCalendars& getCalendars() const { return _calendars; }
			virtual Sections::Type& getSections() const {
				throw "TODO : necessary because boost:optional on an abstract type is not possible"; }
			//@}
							   
		};
		
		typedef std::vector<boost::shared_ptr<Scenario> > Scenarios;
		
	}
}

#endif // SYNTHESE_Scenario_h__
