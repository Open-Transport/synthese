#include "14_geography/Projection.h"

#include "06_geometry/Point2D.h"
#include "06_geometry/Point3D.h"
#include "06_geometry/Angle.h"

#include <cmath>


using namespace synthese::geometry;


namespace synthese
{

namespace geography
{


    typedef struct {
	double lambda; // Longitude
	double phi;    // Latitude
	double he;     // Height above ellipsoid
    } LatLonHe;

    typedef struct {
	double x; 
	double y; 
	double z; 
    } XYZ;




double
Ntf2Pos (double p, double m, double s)
{
    return p + (m * 60.0 + s) / 3600.0;
}




double
ALG0001 (double phi, double e)
{
    double temp = ( 1 - ( e * sin ( phi ) ) ) / ( 1 + ( e * sin( phi ) ) );
    double L = log ( tan ( (M_PI_4) + (phi/2) ) * pow (temp, (e/2) ));
    return L;
}



double
ALG0002 (double L, double e, double epsilon)
{
    double phi_im1 (2 * atan (exp (L)) - M_PI_2);
    double phi_i;

    int i = 0;
    do
    {
        if (i != 0) phi_im1 = phi_i;
	++i;
        double temp = ( 1 + ( e * sin( phi_im1 ) ) ) / ( 1 - ( e * sin( phi_im1 ) ) );
        phi_i = 2 * atan ( pow (temp, (e/2)) * exp (L) ) - M_PI_2;
    }
    while (std::abs(phi_i - phi_im1) >= epsilon);

    return phi_i;
}



LatLonHe
ALG0004(double X, double Y, 
	double n, double c, 
	double Xs, double Ys, 
	double lambdac, double e, double epsilon)
{
    double R (sqrt( pow((X - Xs),2) + pow((Y - Ys),2) ));
    double gamma (atan((X - Xs)/(Ys - Y)));

    LatLonHe ll;
    ll.lambda = lambdac + (gamma / n);

    double L ((-1 / n) * log(std::abs(R/c)));

    ll.phi = ALG0002 (L,e,epsilon);

    return ll;
}




double
ALG0021(double phi,double a,double e)
{
    double N = a/sqrt( 1 - e * e * sin(phi) * sin(phi) );
    return N;
}

XYZ
ALG0009 (double lambda, double phi, double he, double a, double e)
{
    double N (ALG0021 (phi,a,e));
    
    XYZ coords;
    coords.x = (N + he) * cos(phi) * cos(lambda);
    coords.y = (N + he) * cos(phi) * sin(lambda);
    coords.z = (N * (1 - e*e)  + he) * sin (phi);

    return coords;
}



LatLonHe
ALG0012 (double X,double Y,double Z,double a,double e,double epsilon)
{
    LatLonHe ll;
    ll.lambda = atan (Y/X);

    double P = sqrt(X*X + Y*Y);

    double phi0 = atan (Z/ (P * (1 - ( (a*e*e)/sqrt(X*X + Y*Y + Z*Z) ) ) ) );
    double phi1;

    int i = 0;
    do
    {
        if (i != 0) phi0 = phi1;
        i++;
        double temp =  pow((1 - ( a * e*e * cos(phi0 )/( P * sqrt(1 - e*e * sin(phi0)*sin(phi0))))),-1);
        phi1 = atan( temp * Z / P );
    }
    while (std::abs(phi1 - phi0) >= epsilon);

    ll.he = (P/cos(phi1)) - (a/sqrt(1 - e*e * sin(phi1)*sin(phi1)));
    ll.phi = phi1;

    return ll;
}



XYZ
ALG0013 (double Tx,double Ty,double Tz,double D,double Rx,double Ry,double Rz, const XYZ& U)
{
    XYZ p;
    p.x = Tx + U.x * (1 + D) + U.z * Ry - U.y * Rz;
    p.y = Ty + U.y * (1 + D) + U.x * Rz - U.z * Rx;
    p.z = Tz + U.z * (1 + D) + U.y * Rx - U.x * Ry;

    return p;
}



    typedef struct {
	double e; 
	double n; 
	double C; 
	double lambdac; 
	double Xs; 
	double Ys; 
    } LambertParams;



LambertParams
ALG0019 (double lambda0,double phi0,double k0,double X0,double Y0,double a,double e)
{
    LambertParams lp;

    lp.lambdac = lambda0;
    lp.n = sin(phi0);
    lp.C = k0 * ALG0021(phi0,a,e) * tan (M_PI_2 - phi0) * exp ( lp.n * ALG0001(phi0,e) );
    lp.Xs = X0;
    lp.Ys = Y0 + k0 * ALG0021(phi0,a,e) * tan (M_PI_2 - phi0) ;
    lp.e = e;

    return lp;
}






LambertParams
ALG0054 (double lambda0,double phi0,double X0,double Y0,double phi1,double phi2,double a,double e)
{
    LambertParams lp;

    lp.lambdac = lambda0;
    lp.n = ( (log( (ALG0021(phi2,a,e)*cos(phi2))/(ALG0021(phi1,a,e)*cos(phi1)) ))/(ALG0001(phi1,e) - ALG0001(phi2,e) ));
    lp.C = ((ALG0021(phi1,a,e)* cos(phi1))/lp.n) * exp(lp.n * ALG0001(phi1,e));

    if (phi0 == (M_PI_2))
    {
        lp.Xs = X0;
        lp.Ys = Y0;
    }
    else
    {
        lp.Xs = X0;
        lp.Ys = Y0 + lp.C * exp(-1 * lp.n * ALG0001(phi0,e));
    }

    lp.e = e;

    return lp;
}




/*
geometry::Point2D 
Projection::ToLambertIIe (const GeoPoint& gp)
{
    
}
*/




GeoPoint 
FromLambertIIe (const geometry::Point2D& p)
{
    return FromLambert (LAMBERT_IIe, p);
}




GeoPoint 
FromLambert (const LambertOrigin& orig, const geometry::Point2D& p)
{
    double epsilon (0.00000000001);
    double n, c, Xs, Ys, lambdac, e, he, a, Tx, Ty, Tz, D, Rx, Ry, Rz;
    
    switch (orig)
    {
    case LAMBERT_II :
	n = 0.7289686274;
	c = 11745793.39;
	Xs = 600000;
	Ys = 6199695.768;
	lambdac = 0.04079234433; // Greenwich
	e = 0.08248325676; 
	he = 100;
	a = 6378249.2;
	Tx = -168;
	Ty = -60;
	Tz = +320;
	D  = 0;
	Rx = Ry = Rz = 0;
        break;
    case LAMBERT_IIe :
	n = 0.7289686274;
	c = 11745793.39;
	Xs = 600000;
	Ys = 8199695.768;
	lambdac = 0.04079234433; // Greenwich
	e = 0.08248325676; 
	he = 100;
	a = 6378249.2;
	Tx = -168;
	Ty = -60;
	Tz = +320;
	D  = 0;
	Rx = Ry = Rz = 0;
        break;
    case LAMBERT_93 :
	n = 0.7256077650;
	c = 11745255.426;
	Xs = 700000;
	Ys = 12655612.050;
	lambdac = 0.04079234433; // Greenwich	
	e = 0.08248325676; 
	he = 100;
	a = 6378249.2;
	Tx = -168;
	Ty = -60;
	Tz = +320;
	D  = 0;
	Rx = Ry = Rz = 0;
        break;
    }

    LatLonHe coords0 = ALG0004(p.getX (), p.getY (), n, c, Xs, Ys, lambdac, e, epsilon);
    XYZ coords1 = ALG0009 (coords0.lambda, coords0.phi, he, a, e);
    coords1 = ALG0013 (Tx,Ty,Tz,D,Rx,Ry,Rz,coords1);
    
    a = 6378137.0;    // ellipsoid WGS84
    double f = 1/298.257223563;
    double b = a*(1-f);
    e = sqrt((a*a - b*b)/(a*a));

    LatLonHe coords = ALG0012(coords1.x, coords1.y, coords1.z, a, e, epsilon);
    
    return GeoPoint (ToDegrees (coords.phi), ToDegrees (coords.lambda), coords.he);
}





}

}

