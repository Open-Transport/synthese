
/** AjaxForm class implementation.
	@file AjaxForm.cpp

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

#include "AjaxForm.hpp"
#include "HTMLModule.h"

using namespace std;

namespace synthese
{
	namespace html
	{
		AjaxForm::AjaxForm(
			const std::string& name,
			const std::string& action
		):	HTMLForm(name, action)
		{}



		string AjaxForm::open(
			string htmlComplement /*= std::string()*/
		) const {
			return openWithJSComplement(string(), htmlComplement);
		}


		std::string AjaxForm::openWithJSComplement(
			const string& sendJSComplement,
			string htmlComplement, /*= std::string()*/
			bool closeJSTag
		) const {
			if (!_updateRight)
				return string();

			stringstream s;
			s	<< "<form "
				<< "name=\"" << _name << "\" "
				<< "action=\"\" "
				<< "onsubmit=\"return false;\" "
				<< htmlComplement
				<< ">"
				<< HTMLModule::GetHTMLJavascriptOpen()
				<< "var xajax = null;"
				<< "function send_ajax_" << _name << "(){"
				<< sendJSComplement
				<< "var content = '';"
				<< "var elems=document.forms['" << _name << "'].elements;"
				<< "document.forms['" << _name << "'].className='waiting';"
				<< "for(var i=0; i<elems.length; ++i){"
				<< "if(elems[i].name.length == 0) continue;"
				<< "if(content.length > 0) content+= '&';"
				<< "content += elems[i].name +'='+ encodeURIComponent(elems[i].value);"
				<< "}"
				<< "if(window.XMLHttpRequest) xajax = new XMLHttpRequest();"
				<< "else if(window.ActiveXObject) xajax = new ActiveXObject('Microsoft.XMLHTTP');"
				<< "else return false;"
				<< "xajax.open('POST','" << _action << "');"
				<< "xajax.setRequestHeader('Content-Type','application/x-www-form-urlencoded');"
				<< "xajax.send(content);"
				<< "xajax.onreadystatechange = function(){"
				<< "if(xajax.readyState == 4) {"
				<< "document.forms['" << _name << "'].className='';"
				<< "if(xajax.responseText) alert(xajax.responseText);"
				<< "}};}"
			;
			if(closeJSTag)
			{
				s << HTMLModule::GetHTMLJavascriptClose();
			}

			return s.str();
		}



		std::string AjaxForm::getSubmitButton(
			const std::string& caption,
			string confirm,
			string icon
		){
			if (!_updateRight)
				return string();

			return HTMLModule::getLinkButton("send_ajax_"+ _name +"();", caption, confirm, icon, true);
		}

}	}
