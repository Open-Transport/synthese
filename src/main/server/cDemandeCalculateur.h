/** @ingroup m33 */
class cDemandeCalculateur
{
        cAccesPADe _Depart;
        cAccesPADe _Arrivee;


        synthese::time::Date _Date
        bool _Aujourdhui;

        tNumeroPeriode _Periode;
        tBool3 _FiltreVelo;
        tBool3 _FiltreHandicape;
        cTarif* _FiltreTarif;
};
