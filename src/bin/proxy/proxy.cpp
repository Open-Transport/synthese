////////////////////////////////////////////////////////////////////////////////
/// SYNTHESE HTTP request proxy.
///	@file proxy.cpp
///	@author Sylvain Pasche
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

#include "01_util/concurrent/locking_queue.hpp"
#include "Exception.h"
#include "HTTPConnection.hpp"
#include "HTTPReply.hpp"
#include "Log.h"
#include "URI.hpp"

#include <iostream>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/program_options.hpp>
#include <boost/tokenizer.hpp>

#ifdef HAVE_CURL
#include <curl/curl.h>
#endif

//#define DEBUG

using namespace boost;
using namespace boost::posix_time;
namespace po = boost::program_options;
using namespace std;

namespace
{
	using namespace synthese;
	using namespace synthese::util;
	using namespace synthese::server;
	void requestHandler(const HTTPRequest& request, HTTPReply& reply);

	boost::asio::io_service _io_service;
	boost::asio::ip::tcp::acceptor _acceptor(_io_service);
	connection_ptr _new_connection(new HTTPConnection(_io_service, &requestHandler));

	struct Request
	{
		Request(string path_, string postData_) : path(path_), postData(postData_)
		{
		}
		string path;
		// if postData is empty -> GET, else POST with the given data.
		string postData;
	};

	locking_queue<Request> queue;
	deque<string> messages;
	const unsigned int MAX_MESSAGES = 1000;
	const unsigned int MAX_QUEUE_SIZE = 1;
	const int CURL_TIMEOUT_MS = 2 * 1000;
	bool dispatching = true;

	// Command line parameters
	int port;
	string target;
	string password;
	size_t queue_size;



	void sendHTML(HTTPReply& reply, const string& output)
	{
		reply.content.append(output);
		reply.headers.insert(make_pair("Content-Length", lexical_cast<string>(output.size())));
		reply.headers.insert(make_pair("Content-Type", "text/html; charset=utf-8"));
	}



	void showStatus(HTTPReply& reply)
	{
		stringstream output_;

		output_ << "<!DOCTYPE html>" <<
			"<head><style>" <<
			"body { padding: 50px; font: 16px 'Helvetica Neue', Helvetica, Arial; } " <<
			"a { color: #107aff; text-decoration: none; } " <<
			"a:hover { text-decoration: underline; } " <<
			"h1 a { font-size: 16px; } " <<
			".messages { font-family: Consolas, fixed; } " <<
			"</style>" <<
			"<link rel=icon type=image/gif href=data:image/gif;base64,R0lGODlhAQABAIAAAP///wAAACH5BAEAAAAALAAAAAABAAEAAAICRAEAOw==>" <<
			"<title>Synthese Proxy Status</title></head>" <<
			"<html><h1>Synthese Proxy Status</h1>" <<
			"<p>Queue size: " << queue.size() << " (max=" << queue_size << ")</p>" <<
			"<h3>Management</h3>"<<"<p>Dispatching status: ";
		if(dispatching)
			output_ << "<span style='color: green'>Dispatching</span>";
		else
			output_ << "<span style='color: red'>Paused</span>";
		output_ << "</p><form method=post action=/action>" <<
			"<label>Password:<input type=password name=password></label>" <<
			"<input type=submit name=clear_queue value='Clear Queue'>" <<
			"<input type=submit name=toggle value='Toggle Dispatching Status'></form>"<<
			"<h3>Last messages</h3><ul class=messages>";

		if(messages.empty())
		{
			output_ << "No messages<br>";
		}
		else
		{
			for(unsigned int i = 0; i < messages.size(); i++)
				output_ << "<li>" << messages[i] << "</li>";
		}

		output_ << "</ul></html>";

		sendHTML(reply, output_.str());
	}



	void addMessage(const string message)
	{
		// Append date time
		ptime now(microsec_clock::local_time());
		stringstream m;
		m << std::setfill ('0') <<
			std::setw (4) << now.date().year() << "/" <<
			std::setw (2) << static_cast<int>(now.date().month()) << "/" <<
			std::setw (2) << now.date().day() << " " <<
			std::setw (2) << now.time_of_day().hours() << ":" <<
			std::setw (2) << now.time_of_day().minutes() << ":" <<
			std::setw (2) << now.time_of_day().seconds() <<
			" : " << message;
		messages.push_front(m.str());
#ifdef DEBUG
		cout << "Message: " << m.str() << endl;
#endif
		while(messages.size() > MAX_MESSAGES)
			messages.pop_back();
	}



	map<string, string> parsePost(const string& content)
	{
		typedef tokenizer<char_separator<char> > _tokenizer;
		char_separator<char> sep(URI::PARAMETER_SEPARATOR.c_str ());
		map<string, string> map;

		// Parsing
		_tokenizer parametersTokens(content, sep);
		BOOST_FOREACH(const string& parameterToken, parametersTokens)
		{
			size_t pos = parameterToken.find(URI::PARAMETER_ASSIGNMENT);
			if (pos == string::npos) continue;

			string parameterName (parameterToken.substr (0, pos));
			string parameterValue;
			if(URI::Decode(parameterToken.substr(pos+1), parameterValue))
			{
				map.insert (make_pair (parameterName, parameterValue));
			}
		}
		return map;
	}



	void requestHandler(const HTTPRequest& httpRequest, HTTPReply& reply)
	{
		reply.status = HTTPReply::ok;

		if(httpRequest.uri == "/status")
		{
			showStatus(reply);
		}
		else if(httpRequest.uri == "/action")
		{
			map<string, string> params = parsePost(httpRequest.postData);
			if(params.find("password") == params.end())
			{
				reply.status = HTTPReply::forbidden;
				sendHTML(reply, "Missing Password\n");
				return;
			}
			if(params["password"] != password)
			{
				reply.status = HTTPReply::forbidden;
				sendHTML(reply, "Wrong password\n");
				return;
			}
			string redir = "<script>setTimeout(function() { location.href = '/status' }, 1500);</script>";
			if(params.find("clear_queue") != params.end())
			{
				while(!queue.empty())
				{
					try
					{
						queue.pop();
					}
					catch(...)
					{
					}
				}
				sendHTML(reply, "Queue cleared" + redir);
			}
			if(params.find("toggle") != params.end())
			{
				if(dispatching)
				{
					dispatching = false;
					sendHTML(reply, "Stopped dispatch" + redir);
				}
				else
				{
					dispatching = true;
					sendHTML(reply, "Started dispatch" + redir);
				}
			}
		}
		else if(httpRequest.uri == "/favicon.ico")
		{
			return;
		}
		else
		{
			if(queue.size() >= queue_size)
			{
				addMessage("Queue is full, dropping " + httpRequest.uri +
					" (POST:" + httpRequest.postData + ")");
				return;
			}

			Request r(httpRequest.uri, httpRequest.postData);
#ifdef DEBUG
			addMessage("Adding request " + r);
#endif
			queue.push(r);
		}
	}



	void handleAccept(
		const boost::system::error_code& e
	){
		if (!e)
		{
			_new_connection->start();
			_new_connection.reset(new HTTPConnection(_io_service, &requestHandler));
			_acceptor.async_accept(
				_new_connection->socket(),
				boost::bind(
					&handleAccept,
					boost::asio::placeholders::error
				)
			);
		}
	}



	void startProxyServer()
	{
		asio::ip::tcp::resolver resolver(_io_service);
		asio::ip::tcp::resolver::query query("0.0.0.0", lexical_cast<string>(port));

		asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
		_acceptor.open(endpoint.protocol());
		_acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
		_acceptor.bind(endpoint);
		_acceptor.listen();

		_acceptor.async_accept(
			_new_connection->socket(),
			boost::bind(&handleAccept, asio::placeholders::error)
		);

		cout << "Listening for requests on port " << port << "..." << endl;
		_io_service.run();
	}



	size_t dummy_writefunction(void* ptr, size_t size, size_t nmemb, void* stream)
	{
		return nmemb * size;
	}



	struct DispatcherThread
	{
		void operator()()
		{
#ifdef HAVE_CURL
			CURL *curl;
			CURLcode res;

			curl = curl_easy_init();
			if(!curl)
			{
				throw Exception("Error while initializing curl");
			}
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, dummy_writefunction);
			curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, CURL_TIMEOUT_MS);
#endif
			while(true)
			{
				if(!dispatching)
				{
					boost::this_thread::sleep(boost::posix_time::milliseconds(500));
					continue;
				}
				Request r = queue.pop(true);
				string targetUrl = target + r.path;
#ifdef DEBUG
				cout << "Dispatching to target:" << targetUrl << endl;
#endif
#ifdef HAVE_CURL
				curl_easy_setopt(curl, CURLOPT_URL, targetUrl.c_str());
				if(!r.postData.empty())
				{
					curl_easy_setopt(curl, CURLOPT_POSTFIELDS, r.postData.c_str());
				}
				else
				{
					curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
				}
				res = curl_easy_perform(curl);
				if(res)
				{
					stringstream msg;
					msg << "Error while dispatching to url " << targetUrl <<
						". (POST:" << r.postData << " code=" << res << ")";
					addMessage(msg.str());
				}
#endif
			}
#ifdef HAVE_CURL
			curl_easy_cleanup(curl);
#endif
		}
	};
}

int main(int ac, char* av[])
{
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "produce help message")
		("port,p", po::value<int>(&port)->default_value(-1), "port to listen on")
		("password", po::value<std::string>(&password), "password for the administration interface")
		("target", po::value<std::string>(&target), "Target URL where to relay requests (e.g. http://localhost:9999/)")
		("queue_size", po::value<size_t>(&queue_size), "Max size of the queue")
		;

	po::variables_map vm;
	po::store(po::parse_command_line(ac, av, desc), vm);
	po::notify(vm);

	if(port < 0)
	{
		cout << "Missing --port" << std::endl;
		return 1;
	}
	if(target.empty())
	{
		cout << "Missing --target" << std::endl;
		return 1;
	}

	if(!queue_size)
	{
		queue_size = MAX_QUEUE_SIZE;
	}
	cout << queue_size << endl;
#ifndef HAVE_CURL
	cout << "Libcurl support not available. The proxy can't work properly." << endl;
	return 1;
#endif

	DispatcherThread dispatcher;
	boost::thread thread(dispatcher);
	startProxyServer();
	return 0;
}
