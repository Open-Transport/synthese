
#include "cLog.h"
#include "SYNTHESE.h"
#include "cElementTrajet.h"
#include "cArretPhysique.h"
#include "LogicalPlace.h"

extern SYNTHESE Synthese;

// MJ to be reviewed completely !!

// cLog::cLog( tNiveauLog __Niveau )
// {
//     if ( __Niveau == LogInconnu )
//         SetNiveau( Synthese.getNiveauLog() );
//     else
//         SetNiveau( __Niveau );
// }



// void cLog::Ecrit( tNumeroMessageStandard __NumeroMessage, const std::string& __Objet, const std::string& __Donnees )
// {
//     if ( ControleNiveau( Synthese._NiveauMessageStandard[ __NumeroMessage ] ) )
//     {
//         std::string __MessageComplet;
//         __MessageComplet << Synthese._MessageStandard[ __NumeroMessage ] << ";" << __Objet << ";" << __Donnees;
//         Ecrit( Synthese._NiveauMessageStandard[ __NumeroMessage ], __MessageComplet, Synthese._CodesMessageStandard[ __NumeroMessage ] );
//     }
// }

// void cLog::EcritElementTrajet( tNiveauLog __Niveau, const cElementTrajet* __ElementTrajet, const std::string& __Message, const std::string& __Code )
// {
//     std::string __MessageET;
//     std::string __MomentISO;
//     __MessageET << "cElementTrajet;";
//     if ( __ElementTrajet->getOrigin() )
//         __MessageET << __ElementTrajet->getOrigin() ->getLogicalPlace() ->getDesignationOD();
//     __MessageET << ";"
//     << __ElementTrajet->getOrigin() ->getRankInLogicalPlace() << ";";
//     __MomentISO.Vide();
//     // MJ review __MomentISO << __ElementTrajet->MomentDepart();
//     __MessageET << __MomentISO << ";";
//     if ( __ElementTrajet->getDestination() )
//         __MessageET << __ElementTrajet->getDestination() ->getLogicalPlace() ->getDesignationOD();
//     __MessageET << ";"
//     << __ElementTrajet->getDestination() ->getRankInLogicalPlace() << ";";
//     __MomentISO.Vide();
//     // MJ review__MomentISO << __ElementTrajet->MomentArrivee();
//     __MessageET << __MomentISO << ";";
//     if ( __ElementTrajet->getLigne() )
//         __MessageET << __ElementTrajet->getLigne() ->getCode();
//     __MessageET << ";";
//     __MessageET << __Message;

//     Ecrit( __Niveau, __MessageET, __Code );
// }

// void cLog::EcritTrajet( tNiveauLog __Niveau, const cTrajet& __Trajet, int __Numero, const std::string& __Message, const std::string& __Code )
// {
//     std::string __MessageTrajet;
//     std::stringSQL __MomentISO;

//     __MessageTrajet << "cTrajet;"
//     << __Numero << ";";
//     for ( int __i = 0; __i < NMAXPROFONDEUR; __i++ )
//     {
//         if ( __i < __Trajet.Taille() )
//             __MessageTrajet << "X";
//         __MessageTrajet << ";";
//     }
//     if ( __Trajet.Taille() )
//     {
//         __MessageTrajet << __Trajet.getOrigin() ->getLogicalPlace() ->getDesignationOD() << ";";
//         __MessageTrajet << __Trajet.PremierElement() ->getOrigin() ->getRankInLogicalPlace() << ";";
//         __MessageTrajet << __Trajet.PremierElement() ->getDistanceCarreeObjectif().DistanceCarree() << ";";
//         __MomentISO.Vide();
//         // MJ review__MomentISO << __Trajet.getMomentDepart();
//         __MessageTrajet << __MomentISO << ";";
//         __MessageTrajet << __Trajet.PremierElement() ->getLigne() ->getCode() << ";";

//         __MessageTrajet << __Trajet.getDestination() ->getLogicalPlace() ->getDesignationOD() << ";";
//         __MessageTrajet << __Trajet.DernierElement() ->getDestination() ->getRankInLogicalPlace() << ";";
//         __MessageTrajet << __Trajet.DernierElement() ->getDistanceCarreeObjectif().DistanceCarree() << ";";
//         __MomentISO.Vide();
//         // MJ reiview __MomentISO << __Trajet.getMomentArrivee();
//         __MessageTrajet << __MomentISO << ";";
//         __MessageTrajet << __Trajet.DernierElement() ->getLigne() ->getCode() << ";";
//     }
//     else
//         __MessageTrajet << ";;;;;;;;;;";
//     __MessageTrajet << __Message;

//     Ecrit( __Niveau, __MessageTrajet, __Code );

// }



// #include "std::stringSQL.h"

// bool cLog::ControleNiveau( tNiveauLog __Niveau ) const
// {
//     return __Niveau >= _Niveau;
// }

// void cLog::Fermer()
// {
//     _Fichier.close();
// }

// cLog::~cLog()
// {
//     Fermer();
// }


// void cLog::Ouvrir( const std::string& __NomFichier )
// {
//     _Fichier.open( __NomFichier.Texte(), ofstream::out | ofstream::app );
// }


// /*! \brief Consignation d'un événement de trajet dans le log
//  \param __Niveau Niveau de l'alerte
//  \param __Message Message complémentaire à ajouter
//  \param __Code Code de l'erreur
// */
// void cLog::Ecrit( tNiveauLog __Niveau, const std::string& __Message, const std::string& __Code )
// {
//     if ( ControleNiveau( __Niveau ) )
//     {
//         std::stringSQL __DateISO;
//         synthese::time::DateTime __Maintenant;
//         __Maintenant.updateDateTime();
//         // MJ review__DateISO << __Maintenant;

//         _Fichier << __DateISO << ";" << __Code << ";" << __Message << endl;
//         _Fichier.flush();
//     }
// }



// /*! \brief Consignation d'un élément de trajet dans le log
//  \param __Niveau Niveau de l'alerte
//  \param __ElementTrajet Element de trajet à consigner
//  \param __Message Message complémentaire à ajouter
//  \param __Code Code de l'erreur
// */
// void cLog::Ecrit( tNiveauLog __Niveau, const cElementTrajet* __ElementTrajet, const std::string& __Message, const std::string& __Code )
// {
//     if ( ControleNiveau( __Niveau ) )
//         EcritElementTrajet( __Niveau, __ElementTrajet, __Message, __Code );
// }

// void cLog::Ecrit( tNiveauLog __Niveau, const cTrajet& __Trajet, int __Numero, const std::string& __Message, const std::string& __Code )
// {
//     if ( ControleNiveau( __Niveau ) )
//         EcritTrajet( __Niveau, __Trajet, __Numero, __Message, __Code );
// }

// void cLog::SetNiveau( tNiveauLog __Niveau )
// {
//     _Niveau = __Niveau;
// }
