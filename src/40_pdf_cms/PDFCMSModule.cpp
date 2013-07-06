
/** PDFCMSModule class implementation.
	@file PDFCMSModule.cpp

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

#include "PDFCMSModule.hpp"

#include "PDF.hpp"
#include "Request.h"

#include <boost/lexical_cast.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace pdf_cms;

	namespace util
	{
		template<>
		const string FactorableTemplate<ModuleClass, PDFCMSModule>::FACTORY_KEY = "40_pdf_cms";
	}


	namespace pdf_cms
	{
		PDFCMSModule::RequestPDFs PDFCMSModule::_requestPDFs;



		//////////////////////////////////////////////////////////////////////////
		/// Request temporary PDF creator.
		/// @param request the request which creates the PDF
		/// @warning Do not delete the created PDF, the request destructor will do
		/// it automatically
		void PDFCMSModule::CreateRequestPDF( const server::Request& request )
		{
			// Check if the request already has a pdf
			DestroyRequestPDF(request);

			// Create the new PDF
			boost::shared_ptr<PDF> pdf(new PDF);

			// Register the new PDF into the list
			_requestPDFs.insert(
				make_pair(
					&request,
					pdf		
			)	);

			// Register the destroyer into the request
			request.addOnDestroyFunction(&PDFCMSModule::DestroyRequestPDF);
		}



		//////////////////////////////////////////////////////////////////////////
		/// Request temporary PDF destroyer.
		/// @param request the request which created the PDF
		void PDFCMSModule::DestroyRequestPDF( const server::Request& request )
		{
			RequestPDFs::iterator it(_requestPDFs.find(&request));
			if(it != _requestPDFs.end())
			{
				_requestPDFs.erase(it);
			}
		}



		//////////////////////////////////////////////////////////////////////////
		/// Request temporary PDF getter.
		/// @param request the request which created the PDF
		/// @throws RequestPDFNotFoundException if the request has not created any
		/// PDF before
		PDF& PDFCMSModule::GetRequestPDF( const server::Request& request )
		{
			// Check if the request has a PDF
			RequestPDFs::iterator it(_requestPDFs.find(&request));
			if(it == _requestPDFs.end())
			{
				throw RequestPDFNotFoundException(request);
			}

			// Return it
			return *it->second;
		}



		PDFCMSModule::RequestPDFNotFoundException::RequestPDFNotFoundException(
			const server::Request& request
		):	Exception("No PDF was created for the request "+ lexical_cast<string>(&request))
		{}
	}

	namespace server
	{
		template<>
		const string ModuleClassTemplate<PDFCMSModule>::NAME = "PDF CMS";



	    template<> void ModuleClassTemplate<PDFCMSModule>::PreInit ()
	    {
	    }



		template<> void ModuleClassTemplate<PDFCMSModule>::Init()
		{
		}



		template<> void ModuleClassTemplate<PDFCMSModule>::Start()
		{
		}



		template<> void ModuleClassTemplate<PDFCMSModule>::End()
		{
		}



		template<> void ModuleClassTemplate<PDFCMSModule>::InitThread()
		{
		}



		template<> void ModuleClassTemplate<PDFCMSModule>::CloseThread()
		{
		}
	}

	namespace pdf_cms
	{
	}
}
