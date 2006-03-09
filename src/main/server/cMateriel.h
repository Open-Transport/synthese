/*! \file cMateriel.h
\brief En-t�te classe cMateriel
*/

#ifndef SYNTHESE_CMATERIEL_H
#define SYNTHESE_CMATERIEL_H

class cMateriel;

//! Indique le code obligatoire pour le mat�riel repr�sentant les jonctions � pied (Provisoire : � remplacer par un param�tre)
#define MATERIELPied 12

#include <string>


/** Impl�mentation de la notion de @ref defMateriel
 @ingroup m15
 @author Hugues Romain
 @date 2001
*/
class cMateriel
{
        std::string vLibelleSimple;
        std::string vArticle;
        std::string vDoc;
        std::string vIndicateur;
        int vVitesseMoyenneMax; // km/h
        int vCode;

    public:

        //! \name Accesseurs
        //@{
        bool EstAPied() const;
        const std::string& getArticle() const;
        const std::string& getIndicateur() const;
        const std::string& getLibelleSimple() const;
        int Code() const;
        int VitesseMoyenneMax() const;
        //@}

        //! \name Modificateurs
        //@{
        void setArticle( const std::string& newArticle );
        void setDoc( const std::string& newVal );
        void setIndicateur( const std::string& newVal );
        void setLibelleSimple( const std::string& newLibelleSimple );
        bool setVitesseMoyenneMax( int );
        //@}

        //! \name Constructeur et destructeur
        //@{
        cMateriel( int );
        ~cMateriel();
        //@}

        //! \name Sorties
        //@{
        /*template <class T> T& toXML(T& Tampon) const
        {
         Tampon << "<materiel code=\"" << vCode << "\">";
         Tampon << "<libelle>" << vLibelleSimple << "</libelle></materiel>";
         
         return (Tampon);
        }*/ 
        //@}
};

#endif
