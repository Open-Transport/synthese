/*
 * Multi Layer Perceptron
 * General Graph Use
 * Christophe Romain, (c) 2005
 */

#ifndef __MPL__
#define __MPL__

#define __MLP_ID__ 150

#define NODE_OFF  0
#define NODE_OK  70  //  70%
#define NODE_ON 100  // 100%

#include <map>
#include <vector>
#include <string>


typedef short          sint16;
typedef long           sint32;
typedef unsigned short uint16;
typedef unsigned long  uint32;
typedef vector<string> dict;

// structure d'entête
typedef struct _header
{
    char name[64];  // nom du maillage
    uint32 stamp;   // identifiant du maillage
    uint16 infos;   // nombre d'infos texte
    uint16 alias;   // nombre d'alias
    uint16 layers;  // nombre de couches
    uint16 nodes;   // nombre de noeuds
    uint32 links;   // nombre de liens
} header;

// structure de couche
typedef struct _layer
{
    char name[16];      // nom de la couche
    uint16 node_offset; // position du premier noeud
    uint16 node_length; // nombre de noeuds
} layer;

// structure de noeud
typedef struct _node
{
    uint16 label;       // identifiant de l'info texte associé
    sint16 value;       // valeur
    uint32 link_offset; // position du premier lien
    uint32 link_length; // nombre de liens
} node;

// structure de lien
typedef struct _nlink {
    uint16 from;   // identifiant du noeud source
    sint16 weight; // poid de la connection
} nlink;

// structure de résultat
typedef struct _eval {
    string id;
    sint16 score;
    sint16 delta;
} cAssocEval; 
typedef vector<cAssocEval> cAssocResult;

class cAssociator {
    header _hdr; // entête
    layer *_lyr; // couches
    node  *_nde; // noeuds
    nlink *_lnk; // liens
    dict   _dct; // doctionnaire des infos
    dict   _mem; // mémoires

    static char *_infilter; // filtre d'entrée
    map< string,string > _alias; // dictionnaire des alias
    map< node*,dict > _tokens; // mots discriminant des infos
    uint16 _nbinput; // nombre d'entrées
    uint16 _nboutput; // nombre de sorties
    bool _isloaded; // indique le chargement correcte
    map< uint16,vector<node*> > _cache; // chached results
public:
    cAssociator();
    cAssociator(const char *netfile);
    ~cAssociator();
    // méthodes d'injection
    void addnode(uint16 layer, const char *label);
    void addlink(uint16 layer1, const char *label1, uint16 layer2, const char *label2, sint16 weight);
    void finalize();
    // méthodes bas niveau
    void tokenize(const string& str, vector<string>& tokens, const string& delimiters = " ");
    void clean();
    void define(const uint16 entry, const char *text);
    void compute(const uint16 entry);
    // méthodes pour synthese
    bool IsLoaded();
    void Try(const char *city, const char *place);
    cAssocResult MatchCity(uint16 maxsize);
    cAssocResult MatchPoint(uint16 maxsize);
    cAssocResult MatchPlace(uint16 maxsize);
};

#endif

