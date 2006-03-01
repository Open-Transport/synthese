/*! \file cChronometre.h
\brief En-t�te et impl�mentations inline classe chronom�tre
*/


#ifndef SYNTHESE_CCHRONOMETRE_H
#define SYNTHESE_CCHRONOMETRE_H


#include <time.h>

/*! \brief Calcul de dur�e d'ex�cution entre deux points de mesure
 \author Hugues Romain
 \date 2000-2001
 @ingroup m01
*/
class cChronometre
{
    protected:
        clock_t _Debut; //!< D�but du chronom�trage
        clock_t _Fin;  //!< Fin du chronom�trage

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



/*! \brief D�marrage du chronom�trage
 \author Hugues Romain
 \date 2000-2005
*/
inline void cChronometre::Start()
{
    _Debut = clock();
}



/*! \brief Arr�t du chronom�trage
 \author Hugues Romain
 \date 2000-2005
*/
inline void cChronometre::Stop()
{
    _Fin = clock();
}



/*! \brief R�sultat du chronom�trage
 \return Le nombre de secondes �coul�es entre le d�but et la fin du chronom�trage
 \author Hugues Romain
 \date 2000-2005
*/
inline double cChronometre::GetDuree() const
{
    return ( double ) ( _Fin - _Debut ) / CLOCKS_PER_SEC;
}

#endif
