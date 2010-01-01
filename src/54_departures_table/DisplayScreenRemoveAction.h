////////////////////////////////////////////////////////////////////////////////
/// DisplayScreenRemoveAction class header.
///	@file DisplayScreenRemoveAction.h
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_DisplayScreenRemove_H__
#define SYNTHESE_DisplayScreenRemove_H__

#include "Action.h"
#include "FactorableTemplate.h"
#include "ActionException.h"

namespace synthese
{
	namespace departurestable
	{
		class DisplayScreen;

		////////////////////////////////////////////////////////////////////
		/// Display screen remove action class.
		///	@ingroup m54Actions refActions
		class DisplayScreenRemoveAction : public util::FactorableTemplate<server::Action, DisplayScreenRemoveAction>
		{
		public:
			static const std::string PARAMETER_DISPLAY_SCREEN_ID;

		private:
			boost::shared_ptr<const DisplayScreen> _displayScreen;

		protected:
			////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			///	@return Generated parameters map
			server::ParametersMap getParametersMap() const;



			////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			///	@param map Parameters map to interpret
			///	@throws ActionException Occurs when some parameters are missing or incorrect.
			void _setFromParametersMap(const server::ParametersMap& map) throw(server::ActionException);

			virtual bool isAuthorized(const security::Profile& profile) const;

		public:
			////////////////////////////////////////////////////////////////////
			/// Runs the action.
			void run(
				server::Request& request
			) throw(server::ActionException);

			////////////////////////////////////////////////////////////////////
			/// Display screen setter
			/// @param screen the screen
			void setDisplayScreen(boost::shared_ptr<const DisplayScreen> screen);
		};
	}
}

#endif // SYNTHESE_DisplayScreenRemove_H__
