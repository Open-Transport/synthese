
/** Html class implementation.
	@file Html.cpp

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

#include "01_util/Html.h"

using namespace std;

namespace synthese
{
	namespace util
	{


		std::string Html::getSelectNumberInput(const std::string& name, int mini, int maxi, int value/*=UNKNOWN_VALUE*/, int step )
		{
			std::map<int,int> m;
			for (int i=mini; i<=maxi; i += step)
				m.insert(std::make_pair(i,i));
			return getSelectInput(name, m, value);
		}

		std::string Html::getTextInput(const std::string& name, const std::string& value, std::string displayTextBeforeTyping/*=""*/)
		{
			stringstream s;
			s << "<input type=\"text\" name=\"" << name << "\" value=\"";
			if (value == "" && displayTextBeforeTyping != "")
				s << displayTextBeforeTyping << "\" onfocus=\"if(this.value == '" << displayTextBeforeTyping << "') this.value='';\"";
			else
				s << value << "\"";
			s << " />";
			return s.str();
		}

		std::string Html::getHiddenInput( const std::string& name, const std::string& value )
		{
			stringstream s;
			s << "<input type=\"hidden\" name=\"" << name << "\" value=\"" << value << "\" />";
			return s.str();
		}

		std::string Html::getSubmitButton( const std::string& caption )
		{
			stringstream s;
			s << "<input type=\"submit\" value=\"" << caption << "\" />";
			return s.str();
		}

		std::string Html::setFocus( const std::string& formName, const std::string& fieldName )
		{
			stringstream s;
			s << "<script type=\"text/javascript\">document." << formName << "." << fieldName << ".focus();</script>";
			return s.str();
		}

		std::string Html::getTextAreaInput( const std::string& name, const std::string& value, int rows, int cols )
		{
			stringstream s;
			s << "<textarea name=\"" << name << "\" rows=\"" << rows << "\" cols=\"" << cols << "\">" 
				<< value << "</textarea>";
			return s.str();
		}

		std::string Html::getOuiNonRadioInput( const std::string& name, bool value )
		{
			map<int, string> m;
			m.insert(make_pair(0, "NON"));
			m.insert(make_pair(1, "OUI"));
			return getRadioInput(name, m, value ? 1 : 0);
		}

		std::string Html::getCheckBox( const std::string& name, const std::string& value, bool checked )
		{
			stringstream s;
			s << "<input type=\"checkbox\" name=\"" << name << value << "\" " << (checked ? " checked=\"true\"" : "") << " />";
			return s.str();
		}

		std::string Html::getPasswordInput( const std::string& name, const std::string& value )
		{
			stringstream s;
			s << "<input type=\"password\" name=\"" << name << "\" value=\"" << value << "\"" << " />";
			return s.str();
		}

		std::string Html::getLinkButton( const std::string& url, const std::string& caption )
		{
			stringstream s;
			s	<< "<table class=\"linkbutton\"><tr>"
				<< "<td class=\"linkbutton\" "
				<< "onmouseover=\"this.className='activatedlinkbutton';\" "
				<< "onmouseout=\"this.className='linkbutton';\" "
				<< "onmousedown=\"this.className='clickedlinkbutton';\" "
				<< "onmouseup=\"this.className='activatedlinkbutton';\" "
				<< "onclick=\"window.location='" << url << "';\" "
				<< ">"
				<< "<a href=\"" << url << "\">" << caption << "</a></td></tr></table>";
			return s.str();
		}
	}
}