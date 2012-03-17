
/** CreateDisplayScreenAction class header.
	@file CreateDisplayScreenAction.h

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

#ifndef SYNTHESE_CreateDisplayScreenAction_H__
#define SYNTHESE_CreateDisplayScreenAction_H__

#include "Action.h"
#include "FactorableTemplate.h"
#include "DisplayScreen.h"

#include <boost/shared_ptr.hpp>

namespace synthese
{
	namespace geography
	{
		class NamedPlace;
	}

	namespace departure_boards
	{
		class DisplayScreenCPU;

		//////////////////////////////////////////////////////////////////////////
		/// 54.15 Display screen creation action.
		/// Key : createdisplayscreen
		/// <h2>Parameters</h2>
		///	<h3>Copy from a template</h3>
		///		- actionParampti : id of the template to copy
		///		- actionParamna : name of the new screen (optional, default = Copie de <template name>)
		///
		/// <h3>Creation of sub-screen</h3>
		///		- actionParamup : id of the parent screen
		///		- actionParamst : role of the new screen
		///		- actionParamna : name of the new screen (optional)
		///
		/// <h3>Creation of screen connected to a CPU</h3>
		///		- actionParamcp : id of the CPU
		///		- actionParamna : name of the new screen (optional)
		///
		/// <h3>Creation of an unlinked screen</h3>
		///		- actionParampli : id of the place where the screen is located
		///		- actionParamna : name of the new screen (optional)
		//////////////////////////////////////////////////////////////////////////
		///	@ingroup m54Actions refActions
		/// @author Hugues Romain
		class CreateDisplayScreenAction:
			public util::FactorableTemplate<server::Action, CreateDisplayScreenAction>
		{
		public:
			static const std::string PARAMETER_LOCALIZATION_ID;
			static const std::string PARAMETER_TEMPLATE_ID;
			static const std::string PARAMETER_CPU_ID;
			static const std::string PARAMETER_UP_ID;
			static const std::string PARAMETER_NAME;
			static const std::string PARAMETER_SUB_SCREEN_TYPE;

		private:
			boost::shared_ptr<const DisplayScreen>			_template;
			boost::shared_ptr<const PlaceWithDisplayBoards>	_place;
			boost::shared_ptr<const DisplayScreenCPU>		_cpu;
			boost::shared_ptr<const DisplayScreen>			_up;
			std::string										_name;
			DisplayScreen::SubScreenType					_subScreenType;

		protected:
			/** Conversion from attributes to generic parameter maps.
			*/
			util::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
			Removes the used parameters from the map.
			*/
			void _setFromParametersMap(const util::ParametersMap& map);

		public:
			/** Action to run, defined by each subclass.
			*/
			void run(server::Request& request);

			void setPlace(const geography::NamedPlace* value);
			void setCPU(boost::shared_ptr<const DisplayScreenCPU> value){ _cpu = value; }
			void setUp(boost::shared_ptr<const DisplayScreen> value){ _up = value; }

			virtual bool isAuthorized(const server::Session* session) const;
		};
	}
}

#endif // SYNTHESE_CreateDisplayScreenAction_H__
