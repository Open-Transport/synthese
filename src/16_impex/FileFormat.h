/** Import class header.
	@file Import.h

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

#ifndef SYNTHESE_impex_Import_h__
#define SYNTHESE_impex_Import_h__

#include "FactoryBase.h"
#include "Env.h"
#include <set>

namespace synthese
{
	namespace impex
	{
		class DataSource;
		
		/** Import factorable base class.
			
			Each import subclass defines a format which can be imported.
			
			@ingroup m16
		*/
		class FileFormat : public util::FactoryBase<FileFormat>
		{
		protected:
			FileFormat(
				util::Env* env = NULL
			);
			
			util::Env*				_env;
			const DataSource*		_dataSource;
			
			
			virtual void _parse(
				const std::string& text,
				std::ostream& os
			) = 0;
			
			
		public:
			/** Generic export method.
				The generic export consists in the export of each registered exportable class
				@author Hugues Romain
				@date 2008
			*/
			virtual void build(
				std::ostream& os
			) = 0;
			
			void parseFiles(
				const std::set<std::string>& paths,
				std::ostream& os
			);
			
			virtual void save(
				std::ostream& os
			) const = 0;
			
			void setDataSource(const DataSource* value);
			void setEnv(util::Env* value);
		};
	}
}

#endif // SYNTHESE_impex_Import_h__
