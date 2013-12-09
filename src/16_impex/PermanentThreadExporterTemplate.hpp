
/** PermanentThreadExporter template header.
	@file PermanentThreadExporter.hpp

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

#ifndef SYNTHESE_impex_PermanentThreadExporter_hpp__
#define SYNTHESE_impex_PermanentThreadExporter_hpp__

#include "Exporter.hpp"

#include "Export.hpp"

namespace synthese
{
	namespace impex
	{
		/** OneFileTypeImporter class.
			@ingroup m16
		*/
		template<class FF>
		class PermanentThreadExporterTemplate:
			virtual public Exporter
		{
		public:
			static const bool EXPORTABLE;
			static const bool IS_PERMANENT_THREAD;

		private:
			mutable bool _hasToStop;

		protected:
			virtual bool isPermanentThread() const { return true; }

			virtual void _onStart() const = 0;

			virtual void _loop() const = 0;

			virtual boost::posix_time::time_duration _getWaitingTime() const = 0;

			virtual void _onStop() const = 0;

			virtual bool parseFiles() const { return false; }
			virtual db::DBTransaction _save() const { return db::DBTransaction(); }

			virtual void killPermanentThread() const;
			virtual void runPermanentThread() const;

		public:
			virtual std::string getFileFormatKey() const { return FF::FACTORY_KEY; }
			virtual std::string getOutputMimeType() const { return std::string(); }
			virtual void build(std::ostream& os) const {}

			void threadRun() const;

			PermanentThreadExporterTemplate(
				const Export& export_
			):	Exporter(export_),
				_hasToStop(false)
			{}
		};



		template<class FF>
		void PermanentThreadExporterTemplate<FF>::runPermanentThread() const
		{
			server::ServerModule::AddThread(
				boost::bind(&PermanentThreadExporterTemplate<FF>::threadRun, this),
				_export.get<Name>() + " (" + FF::FACTORY_KEY +")"
			);
		}



		template<class FF>
		void PermanentThreadExporterTemplate<FF>::killPermanentThread() const
		{
			_hasToStop = true;
		}



		template<class FF>
		void synthese::impex::PermanentThreadExporterTemplate<FF>::threadRun() const
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

				this_thread::sleep(_getWaitingTime());
			}

			_onStop();
		}



		template<class FF>
		const bool PermanentThreadExporterTemplate<FF>::EXPORTABLE(false);

		template<class FF>
		const bool PermanentThreadExporterTemplate<FF>::IS_PERMANENT_THREAD(true);
}	}

#endif // SYNTHESE_impex_OneFileTypeImporter_hpp__
