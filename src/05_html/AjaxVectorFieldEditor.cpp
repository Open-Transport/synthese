
/** AjaxVectorFieldEditor class implementation.
	@file AjaxVectorFieldEditor.cpp

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

#include "AjaxVectorFieldEditor.hpp"
#include "PropertiesHTMLTable.h"

#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost;
using namespace boost::algorithm;

namespace synthese
{
	using namespace util;

	namespace html
	{
		const string AjaxVectorFieldEditor::CSS_AUTO_ORDER("autoorder");
		const string AjaxVectorFieldEditor::SelectField::CSS_UNIQUE("unique");



		AjaxVectorFieldEditor::AjaxVectorFieldEditor(
			const std::string& parameterName,
			const std::string& action,
			const Fields& fields,
			bool autoOrderTable
		):	AjaxForm(parameterName + "_AJAX_EDITOR_FORM", action),
			_parameterName(parameterName),
			_fields(fields),
			_autoOrderTable(autoOrderTable)
		{}



		void AjaxVectorFieldEditor::display(
			std::ostream& stream,
			std::string htmlComplement /*= std::string() */
		){
			stream << openWithJSComplement(
				"document.forms['" + _name + "'].elements['"+ _parameterName + "'].value = send_vector_field('"+ _getTableId() +"');",
				htmlComplement
			);

			addHiddenField(_parameterName, string());

			HTMLTable::ColsVector c;
			BOOST_FOREACH(const Fields::value_type& field, _fields)
			{
				c.push_back(field->getTitle());
			}
			c.push_back(string());

			HTMLTable t(c, PropertiesHTMLTable::CSS_CLASS, _getTableId());
			stream << t.open();
			t.body(stream, _autoOrderTable ? CSS_AUTO_ORDER : string());
			t.foot(stream);
			stream << t.row();

			BOOST_FOREACH(const Fields::value_type& field, _fields)
			{
				stream << t.col();
				field->getField(stream);
			}

			stream << t.col();
			stream << HTMLModule::getHTMLLink(
				"add_new_row(this);",
				HTMLModule::getHTMLImage("/admin/img/add.png", "+", string()),
				string(),
				true,
				"Ajouter l'élément"
			);

			stream << t.row();
			stream << t.col(_fields.size() + 1, string(), true);
			stream << getSubmitButton(
				"Enregistrer les modifications",
				string(),
				"/admin/img/disk.png"
			);
			stream << t.close();

			stream << AjaxForm::close();

			stream << HTMLModule::GetHTMLJavascriptOpen();

			// Initialization of fields
			size_t fieldRank(0);
			BOOST_FOREACH(const Fields::value_type& field, _fields)
			{
				field->getInit(stream, _getTableId(), fieldRank++);
			}

			// Insertion of values
			BOOST_FOREACH(const Rows::value_type& row, _rows)
			{
				stream << "add_row('" << _getTableId() << "', [";

				Fields::const_iterator it(_fields.begin());
				BOOST_FOREACH(const Row::value_type& value, row)
				{
					if(it != _fields.begin())
					{
						stream << ",";
					}
					(*it)->outputValue(stream, value);
					++it;
				}

				stream << "]);";
			}

			stream << HTMLModule::GetHTMLJavascriptClose();
		}



		std::string AjaxVectorFieldEditor::_getTableId() const
		{
			return _name +"_TABLE_ID";
		}



		void AjaxVectorFieldEditor::addRow(
			const Row& row
		){
			assert(row.size() == _fields.size());

			_rows.push_back(row);
		}


		AjaxVectorFieldEditor::Field::Field(
			const std::string& title
		):	_title(title)
		{}



		AjaxVectorFieldEditor::SelectField::SelectField(
			const std::string& title,
			bool unique,
			const Choices& choices
		):	AjaxVectorFieldEditor::Field(title),
			_unique(unique),
			_choices(choices)
		{}



		void AjaxVectorFieldEditor::SelectField::getField(ostream& stream) const
		{
			stream << "<select";
			if(_unique)
			{
				stream << " class=\"" << CSS_UNIQUE << "\"";
			}
			stream << "></select>";
		}



		void AjaxVectorFieldEditor::SelectField::getInit(
			std::ostream& stream,
			const std::string& formName,
			std::size_t fieldRank
		) const	{
			stream << "var select = get_add_row('" << formName << "').cells[" << fieldRank << "].childNodes[0];";
			BOOST_FOREACH(const Choices::value_type& choice, _choices)
			{
				stream << "insert_select(select,\"" << choice.first << "\", \"" << choice.second << "\");";
			}
		}



		void AjaxVectorFieldEditor::SelectField::outputValue( std::ostream& stream, const std::vector<std::string>& value ) const
		{
			stream << "\"";
			if(!value.empty() && !value.front().empty())
			{
				stream << lexical_cast<RegistryKeyType>(value.front());
			}
			stream << "\"";
		}


		void AjaxVectorFieldEditor::TextInputField::getField( std::ostream& stream ) const
		{
			stream << "<input type=\"text\" value=\"\" onkeypress=\"key_press(event);\" />";
		}



		AjaxVectorFieldEditor::TextInputField::TextInputField(
			const std::string& title
		):	AjaxVectorFieldEditor::Field(title)
		{}



		void AjaxVectorFieldEditor::TextInputField::getInit( std::ostream& stream, const std::string& formName, std::size_t fieldRank ) const
		{
		}



		void AjaxVectorFieldEditor::TextInputField::outputValue( std::ostream& stream, const std::vector<std::string>& value ) const
		{
			if(!value.empty())
				stream << "\"" << replace_all_copy(replace_all_copy(value.front(),"\"","'"),"\\","\\\\") << "\"";
		}



		void AjaxVectorFieldEditor::TextAutoCompleteInputField::getField( std::ostream& stream ) const
		{
			stream <<
				GetTextInputAutoCompleteFromService(
				_nameField+"Field",
				_valueID,
				_valueName,
				_service,
				_serviceRows,
				_serviceRow,
				_extraParamName,
				_extraParamDivID,
				_bottomButton,
				_IDButton,
				_useID,
				_viewID
			);
		}



		AjaxVectorFieldEditor::TextAutoCompleteInputField::TextAutoCompleteInputField(
			const std::string& title,
			const std::string& nameField,
			const std::string& valueID,
			const std::string& valueName,
			const std::string& service,
			const std::string& serviceRows,
			const std::string& serviceRow,
			const std::string& extraParamName,
			const std::string& extraParamDivID,
			bool bottomButton,
			bool IDButton,
			bool useID,
			bool viewID
		):	AjaxVectorFieldEditor::Field(title),
		_nameField(nameField),
		_valueID(valueID),
		_valueName(valueName),
		_service(service),
		_serviceRows(serviceRows),
		_serviceRow(serviceRow),
		_extraParamName(extraParamName),
		_extraParamDivID(extraParamDivID),
		_bottomButton(bottomButton),
		_IDButton(IDButton),
		_useID(useID),
		_viewID(viewID)
		{}



		void AjaxVectorFieldEditor::TextAutoCompleteInputField::getInit( std::ostream& stream, const std::string& formName, std::size_t fieldRank ) const
		{
		}



		void AjaxVectorFieldEditor::TextAutoCompleteInputField::outputValue( std::ostream& stream, const std::vector<std::string>& value ) const
		{
			if(value.size() > 1)
				stream << "[\"" << replace_all_copy(value[0],"\"","'") << "\", \"" << replace_all_copy(value[1],"\"","'") << "\"]";
		}
}	}
