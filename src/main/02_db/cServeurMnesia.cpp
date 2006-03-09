/*
  Mnesia<->C++ Wrapper
  Christophe Romain <chris@erlang-fr.org>
  2005
*/

#include "cServeurMnesia.h"
#include <stdlib.h>
#include <iostream>


// #define TESTING // juste le temps de debuger


unsigned short cServeurMnesia::_NodeId = 1;

cServeurMnesia::cServeurMnesia( const std::string& __Node, const std::string& __Cookie )
{
    _ErlFD = -1;
    erl_init( NULL, 0 );
    if ( erl_connect_init( _NodeId, __Cookie.Texte(), 1 ) != -1 )
    {
        _ErlFD = erl_connect( __Node.Texte() );
        if ( _ErlFD > 0 )
            _NodeId++;
        else
            cerr << "can not connect to " << __Node.Texte() << endl;
    }
}

cServeurMnesia::~cServeurMnesia()
{
    if ( _ErlFD > 0 )
        erl_close_connection( _ErlFD );
    _ErlFD = -1;
}

int cServeurMnesia::Select( const std::string& __Table, const cTableauDynamique<std::string>& __Champs, const std::string& __Where, const cTableauDynamique<std::string>& __Ordre, int __Limite, cResultatBaseDeDonnees& __Resultat )
{
    std::string __Args;
    ETERM *__Eres;
    int __Count = 0;
    __Args << "[" << __Table << ",[";
    for ( int i = 0; i < __Champs.size (); i++ )
    {
        if ( i > 0 )
            __Args << ",";
        __Args << __Champs[ i ];
    }
    __Args << "],";
    __Args << _where2erl( __Where );
    __Args << "]";
    __Eres = _call( "rcsdb", "select", __Args );
    if ( __Eres )
    {
        ETERM * __List;
        __Resultat.Vide();
        for ( __List = __Eres; ! ERL_IS_EMPTY_LIST( __List ); __List = ERL_CONS_TAIL( __List ) )
        {
            ETERM * __Tuple = ERL_CONS_HEAD( __List );
            __Resultat.AddEnregistrement();
            if ( ERL_IS_TUPLE( __Tuple ) )
            {
                for ( int __Position = 0; __Position < ERL_TUPLE_SIZE( __Tuple ); __Position++ )
                {
                    ETERM *__Value = erl_element( __Position, __Tuple );
                    /*
                    cResultatBaseDeDonnees_Cellule_Nombre __Num;
                    cResultatBaseDeDonnees_Cellule_Texte __Txt;
                    // nom du champ
                    //__Resultat.SetPositionChamp("label", __Position);
                    // champ entier
                    if (ERL_IS_INTEGER(__Value))
                        __Num = ERL_INT_VALUE(__Value);
                    if (ERL_IS_UNSIGNED_INTEGER(__Value))
                        __Num = ERL_INT_UVALUE(__Value);
                    // champ flotant
                    if (ERL_IS_FLOAT(__Value))
                        __Num = ERL_FLOAT_VALUE(__Value);
                    // chaine
                    if (ERL_IS_ATOM(__Value))
                        __Txt = ERL_ATOM_PTR(__Value);
                    if (ERL_IS_LIST(__Value))
                        __Txt = erl_iolist_to_string(__Value);
                    // enregistrement du résultat
                    //__Resultat.SetValeur(__Position,__Cellue);
                    */
                    erl_free_term( __Value );
                }
            }
        }
        erl_free_term( __Eres );
    }
    return __Count;
}

bool cServeurMnesia::Insert( const std::string& __Table, const std::string& __Valeurs, bool __Replace = true )
{
    std::string __Args;
    ETERM *__Eres;
    bool __IsOK = false;
    __Args << "[" << __Table;
    __Args << _value2erl( __Valeurs );
    __Args << "]";
    __Eres = _call( "rcsdb", "insert", __Args );
    if ( __Eres )
    {
        if ( strncmp( "ok", ( const char* ) ERL_ATOM_PTR( __Eres ), 2 ) == 0 )
            __IsOK = true;
        erl_free_term( __Eres );
    }
    return __IsOK;
}

int cServeurMnesia::Update( const std::string& __Table, const cTableauDynamique<std::string>& __Champs, const std::string& __Valeurs, const std::string& __Where, int __Limite = INCONNU )
{
    std::string __Args;
    ETERM *__Eres;
    int __Count = -1;
    __Args << "[" << __Table << ",";
    __Args << _value2erl( __Valeurs );
    __Args << _where2erl( __Where );
    __Args << "]";
    __Eres = _call( "rcsdb", "update", __Args );
    if ( __Eres )
    {
        __Count = ERL_INT_VALUE( __Eres );
        erl_free_term( __Eres );
    }
    return __Count;
}

bool cServeurMnesia::Delete( const std::string& __Table, const std::string& __Where, const cTableauDynamique<std::string>& __Ordre, int __Limite = INCONNU )
{
    std::string __Args;
    ETERM *__Eres;
    bool __IsOK = false;
    __Args << "[" << __Table << ",";
    __Args << _where2erl( __Where );
    __Args << "]";
    __Eres = _call( "rcsdb", "delete", __Args );
    if ( __Eres )
    {
        if ( strncmp( "ok", ( const char* ) ERL_ATOM_PTR( __Eres ), 2 ) == 0 )
            __IsOK = true;
        erl_free_term( __Eres );
    }
    return __IsOK;
}

int cServeurMnesia::Count( const std::string& __Table, const std::string& __Where )
{
    std::string __Args;
    int __Resultat = -1;
    ETERM *__Eres;
    __Args << "[" << __Table;
    __Args << "," << _where2erl( __Where ) << "]";
    __Eres = _call( "rcsdb", "count", __Args );
    if ( __Eres )
    {
        __Resultat = ERL_INT_VALUE( __Eres );
        erl_free_term( __Eres );
    }
    return __Resultat;
}

int cServeurMnesia::Max( const std::string& __Table, const std::string& __Champ, const std::string& __Where )
{
    std::string __Args;
    int __Resultat = -1;
    ETERM *__Eres;
    __Args << "[" << __Table;
    __Args << "," << __Champ;
    __Args << "," << _where2erl( __Where ) << "]";
    __Eres = _call( "rcsdb", "max", __Args );
    if ( __Eres )
    {
        __Resultat = ERL_INT_VALUE( __Eres );
        erl_free_term( __Eres );
    }
    return __Resultat;
}

int cServeurMnesia::NextID( const std::string& __Table )
{
    std::string __Args;
    int __Resultat = -1;
    ETERM *__Eres;
    __Args << "[" << __Table << "]";
    __Eres = _call( "rcsdb", "nextid", __Args );
    if ( __Eres )
    {
        __Resultat = ERL_INT_VALUE( __Eres );
        erl_free_term( __Eres );
    }
    return __Resultat;
}

// fonction obsolete, laissé au cas ou
void cServeurMnesia::SQL( const std::stringSQL& __RequeteSQL )
{
    ETERM * __Eres;
    __Eres = _call( "rcsdb", "sql", __RequeteSQL.Texte() );
    erl_free_term( __Eres );
}

// fonction temporaire pour format where
// il faut des espaces partout
// il faut ecrire en minuscule
// les chaines doivent être avec " et non '
// seul AND est géré
std::string cServeurMnesia::_where2erl( const std::string& __Where )
{
    // in:  WHERE nom = "romain" AND id < 3
    // out: [{nom,'==',"romain"},{id,'<',3}]
    std::string __Final;
    char buffer[ 1024 ];
    char *ptr, *last;
    int cont = 1;
    __Final << "[";
    strncpy( buffer, __Where.Texte(), 1024 );
    ptr = buffer;
    while ( cont )
    {
        while ( *ptr == ' ' )
            ptr++;
        while ( *ptr != ' ' )
            ptr++;
        __Final << "{";
        while ( *ptr == ' ' )
            ptr++;
        last = ptr;
        while ( *ptr != ' ' )
            ptr++;
        *ptr = 0;
        ptr++;
        __Final << std::string( last );
        while ( *ptr == ' ' )
            ptr++;
        last = ptr;
        while ( *ptr != ' ' )
            ptr++;
        *ptr = 0;
        ptr++;
        if ( *last == '=' )
            __Final << ",'==',";
        else
            __Final << ",'" << std::string( last ) << "',";
        while ( *ptr == ' ' )
            ptr++;
        last = ptr;
        while ( *ptr != ' ' && *ptr )
            ptr++;
        cont = *ptr;
        *ptr = 0;
        ptr++;
        __Final << std::string( last ) << "}";
        if ( cont )
            __Final << ",";
    }
    __Final << "]";
    return __Final;
}

// fonction temporaire pour format value
// il faut des espaces partout
// il faut ecrire en minuscule
// les chaines doivent être avec " et non '
// seul AND est géré
std::string cServeurMnesia::_value2erl( const std::string& __Where )
{
    // in:  nom = "toto" AND prenom = "titi"
    // out: [{nom,"toto"},{prenom,"titi"}]
    std::string __Final;
    char buffer[ 1024 ];
    char *ptr, *last;
    int cont = 1;
    __Final << "[";
    strncpy( buffer, __Where.Texte(), 1024 );
    ptr = buffer;
    while ( cont )
    {
        while ( *ptr == ' ' )
            ptr++;
        last = ptr;
        while ( *ptr != ' ' )
            ptr++;
        *ptr = 0;
        ptr++;
        __Final << "{" << std::string( last );
        while ( *ptr == ' ' || *ptr == '=' )
            ptr++;
        last = ptr;
        while ( *ptr != ' ' )
            ptr++;
        *ptr = 0;
        ptr++;
        __Final << "," << std::string( last ) << "}";
        while ( *ptr == ' ' )
            ptr++;
        last = ptr; // contiendra AND ou rien
        while ( *ptr != ' ' && *ptr )
            ptr++;
        cont = *ptr;
        *ptr = 0;
        ptr++;
        if ( cont )
            __Final << ",";
    }
    __Final << "]";
    return __Final;
}


ETERM *cServeurMnesia::_call( const std::string& __Mod, const std::string& __Fun, const std::string& __Args )
{
    ETERM * __Eres, *__Earg;
    __Eres = NULL;
    if ( _ErlFD > 0 )
    {
#ifdef TESTING
        cout << "> " << __Mod << ":" << __Fun << __Args << endl;
#endif

        __Earg = erl_format( __Args.Texte() );
        __Eres = erl_rpc( _ErlFD, __Mod.Texte(), __Fun.Texte(), __Earg );
        erl_free_term( __Earg );
    }
    return __Eres;
}

void cServeurMnesia::_send( const std::string& dst, const std::string& msg )
{
    /*
    ETERM *edst,*emsg;
    emsg = erl_format(msg);
    edst = erl_format(dst);
    erl_send(_ErlFD,edst,emsg);
    erl_free_term(edst);
    erl_free_term(emsg);
    */
}

void cServeurMnesia::_recv()
{
    /*
    unsigned char buf[BUFSIZE];
    ErlMessage emsg;
    ETERM *fromp,*tuplep,*fnp,*argp,*resp;
    got = erl_receive_msg(fd,buf,BUFSIZE,&emsg);
    if (got == ERL_TICK) {
        fprintf(stderr,"got erl tick\n");
    } else if (got == ERL_ERROR) {
        fprintf(stderr,"got erl error\n");
        loop = 0;
    } else {
        if (emsg.type == ERL_REG_SEND) {
            fromp = erl_element(2,emsg.msg);
            tuplep = erl_element(3,emsg.msg);
            fnp = erl_element(1,tuplep);
            argp = erl_element(2,tuplep);

            if (strncmp(ERL_ATOM_PTR(fnp),"foo",3) == 0) {
                res = foo(ERL_INT_VALUE(argp));
            } else if (strncmp(ERL_ATOM_PTR(fnp),"bar",3) == 0) {
                res = bar(ERL_INT_VALUE(argp));
            } else if (strncmp(ERL_ATOM_PTR(fnp),"quit",3) == 0) {
                res = 0;
                fprintf(stderr,"got quit message\n");
                loop = 0;
            }

    resp = erl_format("{erlnode,~i}",res);
    erl_send(fd,fromp,resp);

            erl_free_term(emsg.from); erl_free_term(emsg.msg);
            erl_free_term(fromp); erl_free_term(tuplep);
            erl_free_term(fnp); erl_free_term(argp);
            erl_free_term(resp);
        }
    }
    */
}


#ifdef TESTING
int main( int argc, char **argv )
{
    cServeurMnesia __srv( "db0@iBook", "rcs" );
    cResultatBaseDeDonnees __Resultat;

    cout << __srv._value2erl( " nom = \"toto\" AND prenom = \"titi\"" );
    std::string __Table( "personnes" );
    std::string __Where1( "where nom = \"romain\"" );
    std::string __Where2( "where nom = \"romain\" and id < 5" );
    std::string __Champ1( "nom" );
    std::string __Champ2( "prenom" );
    cTableauDynamiqueObjets<std::string> __Champs;
    __Champs += __Champ1;
    __Champs += __Champ2;

    __srv.Select( __Table, __Champs, __Where1, NULL, 0, __Resultat );
    cout << __srv.Max( __Table, std::string( "id" ), __Where1 ) << endl;
    cout << __srv.Count( __Table, __Where1 ) << endl;
    cout << __srv.Count( __Table, __Where2 ) << endl;
}
#endif

