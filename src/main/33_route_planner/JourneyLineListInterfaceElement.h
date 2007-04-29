
/** JourneyLineListInterfaceElement class header.
	@file JourneyLineListInterfaceElement.h

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

#include "11_interfaces/LibraryInterfaceElement.h"

namespace synthese
{
	namespace interfaces
	{
		class ValueInterfaceElement;
	}

	namespace routeplanner
	{
		class JourneyLineListInterfaceElement : public interfaces::LibraryInterfaceElement
		{
		private:
			boost::shared_ptr<interfaces::ValueInterfaceElement> _displayPedestrianLines;
			boost::shared_ptr<interfaces::ValueInterfaceElement> _rowStartHtml;
			boost::shared_ptr<interfaces::ValueInterfaceElement> _rowEndHtml;
			boost::shared_ptr<interfaces::ValueInterfaceElement> _pixelWidth;
			boost::shared_ptr<interfaces::ValueInterfaceElement> _pixelHeight;
			
		public:
			/** Display.
				@param stream Stream to write on
				@param parameters Runtime parameters
				@param object (Journey*) Journey to display
				@param site Site to display
			*/
			std::string display(
				std::ostream& stream
				, const interfaces::ParametersVector& parameters
				, interfaces::VariablesMap& variables
				, const void* object = NULL
				, const server::Request* request = NULL
				) const;

			/** Parser.
			@param text Text to parse : standard list of parameters :
				-# Affichage des lignes à pied
				-# HTML en début de ligne (conseillé tr)
				-# HTML en fin de ligne (conseillé /tr)
				-# Largeur en pixels de la case de lignes
				-# Hauteur en pixels de la case de lignes
			*/
			void storeParameters(interfaces::ValueElementList& vel);
		};

	}
}
