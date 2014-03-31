
/** connector class header.
	@file connector.hpp
	@date 2013

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

#include <string>
#include <iostream>

#include <boost/thread/recursive_mutex.hpp>
#include <boost/shared_ptr.hpp>
#include <mysql.h>
#include <pthread.h>
#include <signal.h>
#include <iconv.h>
#include <stdlib.h>
#include <boost/lexical_cast.hpp>

#include "Log.h"
#include "DeparturesTableModule.h"
#include "Exception.h"
#include "time.h"

//#define DEBUG_RT_FULL 1
#define DEBUG_RT 1

struct st_mysql;
typedef struct st_mysql MYSQL;

const int MYSQL_CONNECTION_NB = 5;

struct ConnectionInfo
{
	std::string host;
	std::string user;
	std::string passwd;
	std::string db;
	int port;
};

typedef struct
{
	bool queryIsOK;
	bool queryIsOver;
	const char * requestStr;
	int connectionIndex;
} queryArgs;


class MySQLResult
{
	private:
		MYSQL_RES* _result;
		mutable MYSQL_ROW _row;
		mutable int _pos;

		template<class T, T DEFAULT_VALUE> 
		T _getValue(int column) const;

		void _resetPosition() const;
		void _incrementPosition() const;
		void _ensurePosition() const;

		void * _iconv;

	public:
		MySQLResult(MYSQL_RES* result);
		~MySQLResult();
		void reset() const;
		bool next() const;
		int getNbData() const;
		int getNbColumns() const;
		int getInt(int column) const;
		long long getLongLong(int column) const;
		double getDouble(int column) const;
		std::string getColumnName(int column) const;
		std::string getText(int column) const;
		std::string getDateTime() const;
		std::string getNom() const;
		std::string getInfo(std::string text) const;
};


class MySQLconnector
{
	private:

		static MYSQL** _connections;
		static bool * _availableConnections;

		static void _initConnection(int connectionIndex);

		static ConnectionInfo _connInfo;

		//This mutex is used to protect read/write operations on _connections
		static boost::recursive_mutex _connectionMutex;
		//This mutex is used to stop threads if all connections are currently in use
		static boost::mutex _connectionFullMutex;
		static int _usedConnectionNb;

		static void *launchQuery(void *argPtr);

		int _connectionIndex;
		void freeUsedConnection();
		void waitForAConnection();

	public:

		MySQLconnector(){};
		boost::shared_ptr <MySQLResult> execQuery(const std::string sql) throw(synthese::Exception);
};



ConnectionInfo MySQLconnector::_connInfo;
boost::recursive_mutex MySQLconnector::_connectionMutex;
boost::mutex MySQLconnector::_connectionFullMutex;
MYSQL** MySQLconnector::_connections(NULL);
bool * MySQLconnector::_availableConnections(NULL);
int MySQLconnector::_usedConnectionNb(0);



using namespace std;
using namespace boost;



MySQLResult::MySQLResult(MYSQL_RES* result) :
	_result(result),
	_row(NULL),
	_iconv (iconv_open("UTF-8","ISO-8859-1"))
{}



MySQLResult::~MySQLResult()
{ 
	mysql_free_result(_result);
	iconv_close(_iconv);
}



void MySQLResult::_resetPosition() const
{
	_pos = -1;
}



void MySQLResult::_incrementPosition() const
{
	_pos++;
}



void MySQLResult::_ensurePosition() const
{
	if(_pos == -1)
	{
		//TODO : throw exception ?
		synthese::util::Log::GetInstance().warn("MySQLResult::_ensurePosition not inside result (use next ())");
		return;
	}
}



template<class T, T DEFAULT_VALUE>
T MySQLResult::_getValue(int column) const
{
	_ensurePosition();

	if (!_row)
		return DEFAULT_VALUE;

	return lexical_cast<T>(_row[column]);
}



void MySQLResult::reset() const
{
	mysql_data_seek(_result, 0);
	_resetPosition();
}



bool MySQLResult::next() const
{
	_row = mysql_fetch_row(_result);
	_incrementPosition();
	return _row != NULL;
}



int MySQLResult::getNbColumns() const
{
	return mysql_num_fields(_result);
}



int MySQLResult::getNbData() const
{
	return mysql_num_rows(_result);
}



string MySQLResult::getColumnName(int column) const
{
	return mysql_fetch_field_direct(_result, column)->name;
}



string MySQLResult::getText(int column) const
{
	if(!_row)
	{
		synthese::util::Log::GetInstance().warn("MySQLResult::getText no row selected");
		return string();
	}

	string text = string(_row[column]);
	
	if(text.empty())
		return string();

	char *pBuf;
	size_t len;
	size_t utf8len;
	
	len = text.length();

	utf8len = 4*text.size();

	pBuf = const_cast<char*>(text.c_str());

	char* utf8buf = (char*) malloc(sizeof(char) * utf8len);

	char* pUtfbuf(utf8buf);

	if(iconv(_iconv, &pBuf, &len, &pUtfbuf, &utf8len) == (size_t) (-1))
	{
		free(utf8buf);
		synthese::util::Log::GetInstance().warn("MySQLResult::getText cannot convert to UTF-8");
		return text;
	}

	*pUtfbuf = 0;

	string result(utf8buf);

	free(utf8buf);

	return result;
}

std::string MySQLResult::getInfo(std::string text) const
{
	for(int i=0; i < getNbColumns(); i++)
	{
		if (getColumnName(i) == text)
			return getText(i);
	}

	synthese::util::Log::GetInstance().warn("MySQLResult::getInfo unknown column " + text);

	return string();
}

void MySQLconnector::_initConnection(int index)
{
	_connInfo.host = synthese::departure_boards::DeparturesTableModule::GetIneoServerIP();
	_connInfo.user = synthese::departure_boards::DeparturesTableModule::GetIneoServerDBLogin();
	_connInfo.passwd = synthese::departure_boards::DeparturesTableModule::GetIneoServerDBPassword();
	_connInfo.db = synthese::departure_boards::DeparturesTableModule::GetIneoServerDBName();
	_connInfo.port = synthese::departure_boards::DeparturesTableModule::GetIneoServerPort();

	stringstream indexStr;
	indexStr << index;
#ifdef DEBUG_RT_FULL
	synthese::util::Log::GetInstance().info("MySQLconnector::_initConnection MySQL initConnexion start, connection index = " + indexStr.str());
#endif

	if(!mysql_thread_safe())
	{
		//TODO : throw Exception ?
		synthese::util::Log::GetInstance().warn("MySQLconnector::_initConnection MySQL client is not compiled as thread-safe");
		return;
	}

	if(_connections[index] != NULL)
	{
#ifdef DEBUG_RT_FULL
		synthese::util::Log::GetInstance().info("MySQL _connection wasn't NULL. Closing previous connection, connection index = " + indexStr.str());
#endif
		mysql_close(_connections[index]);
	}

	_connections[index] = mysql_init(NULL);

	if(_connections[index] == NULL)
	{
		synthese::util::Log::GetInstance().warn("MySQLconnector::_initConnection mysql_init() failed, connection index = " + indexStr.str());
		return;
	}
  
	if(mysql_real_connect(
		_connections[index], _connInfo.host.c_str(), _connInfo.user.c_str(),
		_connInfo.passwd.c_str(), _connInfo.db.c_str(),
		_connInfo.port, NULL, CLIENT_MULTI_STATEMENTS) == NULL)
	{
		synthese::util::Log::GetInstance().warn("MySQLconnector::_initConnection can't connect to MySQL server, connection index = " + indexStr.str());
		return;
	}

#ifdef DEBUG_RT_FULL
	synthese::util::Log::GetInstance().info("MySQLconnector::_initConnection MySQL connection is ok, connection index = " + indexStr.str());
#endif
}

// POSIX Thread Method
void* MySQLconnector::launchQuery(void *argPtr)
{
	queryArgs *args = (queryArgs*)argPtr;
	stringstream indexStr;
	indexStr << args->connectionIndex;

#ifdef DEBUG_RT_FULL
	synthese::util::Log::GetInstance().info("MySQLconnector::launchQuery MySQL launchQuery start, connection index = " + indexStr.str());
#endif
	
	const int NUM_QUERY_RETRY = 5;
    
	for(int i=0; i<NUM_QUERY_RETRY; i++)
	{
		if(_connections[args->connectionIndex] == NULL)  
		{
			synthese::util::Log::GetInstance().warn("MySQLconnector::launchQuery connexion " + indexStr.str() + " is NULL, (re)connecting in progress");
			_initConnection(args->connectionIndex);
		}
		else
		{
			if(mysql_query(_connections[args->connectionIndex],args->requestStr))
			{
				unsigned int myErrno = mysql_errno(_connections[args->connectionIndex]);
				stringstream errnoStr;
				errnoStr << myErrno;
				args->queryIsOK = false;

				synthese::util::Log::GetInstance().warn("MySQLconnector::launchQuery error during mysql_query() with code " + errnoStr.str());
#ifdef DEBUG_RT
				synthese::util::Log::GetInstance().warn(mysql_error(_connections[args->connectionIndex]));
#endif
				// Clientside error, trying again
				if(myErrno >= 2000)
				{
					_initConnection(args->connectionIndex);
				}
				// Server side error, do not worth a retry
				else
					break;
			}
			else
			{
#ifdef DEBUG_RT_FULL
				synthese::util::Log::GetInstance().info("MySQLconnector::launchQuery MySQL query success, connection index = " + indexStr.str());
#endif
				args->queryIsOK = true;
				break;
			}
		}
	}

	args->queryIsOver = true;

#ifdef DEBUG_RT_FULL
	synthese::util::Log::GetInstance().info("MySQLconnector::launchQuery MySQL launchQuery end, connection index = " + indexStr.str());
#endif
	return NULL;
}

void MySQLconnector::freeUsedConnection()
{
	boost::recursive_mutex::scoped_lock lock(_connectionMutex);

#ifdef DEBUG_RT_FULL
	stringstream startMessage;
	startMessage << "MySQLconnector::freeUsedConnection MySQL freeUsedConnection start,"
				 << "connection index = " << _connectionIndex << ", usedConnectionNb = " << _usedConnectionNb;
	synthese::util::Log::GetInstance().info(startMessage.str());
#endif

	// Flag the connection as available
	_availableConnections[_connectionIndex] = true;
	_usedConnectionNb--;

	// If _connectionFullMutex was locked, now we unlock it 
	if(_usedConnectionNb == MYSQL_CONNECTION_NB - 1)
	{
#ifdef DEBUG_RT_FULL
		synthese::util::Log::GetInstance().info("MySQLconnector::freeUsedConnection unlock connection full mutex");
#endif
		_connectionFullMutex.unlock();
	}
}

void MySQLconnector::waitForAConnection()
{
#ifdef DEBUG_RT_FULL
	synthese::util::Log::GetInstance().info("MySQLconnector::waitForAConnection waiting for _connectionFullMutex lock");
#endif

	// First : we stop if _connections is FULL
	_connectionFullMutex.lock();

#ifdef DEBUG_RT_FULL
	synthese::util::Log::GetInstance().info("MySQLconnector::waitForAConnection _connectionFullMutex locked");
#endif

	// Now we are sure that there is at least one connection free, lock on read write
	boost::recursive_mutex::scoped_lock lock(_connectionMutex);

#ifdef DEBUG_RT_FULL
	synthese::util::Log::GetInstance().info("MySQLconnector::waitForAConnection _connectionMutex locked");
#endif

	_connectionIndex = -1;

	// Check if it is the first access to database
	if(_availableConnections == NULL)
	{
#ifdef DEBUG_RT
		synthese::util::Log::GetInstance().info("MySQLconnector::waitForAConnection _availableConnections is NULL, first database connection");
#endif

		// Allocate and initialise static vars
		_availableConnections = (bool *)malloc(MYSQL_CONNECTION_NB * sizeof(bool));
		_connections = (MYSQL**)malloc(MYSQL_CONNECTION_NB * sizeof(MYSQL*));

		for(int i=0; i<MYSQL_CONNECTION_NB; i++)
		{
			_availableConnections[i] = true;
			_connections[i] = NULL;
		}
		_connectionIndex = 0;
	}
	else
	{
		for(int i=0; i<MYSQL_CONNECTION_NB; i++)
		{
			// Stop on first available connection (there is one because _connectionFullMutex wasn't locked)
			if(_availableConnections[i])
			{
				_connectionIndex = i;
				break;
			}
		}
	}

	// Flag the connection we're using
	_availableConnections[_connectionIndex] = false;
	_usedConnectionNb++;

	// If connection index is not the last one, don't need to lock _connectionFullMutex anymore
	if(_usedConnectionNb != MYSQL_CONNECTION_NB)
	{
		_connectionFullMutex.unlock();
	}
	else
	{
		synthese::util::Log::GetInstance().warn("MySQLconnector::waitForAConnection _connectionFullMutex stay locked, all threads are busy");
	}
}

shared_ptr<MySQLResult> MySQLconnector::execQuery(const string sql) throw (synthese::Exception)
{
	// Check available connections
	waitForAConnection();
	// Do query
	queryArgs *args = (queryArgs*)malloc(sizeof(queryArgs));

	args->queryIsOK = false;
	args->queryIsOver = false;
	args->requestStr = sql.c_str();
	args->connectionIndex = _connectionIndex;

	bool queryTimedout = true;

#ifdef DEBUG_RT_FULL
	synthese::util::Log::GetInstance().info("MySQLconnector::execQuery try to create thread");
#endif
	
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); 
	pthread_t thread_handle;

	int ret = pthread_create(&thread_handle, &attr, &launchQuery, (void*) args);
	pthread_attr_destroy(&attr);

	if (ret)
	{
		stringstream errnoCreate;
		errnoCreate << "MySQLconnector::execQuery failed thread creation : errno = " << ret;
		synthese::util::Log::GetInstance().warn(errnoCreate.str());
		
		stringstream err1,err2,err3,err4;
		err1 << "\tMySQLconnector::execQuery EAGAIN = " << EAGAIN;
		err2 << "\tMySQLconnector::execQuery EINVAL = " << EINVAL;
		err3 << "\tMySQLconnector::execQuery EPERM  = " << EPERM;
		err4 << "\tMySQLconnector::execQuery ENOMEM = " << ENOMEM;

		synthese::util::Log::GetInstance().warn(err1.str());
		synthese::util::Log::GetInstance().warn(err2.str());
		synthese::util::Log::GetInstance().warn(err3.str());
		synthese::util::Log::GetInstance().warn(err4.str());

		freeUsedConnection();
		free(args);

		throw synthese::Exception("MySQLconnector : can't create new thread");
	}

	unsigned int cpt = 0;

	// Total wait 1000ms, except if MySQL server was gone
	while(cpt < 200)
	{
		// Wait 5000 usec = 5 msec
		usleep(5000);
		cpt++;

		if(args->queryIsOver)
		{
#ifdef DEBUG_RT
			stringstream durationMsg;
			durationMsg << "MySQLconnector::execQuery ended, duration = " << cpt*5 << " ms";
			synthese::util::Log::GetInstance().info(durationMsg.str());
#endif
			queryTimedout = false;
			break;
		}
	}

	if(queryTimedout)
	{
		synthese::util::Log::GetInstance().warn("MySQLconnector::execQuery MySQL too long (more than 1000 ms)");
		// Don't forget to kill thread
		pthread_cancel(thread_handle);
		freeUsedConnection();
		free(args);
		throw synthese::Exception("MySQLconnector : MySQL too long");
	}
	if(!args->queryIsOK)
	{
		synthese::util::Log::GetInstance().warn("MySQLconnector::execQuery MySQL thread query failed");
		freeUsedConnection();
		free(args);
		throw synthese::Exception("MySQLconnector : MySQL thread query failed");
	}

	MYSQL_RES* result = mysql_store_result(_connections[_connectionIndex]);

	free(args);
	freeUsedConnection();

	return shared_ptr<MySQLResult>(new MySQLResult(result));
}
