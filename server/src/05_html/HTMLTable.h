////////////////////////////////////////////////////////////////////////////////
/// HTMLTable class header.
///	@file HTMLTable.h
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_HtmlTable_h__
#define SYNTHESE_HtmlTable_h__

#include <string>
#include <vector>

namespace synthese
{
	namespace html
	{
		/** HTML Table drawer class.

			@warning The order of the method calls is important to handle the closing tags. Do not use more than one call to a methode in a stream writing.

			@ingroup m05
		*/
		class HTMLTable
		{
		public:
			typedef std::vector<std::string> ColsVector;

		protected:
			std::string			_headers;	//<! First row composed by th columns. Will be displayed between tr tags
			std::string	_id;
			int					_cols;

			int					_getColsNumber() const;

			typedef enum
			{
				UNKNOWN_SECTION,
				HEAD,
				BODY,
				FOOT
			} Section;

			Section _section;

		private:
			int					_curCol;
			int					_curRow;
			const std::string	_className;
			bool				_lastColWasH;
			bool				_rowOpen;

			void _closeRow(std::ostream& stream);
			void _closeSection(std::ostream& stream);

		public:

			/** Constructor with columns number.
				@param cols Number of columns (default = unknown = 0)
				@param className Name of css class (default = undefined)
				@author Hugues Romain
				@date 2007
			*/
			HTMLTable(
				int cols=0,
				std::string className = std::string(),
				std::string id = std::string()
			);


			/** Constructor with vector of header contents.
				@param header Header vector. Each element of the vector represents an header.
				@author Hugues Romain
				@date 2007

				A colspan header is available : put the same column name twice (or more) in two following cells.
			*/
			HTMLTable(
				const ColsVector& header,
				std::string className = "",
				std::string id = std::string()
			);

			virtual ~HTMLTable();

			/** Opens the table.
				If defined, the headers are outputed.
			*/
			virtual std::string open();

			/** Closes the table.
			*/
			virtual std::string close();

			/** Adds a cell to the table.
				@param colSpan Number of columns the cell will use
				@param className CSS class name
				@param isHeader If true creates a th cell, else a td one
				@param style Style of the cell content (CSS code)
				@param rowSpan Number of rows the cell will use
				If the col number overloads the table col width, then a row is automatically defined. But this row can not have a CSS class name.
			*/
			std::string col(
				std::size_t colSpan = 1,
				std::string className = std::string(),
				bool isHeader = false,
				std::string style = std::string(),
				std::size_t rowSpan = 1
			);

			/** Adds a row to the table.
				@param className CSS class name
			*/
			virtual std::string row(std::string className=std::string(), std::string idName=std::string());

			/** Adds a cell at the specified column.
				If the current col is before the specified one, then additional cols are added, else a new row is created (without any CSS)
				@param colNumber Specified column position of the cell
				@param colSpan Number of columns the cell will use
				@param className CSS class name
			*/
			std::string goCol(int colNumber, int colSpan=1, std::string className="");

			void head(
				std::ostream& stream,
				std::string className = std::string()
			);
			void body(
				std::ostream& stream,
				std::string className = std::string()
			);
			void foot(
				std::ostream& stream,
				std::string className = std::string()
			);

			const std::string& getId() const { return _id; }
		};
	}
}

#endif // SYNTHESE_HtmlTable_h__
