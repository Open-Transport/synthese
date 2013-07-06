
/** PDFCMSModule class header.
	@file PDFCMSModule.hpp

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

#ifndef SYNTHESE_PDFCMSModule_H__
#define SYNTHESE_PDFCMSModule_H__

#include "ModuleClassTemplate.hpp"

namespace synthese
{
	/**	@defgroup m40Actions 40 Actions
		@ingroup m40

		@defgroup m40Pages 40 Pages
		@ingroup m40

		@defgroup m40Functions 40 Functions
		@ingroup m40

		@defgroup m40Exceptions 40 Exceptions
		@ingroup m40

		@defgroup m40LS 40 Table synchronizers
		@ingroup m40

		@defgroup m40Admin 40 Administration pages
		@ingroup m40

		@defgroup m40Rights 40 Rights
		@ingroup m40

		@defgroup m40Logs 40 DB Logs
		@ingroup m40

		@defgroup m40 40 Map drawing
		@ingroup m3


		@{
	*/

	/** 40 Map module namespace. */
	namespace pdf_cms
	{
		class PDF;

		/** 40 Map module class. */
		class PDFCMSModule:
			public server::ModuleClassTemplate<PDFCMSModule>
		{
		public:
			typedef std::map<const server::Request*, boost::shared_ptr<PDF> > RequestPDFs;

			class RequestPDFNotFoundException:
				public synthese::Exception
			{
			public:
				RequestPDFNotFoundException(const server::Request& request);
			};

		private:
			static RequestPDFs _requestPDFs;

		public:
			static void CreateRequestPDF(const server::Request& request);
			static PDF& GetRequestPDF(const server::Request& request);
			static void DestroyRequestPDF(const server::Request& request);
		};
	}
	/** @} */
}

#endif
