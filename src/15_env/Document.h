
/// Document class header.
///	@file Document.h
///	@author Hugues Romain
///	@date 2008
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_ENV_DOCUMENT_H
#define SYNTHESE_ENV_DOCUMENT_H

#include "Registrable.h"
#include "Registry.h"
#include "UId.h"
#include <string>

namespace synthese
{
	namespace env
	{
		//////////////////////////////////////////////////////////////////////////
		/// @ingroup m35
		//////////////////////////////////////////////////////////////////////////
		class Document : public virtual util::Registrable
		{
		public:
		    
			typedef enum
				{
					DOCUMENT_TYPE_PDF,
					DOCUMENT_TYPE_HTML,
					DOCUMENT_TYPE_GIF,
					DOCUMENT_TYPE_JPEG,
					DOCUMENT_TYPE_PNG
			} DocumentType;


			/// Chosen registry class.
			typedef util::Registry<Document>	Registry;

		private:

			std::string _url; //!< Document url for display with web server
			DocumentType _type;    //!< Document type
			std::string _globalDescription; //!< Context independant description
			std::string _localDescription;  //!< Context dependant description (ex : relative to a certain page)
		    
		 public:

				Document( const uid& id,
				  const std::string& url,
				  const std::string& globalDescription,
				  const std::string& localDescription);

				virtual ~Document();


			//! @name Getters/Setters
			//@{
				const std::string& getUrl () const;
				DocumentType getType () const;
				const std::string& getLocalDescription() const;
				const std::string& getGlobalDescription () const;
			//@}
		};
	}
}

#endif
