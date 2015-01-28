
/** Destination class header.
	@file Destination.hpp

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

#ifndef SYNTHESE_pt_Destination_hpp__
#define SYNTHESE_pt_Destination_hpp__

#include "Object.hpp"

#include "DataSourceLinksField.hpp"
#include "Registrable.h"
#include "Registry.h"
#include "ImportableTemplate.hpp"

namespace synthese
{
	FIELD_STRING(DisplayedText)
	FIELD_STRING(TtsText)
	FIELD_STRING(Comment)
	
	typedef boost::fusion::map<
		FIELD(Key),
		FIELD(DisplayedText),
		FIELD(TtsText),
		FIELD(Comment),
		FIELD(impex::DataSourceLinks)
	> DestinationSchema;

	namespace pt
	{
		/** Destination class.
			@ingroup m35
		*/
		class Destination:
			public virtual Object<Destination, DestinationSchema>,
			public virtual util::Registrable,
			public impex::ImportableTemplate<Destination>
		{
		public:
			/// Chosen registry class.
			typedef util::Registry<Destination> Registry;

			Destination(
				util::RegistryKeyType id = 0
			);

		private:
			std::string _displayedText;
			std::string _ttsText;
			std::string _comment;

		public:
			const std::string& getDisplayedText() const { return _displayedText; }
			const std::string& getTTSText() const { return _ttsText; }
			const std::string& getComment() const { return _comment; }

			void setDisplayedText(const std::string& value){ _displayedText = value; }
			void setTTSText(const std::string& value){ _ttsText = value; }
			void setComment(const std::string& value){ _comment = value; }
		};
	}
}

#endif // SYNTHESE_pt_Destination_hpp__
