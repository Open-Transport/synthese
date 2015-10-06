
/** Export class header.
	@file Export.hpp

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

#ifndef SYNTHESE_server_Export_hpp__
#define SYNTHESE_server_Export_hpp__

#include "Object.hpp"

#include "CoordinatesSystem.hpp"
#include "CMSScriptField.hpp"
#include "EnumObjectField.hpp"
#include "Exporter.hpp"
#include "FileFormat.h"
#include "ParametersMapField.hpp"
#include "PointerField.hpp"
#include "Registrable.h"
#include "Registry.h"
#include "SchemaMacros.hpp"

namespace synthese
{
	namespace util
	{
		class Env;
	}

	namespace impex
	{
		class Exporter;

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Name),
			FIELD(FileFormatKey),
			FIELD(Parameters),
			FIELD(Active),
			FIELD(Documentation)
		> ExportRecord;



		//////////////////////////////////////////////////////////////////////////
		/// Export class.
		///	@ingroup m16
		/// @author Camille Hue
		/// @since 3.9.0
		class Export:
			public Object<Export, ExportRecord>
		{
		public:
			/// Chosen registry class.
			typedef util::Registry<Export>	Registry;

		private:
			mutable boost::shared_ptr<Exporter> _autoExporter;
		
			boost::shared_ptr<Exporter> _getAutoExporter() const;

		public:
			Export(util::RegistryKeyType id = 0);

			//! @name Services
			//@{
				bool canExport() const;
				bool isPermanentThread() const;
				boost::shared_ptr<Exporter> getExporter() const;
				virtual void addAdditionalParameters(util::ParametersMap& map, std::string prefix) const;
			//@}

			//! @name Modifiers
			//@{
				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
				virtual void unlink();
			//@}

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;
		};
}	}

#endif // SYNTHESE_server_Export_hpp__
