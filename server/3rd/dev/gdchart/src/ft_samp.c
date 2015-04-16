/* GDCHART 0.11.1b  3D Bar sample  17 June 2001 */

/* NOTE: fonts and locations specified are machine specific       */
/*       should be able to set font-search-path env GDFONTPATH    */
/*       see libgd documentation                                  */
/*       will default to builtin font, if specified TTF not found */

#include <stdio.h>
 
#include "gdc.h"
#include "gdchart.h"

#define NUM_SETS	1
#define NUM_POINTS	4

main()
{
    float           data  [ NUM_SETS ][ NUM_POINTS ];
    unsigned long   extclr[ NUM_SETS ][ NUM_POINTS ];
    char            *lbls[] = { "angle - 45", "font - zirkle", "ptsz - 10", "Color -  blue", "" };
 
    get_data( data, 0, 500 );
    get_individual_colors( extclr );

	GDC_BGColor = 0xFFFFFF;
	GDC_PlotColor = 0x4080FF;
    GDC_ExtColor  = &(extclr[0][0]);            /* set color option */
	GDC_title = "GDC_title\r\n(benjamingothic 12)";
	GDC_title_font = "/usr/share/enlightenment/E-docs/benjamingothic.ttf";
	GDC_title_ptsize = 12;

	GDC_xtitle = "GDC_xtitle\r\n(x-files 12)";
	GDC_xtitle_font = "/usr/share/enlightenment/E-docs/x-files.ttf";
	GDC_xtitle_ptsize = 12;

	GDC_ytitle = "GDC_ytitle\r\n(times 10 i)";
	GDC_ytitle_font = "/dos/C/windows/fonts/timesi.ttf";
	GDC_ytitle_ptsize = 10;

	GDC_xaxis_angle = 45.0;
	GDC_xaxis_font = "/usr/share/enlightenment/themes/DEFAULT/ttfonts/zirkle.ttf";
	GDC_xaxis_ptsize = 11.0;
	GDC_XLabelColor  = 0x000080;

    GDC_bar_width = 60;                         /* (%)              */
	GDC_image_type = GDC_PNG;

                                  /* ---- call the lib V0.95b ----- */
	GDC_out_graph( 400, 300,        /* width, height */
				 stdout,          /* open FILE pointer       */
				 GDC_3DBAR,       /* chart type              */
				 NUM_POINTS,      /* num points per data set */
				 lbls,            /* X labels array of char* */
				 1,        /* number of data sets     */
				 (float*)data,    /* data                    */
				 (float*)NULL );  /* no right-hand-axis data */

    exit(0);
}

/* --------------------------------------------------------- */
/* sample data gathering routine                             */
/* data can come from anywhere, generally a DB or data file  */
/* here it's randomly generated                              */
/* --------------------------------------------------------- */
#include <stdlib.h>     /* for rand() */
#include <time.h>       /* for seed   */
get_data( float data[NUM_SETS][NUM_POINTS],
          int	low,
          int	high )
{
    int i, j;
    srand( (unsigned int)time((time_t)NULL) );
    for( i=0; i<NUM_SETS; ++i )
        for( j=0; j<NUM_POINTS; ++j )
            /* random number between low & high */
            data[i][j] = 1.0+low+(high * rand()/(RAND_MAX+1.0));
}

/* -------- also random colors ----------------------------- */
get_individual_colors( unsigned long extclr[NUM_SETS][NUM_POINTS] )
{
    int i, j;
    for( i=0; i<NUM_SETS; ++i )
        for( j=0; j<NUM_POINTS; ++j )
            extclr[i][j] = (unsigned long)rand();
}


