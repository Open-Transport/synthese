
/** EMail class header.
	@file EMail.h

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

#ifndef SYNTHESE_util_EMail_h__
#define SYNTHESE_util_EMail_h__

#include <string>
#include <vector>
#include <exception>

namespace synthese
{
	namespace util
	{
		/** E-Mail class.
			@ingroup m01
		*/
		class EMail
		{
		public:
			typedef enum
			{
				EMAIL_TEXT,
				EMAIL_HTML
			} Format;

			class Exception : public std::exception
			{

			};

		private:
			Format						_format;
			std::string					_content;
			std::vector<std::string>	_recipients;
			std::string					_sender;
			std::string					_subject;

		public:
			void setFormat(Format value);
			void setContent(const std::string& value);
			void addRecipient(const std::string& value);
			void setSender(const std::string& value);
			void setSubject(const std::string& value);

			void send(const std::string& smtpServer, std::string service = "mail") const;
		};
	}
}

#endif // SYNTHESE_util_EMail_h__
