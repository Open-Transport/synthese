/** 00_tcp module wide definitions
	@file 00_tcp/module.h
*/

#ifndef SYNTHESE_TCP_MODULE_H
#define SYNTHESE_TCP_MODULE_H

#include <string>


namespace synthese
{

	/** @defgroup m00 00 TCP socket client server implementation

		@{
	*/

	/** 00_tcp namespace */
	namespace tcp
	{

	    const std::string PROTOCOL_TYPE_TCP ("tcp");
	    const std::string PROTOCOL_TYPE_UDP ("udp");


	}

	/** @} */

}

#endif

