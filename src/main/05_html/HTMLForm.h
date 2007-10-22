
/** HTMLForm class header.
	@file HTMLForm.h

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

#ifndef SYNTHESE_HTMLForm_h__
#define SYNTHESE_HTMLForm_h__

#include <map>
#include <vector>
#include <utility>
#include <string>
#include <sstream>

#include "01_util/Constants.h"

#include "05_html/HTMLModule.h"

namespace synthese
{
	namespace time
	{
		class DateTime;
		class Date;
	}

	namespace html
	{
		/** HTML form generator class.
			@ingroup m05
		*/
		class HTMLForm
		{
		public:
			typedef std::map<std::string, std::string> HiddenFieldsMap;

		protected:
			typedef std::map<std::string, int> IDCounterMap;
			
			const std::string	_name;
			const std::string	_action;
			IDCounterMap		_idCounter;
			HiddenFieldsMap		_hiddenFields;
			HiddenFieldsMap		_initialFields;
			bool				_updateRight;


			/** Field unique ID generator.
				@param name Name of the field
				@param increment Indicates if the call must be counted in order to increment the id generated by the next call with the same name value
				@return std::string The generated unique ID
				@author Hugues Romain
				@date 2007
				
				The generated id is the concatenation of :
					- the form name,
					- the field name,
					- the keyword AUTOGENERATEDFORMID
					- an auto incremental ID if the field is already present and if the call is registered
				The keywords are separated by a _ characters.
			*/
			std::string _getFieldId(const std::string& name, bool increment=true);


			
		public:

			/** Removes an hidden field if exists, to avoid to an interactive field to be in conflict with an hidden one.
				@param name Name of the hidden field to remove
				@param value Initial value of the field
				@author Hugues Romain
				@date 2007
			*/
			void removeHiddenFieldIfExists(const std::string& name, const std::string& value);



			/** Update right setter.
				@param value Value to set
				@author Hugues Romain
				@date 2007				
			*/
			void setUpdateRight(bool value);


			/** Constructor.
				@param name Name of the form in the HTML document
				@param action Action to run at the form submit
				@author Hugues Romain
				@date 2007				
			*/
			HTMLForm(const std::string& name, const std::string& action);

			/** Generator of the opening HTML tag.
				@return std::string The HTML generated code.
				@author Hugues Romain
				@date 2007
				
			*/
			std::string open(const std::string htmlComplement = std::string());

			/** Generator of the closing HTML tag.
				@return std::string The HTML generated code.
				@author Hugues Romain
				@date 2007
				
			*/
			std::string close();

			/** Generator of a script that gives the focus to a specific field at the page load.
				@param fieldName Name of the field to focus
				@return std::string The generated HTML code
				@author Hugues Romain
				@date 2007
				
			*/
			std::string setFocus(const std::string& fieldName);


			std::string getHiddenFields() const;

			/** HTML Link button generator.
				@return std::string The generated HTML Code
				@author Hugues Romain
				@date 2007
				@note To override some field values, use HTMLModule::getLinkButon(getURL(HiddenFieldsMap), ...) instead.
			*/
			std::string getLinkButton(const std::string& caption, const std::string confirm="", const std::string icon="") const;

			/** URL Generator.
				@param overridingFields Fields value to output instead of the registered ones.
				@return std::string The URL launching the form (GET action)
				@author Hugues Romain
				@date 2007				
			*/
			std::string getURL(HiddenFieldsMap overridingFields = HiddenFieldsMap(), bool withInitialValues=true) const;

			/** External sorted selection list HTML input field (template)
				@param name Name of the field
				@param choices List of choices : a map sorted by elements of S (the sort value is not read), giving pairs "value => text"
				@param value Default selected value
				@return Selection list HTML input field
				@author Hugues Romain
				@date 2007					
			*/
			template<class K, class T>
			std::string getSelectInput(const std::string& name, const std::vector<std::pair<K, T> >& choices, const K& value);

			template<class K>
			std::string getRadioInput(const std::string& name, const K& valueIfSelected, const K& valueToSelect, const std::string label="");

			template<class K>
			std::string getRadioInput(const std::string& name, const std::vector<std::pair<K, std::string> >& choices, const K& value);

			/** Simple text HTML input generator.
				@param name Name of the input in the form
				@param value Default value present in the field at the page load
				@param displayTextBeforeTyping Text present in the field at the page load, that disapears when the field obtain the focus
				@return std::string The generated HTML code
				@author Hugues Romain
				@date 2007
			*/
			std::string getTextInput(const std::string& name, const std::string& value, std::string displayTextBeforeTyping="");

			static std::string GetTextInput(
				const std::string& name
				, const std::string& value
				, std::string displayTextBeforeTyping = std::string()
				, std::string fieldId = std::string()
			);


			std::string getPasswordInput(const std::string& name, const std::string& value);

			/** Interactive calendar input generator.
				@param name Name of the field
				@param value Default value of the calendar
				@return std::string The generated HTML code
				@author Hugues Romain
				@date 2007

				In order to use the calendar field, you must include the following lines int the header of the page :
				@code
<style type="text/css">@import url(calendar-win2k-1.css);</style>
<script type="text/javascript" src="calendar.js"></script>
<script type="text/javascript" src="lang/calendar-en.js"></script>
<script type="text/javascript" src="calendar-setup.js"></script>
				@endcode
			*/
			std::string getCalendarInput(const std::string& name, const time::DateTime& value);

			static std::string GetCalendarInput(
				const std::string& name
				, const time::DateTime& value
				, std::string fieldId = std::string()
				, std::string triggerId = std::string()
				, std::string spanId = std::string()
			);

			/** Interactive calendar input generator.
				@param name Name of the field
				@param value Default value of the calendar
				@return std::string The generated HTML code
				@author Hugues Romain
				@date 2007

				In order to use the calendar field, you must include the following lines int the header of the page :
				@code
<style type="text/css">@import url(calendar-win2k-1.css);</style>
<script type="text/javascript" src="calendar.js"></script>
<script type="text/javascript" src="lang/calendar-en.js"></script>
<script type="text/javascript" src="calendar-setup.js"></script>
				@endcode
			*/
			std::string getCalendarInput(const std::string& name, const time::Date& value);

			static std::string GetCalendarInput(
				const std::string& name
				, const time::Date& value
				, std::string fieldId = std::string()
				, std::string triggerId = std::string()
				, std::string spanId = std::string()
			);

			/** Register of an hidden field.
				@param name Name of the field
				@param value Value of the field
				@author Hugues Romain
				@date 2007
				
				The corresponding code is generated at the form closing.
				If the field already exists, its value is replaced by the last one.
			*/
			void addHiddenField(const std::string& name, const std::string& value);

			std::string getTextAreaInput(const std::string& name, const std::string& value, int rows, int cols);

			std::string getSubmitButton(const std::string& caption);

			std::string getCheckBox(const std::string& name, const std::string& value, bool checked);

			std::string getSelectNumberInput(const std::string& name, int mini, int maxi, int value=UNKNOWN_VALUE, int step=1);

			/** Oui Non radio input.
					@param name Name of the field
					@param value Value of the field
					@return HTML input command
					@todo integrate language parameter to internationalize the labels
			*/
			std::string getOuiNonRadioInput(const std::string& name, bool value);
		};


		template<class K, class T>
		std::string HTMLForm::getSelectInput( const std::string& name, const std::vector<std::pair<K, T> >& choices, const K& value )
		{
			std::stringstream s;

			if (!_updateRight)
			{
				for (typename std::vector<std::pair<K, T> >::const_iterator it = choices.begin(); it != choices.end(); ++it)
					if (it->first == value)
						s << it->second;
			}
			else
			{
				s << "<select name=\"" << name << "\">";
				for (typename std::vector<std::pair<K, T> >::const_iterator it = choices.begin(); it != choices.end(); ++it)
				{
					s << "<option value=\"" << it->first << "\"";
					if (it->first == value)
						s << " selected=\"1\"";
					s << ">" << it->second << "</option>";
				}
				s << "</select>";
			}
			return s.str();
		}

		template<class K>
		std::string HTMLForm::getRadioInput(const std::string& name, const K& valueIfSelected, const K& valueToSelect, const std::string label)
		{
			if (!_updateRight)
			{
				return (valueIfSelected == valueToSelect)
					? label
					: std::string();
			}

			std::stringstream s;
			s << "<input name=\"" << name << "\" type=\"radio\" value=\"" << valueIfSelected << "\"";
			if (valueIfSelected == valueToSelect)
				s << " checked=\"1\"";
			s << " />" << label;
			return s.str();
		}

		template<class K>
		std::string HTMLForm::getRadioInput(const std::string& name, const std::vector<std::pair<K, std::string> >& choices, const K& value)
		{
			std::stringstream s;
			for (typename std::vector<std::pair<K, std::string> >::const_iterator it = choices.begin(); it != choices.end(); ++it)
			{
				s << getRadioInput(name, it->first, value, it->second);
			}
			return s.str();
		}

	}
}

#endif // SYNTHESE_HTMLForm_h__
