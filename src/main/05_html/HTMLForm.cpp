
/** HTMLForm class implementation.
	@file HTMLForm.cpp

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

#include <sstream>
#include <map>

#include "04_time/DateTime.h"
#include "04_time/Date.h"

#include "05_html/HTMLForm.h"

using namespace std;

namespace synthese
{
	namespace html
	{


		std::string HTMLForm::open()
		{
			stringstream s;
			s	<< "<form "
				<< "name=\"" << _name << "\" "
				<< "method=\"post\" "
				<< "action=\"" << _action << "\" "
				<< ">";
			return s.str();
		}

		HTMLForm::HTMLForm( const string& name, const string& action)
			: _name(name), _action(action)
		{

		}

		std::string HTMLForm::getSelectNumberInput(const std::string& name, int mini, int maxi, int value/*=UNKNOWN_VALUE*/, int step )
		{
			std::map<int,int> m;
			for (int i=mini; i<=maxi; i += step)
				m.insert(std::make_pair(i,i));
			return getSelectInput(name, m, value);
		}

		std::string HTMLForm::getTextInput(const std::string& name, const std::string& value, std::string displayTextBeforeTyping/*=""*/)
		{
			_removeHiddenFieldIfExists(name);
			stringstream s;
			s	<< "<input "
				<< "type=\"text\" "
				<< "name=\"" << name << "\" "
				<< "value=\"";
			if (value == "" && displayTextBeforeTyping != "")
				s	<< displayTextBeforeTyping << "\" "
					<< "onfocus=\"if(this.value == '" << displayTextBeforeTyping << "') this.value='';\" ";
			else
				s << value << "\" ";
			s	<< "id=\"" << _getFieldId(name) << "\" "
				<< "/>";
			return s.str();
		}

		std::string HTMLForm::getSubmitButton( const std::string& caption )
		{
			stringstream s;
			s	<< "<input "
				<< "type=\"submit\" "
				<< "value=\"" << caption << "\" "
				<< "/>";
			return s.str();
		}

		std::string HTMLForm::setFocus(const std::string& fieldName )
		{
			stringstream s;
			s	<< "<script type=\"text/javascript\">"
				<< "document.getElementById('" << _getFieldId(fieldName, false) << "').focus();"
				<< "</script>";
			return s.str();
		}

		std::string HTMLForm::getTextAreaInput( const std::string& name, const std::string& value, int rows, int cols )
		{
			_removeHiddenFieldIfExists(name);
			stringstream s;
			s	<< "<textarea "
				<< "name=\"" << name << "\" "
				<< "rows=\"" << rows << "\" "
				<< "cols=\"" << cols << "\" "
				<< "id=\"" << _getFieldId(name) << "\" "
				<< ">" 
				<< value << "</textarea>";
			return s.str();
		}

		std::string HTMLForm::getOuiNonRadioInput( const std::string& name, bool value )
		{
			map<int, string> m;
			m.insert(make_pair(0, "NON"));
			m.insert(make_pair(1, "OUI"));
			return getRadioInput(name, m, value ? 1 : 0);
		}

		std::string HTMLForm::getCheckBox( const std::string& name, const std::string& value, bool checked )
		{
			_removeHiddenFieldIfExists(name);
			stringstream s;
			s	<< "<input "
				<< "type=\"checkbox\" "
				<< "name=\"" << name << value << "\" "
				<< (checked ? "checked=\"true\" " : "")
				<< "id=\"" << _getFieldId(name) << "\" "
				<< "/>";
			return s.str();
		}

		std::string HTMLForm::getPasswordInput( const std::string& name, const std::string& value )
		{
			_removeHiddenFieldIfExists(name);
			stringstream s;
			s	<< "<input "
				<< "type=\"password\" "
				<< "name=\"" << name << "\" "
				<< "value=\"" << value << "\" "
				<< "id=\"" << _getFieldId(name) << "\" "
				<< " />";
			return s.str();
		}

		std::string HTMLForm::getCalendarInput( const std::string& name, const time::DateTime& value )
		{
			_removeHiddenFieldIfExists(name);
			string fieldId = _getFieldId(name);
			string triggerId = _getFieldId(name + "TRIGGER");
			stringstream s;
			s	<< "<input "
					<< "type=\"text\" "
					<< "readonly=\"1\" "
					<< "name=\"" << name << "\" "
					<< "id=\"" << fieldId << "\" "
					<< "value=\"" << (value.isUnknown() ? "" : value.toString()) << "\" "
				<< "/><img "
					<< "src=\"img.gif\" "
					<< "style=\"cursor:pointer\" "
					<< "title=\"Date\" "
					<< "id=\"" << triggerId << "\" "
					<< "onmouseover=\"this.style.background=’red’;\" "
					<< "onmouseout=\"this.style.background=’’;\" "
				<< "/>"
				<< "<script type=\"text/javascript\">"
					<< "Calendar.setup({"
					<< "inputField:\"" << fieldId << "\","
					<< "button:\"" << triggerId << "\","
					<< "showsTime : true,"
					<< "ifFormat :\"%d/%m/%Y %H:%M\","
					<< "electric : false,"
					<< "singleClick:true,";
			if (!value.isUnknown())
				s << "date:new Date(" << value.getYear() <<","<< value.getMonth() <<","<< value.getDay() <<","<< value.getHours() <<"," << value.getMinutes() <<",0),";
			s		<< "firstDay:1"
					<< "});"
				<< "</script>";
			
			return s.str();
		}

		std::string HTMLForm::getCalendarInput( const std::string& name, const time::Date& value )
		{
			_removeHiddenFieldIfExists(name);
			string fieldId = _getFieldId(name);
			string triggerId = _getFieldId(name + "TRIGGER");
			stringstream s;
			s	<< "<input "
					<< "type=\"text\" "
					<< "readonly=\"1\" "
					<< "name=\"" << name << "\" "
					<< "id=\"" << fieldId << "\" "
					<< "value=\"" << (value.isUnknown() ? "" : value.toString()) << "\" "
				<< "/><img "
					<< "src=\"img.gif\" "
					<< "style=\"cursor:pointer\" "
					<< "title=\"Date\" "
					<< "id=\"" << triggerId << "\" "
					<< "onmouseover=\"this.style.background=’red’;\" "
					<< "onmouseout=\"this.style.background=’’;\" "
				<< "/>"
				<< "<script type=\"text/javascript\">"
					<< "Calendar.setup({"
					<< "inputField:\"" << fieldId << "\","
					<< "button:\"" << triggerId << "\","
					<< "showsTime : false,"
					<< "ifFormat :\"%d/%m/%Y\","
					<< "electric : false,"
					<< "singleClick:true,";
			if (!value.isUnknown())
				s << "date:new Date(" << value.getYear() <<","<< value.getMonth() <<","<< value.getDay() << "),";
			s		<< "firstDay:1"
					<< "});"
				<< "</script>";
			
			return s.str();
		}

		std::string HTMLForm::close()
		{
			stringstream s;
			s << getHiddenFields();
			s << "</form>";
			return s.str();
		}

		std::string HTMLForm::_getFieldId( const std::string& name, bool increment )
		{
			// Field ID counter
			int counter = 0;
			if (increment)
			{
				IDCounterMap::iterator it = _idCounter.find(name);
				if (it == _idCounter.end())
					_idCounter.insert(make_pair(name, 0));
				else
					counter = it->second++;
			}

			// Code generation
			stringstream s;
			s	<< _name << "_" << name << "_AUTOGENERATEDFIELDID";
			if (increment && counter)
				s << "_" << counter;
			
			// Return
			return s.str();
		}

		void HTMLForm::addHiddenField( const std::string& name, const std::string& value )
		{
			HiddenFieldsMap::iterator it = _hiddenFields.find(name);
			if (it == _hiddenFields.end())
				_hiddenFields.insert(make_pair(name, value));
			else
				it->second = value;
		}

		std::string HTMLForm::getLinkButton(const std::string& caption, const std::string confirm, const std::string icon)
		{
			stringstream url;
			url << _action << "?";
			for (HiddenFieldsMap::const_iterator it = _hiddenFields.begin(); it != _hiddenFields.end(); ++it)
			{
				if (it != _hiddenFields.begin())
					url << "&";
				url << it->first << "=" << it->second;
			}
			return HTMLModule::getLinkButton(url.str(), caption, confirm, icon);
		}

		std::string HTMLForm::getHiddenFields() const
		{
			stringstream s;
			for (HiddenFieldsMap::const_iterator it = _hiddenFields.begin(); it != _hiddenFields.end(); ++it)
				s << "<input type=\"hidden\" name=\"" << it->first << "\" value=\"" << it->second << "\" />";
			return s.str();
		}

		void HTMLForm::_removeHiddenFieldIfExists( const std::string& name )
		{
			HiddenFieldsMap::iterator it = _hiddenFields.find(name);
			if (it != _hiddenFields.end())
				_hiddenFields.erase(it);
		}
	}
}
