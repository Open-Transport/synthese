
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
//#include <my_global.h> //break compilation : is this usefull ?

#include "Log.h"
#include "DeparturesTableModule.h"
#include "Exception.h"
#include "time.h"

//#define DEBUG_RT_FULL 1
#define DEBUG_RT 1

struct st_mysql;
typedef struct st_mysql MYSQL;

const int MYSQL_CONNECTION_NB = 2;

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

		//void doQuery(char * str,bool & queryIsOK);
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
		synthese::util::Log::GetInstance().warn("Not inside result (use next ())...");
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
	if (!_row)
		return "ERROR : NO SQL RESULTS";

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
		synthese::util::Log::GetInstance().warn("MySQLResult::getText canno't convert to UTF-8 !!!");
		return text;
	}

	*pUtfbuf = 0;

	string result(utf8buf);

	free(utf8buf);

	return result;
}

std::string MySQLResult::getInfo(std::string text) const
{
	std::string LOG = "";

	for(int i=0; i < getNbColumns(); i++){
		if (getColumnName(i) == text)
			return getText(i);
		LOG += getColumnName(i) + " ; ";
	}

	return "ERROR : LOG > " + LOG + " and UNKNOW COLUMN > " + text;
}

void MySQLconnector::_initConnection(int index)
{
	_connInfo.host = synthese::departure_boards::DeparturesTableModule::GetIneoServerIP();
	_connInfo.user = synthese::departure_boards::DeparturesTableModule::GetIneoServerDBLogin();
	_connInfo.passwd = synthese::departure_boards::DeparturesTableModule::GetIneoServerDBPassword();
	_connInfo.db = synthese::departure_boards::DeparturesTableModule::GetIneoServerDBName();
	_connInfo.port = synthese::departure_boards::DeparturesTableModule::GetIneoServerPort();

	stringstream warnStart;
	warnStart << "MySQL initConnexion start, connection index = " << index << " !!";
	synthese::util::Log::GetInstance().warn(warnStart.str());

	if(!mysql_thread_safe())
	{
		//TODO : throw Exception ?
		synthese::util::Log::GetInstance().warn("MySQL client not compiled as thread-safe.");
		return;
	}

	if(_connections[index] != NULL)
	{
		stringstream warnMsg;
		warnMsg << "MySQL _connection wasn't NULL close previous connexion, connection index = " << index << " !!";
		synthese::util::Log::GetInstance().warn(warnMsg.str());
		mysql_close(_connections[index]);
	}

	_connections[index] = mysql_init(NULL);
	if(_connections[index] == NULL)
	{
		stringstream warnMsg1;
		warnMsg1 << "mysql_init() failed, connection index = " << index << " !!";
		synthese::util::Log::GetInstance().warn(warnMsg1.str());
		return;
	}
  
	if(mysql_real_connect(
		_connections[index], _connInfo.host.c_str(), _connInfo.user.c_str(),
		_connInfo.passwd.c_str(), _connInfo.db.c_str(),
		_connInfo.port, NULL, CLIENT_MULTI_STATEMENTS) == NULL)
	{
		stringstream warnMsg2;
		warnMsg2 << "Can't connect to MySQL server, connection index = " << index << " !!";
		synthese::util::Log::GetInstance().warn(warnMsg2.str());
		return;
	}

#ifdef DEBUG_RT_FULL
	stringstream warnMsg3;
	warnMsg3 << "MySQL connexion is OK, connection index = " << index << " !!";
	synthese::util::Log::GetInstance().warn(warnMsg3.str());
#endif
}


//POSIX Thread Method
void* MySQLconnector::launchQuery(void *argPtr)
{
	queryArgs *args = (queryArgs*)argPtr;

#ifdef DEBUG_RT_FULL
	stringstream warnStart;
	warnStart << "MySQL launchQuery start, connection index = " << args->connectionIndex << " !!";
	synthese::util::Log::GetInstance().warn(warnStart.str());
#endif
	
	const int NUM_QUERY_RETRY = 10;
    
	for(int i=0; i<NUM_QUERY_RETRY; i++)
	{
		if(_connections[args->connectionIndex] == NULL)  
		{
			stringstream warnMsg;
			warnMsg << "MySQL launchQuery : connexion " << args->connectionIndex << "was NULL : try reconnect";
			synthese::util::Log::GetInstance().warn(warnMsg.str());
			_initConnection(args->connectionIndex);
		}
		else
		{
			if(mysql_query(_connections[args->connectionIndex],args->requestStr))
			{
				unsigned int myxaverrno = mysql_errno(_connections[args->connectionIndex]);
				args->queryIsOK = false;

				stringstream errnoQuery;
				errnoQuery << "MySQL mysql_query failed : errno = " << myxaverrno << ", connection nb = " << args->connectionIndex;

				// CR_SERVER_GONE_ERROR
				if(myxaverrno == 2006)
				{
					synthese::util::Log::GetInstance().warn("MySQL disconected CR_SERVER_GONE_ERROR");
				}

				synthese::util::Log::GetInstance().warn(errnoQuery.str());
				_initConnection(args->connectionIndex);
			}
			else
			{
				stringstream warnMsg2;
				warnMsg2 << "MySQL mysql_query sucess, connection index = " << args->connectionIndex << " !!";
				synthese::util::Log::GetInstance().warn(warnMsg2.str());
				args->queryIsOK = true;
				break;
			}
		}
	}

	args->queryIsOver = true;

#ifdef DEBUG_RT_FULL
	stringstream warnEnd;
	warnEnd << "MySQL launchQuery end, connection index = " << args->connectionIndex << " !!";
	synthese::util::Log::GetInstance().warn(warnEnd.str());
#endif
	return NULL;
}

void MySQLconnector::freeUsedConnection()
{
	boost::recursive_mutex::scoped_lock lock(_connectionMutex);

#ifdef DEBUG_RT_FULL	
	stringstream warnMsg;
	warnMsg << "freeUsedConnection, connection index = " << _connectionIndex << ", usedConnectionNb = " << _usedConnectionNb << "!!";
	synthese::util::Log::GetInstance().warn(warnMsg.str());
#endif

	// Write connection not in use
	_availableConnections[_connectionIndex]=true;
	_usedConnectionNb--;

	// If _connectionFullMutex was locked, now we unlock it 
	if(_usedConnectionNb == MYSQL_CONNECTION_NB - 1)
	{
		synthese::util::Log::GetInstance().warn("unlock FULL connection mutex");
		_connectionFullMutex.unlock();
	}
}

void MySQLconnector::waitForAConnection()
{
#ifdef DEBUG_RT_FULL
	synthese::util::Log::GetInstance().warn("waitForAConnection : wait for _connectionFullMutex");
#endif

	// First : we stop if _connections is FULL
	_connectionFullMutex.lock();

#ifdef DEBUG_RT_FULL
	synthese::util::Log::GetInstance().warn("waitForAConnection : wait for _connectionMutex");
#endif

	// Now we are sure that at less there is one connection FREE
	// Now lock on read/write operations 
	boost::recursive_mutex::scoped_lock lock(_connectionMutex);

#ifdef DEBUG_RT_FULL
	//synthese::util::Log::GetInstance().warn("waitForAConnection : AFTER MUTEX");
#endif

	_connectionIndex = -1;

	// Check if it is first access to BDD
	if(_availableConnections == NULL)
	{
#ifdef DEBUG_RT
		synthese::util::Log::GetInstance().warn("_availableConnections==NULL : first BDD connection");
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
	// That is not the first access to BDD
	else
	{
		for(int i=0; i<MYSQL_CONNECTION_NB; i++)
		{
			// Stop on first available connection (it must be one because connection is not FULL)
			if(_availableConnections[i])
			{
				_connectionIndex = i;
				break;
			}
		}
	}

	// Write connection in use
	_availableConnections[_connectionIndex] = false;
	_usedConnectionNb++;

#ifdef DEBUG_RT_FULL
	for(int i=0;i<MYSQL_CONNECTION_NB;i++){
		stringstream warnMsg1;
		warnMsg1 << "after _availableConnections["
				<< i << "] = " << _availableConnections[i];
		synthese::util::Log::GetInstance().warn(warnMsg1.str());
	}

	stringstream warnMsg;
	warnMsg << "waitForAConnection, GO for connection index = " << _connectionIndex
			<< ", usedConnectionNb = " << _usedConnectionNb << "!!";
	synthese::util::Log::GetInstance().warn(warnMsg.str());
#endif

	// If connection index is not last one, _connections is not FULL
	if(_usedConnectionNb != MYSQL_CONNECTION_NB)
	{
		//synthese::util::Log::GetInstance().warn("unlock _connectionFullMutex");
		_connectionFullMutex.unlock();
	}
	// _connectionFullMutex stay locked
	else
	{
		synthese::util::Log::GetInstance().warn("FULL !!!!! _connectionFullMutex stay locked");
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
	synthese::util::Log::GetInstance().warn("MySQL try to create thread ");
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
		errnoCreate << "MySQL failed thread creation : errno = " << ret;
		synthese::util::Log::GetInstance().warn(errnoCreate.str());
		
		stringstream err1,err2,err3,err4;
		err1 << "EAGAIN = " << EAGAIN;
		err2 << "EINVAL = " << EINVAL;
		err3 << "EPERM  = " << EPERM;
		err4 << "ENOMEM = " << ENOMEM;

		synthese::util::Log::GetInstance().warn(err1.str());
		synthese::util::Log::GetInstance().warn(err3.str());
		synthese::util::Log::GetInstance().warn(err2.str());
		synthese::util::Log::GetInstance().warn(err4.str());
		freeUsedConnection();
		free(args);

		throw synthese::Exception("MySQL connector : can't create new thread");
	}

	unsigned int cpt = 0;

	// Total wait 5000ms
	while(cpt < 500)
	{
		// Wait 10000 usec = 10 msec
		usleep(10000);
		cpt++;

		if(args->queryIsOver)
		{
			queryTimedout = false;
			stringstream warnMsg;
			warnMsg << "Mysql query duration = " << cpt*10 << " ms";
			synthese::util::Log::GetInstance().warn(warnMsg.str());
			break;
		}
	}

	if(queryTimedout)
	{
		synthese::util::Log::GetInstance().warn("MYSQL TOO LONG (more than 5000 ms) ");
		// Don't forget to kill thread
		pthread_cancel(thread_handle);
		freeUsedConnection();
		free(args);
		throw synthese::Exception("MySQL connector : MYSQL TOO LONG");
	}
	if(!args->queryIsOK)
	{
		synthese::util::Log::GetInstance().warn("MYSQL thread query failed");
		freeUsedConnection();
		free(args);
		throw synthese::Exception("MySQL connector : MYSQL thread query failed");
	}

	MYSQL_RES* result = mysql_store_result(_connections[_connectionIndex]);

	free(args);
	if(!result)
	{
		synthese::util::Log::GetInstance().warn("MySQL error in mysql_store_result()");
		freeUsedConnection();
		synthese::Exception("MySQL connector : MySQL error in mysql_store_result()");
	}

	freeUsedConnection();
	return shared_ptr<MySQLResult>(new MySQLResult(result));
}
