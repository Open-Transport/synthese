/** Main file of the SYNTHESE server. (MODULE 70)
@file main.cpp
*/


#include "SYNTHESE.h"
#ifdef WIN32
#include "stdafx.h"
#include "WinSYNTHESE.h"
#endif
#ifdef UNIX
#include <pthread.h>
#include <signal.h>
#endif
#include <iostream>
#include <string>
#include <sstream>
#include "00_tcp/Socket.h"
#include "01_util/HtmlFilter.h"

#include <boost/iostreams/filtering_stream.hpp>


#include "Parametres.h"
#include "70_server/Request.h"




#ifdef _DEBUG
#define DEBUG
#define new DEBUG_NEW
#endif

/** @addtogroup m00
 @{
*/

//! @name Constantes serveur TCP
//@{
// Masque r�seau des machines authoris�es � se connecter:
// localhost implique connection uniquement depuis la m�me machine
// * implique connection depuis toute machine
// 255.255.255.0 implique connection uniquement depuis le r�seau local
#define DEF_SERV       "*" 
// Port d'�coute par d�faut du serveur
#define DEF_PORT       3591 
// Protocole de communication: "tcp" ou "udp"
#define DEF_PROTO      "tcp" 
// Message d'accueil de Synthese
#define WELCOME_MSG    "Welcome to SYNTHESE" 
// Nombre de threads client
#define NB_MAX_CLIENT  32 
// D�lai de r�ception de la requ�te avant fermeture d'une connection en secondes
#define CLIENT_TIMEOUT 10 
// Taille du tampon d'�change sur le r�seau
#define BUFFER_SIZE    4096 
// D�lai au del� duquel un thread est d�clar� en d�faut
// en cas de d�faut, le calcul est annul� et le thread r�initialis�
// le temps est un multiple de THREAD_PROBE
#define THREAD_TIMEOUT 12 
// P�riode de v�rification des threads en seconde
#define THREAD_PROBE   10 
//@}


// Instance de Synthese
SYNTHESE Synthese;

// Instance de la couche r�seau
synthese::tcp::Socket srvSocket;

// Application Win32
#ifdef WIN32
CWinApp theApp;
#endif

//using std::cout;
using namespace std;


#ifdef UNIX 
// Mutex associ� au serveur
pthread_mutex_t ServerMutex = PTHREAD_MUTEX_INITIALIZER;
// Mutex associ� au calculateur
pthread_mutex_t CalculMutex = PTHREAD_MUTEX_INITIALIZER;
// Informations threads
pthread_attr_t ThreadsAttr;
#endif

typedef struct _thread_status
{
#ifdef WIN32
    int id;
#endif
#ifdef UNIX

    pthread_t id;
#endif

    int counter;
    bool running;
    SOCKET socket;
}
thread_context_t;
thread_context_t ThreadsContext[ NB_MAX_CLIENT ];
int NumberOfThreads = NB_MAX_CLIENT;
int RunningThreads = 0;


#ifdef UNIX 
/** \brief Arret d'un calculateur
    \author Christophe Romain
    \date 2005
    */
void ThreadExit( int sig )
{
    //pthread_t self = pthread_self();
    pthread_exit( 0 );
}

/** \brief Arret total de Synthese
    \author Christophe Romain
    \date 2005
    */
void ServerExit( int sig )
{
    thread_context_t * pContext = ThreadsContext;
    for ( int t = 0; t < NumberOfThreads; t++ )
        srvSocket.close( pContext->socket );
    cout << "Terminaison du serveur." << endl;
    exit( 0 );
}
#endif

/** \brief Prise en compte d'une requ�te client au sein d'un thread d�di�
    \author Christophe Romain
    \date 2005
*/
void *ServerThread( void *args )
{
    char __Buffer[ BUFFER_SIZE ];
    // Le num�ro de thread est pass� en argument
    int __ThreadNumber = ( int ) args;
    // R�cup�ration de contexte de ce thread
    thread_context_t *pContext = &ThreadsContext[ __ThreadNumber ];

#ifdef WIN32
    // La version Win32 n'est pas multi-thread
    pContext->id = 0;
#endif
#ifdef UNIX
    // D�finition du gestionnaire de signaux. USR1 provoque la terminaison.
    signal( SIGUSR1, ThreadExit );
#endif

    while ( 1 )
    {
        // Initialisation du contexte
        pContext->counter = 0;
        pContext->running = false;
#ifdef WIN32
        // Attente de connection
        // Note: la version Win32 est mono-utilisateur
        pContext->socket = Socket.Accept();
#endif
#ifdef UNIX
        // Attente de connection
        pthread_mutex_lock( &ServerMutex );
        pContext->socket = srvSocket.acceptConnection ();
        RunningThreads++;
        pthread_mutex_unlock( &ServerMutex );
#endif

        pContext->counter = 0;
        pContext->running = true;
#ifdef DEBUG

        cout << "accept: " << __ThreadNumber << ":" << RunningThreads << endl;
        cout << "on " << pContext->id << endl;
#endif

        if ( pContext->socket != SOCKET_ERROR )
        {
#ifdef DEBUG
            cout << "open" << endl;
#endif

            try
            {
#ifdef DEBUG
                cout << "sending welcome" << endl;
#endif
                // Envoi du message de bienvenue
                srvSocket.write( pContext->socket, WELCOME_MSG"\r\n", strlen( WELCOME_MSG ) + 2, 0 );
#ifdef DEBUG

                cout << "waiting for query" << endl;
#endif
                // Attente de la requ�te
                srvSocket.read( pContext->socket, __Buffer, BUFFER_SIZE, CLIENT_TIMEOUT );

                // Utilisation des objets Synthese et appel du calculateur
                synthese::server::Request query (__Buffer);

		// MJ : we set the HTML filter here and we do not use any std::stringHTML anymore. 
		// The data is kept in its original format as long as possible
		std::stringstream htmlResult;

		boost::iostreams::filtering_ostream pCtxt;
		pCtxt.push (synthese::util::HtmlFilter());
		pCtxt.push (htmlResult);

#ifdef DEBUG

                cout << "query: " << __Buffer << endl;
#endif

                Synthese.ExecuteRequete( pCtxt, pCtxt, query, ( long ) ( pContext->id ) );
#ifdef DEBUG

                cout << "sending result" << endl;
#endif
                // Envoi de la r�ponse
                srvSocket.write( pContext->socket, htmlResult.str().c_str(), htmlResult.str().size(), 0 );
                /** TODO: ATTENTION
                  ajouter une s�quence de terminaison
                  et n'envoyer que cela si timeout calculateur
                 */
            }
            catch ( const char * err )
            {
#ifdef DEBUG
                cout << "Erreur Socket: " << err << endl;
#endif

            }
            srvSocket.close( pContext->socket );
#ifdef DEBUG

            cout << "close" << endl;
#endif

        }
        // fin d'utilisation
#ifdef UNIX
        pthread_mutex_lock( &ServerMutex );
        RunningThreads--;
        pthread_mutex_unlock( &ServerMutex );
#endif

    }
    return NULL;
}

/** \brief suppervise les threads et d�truit les probl�mes
    \author Christophe Romain
    \date 2005
*/
void timer( int sig )
{
#ifdef DEBUG
    cout << "---" << endl;
#endif

    for ( int t = 0; t < NumberOfThreads; t++ )
    {
        thread_context_t *pContext = &ThreadsContext[ t ];
#ifdef DEBUG

        cout << pContext->id << ":" << pContext->counter << ":" << pContext->running << endl;
#endif

        if ( ++( pContext->counter ) >= THREAD_TIMEOUT )
        {
            // R�initialisation des compteur
            pContext->counter = 0;
            // Terminaison des threads en d�faut
            if ( pContext->running )
            {
#ifdef DEBUG
                cout << "kill " << pContext->id << endl;
#endif
                /** Un thread est consid�r� en d�faut lorsque son �tat
                  est en cours de calcul et lorsque son d�lai d'execution
                  d�passe THREAD_TIMEOUT.
                  La socket est alors ferm�e (note: on peut envoyer un message ?)
                  Le thread est d�truit
                  Un nouveau thread est cr�� pour le remplacer
                  */
                srvSocket.close( pContext->socket );
#ifdef UNIX

                Synthese.TermineCalculateur( ( long ) ( pContext->id ) );
                pthread_kill( pContext->id, SIGUSR1 );
                pthread_create( &( pContext->id ), &ThreadsAttr, ServerThread, ( void* ) t );
#endif

            }

        }
    }
#ifdef UNIX
    // On relance la sentinelle
    alarm( THREAD_PROBE );
#endif
}

/*!    \brief Usage de la commande
  \param __Nom Nom de l'�x�cutable
  \author Hugues Romain
  \date 2003-2005
  */
void AfficheUsage( const std::string& __Nom )
{
    cout << "Usage: " << __Nom << endl;
    cout << " -l [all|debug|info|warning|error|none]   niveau de log" << endl;
    cout << " -d chemin                                repertoire d'ecriture des fichiers de log" << endl;
    cout << " -c [0-n]            nombre maximal calculateurs par environnement" << endl;
    cout << " -t [1-1024]         nombre de connexions simultan�es g�r�es" << endl;
    cout << " -p port_num         �coute sur le port sp�cifi�" << endl;
    cout << " -b database         path vers le r�pertoire base de donn�e" << endl;
    cout << " -a dict             path vers le dictionnaire de l'associateur" << endl;
    exit( EXIT_FAILURE );
}

#ifdef UNIX
int main( int argc, char **argv )
#endif
#ifdef WIN32
int _tmain( int argc, TCHAR* argv[], TCHAR* envp[] )
#endif
{
#ifdef WIN32
    int nRetCode = 0;
    // Initialise MFC et affiche un message d'erreur en cas d'�chec
    if ( !AfxWinInit( ::GetModuleHandle( NULL ), NULL, ::GetCommandLine(), 0 ) )
    {
        // TODO : modifiez le code d'erreur selon les besoins
        _tprintf( _T( "Erreur irr�cup�rable�: l'initialisation MFC a �chou�\n" ) );
        nRetCode = 1;
    }
    else
    {
#endif
        // Test marc
        // testWithPlentyOfLines ();
        // fin test marc

        // D�clarations
        int i;
        int __NombreCalculateursParEnvironnement = NOMBRE_CALCULATEURS_PAR_ENVIRONNEMENT_DEFAUT;
        int __PortServeur = DEF_PORT;
        std::string database, associator;

        //recuperation des arguments
        for ( i = 1; i < argc; i++ )
        {
            if ( argv[ i ][ 0 ] == '-' )
            {
                switch ( argv[ i ][ 1 ] )
                {
                    case 'l':     // Log niveau
                        if ( ++i >= argc )
                            AfficheUsage( argv[ 0 ] );
                        if ( !strncmp( argv[ i ], "all", 3 ) )
                            Synthese.SetNiveauLog( LogAll );
                        if ( !strncmp( argv[ i ], "debug", 5 ) )
                            Synthese.SetNiveauLog( LogDebug );
                        if ( !strncmp( argv[ i ], "info", 4 ) )
                            Synthese.SetNiveauLog( LogInfo );
                        if ( !strncmp( argv[ i ], "warning", 7 ) )
                            Synthese.SetNiveauLog( LogWarning );
                        if ( !strncmp( argv[ i ], "error", 5 ) )
                            Synthese.SetNiveauLog( LogError );
                        if ( !strncmp( argv[ i ], "none", 4 ) )
                            Synthese.SetNiveauLog( LogNone );
                        break;
                    case 'd':     // Log chemin
                        if ( ++i >= argc )
                            AfficheUsage( argv[ 0 ] );
                        Synthese.SetCheminLog( argv[ i ] );
                        break;
                    case 'p':     // Ports
                        if ( ++i >= argc )
                            AfficheUsage( argv[ 0 ] );
                        __PortServeur = atoi( argv[ i ] );
                        if ( __PortServeur < 0 )
                            __PortServeur = DEF_PORT;
                        break;
                    case 'b':     // Database
                        if ( ++i >= argc )
                            AfficheUsage( argv[ 0 ] );
                        database = std::string( argv[ i ] );
                        break;
                    case 'a':     // Associator
                        if ( ++i >= argc )
                            AfficheUsage( argv[ 0 ] );
                        associator = std::string( argv[ i ] );
                        break;
                    case 'c':     // Calculateurs
                        if ( ++i >= argc )
                            AfficheUsage( argv[ 0 ] );
                        __NombreCalculateursParEnvironnement = atoi( argv[ i ] );
                        /* attention, un nombre trop grand ici fait cracher ! */
                        /* HUGUES -> voir si pas obsolete et fixer limite */
                        if ( __NombreCalculateursParEnvironnement < 0 )
                            __NombreCalculateursParEnvironnement = NOMBRE_CALCULATEURS_PAR_ENVIRONNEMENT_DEFAUT;
                        break;
                    case 't':     // Threads
                        if ( ++i >= argc )
                            AfficheUsage( argv[ 0 ] );
                        NumberOfThreads = atoi( argv[ i ] );
                        if ( ( NumberOfThreads < 1 ) || ( NumberOfThreads > 1024 ) )
                            NumberOfThreads = NB_MAX_CLIENT;
                        break;
                    default:
                        if ( ++i >= argc )
                            AfficheUsage( argv[ 0 ] );
                }
            }
        }

        /* Verification du renseignement des variables */
        if ( !database.size () || !associator.size () )
            AfficheUsage( argv[ 0 ] );

        /* Chargement de Synthese */
//        if ( !Synthese.Charge( database ) )  // MJ replace this by load SQLlite
//            return EXIT_FAILURE;

        /* Chargement de Synthese */
        //        if(!Synthese.InitAssociateur(associator))
        //          return EXIT_FAILURE;

        cout << "Chargement termine." << endl;

        try
        {
            // Initialisation de la couche r�seau en mode serveur
            srvSocket.open( DEF_SERV, __PortServeur, DEF_PROTO );
            srvSocket.connectToServer();
        }
        catch ( const char * err )
        {
            cout << "Erreur socket: " << err << endl;
            exit( 1 );
        }

        cout << "Serveur pret." << endl;

#ifdef WIN32
        //AfxBeginThread(ServerThread, NULL);
        ServerThread( ( void* ) 0 );
#endif
#ifdef UNIX
        // Cr�ation des threads
        pthread_attr_init( &ThreadsAttr );
        //pthread_attr_setdetachstate(&ThreadsAttr, PTHREAD_CREATE_DETACHED);
        //pthread_attr_setstacksize(&ThreadsAttr, 1048576);
        thread_context_t *pContext;
        pContext = ThreadsContext;
        for ( int t = 0; t < NumberOfThreads; t++, pContext++ )
            pthread_create( &( pContext->id ), &ThreadsAttr, ServerThread, ( void* ) t );

        // D�finition des gestionnaires de signaux
        // TERM et KILL stoppe le serveur
        // ALRM active la sentinelle
        signal( SIGINT, ServerExit );
        signal( SIGKILL, ServerExit );
        signal( SIGTERM, ServerExit );
        signal( SIGALRM, timer );

        // On lance la sentinelle
        alarm( THREAD_PROBE );

        // Boucle infini
        while ( 1 )
        {
            pContext = ThreadsContext;
            for ( int t = 0; t < NumberOfThreads; t++, pContext++ )
                pthread_join( pContext->id, NULL );
            cout << "Probl�mes de calculateur ??" << endl;
        }
#endif

        return EXIT_SUCCESS;
#ifdef WIN32

    }
    return nRetCode;
#endif
}

/** @} */

