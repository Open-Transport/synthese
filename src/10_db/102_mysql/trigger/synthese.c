
/** SYNTHESE MySQL UDF module for database modification HTTP notifications.
	@file synthese.c
	@author Sylvain Pasche

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

#ifdef STANDARD
/* STANDARD is defined, don't use any mysql functions */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef __WIN__
typedef unsigned __int64 ulonglong;	/* Microsofts 64 bit types */
typedef __int64 longlong;
#else
typedef unsigned long long ulonglong;
typedef long long longlong;
#endif /*__WIN__*/
#else
#include <my_global.h>
#include <my_sys.h>
#if defined(MYSQL_SERVER)
#include <m_string.h>
#else
/* when compiled as standalone */
#include <string.h>
#endif
#endif
#include <mysql.h>
#include <ctype.h>

#include <curl/curl.h>

#ifdef HAVE_DLOPEN

#define LOG1(p0) do { printf("notify_synthese_http: " p0); fflush(stdout); } while(0)
#define LOG2(p0, p1) do { printf("notify_synthese_http: " p0, p1); fflush(stdout); } while(0)

#define CURL_TIMEOUT_MS 5000


/* Set this to 1 to enable log messages. They will appear in the MySQL error log */
#if 0
#define DBG1(p0) LOG1(p0)
#define DBG2(p0, p1) LOG2(p0, p1)
#define SHOW_HTTP_OUTPUT
#else
#define DBG1(p0)
#define DBG2(p0, p1)
#endif


my_bool notify_synthese_http_init(
	UDF_INIT *initid __attribute__((unused)),
	UDF_ARGS *args __attribute__((unused)),
	char *message __attribute__((unused))
) {
	if (args->arg_count != 2 ||
		args->arg_type[0] != STRING_RESULT ||
		args->arg_type[1] != STRING_RESULT)
	{
		strcpy(message, "Wrong arguments to notify_synthese_http");
		return 1;
	}
	return 0;
}

static size_t dummy_writefunction(void* ptr, size_t size, size_t nmemb, void* stream)
{
	return nmemb * size;
}

longlong notify_synthese_http(
	UDF_INIT *initid __attribute__((unused)),
	UDF_ARGS *args,
	char *is_null __attribute__((unused)),
	char *error __attribute__((unused))
) {
	CURL *curl;
	CURLcode res;
	char url[512];
	char postfields[2048];

	if (!args->args[0] || !args->lengths[0] || !args->args[1] || !args->lengths[1])
	{
		LOG1("URL is NULL or empty\n");
		*error = 1;
		return 0;
	}

	curl = curl_easy_init();
	if (!curl)
	{
		LOG1("error initializing curl\n");
		*error = 1;
		return 0;
	}
	curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, CURL_TIMEOUT_MS);

	if (args->lengths[0] >= sizeof(url))
	{
		goto buffer_too_small;
	}
	memcpy(url, args->args[0], args->lengths[0]);
	url[args->lengths[0]] = 0;

	DBG2("posting to url %s\n", url);
	curl_easy_setopt(curl, CURLOPT_URL, url);

	if (args->lengths[1] >= sizeof(postfields))
	{
		goto buffer_too_small;
	}
	memcpy(postfields, args->args[1], args->lengths[1]);
	postfields[args->lengths[1]] = 0;

	DBG2("postfields: %s\n", postfields);

	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postfields);

#ifndef SHOW_HTTP_OUTPUT
	// dummy write function to prevent output from being written to standard output.
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, dummy_writefunction);
#endif

	res = curl_easy_perform(curl);
	DBG2("curl return code: %i\n", res);

	if (res)
	{
		*error = 1;
		LOG2("curl returned an error while performing the request, code=%i\n", res);
	}

	curl_easy_cleanup(curl);
	return 0;

buffer_too_small:

	*error = 1;
	curl_easy_cleanup(curl);
	LOG1("error, buffer is too small\n");
	return 0;
}

#endif /* HAVE_DLOPEN */
