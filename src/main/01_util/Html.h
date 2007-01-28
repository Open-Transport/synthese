
/** Html class header.
	@file Html.h

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

#ifndef SYNTHESE_Html_H__
#define SYNTHESE_Html_H__

#include <string>
#include <sstream>
#include <map>

#include "01_util/Constants.h"

namespace synthese
{
	namespace util
	{
		class Html
		{
		public:
			template<class K, class T>
			static std::string getSelectInput(const std::string& name, const std::map<K, T>& choices, const K& value);
			
			static std::string getSelectNumberInput(const std::string& name, int mini, int maxi, int value=UNKNOWN_VALUE);

			static std::string getTextInput(const std::string& name, const std::string& value, std::string displayTextBeforeTyping="");

			static std::string getHiddenInput(const std::string& name, const std::string& value);

			static std::string getSubmitButton(const std::string& caption);

			static std::string setFocus(const std::string& formName, const std::string& fieldName);

		};

		template<class K, class T>
			static std::string Html::getSelectInput(const std::string& name, const std::map<K, T>& choices, const K& value)
		{
			std::stringstream s;
			s << "<select name=\"" << name << "\">";
			for (std::map<K, T>::const_iterator it = choices.begin(); it != choices.end(); ++it)
			{
				s << "<option value=\"" << it->first << "\"";
				if (it->first == value)
					s << " selected=\"1\"";
				s << ">" << it->second << "</option>";
			}
			s << "</select>";
			return s.str();
		}
	}
}

#endif // SYNTHESE_Html_H__
