////////////////////////////////////////////////////////////////////////////////
///	Importable class header.
///	@file Importable.h
///	@author Hugues Romain (RCS)
///	@date sam fév 21 2009
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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

#ifndef SYNTHESE_Importable_h__
#define SYNTHESE_Importable_h__

#include <string>

namespace synthese
{
	namespace impex
	{
		class DataSource;

		////////////////////////////////////////////////////////////////////////
		/// Importable class.
		/// @ingroup m16
		class Importable
		{
		public:
			
		private:
			// Attributes
			std::string			_codeBySource;
			const DataSource*	_dataSource;

		public:
			/////////////////////////////////////////////////////////////////////
			/// DataSource Constructor.
			Importable(
				const std::string codeBySource = std::string(),
				const DataSource* const source = NULL
			);
			
			//! @name Getters
			//@{
				const std::string& getCodeBySource() const;
				const DataSource* getDataSource() const;
			//@}
		
			//! @name Setters
			//@{
				void setCodeBySource(const std::string& value);
				void setDataSource(const DataSource* value);
			//@}

			//! @name Modifiers
			//@{
			//@}

			//! @name Queries
			//@{
			//@}
		
			//! @name Static algorithms
			//@{
			//@}
		};
	}
}

#endif // SYNTHESE_Importable_h__
