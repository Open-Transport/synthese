/*! \file cChronometre.h
\brief En-tête et implémentations inline classe chronomètre
*/


#ifndef SYNTHESE_CCHRONOMETRE_H
#define SYNTHESE_CCHRONOMETRE_H


#include <time.h>

/*! \brief Calcul de durée d'exécution entre deux points de mesure
 \author Hugues Romain
 \date 2000-2001
 @ingroup m01
*/
class cChronometre
{
    protected:
        clock_t _Debut; //!< Début du chronométrage
        clock_t _Fin;  //!< Fin du chronométrage

    public:
        //! \name Modificateurs
        //@{
        void Start();
        void Stop();
        //@}

        //! \name Accesseurs
        //@{
        double GetDuree() const;
        //@}
};



/*! \brief Démarrage du chronométrage
 \author Hugues Romain
 \date 2000-2005
*/
inline void cChronometre::Start()
{
    _Debut = clock();
}



/*! \brief Arrêt du chronométrage
 \author Hugues Romain
 \date 2000-2005
*/
inline void cChronometre::Stop()
{
    _Fin = clock();
}



/*! \brief Résultat du chronométrage
 \return Le nombre de secondes écoulées entre le début et la fin du chronométrage
 \author Hugues Romain
 \date 2000-2005
*/
inline double cChronometre::GetDuree() const
{
    return ( double ) ( _Fin - _Debut ) / CLOCKS_PER_SEC;
}

#endif
