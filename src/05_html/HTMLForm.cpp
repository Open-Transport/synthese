
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
#include "TinyMCE.hpp"

using namespace std;
using namespace boost;
using namespace boost::gregorian;
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;

	namespace html
	{

		std::string HTMLForm::open(
			std::string htmlComplement
		) const {
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



		std::string HTMLForm::getSelectNumberInput(
			const std::string& name
			, int mini
			, int maxi
			, int value
			, int step
			, std::string unknownValueText,
			string nulValueText
		){
			// Right control
			if (!_updateRight)
			{
				return
					(	value == 0 ?
						nulValueText :
						(	value == UNKNOWN_VALUE ?
							unknownValueText :
							lexical_cast<string>(value)
					)	)
				;
			}

			// Init
			std::vector<pair<optional<int>, string> > m;

			// Generation of the suite
			for(int i((step > 0) ? mini : maxi);
				(step > 0) ? (i <= maxi) : (i >= mini);
				i += step
			){
				m.push_back(
					make_pair(
						i,
						(	i==0 ?
							nulValueText :
							(	i == UNKNOWN_VALUE ?
								unknownValueText :
								lexical_cast<string>(i)
				)	)	)	);
			}

			// HTML Code
			return getSelectInput(name, m, optional<int>(value));
		}



		std::string HTMLForm::getTextInput(
			const std::string& name,
			const std::string& value,
			string displayTextBeforeTyping/*=""*/,
			string className
		){
			if (!_updateRight)
				return HTMLModule::HTMLEncode(value);

			removeHiddenFieldIfExists(name, value);

			return GetTextInput(name, value, displayTextBeforeTyping, _getFieldId(name), className);
		}



		std::string HTMLForm::GetTextInput(
			const std::string& name,
			const std::string& value,
			std::string displayTextBeforeTyping /*= std::string() */,
			std::string fieldId,
			string className
		){
			if (fieldId.empty())
				fieldId = name + "__ID";

			stringstream s;
			s	<< "<input "
				<< "type=\"text\" "
				<< "name=\"" << name << "\" "
				<< "value=\"";
			if (value.empty() && displayTextBeforeTyping != "")
			{
				s <<
					displayTextBeforeTyping << "\" " <<
					"onfocus=\"if(this.value == '" << displayTextBeforeTyping << "') this.value='';\" ";
			}
			else
			{
				s << HTMLModule::HTMLEncode(value) << "\" ";
			}
			s << "id=\"" << fieldId << "\" ";
			if(!className.empty())
			{
				s << "class=\"" << className << "\" ";
			}
			s << "/>";
			return s.str();
		}



		std::string HTMLForm::getTextInputAutoCompleteFromTableSync(
			const std::string& site,
			const std::string& name,
			const std::string& valueID,
			const std::string& valueName,
			const std::string& tableID,
			const std::string& extraParamName,
			const std::string& extraParamDivID,
			bool bottomButton,
			bool IDButton,
			bool useID,
			bool viewID,
			std::string displayTextBeforeTyping,
			std::string fieldId,
			string className
		){
			if (fieldId.empty())
				fieldId = name + "__ID";

			string fieldIdAutoComplete;

			if(useID)
				fieldIdAutoComplete = name + "Selection__ID";
			else
				fieldIdAutoComplete = fieldId;

			stringstream s;

			s	<< "<div id=\"div" << name << "Selection\">"
					<< "<input "
					<< "type=\"text\" "
					<< "name=\"" << (useID ? name + "Selection": name) << "\" "
					<< "value=\"" << valueName << "\" "
					<< "id=\"" << (useID ? name + "Selection__ID": fieldId) << "\" "
					<< "style=\"width:144px; margin-right:0px;\"/>";
			if(bottomButton)
			{
				s	<< "<input "
					<< "type=\"button\" "
					<< "value=\"v\" "
					<< "onclick=\"if($('.ui-autocomplete').is(':visible')) $('#" << fieldIdAutoComplete << "').autocomplete('close','');else {$('#" << fieldIdAutoComplete << "').autocomplete('search','');$('#" << fieldIdAutoComplete << "').focus();}\" "
					<< "style=\"margin-left:0px;margin-right:0px;\"/> ";
			}
			if(IDButton)
			{
				s	<< "<input "
					<< "type=\"button\" "
					<< "value=\"ID\" "
					<< "onclick=\"$('#div" << name << "Selection').hide();$('#div" << name << "').show();\" "
					<< "style=\"margin-left:0px;\"/>";
			}
			s	<< "</div>";
			if(useID)
			{
				s << "<div id=\"div" << name << "\" style=\"display:none\">"
					<< "<input type=\"text\" "
					<< "name=\"" << name << "\" "
					<< "value=\"" << valueID << "\" "
					<< "id=\"" << fieldId << "\" "
					<< "style=\"margin-right:0px;\" />"

					<< "<input type=\"button\" "
					<< "value=\"ID\" "
					<< "onclick=\"$('#div" << name << "Selection').show();$('#div" << name << "').hide();\" "
					<< "style=\"margin-left:0px;\" />"
				<< "</div>";
			}

			s << "<script type=\"text/javascript\">"
				<< "$(function() {"
					<< "$('#" << fieldIdAutoComplete << "').autocomplete({"
						<< "source: function( request, response ) {"
							<< "$.ajax({"
								<< "url: 'synthese',"
								<< "dataType: 'json',"
								<< "data: {"
									<< "SERVICE: 'lr',";
									if(!site.empty())
										s << "si: " << site << ",";
									if(!extraParamName.empty())
										s << extraParamName << ": " << "$('#" << extraParamDivID << "__ID').val(),";
									s << "output_format: 'json',"
									<< "table: " << tableID << ","
									<< "n: 10,"
									<< "t: request.term"
								<< "},"
								<< "success: function( data ) {"
									<< "response( $.map( data.rows.row, function( item ) {"
										<< "return {"
											<< "label: item.name" << (viewID ? " + ' (' + item.roid + ')'," : ",")
											<< "value: item.name,"
											<< "id: item.roid"
										<< "}"
									<< "}));"
								<< "}"
							<< "});"
						<< "},";
			if(useID)
			{
						s << "select: function( event, ui ) {"
							<< "$('#" << fieldId << "').val(ui.item.id);"
						<< "},";
			}
						s << "minLength: 0,"
						<< "autoFocus: true,"
						<< "open: function() {"
							<< "$(this).removeClass('ui-corner-all').addClass('ui-corner-top');"
						<< "},"
						<< "close: function() {"
							<< "$(this).removeClass('ui-corner-top').addClass('ui-corner-all');"
						<< "}"
					<< "});"
				<< "});"
			<< "</script>";

			return s.str();
		}



		std::string HTMLForm::getTextInputAutoCompleteFromService(
			const std::string& site,
			const std::string& name,
			const std::string& valueID,
			const std::string& valueName,
			const std::string& service,
			const std::string& rows,
			const std::string& row,
			const std::string& extraParamName,
			const std::string& extraParamDivID,
			bool bottomButton,
			bool IDButton,
			bool useID,
			bool viewID,
			std::string displayTextBeforeTyping,
			std::string fieldId,
			string className
		){
			if (fieldId.empty())
				fieldId = name + "__ID";

			string fieldIdAutoComplete;

			if(useID)
				fieldIdAutoComplete = name + "Selection__ID";
			else
				fieldIdAutoComplete = fieldId;

			stringstream s;

			s	<< "<div id=\"div" << name << "Selection\">"
					<< "<input "
					<< "type=\"text\" "
					<< "name=\"" << (useID ? name + "Selection": name) << "\" "
					<< "value=\"" << valueName << "\" "
					<< "id=\"" << (useID ? name + "Selection__ID": fieldId) << "\" "
					<< "style=\"width:144px; margin-right:0px;\"/>";
			if(bottomButton)
			{
				s	<< "<input "
					<< "type=\"button\" "
					<< "value=\"v\" "
					<< "onclick=\"if($('.ui-autocomplete').is(':visible')) $('#" << fieldIdAutoComplete << "').autocomplete('close','');else {$('#" << fieldIdAutoComplete << "').autocomplete('search','');$('#" << fieldIdAutoComplete << "').focus();}\" "
					<< "style=\"margin-left:0px;margin-right:0px;\"/> ";
			}
			if(IDButton)
			{
				s	<< "<input "
					<< "type=\"button\" "
					<< "value=\"ID\" "
					<< "onclick=\"$('#div" << name << "Selection').hide();$('#div" << name << "').show();\" "
					<< "style=\"margin-left:0px;\"/>";
			}
			s	<< "</div>";
			if(useID)
			{
				s << "<div id=\"div" << name << "\" style=\"display:none\">"
					<< "<input type=\"text\" "
					<< "name=\"" << name << "\" "
					<< "value=\"" << valueID << "\" "
					<< "id=\"" << fieldId << "\" "
					<< "style=\"margin-right:0px;\" />"

					<< "<input type=\"button\" "
					<< "value=\"ID\" "
					<< "onclick=\"$('#div" << name << "Selection').show();$('#div" << name << "').hide();\" "
					<< "style=\"margin-left:0px;\" />"
				<< "</div>";
			}

			s << "<script type=\"text/javascript\">"
				<< "$(function() {"
					<< "$('#" << fieldIdAutoComplete << "').autocomplete({"
						<< "source: function( request, response ) {"
							<< "$.ajax({"
								<< "url: 'synthese',"
								<< "dataType: 'json',"
								<< "data: {"
									<< "SERVICE: '" << service << "',";
									if(!site.empty())
										s << "si: " << site << ",";
									if(!extraParamName.empty())
										s << extraParamName << ": " << "$('#" << extraParamDivID << "__ID').val(),";
									s << "output_format: 'json',"
									<< "n: 10,"
									<< "t: request.term"
								<< "},"
								<< "success: function( data ) {"
									<< "response( $.map( data." << rows << "." << row << ", function( item ) {"
										<< "return {"
											<< "label: item.name" << (viewID ? " + ' (' + item.roid + ')'," : ",")
											<< "value: item.name,"
											<< "id: item.roid"
										<< "}"
									<< "}));"
								<< "}"
							<< "});"
						<< "},";
			if(useID)
			{
						s << "select: function( event, ui ) {"
							<< "$('#" << fieldId << "').val(ui.item.id);"
						<< "},";
			}
						s << "minLength: 0,"
						<< "autoFocus: true,"
						<< "open: function() {"
							<< "$(this).removeClass('ui-corner-all').addClass('ui-corner-top');"
						<< "},"
						<< "close: function() {"
							<< "$(this).removeClass('ui-corner-top').addClass('ui-corner-all');"
						<< "}"
					<< "});"
				<< "});"
			<< "</script>";

			return s.str();
		}



		std::string HTMLForm::getSubmitButton( const std::string& caption )
		{
			if (!_updateRight)
				return string();

			stringstream s;
			s	<< "<input "
				<< "type=\"submit\" "
				<< "value=\"" << HTMLModule::HTMLEncode(caption) << "\" "
				<< "/>";
			return s.str();
		}



		std::string HTMLForm::setFocus(const std::string& fieldName ) const
		{
			if (!_updateRight)
				return string();

			stringstream s;
			s	<< HTMLModule::GetHTMLJavascriptOpen()
				<< "document.forms." << _name << ".elements." << fieldName << ".focus();"
				<< HTMLModule::GetHTMLJavascriptClose();
			return s.str();
		}



		std::string HTMLForm::setFocus( const std::string& fieldName, int fieldRank ) const
		{
			if (!_updateRight)
				return string();

			stringstream s;
			s	<< HTMLModule::GetHTMLJavascriptOpen()
				<< "document.forms." << _name << ".elements." << fieldName << "[" << fieldRank << "].focus();"
				<< HTMLModule::GetHTMLJavascriptClose();
			return s.str();
		}



		std::string HTMLForm::getTextAreaInput(
			const std::string& name,
			const std::string& value,
			size_t rows,
			size_t cols,
			bool tinyMCE
		){
			if (!_updateRight)
			{
				return HTMLModule::HTMLEncode(value);
			}

			removeHiddenFieldIfExists(name, value);

			return GetTextAreaInput(name, value, rows, cols, tinyMCE, _getFieldId(name));
		}



		std::string HTMLForm::GetTextAreaInput(
			const std::string& name,
			const std::string& value,
			std::size_t rows,
			std::size_t cols,
			bool tinyMCE,
			string id
		){
			stringstream s;
			s	<< "<textarea "
				<< "name=\"" << name << "\" "
				<< "rows=\"" << rows << "\" "
				<< "cols=\"" << cols << "\" ";
			if(!id.empty())
			{
				s << "id=\"" << id << "\" ";
			}
			if(tinyMCE)
			{
				s << "class=\"" << TinyMCE::MCE_EDITOR_ACTIVATION_CLASS << "\" ";
			}
			s <<
				">" <<
				HTMLModule::HTMLEncode(value) << "</textarea>"
			;
			return s.str();
		}



		std::string HTMLForm::getOuiNonRadioInput( const std::string& name, bool value )
		{
			vector<pair<optional<int>, string> > m;
			m.push_back(make_pair(0, "NON"));
			m.push_back(make_pair(1, "OUI"));
			return getRadioInputCollection(name, m, optional<int>(value ? 1 : 0));
		}

		std::string HTMLForm::getCheckBox( const std::string& name, const std::string& value, bool checked )
		{
			if (!_updateRight)
				return checked ? "OUI" : "NON";

			removeHiddenFieldIfExists(name, value);
			stringstream s;
			s	<< "<input "
				<< "type=\"checkbox\" "
				<< "value=\"1\" "
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



		std::string HTMLForm::getCalendarInput(
			const std::string& name,
			const ptime& value
		){
			if (!_updateRight)
			{
				if(value.is_not_a_date_time())
				{
					return "(indéfini)";
				}
				return to_simple_string(value.date()) +" "+ to_simple_string(value.time_of_day());
			}

			removeHiddenFieldIfExists(name, to_simple_string(value.date()) +" "+ to_simple_string(value.time_of_day()));
			string fieldId = _getFieldId(name);
			string spanId = _getFieldId(name + "SPAN");
			string triggerId = _getFieldId(name + "TRIGGER");

			return GetCalendarInput(name, value, fieldId, triggerId, spanId);
		}

		std::string HTMLForm::GetCalendarInput(
			const std::string& name
			, const ptime& value
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
				<< "type=\"text\" "
				<< "name=\"" << name << "\" "
				<< "id=\"" << fieldId << "\" "
				<< "value=\"" << (value.is_not_a_date_time() ? string() : to_iso_extended_string(value.date()) +" "+ to_simple_string(value.time_of_day())) << "\" "
				<< "/>"
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
				<< "button:\"" << triggerId << "\","
				<< "showsTime : true,"
				<< "ifFormat :\"%Y-%m-%d %H:%M\","
				<< "daFormat :\"%e/%m/%Y %H:%M\","
				<< "electric : false,"
				<< "singleClick:true,";
			if (!value.is_not_a_date_time())
				s << "date:new Date(" << value.date().year() <<","<< static_cast<int>(value.date().month()) <<","<< value.date().day() <<","<< value.time_of_day().hours() <<"," << value.time_of_day().minutes() <<",0),";
			s		<< "firstDay:1"
				<< "});"
				<< HTMLModule::GetHTMLJavascriptClose();

			return s.str();

		}

		std::string HTMLForm::getCalendarInput(
			const std::string& name,
			const date& value
		){
			if (!_updateRight)
				return to_simple_string(value);

			removeHiddenFieldIfExists(name, to_simple_string(value));

			return GetCalendarInput(
				name
				, value
				, _getFieldId(name)
				, _getFieldId(name + "TRIGGER")
				, _getFieldId(name + "SPAN")
			);
		}

		std::string HTMLForm::GetCalendarInput(
			const std::string& name,
			const date& value
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
				<< "type=\"text\" "
				<< "name=\"" << name << "\" "
				<< "id=\"" << fieldId << "\" "
				<< "value=\"" << (value.is_not_a_date() ? string() : to_iso_extended_string(value)) << "\" "
				<< "/>"
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
				<< "button:\"" << triggerId << "\","
				<< "showsTime : false,"
				<< "ifFormat :\"%Y-%m-%d\","
				<< "daFormat :\"%e/%m/%Y\","
				<< "electric : false,"
				<< "singleClick:true,";
			if (!value.is_not_a_date())
			{
				s << "date:new Date(" << value.year() <<","<< static_cast<int>(value.month()) <<","<< value.day() << "),";
			}
			s	<< "firstDay:1"
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
					counter = ++it->second;
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

		std::string HTMLForm::getHiddenFields()
		{
			if (!_updateRight)
				return string();

			stringstream s;
			for (HiddenFieldsMap::const_iterator it = _hiddenFields.begin(); it != _hiddenFields.end(); ++it)
			{
				s <<
					"<input type=\"hidden\" name=\"" << it->first << "\" " <<
					"value=\"" << HTMLModule::HTMLEncode(it->second) << "\" " <<
					"id=\"" << _getFieldId(it->first) << "\" " <<
				"/>";
			}
			_hiddenFields.clear();
			return s.str();
		}

		void HTMLForm::removeHiddenFieldIfExists( const std::string& name, const std::string& value )
		{
			HiddenFieldsMap::iterator it = _hiddenFields.find(name);
			if (it != _hiddenFields.end())
				_hiddenFields.erase(it);
			_initialFields[name] = value;
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
				<< "alt=\"" << HTMLModule::HTMLEncode(alt) << "\" "
				<< " />";
			return s.str();
		}

		std::string HTMLForm::getSubmitOnPopupLink( const std::string& content, int width, int height )
		{
			stringstream s;
			s	<< HTMLModule::GetHTMLJavascriptOpen()
				<< "function pop_" << _name << "() {"
				<< "my_form = eval('document.forms." << _name << "');"
				<< "window.open(\"" << _action << "?\",\"" << _name << "popup\",\"height=" << height << ",width=" << width << ",menubar='no',toolbar='no',location='no',status='no',scrollbars='no',resizable=1\");"
				<< "my_form.target = \"" << _name << "popup\";"
				<< "my_form.submit();}"
				<< HTMLModule::GetHTMLJavascriptClose()
				<< "<a href=\"#\" onclick=\"pop_" << _name << "();\">"
				<< content
				<< "</a>";
			return s.str();
		}

		std::string HTMLForm::getFieldId( const std::string& name )
		{
			return _getFieldId(name, false);
		}

		const std::string& HTMLForm::getName() const
		{
			return _name;
		}



		bool HTMLForm::getUpdateRight() const
		{
			return _updateRight;
		}
}	}
