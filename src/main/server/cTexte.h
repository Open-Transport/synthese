/*! \brief Gestion des cha�nes de caract�res
*/

#ifndef SYNTHESE_CTEXTE_H
#define SYNTHESE_CTEXTE_H

#include <cstdlib>
using namespace std;

class cTexte;

//! \name Format g�n�ral des fichiers
//@{
// Type d'une section d�termin� par le premier caract�re de la ligne
// Type de section voir les define en d�but de FormatsFichiers.h
typedef int TypeSection;
#define TYPESection  '#'
#define TYPESousSection '['
#define TYPELien  '>'
#define TYPECommentaire '/'
#define TYPEAutre  2
#define TYPEVide  1
#define TYPEError  3 
//@}

#define LONGUEURNUMEROS 6

#include <fstream>
#include "Parametres.h"
#include "cTableauDynamique.h"

/*! \brief Classe cha�ne de carac�tres am�lior�e, assurant la conversion �ventuelle en un format propre
 
La gestion de l'allocation m�moire est automatique.
 
\todo Nettoyer les fonctions de copie
 @ingroup m01
*/
class cTexte : public cTableauDynamique<char>
{
    protected:
        //! \name Gestion interne
        //@{
        virtual int ConvertChar( const char c, char* Tampon = NULL ) const;
        virtual bool CompareChar( const char, const char ) const;
        virtual void Finalise();
        //@}

    public:
        //! \name Gestion de fichiers (� supprimer d�s que possible)
        //@{
        TypeSection LireLigne( ifstream& );
        bool ProchaineSection( ifstream&, int TypeSection );
        bool RechercheSection( ifstream&, const cTexte& Intitule, char CarEntreeSection );
        TypeSection TypeSectionTampon() const;
        //@}

        //! \name Contructeurs et destructeurs
        //@{
        explicit cTexte( int TailleMax = 0, bool AugmentationParDoublement = false );
        cTexte( const char * );
        cTexte( const string );
        cTexte( const cTexte& );
        virtual ~cTexte();
        //@}

        //! \name Modificateurs
        //@{
        cTexte& setTampon();
        cTexte& ChangeTexte( char* newTexte, int newTailleMax = 0 );
        cTexte& ChangeTexte( cTexte& newTexte );
        cTexte& Copie( const cTexte& Source );
        cTexte& Copie( const cTexte& Source, int LongueurMax );
        cTexte& Copie( const char* Source );
        cTexte& Copie( const char* Source, int LongueurMax );
        cTexte& Copie( const int Source, int NombreChiffres = 0 );
        cTexte& fAdresseComplete( const cTexte& AdresseBase );
        cTexte& Repertoire();
        cTexte& Trim();
        cTexte& operator=( const cTexte& );
        //@}

        //! \name Calculateurs
        //@{
        bool Compare( const cTexte& Autre, int NombreChiffres = 0, int Position1 = 0, int Position2 = 0 ) const;
        int RechercheOccurenceGauche( char curChar, int NombreOccurences = 1, int Position = 0 ) const;
        cTexte Extrait( int Position, int Longueur = INCONNU ) const;
        int TailleApresCodage( const char* ) const;
        int operator==( const char* ) const;
        //@}

        //! \name Accesseurs
        //@{
        int GetNombre( int NombreChiffres = 0, int Position = 0 ) const;
        char* Texte() const;
        //@}
};

std::ostream& operator<<( std::ostream& flux, const cTexte& Obj );

cTexte& operator<<( cTexte& Obj, const char* Data );
cTexte& operator<<( cTexte& Obj, int Data );
cTexte& operator<<( cTexte& Obj, const cTexte& Data );


/*! \brief Classe de texte destin�e � recevoir du texte sans caract�re accentu�
*/
class cTexteSansAccent : public cTexte
{
    protected:
        int ConvertChar( const char c, char* Tampon = NULL ) const;
        bool CompareChar( const char, const char ) const;

    public:
        //! \name Constructeurs et destructeurs
        //@{
        //@}
};



/*! \brief Classe de texte destin�e � recevoir du texte au langage PostScript
*/
class cTextePostScript : public cTexte
{
    protected:
        int ConvertChar( const char c, char* Tampon = NULL ) const;

    public:
        //! \name Constructeurs et destructeurs
        //@{
        //@}
};



/*! \brief Classe de texte destin�e � recevoir du texte au langage HTML
 
Le but de cette classe est en particulier de remplacer les caract�res accentu�s par les combinaisons standard de remplacement ex: � => &eacute;
*/
class cTexteHTML : public cTexte
{
    protected:
        int ConvertChar( const char c, char* Tampon = NULL ) const;

    public:
        //! \name Constructeurs et destructeurs
        //@{
        cTexteHTML( int TailleMax = 0, bool AugmentationParDoublement = false ) : cTexte( TailleMax, AugmentationParDoublement ) { }
        //@}
       
};




        /*! \brief Classe de texte destin�e � recevoir des donn�es suivant le format de codage interne � SYNTHESE (pour passage dans les URLs par exemple)
        */
    class cTexteCodageInterne : public cTexte
        {
            public:
                cTexteCodageInterne() : cTexte() { } //!< Constructeur


	};



                /*! \brief Classe de texte destin�e � recevoir des donn�es uniquement en minuscules et sans accent (voir cTexteMinuscules::ConvertChar() pour les d�tails des r�gles)
                 \author Hugues Romain
                 \date 2005
                */
            class cTexteMinuscules : public cTexteSansAccent
                {
                    protected:
                        int ConvertChar( const char c, char* Tampon = NULL ) const;

                    public:
                };


                /*! \brief Ecriture sur l'objet depuis un flux d'entr�e quelconque
                 \param flux Le flux d'entr�e
                 \param Obj L'objet cTexte sur lequel �crire
                 \return Le flux d'entr�e
                 \warning Cette fonction n'�tant pas utilis�e en production, elle utilise un espace m�moire inutilement important, et ne v�rifie pas correctement l'allocation m�moire de Tampon
                */
	    
                template <class T>
                inline T& operator>>( T& flux, cTexte& Obj )
                {
                    char * Tampon = ( char* ) malloc( TAILLETAMPON * sizeof( char ) );
                    flux >> Tampon;
                    Obj.Vide();
                    Obj.Copie( Tampon );
                    free( Tampon );
                    return ( flux );
                }
	    

#endif
