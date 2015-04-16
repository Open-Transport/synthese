
/** TinyMCE class header.
	@file TinyMCE.hpp

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

#ifndef SYNTHESE_html_TinyMCE_hpp__
#define SYNTHESE_html_TinyMCE_hpp__

#include <string>

namespace synthese
{
	namespace html
	{
		//////////////////////////////////////////////////////////////////////////
		/// TinyMCE helper class.
		/// See http://wiki.moxiecode.com/index.php/TinyMCE:Index
		///	@ingroup m05
		/// @author Hugues Romain
		/// @version 3.2.1
		/// @date 2010
		class TinyMCE
		{
		private:
			bool _withImageManager;
			bool _withFileManager;
			bool _withPageBreak;
			bool _withSpellChecker;
			bool _withEmoticons;
			std::string _ajaxSaveURL;

		public:
			static const std::string MCE_EDITOR_ACTIVATION_CLASS;

			TinyMCE();

			//! @name Setters
			//@{
				void setWithImageManager(bool value){ _withImageManager = value; }
				void setWithFileManager(bool value){ _withFileManager = value; }
				void setWithPageBreak(bool value){ _withPageBreak = value; }
				void setWithSpellChecker(bool value){ _withSpellChecker = value; }
				void setWithEmoticons(bool value){ _withEmoticons = value; }
				void setAjaxSaveURL(const std::string& value){ _ajaxSaveURL = value; }
			//@}


			//////////////////////////////////////////////////////////////////////////
			/// Generates opening script.
			std::string open() const;



			//////////////////////////////////////////////////////////////////////////
			/// @param name name of the field
			/// @param value value of the field
			static std::string GetFakeFormWithInput(
				const std::string& name,
				const std::string& value
			);
		};
}	}

#endif // SYNTHESE_html_TinyMCE_hpp__
