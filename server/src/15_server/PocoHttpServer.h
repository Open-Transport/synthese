////////////////////////////////////////////////////////////////////////////////
/// Server Module class header.
///	@file ServerModule.h
///	@author Hugues Romain
///
/// @warning This header must always be included before all other ones to avoid
/// the windows bug "WinSock.h has already been included"
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_SERVER_POCOHTTPSERVER_H
#define SYNTHESE_SERVER_POCOHTTPSERVER_H

#include "Poco/Util/ServerApplication.h"

namespace synthese
{

	namespace server
	{
		class PocoHttpServer: public Poco::Util::ServerApplication
		{
		private:
			unsigned short _port;

		public:
			PocoHttpServer(unsigned short port);

			~PocoHttpServer();

		protected:
			void initialize(Application& self);

			void uninitialize();

			int main(const std::vector<std::string>& args);
		};

	}
}

#endif // SYNTHESE_SERVER_POCOHTTPSERVER_H
