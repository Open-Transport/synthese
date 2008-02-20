
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

#include "HTMLForm.h"

#include "04_time/DateTime.h"

#include "01_util/Conversion.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace time;

	namespace html
	{

		std::string HTMLForm::open(const std::string htmlComplement)
		{
			if (!_updateRight)
				return string();

			stringstream s;
			s	<< "<form "
				<< "name=\"" << _name << "\" "
				<< "method=\"post\" "
				<< "action=\"" << _action << "\" "
				<< htmlComplement
				<< ">";
			return s.str();
		}

		HTMLForm::HTMLForm( const string& name, const string& action)
			: _name(name), _action(action), _updateRight(true)
		{

		}

		std::string HTMLForm::getSelectNumberInput(const std::string& name, int mini, int maxi, int value/*=UNKNOWN_VALUE*/, int step )
		{
			if (!_updateRight)
				return Conversion::ToString(value);

			std::vector<pair<int,int> > m;
			for (int i=mini; i<=maxi; i += step)
				m.push_back(std::make_pair(i,i));
			return getSelectInput(name, m, value);
		}

		std::string HTMLForm::getTextInput(const std::string& name, const std::string& value, std::string displayTextBeforeTyping/*=""*/)
		{
			if (!_updateRight)
				return value;

			removeHiddenFieldIfExists(name, value);

			return GetTextInput(name, value, displayTextBeforeTyping, _getFieldId(name));
		}



		std::string HTMLForm::GetTextInput(
			const std::string& name
			, const std::string& value
			, std::string displayTextBeforeTyping /*= std::string() */
			, std::string fieldId
		){
			if (fieldId.empty())
				fieldId = name + "__ID";

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
			s	<< "id=\"" << fieldId << "\" "
				<< "/>";
			return s.str();
		}

		std::string HTMLForm::getSubmitButton( const std::string& caption )
		{
			if (!_updateRight)
				return string();

			stringstream s;
			s	<< "<input "
				<< "type=\"submit\" "
				<< "value=\"" << caption << "\" "
				<< "/>";
			return s.str();
		}

		std::string HTMLForm::setFocus(const std::string& fieldName )
		{
			if (_updateRight)
				return string();

			stringstream s;
			s	<< HTMLModule::GetHTMLJavascriptOpen()
				<< "document.getElementById('" << _getFieldId(fieldName, false) << "').focus();"
				<< HTMLModule::GetHTMLJavascriptClose();
			return s.str();
		}

		std::string HTMLForm::getTextAreaInput( const std::string& name, const std::string& value, int rows, int cols )
		{
			if (!_updateRight)
				return value;

			removeHiddenFieldIfExists(name, value);
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
			vector<pair<int, string> > m;
			m.push_back(make_pair(0, "NON"));
			m.push_back(make_pair(1, "OUI"));
			return getRadioInput(name, m, value ? 1 : 0);
		}

		std::string HTMLForm::getCheckBox( const std::string& name, const std::string& value, bool checked )
		{
			if (!_updateRight)
				return string();

			removeHiddenFieldIfExists(name, value);
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
			if (!_updateRight)
				return string();
		
			removeHiddenFieldIfExists(name, value);
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
			if (!_updateRight)
				return value.toString();

			removeHiddenFieldIfExists(name, value.toString());
			string fieldId = _getFieldId(name);
			string spanId = _getFieldId(name + "SPAN");
			string triggerId = _getFieldId(name + "TRIGGER");

			return GetCalendarInput(name, value, fieldId, triggerId, spanId);
		}

		std::string HTMLForm::GetCalendarInput(
			const std::string& name
			, const time::DateTime& value
			, std::string fieldId
			, std::string triggerId
			, std::string spanId
		){
			if (fieldId.empty())
				fieldId = name + "__FID";
			if (triggerId.empty())
				triggerId = name + "__TID";
			if (spanId.empty())
				spanId = name + "__SID";

			stringstream s;
			s	<< "<input "
				<< "type=\"hidden\" "
				<< "readonly=\"1\" "
				<< "name=\"" << name << "\" "
				<< "id=\"" << fieldId << "\" "
				<< "value=\"" << (value.isUnknown() ? string() : value.toSQLString(false)) << "\" "
				<< "/><span class=\"calendar_display\" id=\"" << spanId << "\">"
				<< value.toString()
				<< "</span>"
				<< "<img "
				<< "src=\"calendar_edit.png\" "
				<< "style=\"cursor:pointer\" "
				<< "title=\"Date\" "
				<< "id=\"" << triggerId << "\" "
				<< "onmouseover=\"this.style.background='red';\" "
				<< "onmouseout=\"this.style.background='';\" "
				<< "/>"
				<< HTMLModule::GetHTMLJavascriptOpen()
				<< "Calendar.setup({"
				<< "inputField:\"" << fieldId << "\","
				<< "displayArea:\"" << spanId << "\","
				<< "button:\"" << triggerId << "\","
				<< "showsTime : true,"
				<< "ifFormat :\"%Y-%m-%d %H:%M\","
				<< "daFormat :\"%e/%m/%Y %H:%M\","
				<< "electric : false,"
				<< "singleClick:true,";
			if (!value.isUnknown())
				s << "date:new Date(" << value.getYear() <<","<< value.getMonth() <<","<< value.getDay() <<","<< value.getHours() <<"," << value.getMinutes() <<",0),";
			s		<< "firstDay:1"
				<< "});"
				<< HTMLModule::GetHTMLJavascriptClose();

			return s.str();

		}

		std::string HTMLForm::getCalendarInput( const std::string& name, const time::Date& value )
		{
			if (!_updateRight)
				return value.toString();

			removeHiddenFieldIfExists(name, value.toString());

			return GetCalendarInput(
				name
				, value
				, _getFieldId(name)
				, _getFieldId(name + "TRIGGER")
				, _getFieldId(name + "SPAN")
			);
		}

		std::string HTMLForm::GetCalendarInput(
			const std::string& name
			, const Date& value
			, string fieldId
			, string triggerId
			, string spanId
		){
			if (fieldId.empty())
				fieldId = name + "__FID";
			if (triggerId.empty())
				triggerId = name + "__TID";
			if (spanId.empty())
				spanId = name + "__SID";

			stringstream s;
			s	<< "<input "
				<< "type=\"hidden\" "
				<< "readonly=\"1\" "
				<< "name=\"" << name << "\" "
				<< "id=\"" << fieldId << "\" "
				<< "value=\"" << (value.isUnknown() ? "" : value.toSQLString(false)) << "\" "
				<< "/><span class=\"calendar_display\" id=\"" << spanId << "\">"
				<< value.toString()
				<< "</span>"
				<< "<img "
				<< "src=\"calendar_edit.png\" "
				<< "style=\"cursor:pointer\" "
				<< "title=\"Date\" "
				<< "id=\"" << triggerId << "\" "
				<< "onmouseover=\"this.style.background='red';\" "
				<< "onmouseout=\"this.style.background='';\" "
				<< "/>"
				<< HTMLModule::GetHTMLJavascriptOpen()
				<< "Calendar.setup({"
				<< "inputField:\"" << fieldId << "\","
				<< "displayArea:\"" << spanId << "\","
				<< "button:\"" << triggerId << "\","
				<< "showsTime : false,"
				<< "ifFormat :\"%Y-%m-%d\","
				<< "daFormat :\"%e/%m/%Y\","
				<< "electric : false,"
				<< "singleClick:true,";
			if (!value.isUnknown())
				s << "date:new Date(" << value.getYear() <<","<< value.getMonth() <<","<< value.getDay() << "),";
			s		<< "firstDay:1"
				<< "});"
				<< HTMLModule::GetHTMLJavascriptClose();

			return s.str();
		}

		std::string HTMLForm::close()
		{
			if (!_updateRight)
				return string();

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

		std::string HTMLForm::getLinkButton(const std::string& caption, const std::string confirm, const std::string icon) const
		{
			return HTMLModule::getLinkButton(getURL(), caption, confirm, icon);
		}

		std::string HTMLForm::getHiddenFields() const
		{
			if (!_updateRight)
				return string();

			stringstream s;
			for (HiddenFieldsMap::const_iterator it = _hiddenFields.begin(); it != _hiddenFields.end(); ++it)
				s << "<input type=\"hidden\" name=\"" << it->first << "\" value=\"" << it->second << "\" />";
			return s.str();
		}

		void HTMLForm::removeHiddenFieldIfExists( const std::string& name, const std::string& value )
		{
			HiddenFieldsMap::iterator it = _hiddenFields.find(name);
			if (it != _hiddenFields.end())
				_hiddenFields.erase(it);
			_initialFields.insert(make_pair(name, value));
		}

		std::string HTMLForm::getURL( HiddenFieldsMap overridingFields, bool withInitialValues ) const
		{
			stringstream url;
			url << _action << "?";

			HiddenFieldsMap fields = overridingFields;

			if (withInitialValues)
				fields.insert(_initialFields.begin(), _initialFields.end());
			
			fields.insert( _hiddenFields.begin(),  _hiddenFields.end());

			for (HiddenFieldsMap::const_iterator it = fields.begin(); it != fields.end(); ++it)
			{
				if (it != fields.begin())
					url << "&amp;";
				url << it->first << "=" << it->second;
			}

			return url.str();
		}

		void HTMLForm::setUpdateRight( bool value )
		{
			_updateRight = value;
		}

		std::string HTMLForm::getImageSubmitButton( const std::string& imageURL, const std::string& alt, string name )
		{
			if (!_updateRight)
				return string();

			stringstream s;
			s	<< "<input "
				<< "type=\"image\" "
				<< "src=\"" << imageURL << "\" "
				<< "name=\"" << name << "\" "
				<< "id=\"" << _getFieldId(name) << "\" "
				<< "alt=\"" << alt << "\" "
				<< " />";
			return s.str();
		}
	}
}
