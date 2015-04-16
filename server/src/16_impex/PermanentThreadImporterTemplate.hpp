
/** PermanentThreadImporter template header.
	@file PermanentThreadImporter.hpp

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

#ifndef SYNTHESE_impex_PermanentThreadImporter_hpp__
#define SYNTHESE_impex_PermanentThreadImporter_hpp__

#include "Importer.hpp"

#include "Import.hpp"
#include "ServerModule.h"

namespace synthese
{
	namespace impex
	{
		/** OneFileTypeImporter class.
			@ingroup m16
		*/
		template<class FF>
		class PermanentThreadImporterTemplate:
			virtual public Importer
		{
		public:
			static const bool IMPORTABLE;
			static const bool IS_PERMANENT_THREAD;

		private:
			mutable bool _hasToStop;
			mutable boost::shared_ptr<boost::thread> _theThread;

		protected:
			virtual void _onStart() const = 0;

			virtual void _loop() const = 0;

			virtual boost::posix_time::time_duration _getWaitingTime() const = 0;

			virtual void _onStop() const = 0;

			virtual bool parseFiles() const { return false; }
			virtual db::DBTransaction _save() const { return db::DBTransaction(); }

			void _threadRun() const;

		public:
			virtual void killPermanentThread() const;
			virtual void runPermanentThread() const;

			PermanentThreadImporterTemplate(
				util::Env& env,
				const Import& import,
				impex::ImportLogLevel minLogLevel,
				const std::string& logPath,
				boost::optional<std::ostream&> outputStream,
				util::ParametersMap& pm
			):	Importer(env, import, minLogLevel, logPath, outputStream, pm),
				_hasToStop(false)
			{}
		};



		template<class FF>
		void PermanentThreadImporterTemplate<FF>::runPermanentThread() const
		{
			_theThread = server::ServerModule::AddThread(
				boost::bind(&PermanentThreadImporterTemplate<FF>::_threadRun, this),
				_import.get<Name>() + " (" + FF::FACTORY_KEY +")"
			);
		}



		template<class FF>
		void PermanentThreadImporterTemplate<FF>::killPermanentThread() const
		{
			_hasToStop = true;
			if(_theThread)
			{
				_theThread->join();
			}
		}



		template<class FF>
		void synthese::impex::PermanentThreadImporterTemplate<FF>::_threadRun() const
		{
			_hasToStop = false;
			_onStart();

			while(!_hasToStop)
			{
				_loop();

				if(_hasToStop)
				{
					break;
				}

				boost::this_thread::sleep(_getWaitingTime());
			}

			_onStop();
		}



		template<class FF>
		const bool PermanentThreadImporterTemplate<FF>::IMPORTABLE(false);

		template<class FF>
		const bool PermanentThreadImporterTemplate<FF>::IS_PERMANENT_THREAD(true);
}	}

#endif // SYNTHESE_impex_OneFileTypeImporter_hpp__
