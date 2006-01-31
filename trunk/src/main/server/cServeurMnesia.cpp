#include "cServeurMnesia.h"
#include <stdlib.h>
#include <iostream>


// #define DEMO // juste le temps de debuger


unsigned short cServeurMnesia::_NodeId = 1;

cServeurMnesia::cServeurMnesia(const cTexte& __Node,const cTexte& __Cookie)
{
    _ErlFD = -1;
    erl_init(NULL,0);
    if (erl_connect_init(_NodeId,__Cookie.Texte(),1) != -1)
    {
        _ErlFD = erl_connect(__Node.Texte());
        if (_ErlFD > 0) _NodeId++;
        else cerr << "can not connect to " << __Node.Texte() << endl;
    }
}

cServeurMnesia::~cServeurMnesia()
{
    if (_ErlFD > 0) erl_close_connection(_ErlFD);
    _ErlFD = -1;
}

tIndex cServeurMnesia::Select(const cTexte& __Table,const cTableauDynamique<cTexte>& __Champs,const cTexte& __Where,const cTableauDynamique<cTexte>& __Ordre,tIndex __Limite,cResultatBaseDeDonnees& __Resultat)
{
    cTexte __Args;
    ETERM *__Eres;
    tIndex __Count = 0;
    __Args << "[" << __Table << ",[";
    for(int i=0; i<__Champs.Taille(); i++)
    {
        if(i>0) __Args << ",";
        __Args << __Champs[i];
    }
    __Args << "],";
    __Args << _where2erl(__Where);
    __Args << "]";
    __Eres = _call("rcsdb","select",__Args);
    if (__Eres)
    {
        ETERM *__List,
        __Resultat.Vide();
        for (__List = __Eres; ! ERL_IS_EMPTY_LIST(__List); __List = ERL_CONS_TAIL(__List)) 
        {
            ETERM *__Tuple = ERL_CONS_HEAD(__List);
            __Resultat.AddEnregistrement();
            if (ERL_IS_TUPLE(__Tuple))
            {
                for(int __Position = 0; __Position < ERL_TUPLE_SIZE(__Tuple); __Position++)
                {
                    ETERM *__Value = erl_element(__Position, __Tuple)
                    cResultatBaseDeDonnees_Cellule __Cellule;
                    // nom du champ
                    //__Resultat.SetPositionChamp("label", __Position);
                    // champ entier
                    if (ERL_IS_INTEGER(__Value)) __Resultat.SetValeur(__Position, ERL_INT_VALUE(t));
                    if (ERL_IS_UNSIGNED_INTEGER(__Value)) __Resultat.SetValeur(__Position, ERL_INT_UVALUE(t));
                    // champ flotant
                    if (ERL_IS_FLOAT(__Value)) __Resultat.SetValeur(__Position, ERL_FLOAT_VALUE(t));
                    // chaine
                    if (ERL_IS_ATOM(__Value)) __Resultat.SetValeur(__Position, ERL_ATOM_PTR(t));
                    if (ERL_IS_LIST(__Value)) __Resultat.SetValeur(__Position, ERL_ATOM_PTR(t));
                }
            }
        }
        erl_free_term(__Eres);
    }
    return __Count;
}

bool cServeurMnesia::Insert(const cTexte& __Table,cResultatBaseDeDonnees& __Valeurs,bool __Replace=true)
{
    cTexte __Args;
    ETERM *__Eres;
    bool __IsOK = false;
    __Args << "[" << __Table;
    // A FINIR, la version SQL est OK
    // -> __Args << "," << "[{nom,\"toto\"},{prenom,\"titi\"}]";
    __Args << "]";
    __Eres = _call("rcsdb","insert",__Args);
    if (__Eres)
    {
        if (strncmp("ok",(const char*)ERL_ATOM_PTR(__Eres),2)==0) __IsOK = true;
        erl_free_term(__Eres);
    }
    return __IsOK;
}

tIndex cServeurMnesia::Update(const cTexte& __Table,const cTableauDynamique<cTexte>& __Champs,const cTableauDynamique<cTexte>& __Valeurs,const cTexte& __Where,tIndex __Limite = INCONNU) 
{
    cTexte __Args;
    ETERM *__Eres;
    tIndex __Count = -1;
    __Args << "[" << __Table << ",";
    // A FINIR, utiliser le insert SQL
    // -> __Args << "," << "[{nom,\"toto\"},{prenom,\"titi\"}]";
    __Args << _where2erl(__Where);
    __Args << "]";
    __Eres = _call("rcsdb","update",__Args);
    if (__Eres)
    {
        __Count = ERL_INT_VALUE(__Eres);
        erl_free_term(__Eres);
    }
    return __Count;
}

bool cServeurMnesia::Delete(const cTexte& __Table,const cTexte& __Where,const cTableauDynamique<cTexte>& __Ordre,tIndex __Limite = INCONNU) 
{
    cTexte __Args;
    ETERM *__Eres;
    bool __IsOK = false;
    __Args << "[" << __Table << ",";
    __Args << _where2erl(__Where);
    __Args << "]";
    __Eres = _call("rcsdb","delete",__Args);
    if (__Eres)
    {
        if (strncmp("ok",(const char*)ERL_ATOM_PTR(__Eres),2)==0) __IsOK = true;
        erl_free_term(__Eres);
    }
    return __IsOK;
}

tIndex cServeurMnesia::Count(const cTexte& __Table,const cTexte& __Where) 
{
    cTexte __Args;
    tIndex __Resultat = -1;
    ETERM *__Eres;
    __Args << "[" << __Table;
    __Args << "," << _where2erl(__Where) << "]";
    __Eres = _call("rcsdb","count",__Args);
    if (__Eres)
    {
        __Resultat = ERL_INT_VALUE(__Eres);
        erl_free_term(__Eres);
    }
    return __Resultat;
}

tIndex cServeurMnesia::Max(const cTexte& __Table,const cTexte& __Champ,const cTexte& __Where) 
{
    cTexte __Args;
    tIndex __Resultat = -1;
    ETERM *__Eres;
    __Args << "[" << __Table;
    __Args << "," << __Champ;
    __Args << "," << _where2erl(__Where) << "]";
    __Eres = _call("rcsdb","max",__Args);
    if (__Eres)
    {
        __Resultat = ERL_INT_VALUE(__Eres);
        erl_free_term(__Eres);
    }
    return __Resultat;
}

tIndex cServeurMnesia::NextID(const cTexte& __Table) 
{
    cTexte __Args;
    tIndex __Resultat = -1;
    ETERM *__Eres;
    __Args << "[" << __Table << "]";
    __Eres = _call("rcsdb","nextid",__Args);
    if (__Eres)
    {
        __Resultat = ERL_INT_VALUE(__Eres);
        erl_free_term(__Eres);
    }
    return __Resultat;
}

// fonction obsolete, laiss� au cas ou
void cServeurMnesia::SQL(const cTexteSQL& __RequeteSQL)
{
    // ETERM *__Eres;
    // __Eres = _call("rcsdb","sql",__RequeteSQL.Texte());
    //erl_free_term(__Eres);
}

// fonction temporaire pour format where
// il faut des espaces partout
// il faut ecrire en minuscule
// les chaines doivent �tre avec " et non '
// seul AND est g�r�
cTexte cServeurMnesia::_where2erl(const cTexte& __Where)
{
    // in:  WHERE nom = "romain" AND id < 3
    // out: [{nom,'==',"romain"},{id,'<',3}]
    cTexte __Final;
    char buffer[1024];
    char *ptr, *last;
    int cont = 1;
    __Final << "[";
    strncpy(buffer,__Where.Texte(),1024);
    ptr = buffer;
    while(cont)
    {
        while(*ptr == ' ') ptr++;
        while(*ptr != ' ') ptr++;
        __Final << "{";
        while(*ptr == ' ') ptr++;
        last = ptr;
        while(*ptr != ' ') ptr++;
        *ptr = 0; ptr++;
        __Final << cTexte(last);
        while(*ptr == ' ') ptr++;
        last = ptr;
        while(*ptr != ' ') ptr++;
        *ptr = 0; ptr++;
        if(*last == '=') __Final << ",'==',";
        else __Final << ",'" << cTexte(last) << "',";
        while(*ptr == ' ') ptr++;
        last = ptr;
        while(*ptr != ' ' && *ptr) ptr++;
        cont = *ptr;
        *ptr = 0; ptr++;
        __Final << cTexte(last) << "}";
        if(cont) __Final << ",";
    }
    __Final << "]";
    return __Final;
}

// fonction temporaire pour format value
// il faut des espaces partout
// il faut ecrire en minuscule
// les chaines doivent �tre avec " et non '
// seul AND est g�r�
cTexte cServeurMnesia::_value2erl(const cTexte& __Where)
{
    // in:  nom = toto AND prenom = titi
    // out: [{nom,"toto"},{prenom,"titi"}]
    cTexte __Final;
    char buffer[1024];
    char *ptr, *last;
    int cont = 1;
    __Final << "[";
    strncpy(buffer,__Where.Texte(),1024);
    ptr = buffer;
    // A FINIR
    __Final << "]";
    return __Final;
}


ETERM *cServeurMnesia::_call(const cTexte& __Mod,const cTexte& __Fun,const cTexte& __Args)
{
    ETERM *__Eres,*__Earg;
    __Eres = NULL;
    if (_ErlFD > 0)
    {
#ifdef DEMO
        printf("> %s:%s(%s).\n",__Mod.Texte(),__Fun.Texte(),__Args.Texte());
#endif
        __Earg = erl_format(__Args.Texte());
        __Eres = erl_rpc(_ErlFD,__Mod.Texte(),__Fun.Texte(),__Earg);
        erl_free_term(__Earg);
    }
    return __Eres;
}

void cServeurMnesia::_send(const cTexte& dst,const cTexte& msg)
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


#ifdef DEMO
int main(int argc, char **argv)
{
    cServeurMnesia __srv("db0@iBook","rcs");
    cResultatBaseDeDonnees __Resultat;

    cTexte __Table("personnes");
    cTexte __Where1("where nom = \"romain\"");
    cTexte __Where2("where nom = \"romain\" and id < 5");
    cTexte __Champ1("nom");
    cTexte __Champ2("prenom");
    cTableauDynamiqueObjets<cTexte> __Champs;
    __Champs += __Champ1;
    __Champs += __Champ2;

    __srv.Select(__Table,__Champs,__Where1,NULL,0,__Resultat);
    cout << __srv.Max(__Table,cTexte("id"),__Where1) << endl;
    cout << __srv.Count(__Table,__Where1) << endl;
    cout << __srv.Count(__Table,__Where2) << endl;
}
#endif

