
/** Importer class implementation.
	@file Importer.cpp

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

#include "Importer.hpp"

#include "DBTransaction.hpp"
#include "Env.h"

namespace synthese
{
	using namespace db;

	namespace impex
	{
		//////////////////////////////////////////////////////////////////////////
		/// Shortcut to Importlogger::log
		void Importer::_log(
			ImportLogger::Level level,
			const std::string& content
		) const	{
			_logger.log(level, content);
		}



		Importer::Importer(
			util::Env& env,
			const Import& import,
			const ImportLogger& logger
		):	_env(env),
			_import(import),
			_logger(logger)
		{}



		DBTransaction Importer::save() const
		{
			DBTransaction result(_save());
			return result;
		}



		void Importer::_logError( const std::string& content ) const
		{
			_logger.logError(content);
		}



		void Importer::_logWarning( const std::string& content ) const
		{
			_logger.logWarning(content);
		}



		void Importer::_logDebug( const std::string& content ) const
		{
			_logger.logDebug(content);
		}



		void Importer::_logInfo( const std::string& content ) const
		{
			_logger.logInfo(content);

		}



		void Importer::_logLoad( const std::string& content ) const
		{
			_logger.logLoad(content);
		}



		void Importer::_logCreation( const std::string& content ) const
		{
			_logger.logCreation(content);
		}
}	}
