
/** AjaxVectorFieldEditor class header.
	@file AjaxVectorFieldEditor.hpp

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

#ifndef SYNTHESE_impex_AjaxVectorFieldEditor_hpp__
#define SYNTHESE_impex_AjaxVectorFieldEditor_hpp__

#include "AjaxForm.hpp"

namespace synthese
{
	namespace html
	{
		/** AjaxVectorFieldEditor class.
			@ingroup m16
		*/
		class AjaxVectorFieldEditor:
			public AjaxForm
		{
		public:
			static const std::string CSS_AUTO_ORDER;

		public:
			class Field
			{
				std::string _title;

			public:
				Field(const std::string& title);
				virtual ~Field() {}

				//! @name Getters
				//@{
					const std::string& getTitle() const { return _title; }
				//@}

				virtual void getField(std::ostream& stream) const = 0;
				virtual void getInit(
					std::ostream& stream,
					const std::string& formName,
					std::size_t fieldRank
				) const = 0;

				virtual void outputValue(std::ostream& stream, const std::vector<std::string>& value) const = 0;
			};



			class SelectField:
				public Field
			{
			public:
				static const std::string CSS_UNIQUE;

				typedef std::vector<std::pair<util::RegistryKeyType, std::string> > Choices;

			private:
				bool _unique;
				const Choices& _choices;

			public:
				SelectField(
					const std::string& title,
					bool unique,
					const Choices& choices
				);
				virtual ~SelectField() {}

				virtual void getField(std::ostream& stream) const;

				virtual void getInit(
					std::ostream& stream,
					const std::string& formName,
					std::size_t fieldRank
				) const;

				virtual void outputValue(std::ostream& stream, const std::vector<std::string>& value) const;
			};


			class TextInputField:
				public Field
			{
			private:

			public:
				TextInputField(
					const std::string& title
				);
				virtual ~TextInputField() {}
				virtual void getField(std::ostream& stream) const;

				virtual void getInit(
					std::ostream& stream,
					const std::string& formName,
					std::size_t fieldRank
				) const;

				virtual void outputValue(std::ostream& stream, const std::vector<std::string>& value) const;
			};


			class TextAutoCompleteInputField:
				public Field
			{
			private:
				std::string _nameField;
				std::string _valueID;
				std::string _valueName;
				std::string _service;
				std::string _serviceRows;
				std::string _serviceRow;
				std::string _extraParamName;
				std::string _extraParamDivID;
				bool _bottomButton;
				bool _IDButton;
				bool _useID;
				bool _viewID;


			public:
				TextAutoCompleteInputField(
					const std::string& title,
					const std::string& nameField,
					const std::string& valueID,
					const std::string& valueName,
					const std::string& service,
					const std::string& serviceRows,
					const std::string& serviceRow,
					const std::string& extraParamName,
					const std::string& extraParamDivID,
					const bool bottomButton,
					const bool IDButton,
					const bool useID,
					const bool viewID
				);

				virtual void getField(std::ostream& stream) const;

				virtual void getInit(
					std::ostream& stream,
					const std::string& formName,
					std::size_t fieldRank
				) const;

				virtual void outputValue(std::ostream& stream, const std::vector<std::string>& value) const;
			};

			typedef std::vector<boost::shared_ptr<Field> > Fields;
			typedef std::vector<std::vector<std::string> > Row;
			typedef std::vector<Row> Rows;

		private:
			std::string _parameterName;
			Fields _fields;
			Rows _rows;
			const bool _autoOrderTable;

			std::string _getTableId() const;

		public:
			////////////////////////////////////////////////////////////////////
			/// Ajax Form constructor.
			///	@param name Name of the parameter in the action request
			///	@param action Action to run at the form submit
			///	@author Hugues Romain
			///	@date 2007
			AjaxVectorFieldEditor(
				const std::string& parameterName,
				const std::string& action,
				const Fields& fields,
				bool autoOrderTable
			);


			void display(
				std::ostream& stream,
				std::string htmlComplement = std::string()
			);



			void addRow(
				const Row& values
			);
		};
	}
}

#endif // SYNTHESE_impex_AjaxVectorFieldEditor_hpp__
