
/** ImportLogger class header.
	@file ImportLogger.hpp

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

#ifndef SYNTHESE_impex_ImportLogger_hpp__
#define SYNTHESE_impex_ImportLogger_hpp__

#include <boost/optional.hpp>
#include <boost/filesystem/path.hpp>
#include <sstream>
#include <fstream>

namespace synthese
{
	namespace impex
	{
		/** ImportLogger class.
			@ingroup m16
		*/
		class ImportLogger
		{
		public:
			typedef enum
			{
				ALL = 0,
				DEBG = 10,
				LOAD = 20,
				CREA = 25,
				INFO = 30,
				WARN = 40,
				NOTI = 50,
				ERROR = 60,
				NOLOG = 99
			} Level;

		private:
			mutable Level _maxLoggedLevel;	//!< Records the worse log level of the content
			const Level _minLevel;	//!< Minimal level of entries to store in the content

			std::auto_ptr<std::ofstream> _fileStream;		//!< Stream on the file where the content must be sent
			boost::optional<std::ostream&> _outputStream;		//!< An output stream where the content must be sent

		public:
			ImportLogger(
				Level minLevel,
				const std::string& logPath,
				boost::optional<std::ostream&> outputStream
			);


			void log(
				Level level,
				const std::string& content
			) const;

			void logInfo(const std::string& content) const;
			void logDebug(const std::string& content) const;
			void logWarning(const std::string& content) const;
			void logError(const std::string& content) const;
			void logCreation(const std::string& content) const;
			void logLoad(const std::string& content) const;

			void logRaw(
				const std::string& content
			) const;

			Level getMaxLoggedLevel() const { return _maxLoggedLevel; }
			boost::optional<std::ostream&> getOutputStream() const { return _outputStream; }
		};
}	}

#endif // SYNTHESE_impex_ImportLogger_hpp__

