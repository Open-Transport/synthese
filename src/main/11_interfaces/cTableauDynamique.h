/*! \file cTableauDynamique.h
\brief En-t�te Classe Tableau Dynamique
*/


#ifndef SYNTHESE_CTABLEAUDYNAMIQUE_H
#define SYNTHESE_CTABLEAUDYNAMIQUE_H

#define TABLEAU_ELEMENT_DEFAUT 0

#include "Parametres.h"

/*! \brief Tableau avec gestion automatique de l'allocation
 \todo bloquer les phases d'�criture avec des sections critiques pour un usage temps r�el
 \warning De mani�re g�n�rale, il est fortement d�conseill� d'utiliser des pointeurs ou r�f�rences vers des �l�ments du tableau, ceux ci pouvant �tre d�plac�s dans la m�moire en cas d'agrandissement du tableau lors d'un ajout d'�l�ment par exemple. L'usage de telles r�f�rences doit se faire uniquement dans un contexte court avec acc�s en lecture uniquement et sous la protection d'une section critique.
 @ingroup m01
*/
template <class C>
class cTableauDynamique
{
    protected:
        C* _Element;     //!< Tableau d'�l�ments
        bool _AugmentationParDoublement; //!< M�thode d'augmentation de l'espace allou�
        int _TailleMax;     //!< Taille de l'espace allou�
        int _Taille;     //!< Taille de l'espace utilis�

        virtual void Construit( int __AncienneTailleMax );
        virtual void Finalise() {}
        virtual void Detruit() {}


    public:
        //! \name Calculateurs
        //@{
        bool IndexValide( int ) const;
        int Taille() const;
        C* GetElement( int __Index = TABLEAU_ELEMENT_DEFAUT ) ;
        C& getElement( int __Index = TABLEAU_ELEMENT_DEFAUT ) ;
        const C& operator[] ( int ) const;
        int operator==( const cTableauDynamique<C>& ) const;
        C& getDernier() ;
        //@}

        //! \name Modificateurs
        //@{
        bool AlloueSiBesoin( int __TailleDemandee, bool __Relatif = false );
        cTableauDynamique<C>& Vide( int __Position = 0 );
        int SetElement( C&, int __i = INCONNU, bool __Finalisation = true );
        int SetElement( const C&, int __i = INCONNU, bool __Finalisation = true );
        int CopieElement( int __IndexSource, int __IndexDestination = INCONNU );
        int Active( int __i = INCONNU );
        cTableauDynamique<C>& operator+=( C& );
        //void     Trie(int (__cdecl *compare )(const void *, const void *));
        //@}


        cTableauDynamique<C>( int __TailleMax = 0, bool __AugmentationParDoublement = true );
        virtual ~cTableauDynamique<C>();

        // Recherche d'un �l�ment
        int Recherche( const C& __Element ) const
        {
            for ( int __i = 0; __i < Taille(); __i++ )
                if ( operator[] ( __i ) == __Element )
                    return __i;
            return INCONNU;
        }


        /*! \brief Ajout d'�l�ment si aucun �l�ment de m�me valeur n'existe
        */
        int AddElementSiInexistant( C& __Element )
        {
            int __i;
            return ( ( __i = Recherche( __Element ) ) == INCONNU
                     ? SetElement( __Element ) != INCONNU
                     : __i )
                   ;
        }
};



/*! \brief Destructeur
 \author Hugues Romain
 \date 2005
*/
template <class C>
inline cTableauDynamique<C>::~cTableauDynamique()
{
    /*! Destruction des objets un par un si n�cessaire */
    Detruit();

    /*! Lib�ration de l'espace m�moire des objets */
    free( _Element );
}


template <class C>
class cTableauDynamiqueObjets : public cTableauDynamique<C>
{
    protected:
        void Construit( int __AncienneTailleMax );
        void Detruit();
};


template <class C>
class cTableauDynamiquePointeurs : public cTableauDynamique<C>
{
    protected:
        void Detruit();

    public:
        int NombreReel() const;
};



/*! \brief Calcul du nombre d'�l�ments non nuls point�s par le tableau
 \return Le nombre d'�l�ments non nuls point�s par le tableau
*/
template <class C>
inline int cTableauDynamiquePointeurs<C>::NombreReel() const
{
    int __NombreElements = 0;

    for ( int __i = 0; __i < this->Taille(); __i++ )
        if ( this->_Element[ __i ] != 0 )
            __NombreElements++;

    return __NombreElements;
}

template <class C>
inline void cTableauDynamiqueObjets<C>::Construit( int __AncienneTailleMax )
{
    for ( int __i = __AncienneTailleMax; __i < this->_TailleMax; __i++ )
        new( this->_Element + __i ) C;
}

template <class C>
inline void cTableauDynamique<C>::Construit( int __AncienneTailleMax )
{
    memset( _Element + __AncienneTailleMax, 0, ( _TailleMax - __AncienneTailleMax ) * sizeof( C ) );
}

template <class C>
inline void cTableauDynamiqueObjets<C>::Detruit()
{
    for ( int __i = 0; __i < this->_TailleMax; __i++ )
        this->_Element[ __i ].~C();
}

template <class C>
inline void cTableauDynamiquePointeurs<C>::Detruit()
{
    for ( int __i = 0; __i < this->_TailleMax; __i++ )
        delete this->_Element[ __i ];
}




/*! \brief Constructeur
 \param TailleMax Taille de l'espace m�moire allou� pour le stockage ult�rieur du texte
 \param AugmentationParDoublement Indique si les augmentations de l'espace m�moire n�cessaires devront se faire syst�matiquement en doublant la m�moire allou�e (sinon, seule la taille requise � chaque d�passement est allou�e). Indiquer true si la taille de la cha�ne de caract�res est susceptible d'�tre augment�e fr�quemment. La perte de m�moire sera compens�e par le gain en vitesse li� � la r�duction du nombre d'appels � l'allocation de m�moire.
*/
template <class C>
inline cTableauDynamique<C>::cTableauDynamique( int __TailleMax, bool __AugmentationParDoublement )
{
    _Element = 0;
    _Taille = 0;
    _AugmentationParDoublement = __AugmentationParDoublement;
    _TailleMax = 0;
    if ( __TailleMax )
        AlloueSiBesoin( __TailleMax );
    Vide();
}



/*! \brief Nombre r�el d'�l�ments pr�sents dans le tableau
 \author Hugues Romain
 \date 2000-2005
*/
template <class C>
inline int cTableauDynamique<C>::Taille() const
{
    return _Taille;
}



/*! \brief Test de validit� d'un index
 \param __Index Index � tester
 \return true si l'index est valide, false sinon
 \author Hugues Romain
 \date 2005
*/
template <class C>
inline bool cTableauDynamique<C>::IndexValide( int __Index ) const
{
    return __Index >= 0 && __Index < Taille();
}



/*! \brief Accesseur pointeur vers �l�ment
 \param __Index Index de l'�l�ment � renvoyer
 \return L'adresse de l'�l�ment s'il existe, 0 sinon
 \author Hugues Romain
 \date 2005
*/
template <class C>
inline C* cTableauDynamique<C>::GetElement( int __Index )
{
    if ( IndexValide( __Index ) )
        return _Element + __Index;
    else
        return 0;
}



/*! \brief Accesseur r�f�rence vers �l�ment
 \param __Index Index de l'�l�ment � renvoyer
 \return R�f�rence vers l'�l�ment
 \warning S'assurer que l'index founi est valide (voir cTableauDynamique<C>::IndexValide())
 \author Hugues Romain
 \date 2005
*/
template <class C>
inline C& cTableauDynamique<C>::getElement( int __Index )
{
    return _Element[ __Index ];
}






/*! \brief Contr�le si l'espace allou� est de taille suffisant pour accueillir une prochaine modification de la cha�ne de caract�res et effectue l'allocation si besoin, avec initialisation � z�ro de l'espace allou�
 \param __TailleDemandee Taille de la cha�ne de caract�res souhait�e
 \param __Relatif true si la taille fournie est en plus de la taille occup�e actuelle, false si la taille demand�e est la taille totale.
 \return true si la cha�ne peut accueillir les nouvelles donn�es pr�vues
 \warning Apr�s l'�x�cution de l'allocation, l'emplacement m�moire de l'ensemble des �l�ments du tableau dynamique est susceptible d'avoir �t� modifi�. Veiller � ne pas utiliser, apr�s l'ex�cution de l'allocation, de r�f�rences vers un �l�ment, affect�e avant l'ex�cution de l'allocation. Elle pourrait pointer vers un espace d�sallou�.
 \author Hugues Romain
 \date 2000-2005
 
La m�thode retourne true si :
 - l'espace m�moire existant est suffisant
 - l'espace m�moire existant est insuffisant, mais un espace m�moire plus important a �t� allou� avec succ�s. Dans ce cas un nouvel espace m�moire est allou� et est initialis� par des valeurs nulles.
 
 La taille de l'espace allou� est �gale � :
  - la taille totale n�cessaire + un caract�re pour stocker le 0 de finalisation si l'allocation par doublement est d�sactiv�e;
  - deux fois la taille totale n�cessaire + un caract�re pour stocker le 0 de finalisation si l'allocation par doublement est activ�e.
 
 Si la seconde solution occupe toujours plus de m�moire que n�cessaire, elle permet d'�viter d'avoir fr�quemment recours � la r�allocation dans le cas o� la cha�ne de caract�res est susceptible de changer de taille fr�quemment.
 
 Pour activer la m�thode d'augmentation de l'espace allou�, deux solutions sont utilisables :
  - d�clarer l'allocation par doublement par le constructeur : voir std::string::std::string(size_t, bool);
  - activer l'allocation par doublement par le modificateur std::string::setAugmentationParDoublement().
*/
template <class C>
bool cTableauDynamique<C>::AlloueSiBesoin( int __TailleDemandee, bool __Relatif )
{
    // Taille n�cessaire
    int __TailleTotaleDemandee = __TailleDemandee;
    if ( __Relatif )
        __TailleTotaleDemandee += _Taille;

    // Contr�le de l'entr�e : l'allocation est-elle n�cessaire?
    if ( __TailleTotaleDemandee <= _TailleMax )
        return true;

    // Sauvegarde de l'ancienne taille max
    int __AncienneTailleMax = _TailleMax;

    // Choix de la taille allou�e pour agrandir
    if ( _AugmentationParDoublement )
        _TailleMax = __TailleTotaleDemandee * 2;
    else
        _TailleMax = __TailleTotaleDemandee;

    // Cas pas d'allocation d�j� effectu�e
    if ( _Element == 0 )
        _Element = ( C* ) malloc( _TailleMax * sizeof( C ) + 1 );
    else
        _Element = ( C* ) realloc( _Element, _TailleMax * sizeof( C ) + 1 );

    // Construction des objets si necessaire
    Construit( __AncienneTailleMax );

    // Ecriture du z�ro terminal
    memset( _Element + _TailleMax, 0, 1 );

    // Sortie succ�s de l'allocation
    return _Element != 0;
}



/*! \brief Accesseur en lecture �l�ment
 \param __Index Index de l'�l�ment � renvoyer
 \return L'adresse de l'�l�ment s'il existe, 0 sinon
 \warning S'assurer que l'index founi est valide (voir cTableauDynamique<C>::IndexValide())
 \author Hugues Romain
 \date 2005
*/
template <class C>
inline const C& cTableauDynamique<C>::operator[] ( int __Index ) const
{
    return _Element[ __Index ];
}



/*! \brief Tronquage/Vidage du tableau
 \param __Position Indique la longueur que doit avoir la cha�ne une fois coup�e. 0 = Vidage total
 \return L'objet lui-m�me
 \author Hugues Romain
 \date 2000-2005
*/
template <class C>
cTableauDynamique<C>& cTableauDynamique<C>::Vide( int __Position )
{
    if ( _Element != 0 && __Position >= 0 && __Position <= _TailleMax )
    {
        _Taille = __Position;
        Finalise();
    }
    return *this;
}



/*! \brief Op�rateur de comparaison directe
 \param __Objet l'objet � comparer
 \return true si les contenus des deux objets sont strictement identiques
 \author Hugues Romain
 \date 2005
*/
template <class C>
int cTableauDynamique<C>::operator==( const cTableauDynamique<C>& __Objet ) const
{
    if ( Taille() != __Objet.Taille() )
        return false;

    for ( int __I = 0; __I < Taille(); __I++ )
        if ( getElement( __I ) != __Objet.getElement( __I ) )
            return false;

    return true;
}



/*! \brief Modificateur/Cr�ateur �l�ment
 \param __Index Index de l'�l�ment � modifier
 \param __Valeur Valeur � donner � l'�l�ment
 \param __Finalisation Indique si la m�thode de finalisation doit �tre lanc�e � l'issue de l'ajout d'�l�ment
 \return L'index donn� � l'�l�ment (INCONNU = op�ration �chou�e)
 \warning Le corps de la fonction est en double pour des raisons de compilation. En cas de modification de cette fonction veiller � modifier l'autre.
 \warning Apr�s l'�x�cution de la modification, l'emplacement m�moire de l'ensemble des �l�ments du tableau dynamique est susceptible d'avoir �t� modifi�. Veiller � ne pas utiliser, apr�s l'ex�cution de la modification, de r�f�rences vers un �l�ment, affect�e avant l'ex�cution de la modification. Elle pourrait pointer vers un espace d�sallou�.
 En particulier, ne jamais passer en argument __Valeur un �l�ment issu du m�me tableau dynamique.
 \author Hugues Romain
 \date 2005
 
 - Si l'index fourni est n�gatif (INCONNU), alors l'�l�ment est ajout� � la fin du tableau
 - Si l'index fourni est positif, alors l'�l�ment est ajout� � l'index d�crit :
   - Si un objet est d�j� pr�sent, il est �cras�
   - Si un objet n'est pas pr�sent alors l'espace est occup�
   - Si l'index est sup�rieur � la taille actuellement allou�e une allocation est effectu�e.
*/
template <class C>
int cTableauDynamique<C>::SetElement( C& __Valeur, int __Index, bool __Finalisation )
{
    // Pr�paration de l'index
    __Index = Active( __Index );
    if ( __Index == INCONNU )
        return INCONNU;

    // Affectations
    _Element[ __Index ] = __Valeur;
    if ( __Finalisation )
        Finalise();
    return __Index;
}


/*! \brief Modificateur/Cr�ateur �l�ment
 \param __Index Index de l'�l�ment � modifier
 \param __Valeur Valeur � donner � l'�l�ment
 \param __Finalisation Indique si la m�thode de finalisation doit �tre lanc�e � l'issue de l'ajout d'�l�ment
 \return L'index donn� � l'�l�ment (INCONNU = op�ration �chou�e)
 \warning Le corps de la fonction est en double pour des raisons de compilation. En cas de modification de cette fonction veiller � modifier l'autre.
 \warning Apr�s l'�x�cution de la modification, l'emplacement m�moire de l'ensemble des �l�ments du tableau dynamique est susceptible d'avoir �t� modifi�. Veiller � ne pas utiliser, apr�s l'ex�cution de la modification, de r�f�rences vers un �l�ment, affect�e avant l'ex�cution de la modification. Elle pourrait pointer vers un espace d�sallou�.
 En particulier, ne jamais passer en argument __Valeur un �l�ment issu du m�me tableau dynamique.
 \author Hugues Romain
 \date 2005
 
 */
template <class C>
int cTableauDynamique<C>::SetElement( const C& __Valeur, int __Index, bool __Finalisation )
{
    /*!
    <li>Si l'index fourni est n�gatif (INCONNU), alors l'�l�ment est ajout� � la fin du tableau</li>
    <li>Si l'index fourni est positif, alors l'�l�ment est ajout� � l'index d�crit :
     <ul>
      <li>Si un objet est d�j� pr�sent, il est �cras�</li>
      <li>Si un objet n'est pas pr�sent alors l'espace est occup�</li>
     </ul>
    </li>
    */
    __Index = Active( __Index );

    /*!
    <li>Si l'index est sup�rieur � la taille actuellement allou�e une allocation est effectu�e. En cas d'�chec de celle-ci la fonction �choue</li>
    */
    if ( __Index == INCONNU )
        return INCONNU;

    // Affectations
    _Element[ __Index ] = __Valeur;

    // Lancement de la proc�dure optionnelle de finalisation
    if ( __Finalisation )
        Finalise();
    return __Index;
}



/*! \brief Ajout d'�l�ment
 \param __Element Element � ajouter
 \warning Cette m�thode ne contr�le pas que l'�l�ment a bien �t� ajout�.
 \warning Apr�s l'�x�cution de l'ajout, l'emplacement m�moire de l'ensemble des �l�ments du tableau dynamique est susceptible d'avoir �t� modifi�. Veiller � ne pas utiliser, apr�s l'ex�cution de l'ajout, de r�f�rences vers un �l�ment, affect�e avant l'ex�cution de l'ajout. Elle pourrait pointer vers un espace d�sallou�.
 \return L'objet lui m�me
 \author Hugues Romain
 \date 2005
*/
template <class C>
cTableauDynamique<C>& cTableauDynamique<C>::operator+=( C& __Element )
{
    AlloueSiBesoin( 1, true );
    _Element[ _Taille ] = __Element;
    _Taille++;
    Finalise();
    return *this;
}



/*! \brief Modificateur/Cr�ateur �l�ment
 \param __IndexSource Index de l'�l�ment � copier
 \param __IndexDestination Index o� copier l'�l�ment (d�faut = INCONNU)
 \return L'index donn� � l'�l�ment (INCONNU = op�ration �chou�e)
 \warning Apr�s l'�x�cution de l'allocation, l'emplacement m�moire de l'ensemble des �l�ments du tableau dynamique est susceptible d'avoir �t� modifi�. Veiller � ne pas utiliser, apr�s l'ex�cution de l'allocation, de r�f�rences vers un �l�ment, affect�e avant l'ex�cution de l'allocation. Elle pourrait pointer vers un espace d�sallou�.
 \author Hugues Romain
 \date 2005
 
 - Si l'index de destination est n�gatif (INCONNU), alors l'�l�ment est ajout� � la fin du tableau
 - Si l'index de destination est positif, alors l'�l�ment est ajout� � l'index d�crit :
   - Si un objet est d�j� pr�sent, il est �cras�
   - Si un objet n'est pas pr�sent alors l'espace est occup�
   - Si l'index est sup�rieur � la taille actuellement allou�e une allocation est effectu�e.
*/
template <class C>
int cTableauDynamique<C>::CopieElement( int __IndexSource, int __IndexDestination )
{
    // Sauvegarde de l'�l�ment � copier en cas de r�allocation du tableau lors de l'�criture (r�f�rence impossible)
    C __ElementACopier = _Element[ __IndexSource ];

    // Copie de l'�l�ment et retour du r�sultat
    return SetElement( __ElementACopier, __IndexDestination );
}



/*! \brief Acc�s au dernier �l�ment du tableau
 \return R�f�rence sur le dernier �l�ment du tableau
 \warning Avant d'utiliser cette m�thode, contr�ler que le tableau n'est pas vide !
 \author Hugues Romain
 \date 2005
*/
template <class C>
C& cTableauDynamique<C>::getDernier()
{
    return _Element[ Taille() - 1 ];
}



/*! \brief Activateur d'�l�ment
 \param __Index Index de l'�l�ment � activer
 \return L'index donn� � l'�l�ment (INCONNU = op�ration �chou�e)
 \warning Apr�s l'�x�cution de la modification, l'emplacement m�moire de l'ensemble des �l�ments du tableau dynamique est susceptible d'avoir �t� modifi�. Veiller � ne pas utiliser, apr�s l'ex�cution de la modification, de r�f�rences vers un �l�ment, affect�e avant l'ex�cution de la modification. Elle pourrait pointer vers un espace d�sallou�.
 En particulier, ne jamais passer en argument __Valeur un �l�ment issu du m�me tableau dynamique.
 \author Hugues Romain
 \date 2005
 
 - Si l'index fourni est n�gatif (INCONNU), alors un �l�ment suppl�mentaire est ajout� puis � la fin du tableau
 - Si l'index fourni est positif, alors l'�l�ment est ajout� � l'index d�crit :
   - Si un objet est d�j� pr�sent, il est conserv� en l'�tat
   - Si un objet n'est pas pr�sent alors l'espace est occup�
   - Si l'index est sup�rieur � la taille actuellement allou�e une allocation est effectu�e.
*/
template <class C>
inline int cTableauDynamique<C>::Active( int __Index )
{
    // Cas index inconnu
    if ( __Index < 0 )
    {
        if ( !AlloueSiBesoin( 1, true ) )
            return INCONNU;
        __Index = _Taille;
        _Taille++;
    }
    // Cas index hors du tableau existant
    else if ( __Index >= _Taille )
    {
        if ( !AlloueSiBesoin( __Index + 1 ) )
            return INCONNU;
        _Taille = __Index + 1;
    }
    return __Index;
}

#endif
