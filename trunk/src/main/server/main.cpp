/** @file main.cpp
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
#include "tcp/cSocket.h"
#include "Parametres.h"
#include "cTexteRequeteSYNTHESE.h"


/*

#include "map/Geometry.h"
#include "map/DBLComparator.h"
#include "map/Map.h"
#include "map/MapBackground.h"
#include "map/XYPoint.h"
#include "map/Vertex.h"
#include "map/Edge.h"
#include "map/Itinerary.h"
#include "map/PhysicalStop.h"
#include "map/MapBackgroundManager.h"
#include "map/Rectangle.h"
#include "map/DrawableBusLine.h"
#include "map/PostscriptCanvas.h"
#include "map/Topography.h"

using namespace synmap;


void testWithPlentyOfLines () {

	Topography topo;
    int iti_key = 0;
    
	const Vertex* A = topo.newVertex (16.0, 22.0);
	const Vertex* B = topo.newVertex (16.0, 19.0);
	const Vertex* C = topo.newVertex (16.0, 16.0);
	const Vertex* D = topo.newVertex (16.0, 12.0);
	const Vertex* E = topo.newVertex (16.0, 9.0);
	const Vertex* F = topo.newVertex (13.0, 6.0);
	const Vertex* G = topo.newVertex (10.0, 3.0);
	const Vertex* H = topo.newVertex (15.0, 3.0);
	const Vertex* I = topo.newVertex (19.0, 14.0);
	const Vertex* J = topo.newVertex (24.0, 14.0);
	const Vertex* K = topo.newVertex (27.0, 17.0);
	const Vertex* L = topo.newVertex (23.0, 17.0);
	const Vertex* M = topo.newVertex (22.0, 11.0);


	std::vector<const Vertex*> vertices;
	std::vector<bool> pstops;

	vertices.push_back (A); pstops.push_back (false);
	vertices.push_back (B); pstops.push_back (false);
	vertices.push_back (C); pstops.push_back (false);
	vertices.push_back (D); pstops.push_back (false);
	vertices.push_back (E); pstops.push_back (false);
	vertices.push_back (F); pstops.push_back (false);
	vertices.push_back (G); pstops.push_back (false);
	
	Itinerary* iti1 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti1->setProperty("name", "bus1");
	iti1->setProperty("lineNumber", "1");
	iti1->setProperty("color", "red");
	
	
	vertices.clear ();
	pstops.clear ();
	vertices.push_back (A); pstops.push_back (false);
	vertices.push_back (B); pstops.push_back (false);
	vertices.push_back (C); pstops.push_back (false);
	vertices.push_back (D); pstops.push_back (false);
	vertices.push_back (E); pstops.push_back (false);
	vertices.push_back (F); pstops.push_back (false);
	vertices.push_back (H); pstops.push_back (false);

	Itinerary* iti2 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti2->setProperty("name", "bus2");
	iti2->setProperty("lineNumber", "2");
	iti2->setProperty("color", "blue");

	vertices.clear ();
	pstops.clear ();
	vertices.push_back (A); pstops.push_back (false);
	vertices.push_back (B); pstops.push_back (false);
	vertices.push_back (C); pstops.push_back (false);
	vertices.push_back (I); pstops.push_back (false);
	vertices.push_back (J); pstops.push_back (false);
	vertices.push_back (K); pstops.push_back (false);
	vertices.push_back (L); pstops.push_back (false);

	Itinerary* iti3 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti3->setProperty("name", "bus3");
	iti3->setProperty("lineNumber", "3");
	iti3->setProperty("color", "green");
	
	
	vertices.clear ();
	pstops.clear ();
	vertices.push_back (A); pstops.push_back (false);
	vertices.push_back (B); pstops.push_back (false);
	vertices.push_back (C); pstops.push_back (false);
	vertices.push_back (I); pstops.push_back (false);
	vertices.push_back (M); pstops.push_back (false);

	Itinerary* iti4 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti4->setProperty("name", "bus4");
	iti4->setProperty("lineNumber", "4");
	iti4->setProperty("color", "magenta");
	
	vertices.clear ();
	pstops.clear ();
	vertices.push_back (topo.newVertex (13.0, 25.0)); pstops.push_back (false);
	vertices.push_back (A); pstops.push_back (false);
	vertices.push_back (B); pstops.push_back (false);
	vertices.push_back (topo.newVertex (13.0, 18.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (10.0, 16.0)); pstops.push_back (false);

	Itinerary* iti5 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti5->setProperty("name", "bus5");
	iti5->setProperty("lineNumber", "5");
	iti5->setProperty("color", "cyan");

	vertices.clear ();
	pstops.clear ();
	vertices.push_back (A); pstops.push_back (false);
	vertices.push_back (B); pstops.push_back (false);
	vertices.push_back (C); pstops.push_back (false);
	vertices.push_back (D); pstops.push_back (false);
	vertices.push_back (topo.newVertex (13.0, 10.0)); pstops.push_back (false);

	Itinerary* iti6 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti6->setProperty("name", "bus6");
	iti6->setProperty("lineNumber", "6");
	iti6->setProperty("color", "yellow");

	vertices.clear ();
	pstops.clear ();
	vertices.push_back (topo.newVertex (21.0, 26.0)); pstops.push_back (false);
	vertices.push_back (A); pstops.push_back (false);
	vertices.push_back (B); pstops.push_back (false);
	vertices.push_back (C); pstops.push_back (false);
	vertices.push_back (topo.newVertex (21.0, 19.0)); pstops.push_back (false);

	Itinerary* iti7 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti7->setProperty("name", "bus7");
	iti7->setProperty("lineNumber", "7");
	iti7->setProperty("color", "magenta");
	
	// Second set of lines
	// Second set of lines
	// Second set of lines
	// Second set of lines
	
	vertices.clear ();
	pstops.clear ();
	vertices.push_back (topo.newVertex (5.0, 11.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (5.0, 8.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (3.0, 6.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (3.0, 3.0)); pstops.push_back (false);

	Itinerary* iti8 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti8->setProperty("name", "bus8");
	iti8->setProperty("lineNumber", "8");
	iti8->setProperty("color", "red");
	
	vertices.clear ();
	pstops.clear ();
	vertices.push_back (topo.newVertex (7.0, 10.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (7.0, 12.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (7.0, 14.0)); pstops.push_back (false); 
	vertices.push_back (topo.newVertex (14.0, 16.0)); pstops.push_back (false);
	vertices.push_back (C); pstops.push_back (false);
	vertices.push_back (D); pstops.push_back (false);
	vertices.push_back (topo.newVertex (7.0, 12.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (7.0, 10.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (5.0, 8.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (3.0, 6.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (3.0, 3.0)); pstops.push_back (false);

	Itinerary* iti9 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti9->setProperty("name", "bus9");
	iti9->setProperty("lineNumber", "9");
	iti9->setProperty("color", "green");


	vertices.clear ();
	pstops.clear ();
	vertices.push_back (topo.newVertex (21.0, 26.0)); pstops.push_back (false);
	vertices.push_back (A); pstops.push_back (false);
	vertices.push_back (B); pstops.push_back (false);
	vertices.push_back (topo.newVertex (25.0, 26.0)); pstops.push_back (false); 

	Itinerary* iti10 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti10->setProperty("name", "bus10");
	iti10->setProperty("lineNumber", "10");
	iti10->setProperty("color", "yellow");

	vertices.clear ();
	pstops.clear ();
	vertices.push_back (M); pstops.push_back (false);
	vertices.push_back (I); pstops.push_back (false);
	vertices.push_back (J); pstops.push_back (false);
	vertices.push_back (K); pstops.push_back (false);
	vertices.push_back (topo.newVertex (27.0, 25.0)); pstops.push_back (false); 
	vertices.push_back (topo.newVertex (24.0, 25.0)); pstops.push_back (false); 
	
	Itinerary* iti11 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti11->setProperty("name", "bus11");
	iti11->setProperty("lineNumber", "11");
	iti11->setProperty("color", "blue");

	vertices.clear ();
	pstops.clear ();   // Reverse of 8
	vertices.push_back (topo.newVertex (3.0, 3.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (3.0, 6.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (5.0, 8.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (5.0, 11.0)); pstops.push_back (false);

	Itinerary* iti12 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti12->setProperty("name", "bus12");
	iti12->setProperty("lineNumber", "12");
	iti12->setProperty("color", "blue");
	
	vertices.clear ();
	pstops.clear ();

	vertices.push_back (M); pstops.push_back (false);
	vertices.push_back (I); pstops.push_back (false);
	vertices.push_back (J); pstops.push_back (false);
	vertices.push_back (K); pstops.push_back (false);
	vertices.push_back (topo.newVertex (27.0, 25.0)); pstops.push_back (false); 
	vertices.push_back (topo.newVertex (24.0, 25.0)); pstops.push_back (false); 
	
	Itinerary* iti13 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti13->setProperty("name", "bus13");
	iti13->setProperty("lineNumber", "13");
	iti13->setProperty("color", "red");

	vertices.clear ();
	pstops.clear ();
	vertices.push_back (topo.newVertex (10.0, 16.0)); pstops.push_back (false);
	vertices.push_back (topo.newVertex (13.0, 18.0)); pstops.push_back (false);
	vertices.push_back (B); pstops.push_back (false);
	vertices.push_back (A); pstops.push_back (false);
	vertices.push_back (topo.newVertex (13.0, 25.0)); pstops.push_back (false);



	Itinerary* iti14 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti14->setProperty("name", "bus14");
	iti14->setProperty("lineNumber", "14");  // reverse of the 5
	iti14->setProperty("color", "cyan");
	
	
	vertices.clear ();
	pstops.clear ();
	vertices.push_back (A); pstops.push_back (false);
	vertices.push_back (B); pstops.push_back (false);
	vertices.push_back (C); pstops.push_back (false);
	vertices.push_back (D); pstops.push_back (false);
	vertices.push_back (E); pstops.push_back (false);
	vertices.push_back (F); pstops.push_back (false);
	vertices.push_back (H); pstops.push_back (false);


	Itinerary* iti15 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti15->setProperty("name", "bus15");  // Same than line 2
	iti15->setProperty("lineNumber", "15");
	iti15->setProperty("color", "green");
	
	vertices.clear ();
	pstops.clear ();

	vertices.push_back (topo.newVertex (25.0, 26.0)); pstops.push_back (false); 
	vertices.push_back (B); pstops.push_back (false);
	vertices.push_back (A); pstops.push_back (false);
	vertices.push_back (topo.newVertex (21.0, 26.0)); pstops.push_back (false);

	Itinerary* iti16 = (Itinerary*) topo.newItinerary(iti_key++, vertices, pstops);
	iti16->setProperty("name", "bus16");  // reverse 10
	iti16->setProperty("lineNumber", "16");
	iti16->setProperty("color", "black");

	std::ofstream of ("/home/mjambert/map.ps");
	Map map (&topo, of, synmap::Rectangle (0.0, 0.0, 35.0, 35.0), 1000, 1000);
	
	map.dump();
}	
*/


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

//! @name Compatibilit� UNIX<>Win32
#ifdef UNIX
#define SOCKET int
#define closesocket(s) close(s)
#endif

// Instance de Synthese
SYNTHESE Synthese;

// Instance de la couche r�seau
cSocket Socket;

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
} thread_context_t;
thread_context_t ThreadsContext[NB_MAX_CLIENT];
int NumberOfThreads = NB_MAX_CLIENT;
int RunningThreads = 0;


#ifdef UNIX
/** \brief Arret d'un calculateur
    \author Christophe Romain
    \date 2005
    */
void ThreadExit(int sig)
{
    //pthread_t self = pthread_self();
    pthread_exit(0);
}

/** \brief Arret totale de Synthese
    \author Christophe Romain
    \date 2005
    */
void ServerExit(int sig)
{
    thread_context_t *pContext = ThreadsContext;
    for(int t=0; t<NumberOfThreads; t++)
        Socket.Close(pContext->socket);
    cout << "Terminaison du serveur." << endl;
    exit(0);
}
#endif

/** \brief Prise en compte d'une requ�te client au sein d'un thread d�di�
    \author Christophe Romain
    \date 2005
*/
void *ServerThread(void *args)
{
    char __Buffer[BUFFER_SIZE];
	// Le num�ro de thread est pass� en argument
    int __ThreadNumber = (int)args;
	// R�cup�ration de contexte de ce thread
    thread_context_t *pContext = &ThreadsContext[__ThreadNumber];

#ifdef WIN32
	// La version Win32 n'est pas multi-thread
	pContext->id = 0;
#endif
#ifdef UNIX
    // D�finition du gestionnaire de signaux. USR1 provoque la terminaison.
    signal(SIGUSR1, ThreadExit);
#endif
    
    while(1)
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
        pContext->socket = Socket.Accept();
        RunningThreads++;
        pthread_mutex_unlock( &ServerMutex );
#endif
        pContext->counter = 0;
        pContext->running = true;
#ifdef DEBUG
        cout << "accept: " << __ThreadNumber << ":" << RunningThreads << endl;
        cout << "on " << pContext->id << endl;
#endif
        if(pContext->socket != SOCKET_ERROR)
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
                Socket.Write(pContext->socket, WELCOME_MSG"\r\n", strlen(WELCOME_MSG)+2, 0);
#ifdef DEBUG
                cout << "waiting for query" << endl;
#endif
                // Attente de la requ�te
                Socket.Read(pContext->socket, __Buffer, BUFFER_SIZE, CLIENT_TIMEOUT);
                
                // Utilisation des objets Synthese et appel du calculateur
                cTexteRequeteSYNTHESE query;
                stringstream pCtxt;
                query << __Buffer;
#ifdef DEBUG
                cout << "query: " << __Buffer << endl;
#endif
                Synthese.ExecuteRequete(pCtxt, pCtxt, query, (long)(pContext->id));
#ifdef DEBUG
                cout << "sending result" << endl;
#endif
                // Envoi de la r�ponse
                Socket.Write(pContext->socket, pCtxt.str().data(), pCtxt.str().size(), 0);
                /** TODO: ATTENTION
                  ajouter une s�quence de terminaison
                  et n'envoyer que cela si timeout calculateur
                 */
            }
            catch (const char *err)
            {
#ifdef DEBUG
                cout << "Erreur Socket: " << err << endl;
#endif
            }
            Socket.Close(pContext->socket);
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
void timer(int sig)
{
#ifdef DEBUG
    cout << "---" << endl;
#endif
    for(int t=0; t<NumberOfThreads; t++)
    {
        thread_context_t *pContext = &ThreadsContext[t];
#ifdef DEBUG
        cout << pContext->id << ":" << pContext->counter << ":" << pContext->running << endl;
#endif
        if(++(pContext->counter) >= THREAD_TIMEOUT)
        {
            // R�initialisation des compteur
            pContext->counter = 0;
            // Terminaison des threads en d�faut
            if(pContext->running)
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
                Socket.Close(pContext->socket);
#ifdef UNIX
                Synthese.TermineCalculateur((long)(pContext->id));
                pthread_kill(pContext->id, SIGUSR1);
                pthread_create(&(pContext->id), &ThreadsAttr, ServerThread, (void*)t);
#endif
            }

        }
    }
#ifdef UNIX
    // On relance la sentinelle
    alarm(THREAD_PROBE);
#endif
}

/*!    \brief Usage de la commande
  \param __Nom Nom de l'�x�cutable
  \author Hugues Romain
  \date 2003-2005
  */
void AfficheUsage(const cTexte& __Nom)
{
    cout << "Usage: " << __Nom << endl;
    cout << " -l [all|debug|info|warning|error|none]   niveau de log" << endl;
    cout << " -d chemin                                repertoire d'ecriture des fichiers de log" << endl;
    cout << " -c [0-n]            nombre maximal calculateurs par environnement" << endl;
    cout << " -t [1-1024]         nombre de connexions simultan�es g�r�es" << endl;
    cout << " -p port_num         �coute sur le port sp�cifi�" << endl;
    cout << " -b database         path vers le r�pertoire base de donn�e" << endl;
    cout << " -a dict             path vers le dictionnaire de l'associateur" << endl;
    exit(EXIT_FAILURE);
}

#ifdef UNIX
int main(int argc, char **argv)
#endif
#ifdef WIN32
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
#endif
{
#ifdef WIN32
    int nRetCode = 0;
    // Initialise MFC et affiche un message d'erreur en cas d'�chec
    if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
    {
        // TODO : modifiez le code d'erreur selon les besoins
        _tprintf(_T("Erreur irr�cup�rable�: l'initialisation MFC a �chou�\n"));
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
        cTexte database, associator;

        //recuperation des arguments
        for (i = 1; i < argc; i++) 
        {
            if (argv[i][0] == '-') 
            {
                switch (argv[i][1]) 
                {
                    case 'l':    // Log niveau
                        if(++i >= argc) AfficheUsage(argv[0]);
                        if(!strncmp(argv[i],"all",3)) Synthese.SetNiveauLog(LogAll);
                        if(!strncmp(argv[i],"debug",5)) Synthese.SetNiveauLog(LogDebug);
                        if(!strncmp(argv[i],"info",4)) Synthese.SetNiveauLog(LogInfo);
                        if(!strncmp(argv[i],"warning",7)) Synthese.SetNiveauLog(LogWarning);
                        if(!strncmp(argv[i],"error",5)) Synthese.SetNiveauLog(LogError);
                        if(!strncmp(argv[i],"none",4)) Synthese.SetNiveauLog(LogNone);
                        break;
                    case 'd':    // Log chemin
                        if(++i >= argc) AfficheUsage(argv[0]);
                        Synthese.SetCheminLog(argv[i]);
                        break;
                    case 'p':    // Ports
                        if(++i >= argc) AfficheUsage(argv[0]);
                        __PortServeur = atoi(argv[i]);
                        if(__PortServeur < 0)
                            __PortServeur = DEF_PORT;
                        break;
                    case 'b':    // Database
                        if(++i >= argc) AfficheUsage(argv[0]);
                        database = cTexte(argv[i]);
                        break;
                    case 'a':    // Associator
                        if(++i >= argc) AfficheUsage(argv[0]);
                        associator = cTexte(argv[i]);
                        break;
                    case 'c':    // Calculateurs
                        if(++i >= argc) AfficheUsage(argv[0]);
                        __NombreCalculateursParEnvironnement = atoi(argv[i]);
                        /* attention, un nombre trop grand ici fait cracher ! */
                        /* HUGUES -> voir si pas obsolete et fixer limite */
                        if(__NombreCalculateursParEnvironnement < 0)
                            __NombreCalculateursParEnvironnement = NOMBRE_CALCULATEURS_PAR_ENVIRONNEMENT_DEFAUT;
                        break;
                    case 't':    // Threads
                        if(++i >= argc) AfficheUsage(argv[0]);
                        NumberOfThreads = atoi(argv[i]);
                        if((NumberOfThreads < 1) || (NumberOfThreads > 1024))
                            NumberOfThreads = NB_MAX_CLIENT;
                        break;
                    default:
                        if(++i >= argc) AfficheUsage(argv[0]);
                }
            }
        }

        /* Verification du renseignement des variables */
        if(!database.Taille() || !associator.Taille())
            AfficheUsage(argv[0]);

        /* Chargement de Synthese */
        if(!Synthese.Charge(database, __NombreCalculateursParEnvironnement))
            return EXIT_FAILURE;
        
        /* Chargement de Synthese */
        if(!Synthese.InitAssociateur(associator))
            return EXIT_FAILURE;
        
        cout << "Chargement termine." << endl;

        try
        {
            // Initialisation de la couche r�seau en mode serveur
            Socket.Open(DEF_SERV, __PortServeur, DEF_PROTO);
            Socket.Server();
        }
        catch (const char *err)
        {
            cout << "Erreur socket: " << err << endl;
            exit(1);
        }
        
        cout << "Serveur pret." << endl;

#ifdef WIN32
        //AfxBeginThread(ServerThread, NULL);
        ServerThread((void*)0);
#endif
#ifdef UNIX
        // Cr�ation des threads
        pthread_attr_init(&ThreadsAttr);
        //pthread_attr_setdetachstate(&ThreadsAttr, PTHREAD_CREATE_DETACHED);
        //pthread_attr_setstacksize(&ThreadsAttr, 1048576);
        thread_context_t *pContext;
        pContext = ThreadsContext;
        for(int t=0; t<NumberOfThreads; t++, pContext++)
            pthread_create(&(pContext->id), &ThreadsAttr, ServerThread, (void*)t);

        // D�finition des gestionnaires de signaux
        // TERM et KILL stoppe le serveur
        // ALRM active la sentinelle
        signal(SIGINT, ServerExit);
        signal(SIGKILL, ServerExit);
        signal(SIGTERM, ServerExit);
        signal(SIGALRM, timer);

        // On lance la sentinelle
        alarm(THREAD_PROBE);

        // Boucle infini
        while(1)
        {
            pContext = ThreadsContext;
            for(int t=0; t<NumberOfThreads; t++, pContext++)
                pthread_join(pContext->id, NULL);
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

