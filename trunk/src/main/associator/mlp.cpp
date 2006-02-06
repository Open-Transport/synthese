/*
 * Multi Layer Perceptron
 * General Graph Use
 * Christophe Romain, (c) 2005
 */

#include <fstream>
#include <iostream>

using namespace std;

#include "mlp.h"

#define DEBUG

char * associator::_infilter = \
        "                                 " \
        "               0123456789       " \
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ      " \
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ       " \
        "                                 " \
        "                              AA " \
        " A C  E I I   OOO    U      A A A" \
        "  CEEEE  II    O O  U UU   ";


void Tokenize(const string& str,
        vector<string>& tokens,
        const string& delimiters = " ")
{
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        lastPos = str.find_first_not_of(delimiters, pos);
        pos = str.find_first_of(delimiters, lastPos);
    }
}

associator::associator(const char *netfile)
{
    ifstream net(netfile,ios::in);
    // lecture de la signature
    net.read((char*)&_hdr,sizeof(header));
    if((_hdr.stamp&0xFF) == __MLP_ID__)
    {
#ifdef DEBUG
        cout << _hdr.name << endl;
        cout << "version " << (_hdr.stamp&0xFF) << " du " \
             << ((_hdr.stamp&0xFF00)>>8) << "/" \
             << ((_hdr.stamp&0xFF0000)>>16) << "/" \
             << (_hdr.stamp>>24) << endl;
#endif
        // lecture du dictionnaire
        for(unsigned int i=0; i<_hdr.infos; i++)
        {
            char buffer[128];
            bzero(buffer,128);
            net.getline(buffer,128,'\0');
            _dct.push_back(buffer);
        }
        // des alias
        for(unsigned int i=0; i<_hdr.alias; i++)
        {
            char word[128],alias[128];
            bzero(word,128);
            bzero(alias,128);
            net.getline(word,128,'\0');
            net.getline(alias,128,'\0');
            _alias[string(alias)] = string(word);
        }
        // lecture des couches
        _lyr = new layer[_hdr.layers];
        net.read((char*)_lyr,_hdr.layers*sizeof(layer));
        // comptage du nombre d'entrées
        _nbinput = 0;
        while(_lyr[_nbinput].name[0] == '+') _nbinput++;
        // lecture des noeuds
        _nde = new node[_hdr.nodes];
        net.read((char*)_nde,_hdr.nodes*sizeof(node));
        // lecture des liens
        _lnk = new nlink[_hdr.links];
        net.read((char*)_lnk,_hdr.links*sizeof(nlink));
        // construction de la liste de mots discriminants
        for(layer *pl=_lyr; pl<_lyr+_hdr.layers; pl++)
        {
            node *pn = _nde + pl->node_offset;
            node *pnstop = pn + pl->node_length;
            // pour chaque couche n'étant pas une entrée
            if(pl->name[0] != '+')
                for(; pn<pnstop; pn++)
                {
                    dict inTokens;
                    dict outTokens;
                    dict::iterator it;
                    // on découpe en mots le libelle
                    Tokenize(_dct[pn->label], inTokens);
                    for(it=inTokens.begin(); it!=inTokens.end(); it++)
                        if(it->length() > 3) // on garde si + de 3 lettres
                            outTokens.push_back(*it);
                    _tokens[pn] = outTokens;
                }
        }
    } else {
        cerr << "bad signature" << endl;
        exit(-1);
    }
    net.close();
    clean();
}

associator::~associator()
{
    // on vide toute la mémoire
    _mem.clear();
    _dct.clear();
    delete _lnk;
    delete _nde;
    delete _lyr;
    bzero(&_hdr,sizeof(header));
}

void associator::clean()
{
    // on vide toutes les chaines d'entrées
    _mem.clear();
    _mem.assign(_hdr.layers,"");
    // on met toutes les valeurs de noeud à 0
    for(layer *pl=_lyr; pl<_lyr+_hdr.layers; pl++)
    {
        node *pn = _nde + pl->node_offset;
        node *pnstop = pn + pl->node_length;
        for(; pn<pnstop; pn++)
            pn->value = 0;
    }
}

void associator::set(const uint16 entry, const char *text)
{
    // seule une couche d'entrée peut être utilisée
    if(_lyr[entry].name[0] == '+')
    {
        // on pointe sur les noeuds d'entrée
        node *inchar = _nde + _lyr[entry].node_offset;
        _mem[entry] = "";
        // par sécurité on limite l'entrée à 64 caractères et on filtre
        char input[64]; 
        memset(input, 0, 64);
        while(_infilter[*text] == ' ') text++;
        for(int i=0; i<64 && *text; i++, text++)
            input[i] = _infilter[*text];
        // on découpe l'entrée pour séparer chaque mot
        string instr(input);
        dict tokens;
        Tokenize(instr, tokens);
        // pour chaque mot, on active les noeuds d'entrée
        dict::iterator it;
        for(it=tokens.begin(); it!=tokens.end(); it++)
        {
            bool found = false;
            if(it->length() > 1)
            {
                // on parcours les entrées
                layer *pl = _lyr + entry;
                node *pn = _nde + pl->node_offset;
                node *pnstop = pn + pl->node_length;
                for(; pn<pnstop; pn++)
                    // on match un alias ?
                    if(it->compare(_dct[pn->label])==0)
                    {
                        // oui alors l'entrée est le mot réel
                        pn->value = NODE_ON;
                        if(_mem[entry].length()) _mem[entry] += " ";
                        _mem[entry] += _alias[*it];
                        found = true;
                    }
            }
            // on à pas de de mot reconnu
            if(!found)
            {
                text = it->c_str();
                while(*text)
                {
                    // on active chaque lettre
                    (inchar+(int)(*text-'A'))->value = NODE_ON;
                    text++;
                }
                // et l'entrée est prise tel quel
                if(_mem[entry].length()) _mem[entry] += " ";
                _mem[entry] += *it;
            }
        }
#ifdef DEBUG
        cout << "---- entrée de " << _lyr[entry].name+1 << ": " << _mem[entry] << endl;
#endif
    }
}

//answer associator::get(const uint16 entry, const uint16 maxsize)
void associator::get(const uint16 entry, const uint16 maxsize)
{
    // on calcul toute couche qui n'est pas une entrée
    if(_lyr[entry].name[0] != '+')
    {
        vector<node*> result;
        // discrétisation des mots en entrée
        dict inTokens;
        Tokenize(_mem[entry-_nbinput], inTokens);
        // on fixe les pointeurs de noeuds
        layer *pl = _lyr + entry;
        node *pn = _nde + pl->node_offset;
        node *pnstop = pn + pl->node_length;
        sint32 maxvalue = 0;
        // pour chaque noeud, on propage et on match
        for(; pn<pnstop; pn++)
        {
            nlink *pk = _lnk + pn->link_offset;
            nlink *pkstop = pk + pn->link_length;
            // on somme chaque entrée puis on moyenne
            sint32 sumvalue = 0;
            for(; pk<pkstop; pk++)
                if(pk->weight > 1) sumvalue += _nde[pk->from].value * pk->weight;
                else sumvalue += _nde[pk->from].value;
            pn->value = (sint16)(sumvalue/pn->link_length);
            // on détecte les correspondances de motif
            dict::iterator inIt, outIt;
            for(inIt=inTokens.begin(); inIt!=inTokens.end(); ++inIt)
                if(inIt->length() >= 3) // un motif fait au moins 3 caractères
                {
                    sint32 bestmatch = 0;
                    for(outIt=_tokens[pn].begin(); outIt!=_tokens[pn].end(); ++outIt)
                        if(outIt->length() >= inIt->length()-2) // on match mot plus grand
                        {
                            const char *pin = inIt->c_str();   // motif saisie
                            const char *pout = outIt->c_str(); // mot en cours
                            sint32 eval = 0;      // evaluation du match
                            sint32 delta = 0;     // différentiel de match
                            // puis on parcours le motif
                            while(*pin && *pout)
                            {
                                if(*pin == *pout) // s'il y à égalité
                                {
                                    // le delta passe en positif
                                    if(delta <= 0) delta = 2;
                                    // ou est augmenté de 10 s'il était déjà positif
                                    else delta += 10;
                                }
                                else // s'il n'y à pas égalité
                                {
                                    // le delta passe en négatif
                                    if(delta >= 0) delta = -2;
                                    // ou est diminué de 10 s'il était déjà négatif
                                    else delta -= 10;
                                    // erreur de saisie: un caractère en trop
                                    if(*(pin+1) == *pout) --pout;
                                    // erreur de saisie: un caractère en moins
                                    else if(*pin == *(pout+1)) --pin;
                                }
                                // on incrémente les deux curseurs
                                ++pin, ++pout;
                                // puis on modifie l'évaluation
                                eval += delta;
                            }
                            // on enleve des points pour les mots plus petits que l'entrée
                            while(*pin) eval -= 10, ++pin;
                            // et met à jour bestmatch
                            if(eval > bestmatch) bestmatch = eval;
                        }
                    // on met à jour la valeur du noeud avec le meilleur match
                    pn->value += bestmatch;
                }
                else if(inIt->length() == 2)
                {
                    for(outIt=_tokens[pn].begin(); outIt!=_tokens[pn].end(); ++outIt)
                    {
                        const char *pin = inIt->c_str();   // motif saisie
                        const char *pout = outIt->c_str(); // mot en cours
                        // favoriser le match des 2 premières lettre si
                        // seulement 2 lettres saisies
                        if(*(pin++) == *(pout++)) 
                            if(*pin == *pout) pn->value += 10;
                    }
                }
            // on met à jour la référence
            if(pn->value > maxvalue) maxvalue = pn->value;
        }
        // pour chaque noeud, on réajuste avec la référence et on selectionne
        for(pn=_nde+pl->node_offset; pn<pnstop; pn++)
        {
            // réajustement référentiel
            sint32 tmpvalue = pn->value;
            tmpvalue *= NODE_ON;
            tmpvalue /= maxvalue;
            pn->value = (sint16)tmpvalue;
            // si le seuil est atteind, on active la sortie
            if(pn->value >= NODE_OK)
            {
                // insertion à la bonne position dans le resultat
                vector<node*>::iterator nit,pos;
                for(pos=nit=result.begin(); nit!=result.end(); nit++)
                    if((*nit)->value>pn->value)
                        pos++;
                result.insert(pos,pn);
            }
        }
#ifdef DEBUG
        uint16 totalsize = result.size();
#endif
        // on tronque le résultat
        if(result.size() > maxsize)
            result.erase(result.begin()+maxsize,result.end());
#ifdef DEBUG
        cout << "---- résultat de " << _lyr[entry].name+1 << " " << totalsize << endl;
        vector<node*>::iterator it;
        for(it=result.begin(); it!=result.end(); it++)
            cout << _dct[(*it)->label] << " = " << (*it)->value << endl;
#endif
    }
}

int main(int argc, char **argv)
{
    associator mem("arrets.bin");
    // on défini les 2 entrées
    mem.set(0,argv[1]);
    mem.set(1,argv[2]);
    // chaque résultat DOIT être récupéré dans l'ordre des couches
    mem.get(2,3);
    mem.get(3,3);
    mem.get(4,10);
    mem.get(5,10);
    return 0;
}

