////////////////////////////////////////////////////////////////////////////////
///	DataSource class header.
///	@file DataSource.h
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

#ifndef SYNTHESE_DataSource_h__
#define SYNTHESE_DataSource_h__

#include "Registrable.h"
#include "Registry.h"

namespace synthese
{
	namespace impex
	{
		////////////////////////////////////////////////////////////////////////
		/// DataSource class.
		/// @ingroup m16
		class DataSource
		:	public virtual util::Registrable
		{
		public:
			// Typedefs
			
			/// Chosen registry class.
			typedef util::Registry<DataSource> Registry;

			
		private:
			// Attributes
			std::string	_name;
			std::string _format;
			
		public:
			/////////////////////////////////////////////////////////////////////
			/// DataSource Constructor.
			DataSource(
				util::RegistryKeyType id = UNKNOWN_VALUE
			);
			
			//! @name Getters
			//@{
				const std::string& getName() const;
				const std::string& getFormat() const;
			//@}
		
			//! @name Setters
			//@{
				void setName(const std::string& value);
				void setFormat(const std::string& value);
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

#endif // SYNTHESE_DataSource_h__
