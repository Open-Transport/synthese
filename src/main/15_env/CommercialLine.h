
/** CommercialLine class header.
	@file CommercialLine.h

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

#ifndef SYNTHESE_CommercialLine_h__
#define SYNTHESE_CommercialLine_h__

#include <string>

#include "01_util/Registrable.h"
#include "01_util/Uid.h"
#include "01_util/RGBColor.h"

namespace synthese
{
	namespace env
	{
		class TransportNetwork;

		/** Commercial line class.
			@ingroup m15
		*/
		class CommercialLine : public util::Registrable<uid, CommercialLine>
		{
		private:
			std::string			_name;		//!< Name (code)
			std::string			_shortName;	//!< Name (cartouche)
			std::string			_longName;	//!< Name for schedule card

			util::RGBColor		_color;		//!< Line color
			std::string			_style;		//!< CSS style (cartouche)
			std::string			_image;		//!< Display image (cartouche)

			TransportNetwork*	_network;	//!< Network

		public:
			CommercialLine();

			const std::string& getStyle () const;
			void setStyle (const std::string& style);

			const TransportNetwork* getNetwork () const;
			void setNetwork (TransportNetwork* network);

			const std::string& getShortName () const;
			void setShortName (const std::string& shortName);

			const std::string& getLongName () const;
			void setLongName (const std::string& longName);

			const std::string& getImage () const;
			void setImage (const std::string& image);

			const util::RGBColor& getColor () const;
			void setColor (const util::RGBColor& color);

			const std::string& getName () const;
			void setName (const std::string& name);
		};
	}
}

#endif // SYNTHESE_CommercialLine_h__
