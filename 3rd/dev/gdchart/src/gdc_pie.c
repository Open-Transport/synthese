/* GDCHART 0.11.3dev  GDC_PIE.C  11 Mar 2003 */
/* Copyright Bruce Verderaime 1998-2004 */

/* ELLIPSE (angled/perspective view)                                     */
/* TODO                                                                  */
/* 	view on/off/%/deg option should be tied to 3d_angle?                 */
/*		limited in two directions only - by X axis, or Y axis            */
/*		ELIPSEY [scaling] only one that make sense?                      */
/*	connections between face&background (effects Y axis only?)           */
/* 	sizing still doesn't work                                     - DONE */
/* 	label distances need to be variable about the ellipse/3d_angle       */
/* #define ELLIPSEX	1.00                                                 */
/* #define ELLIPSEY	0.30                                                 */

#define GDC_INCL
#define GDC_LIB
#include "gdc.h"	/* gdc.h before system includes to pick up features */

#include <stdio.h>

#include "gdcpie.h"

/* rem circle:  x = rcos(@), y = rsin(@)	*/

extern struct	GDC_FONT_T	GDC_fontc[];

#define SET_RECT( gdp, x1, x2, y1, y2 )	gdp[0].x = gdp[3].x = x1,	\
										gdp[0].y = gdp[1].y = y1,	\
										gdp[1].x = gdp[2].x = x2,	\
										gdp[2].y = gdp[3].y = y2

#define PX( x )				( cx + (int)( ((float)rad*ellipsex)*sin(pscl*(double)(x)) ) )		/* expects a val */
#define PY( x )				( cy - (int)( ((float)rad*ellipsey)*cos(pscl*(double)(x)) ) )		/* expects a val */

#define CX( i,d )		( cx                +	\
						  (d? xdepth_3D: 0) +	\
						  (int)( (double)(GDCPIE_explode?GDCPIE_explode[(i)]:0) * sin((double)(slice_angle[0][i])) ) )
#define CY( i,d )		( cy                -	\
						  (d? ydepth_3D: 0) -	\
						  (int)( (double)(GDCPIE_explode?GDCPIE_explode[(i)]:0) * cos((double)(slice_angle[0][i])) ) )
/* expect slice number:     i (index into slice_angle array) *\ 
 *   and position on slice: f (0: slice middle,              *
 *                             1: leading (clockwise),       *
 *                             2: trailing edge)             *
 *   and 3D depth:          d (0: do depth,                  *
 *                             1: no depth adjustment)       *
\* adjusts for explosion                                     */
#define IX( i,f,d )		( CX(i,d) + (int)( (double)rad*ellipsex * sin((double)(slice_angle[f][i])) ) )
#define IY( i,f,d )		( CY(i,d) - (int)( (double)rad*ellipsey * cos((double)(slice_angle[f][i])) ) )
/* same as above except o is angle */
#define OX( i,o,d )		( CX(i,d) + (int)( (double)rad*ellipsex * sin((double)(o)) ) )
#define OY( i,o,d )		( CY(i,d) - (int)( (double)rad*ellipsey * cos((double)(o)) ) )

#define TO_INT_DEG(o)		(int)rint( (double)((o)/(2.0*M_PI)*360.0) )
#define TO_INT_DEG_FLOOR(o)	(int)floor( (double)((o)/(2.0*M_PI)*360.0) )
#define TO_INT_DEG_CEIL(o)	(int)ceil( (double)((o)/(2.0*M_PI)*360.0) )
#define TO_RAD(o)			( (o)/360.0*(2.0*M_PI) )
																					/* assume !> 4*PI */
#define MOD_2PI(o)			( (o)>=(2.0*M_PI)? ((o)-(2.0*M_PI)): (((o)<0)? ((o)+(2.0*M_PI)): (o)) )
#define MOD_360(o)			( (o)>=360? (o)-360: (o) )								/* assume !> 720 */ 

struct tmp_slice_t { int	i;					/* original index */
					 char	hidden;				/* 'behind' top [3D] pie */
					 float	angle;				/* radian  */
					 float	slice; };			/* radian */
static float				pie_3D_rad;			/* user requested 3D angle in radians */

/* degrees (radians) between angle a, and depth angle */
/* 1&2, so comparisons can be done. */
#define RAD_DIST1( a )		( (dist_foo1=ABS(((a>-.00001&&a<.00001)?0.00001:a)-pie_3D_rad)), ((dist_foo1>M_PI)? ABS(dist_foo1-2.0*M_PI): dist_foo1) )
#define RAD_DIST2( a )		( (dist_foo2=ABS(((a>-.00001&&a<.00001)?0.00001:a)-pie_3D_rad)), ((dist_foo2>M_PI)? ABS(dist_foo2-2.0*M_PI): dist_foo2) )
static float				dist_foo1, dist_foo2;

/* ------------------------------------------------------- *\ 
 * oof!  cleaner way???
 * order by angle opposite (180) of depth angle
 * comparing across 0-360 line
\* ------------------------------------------------------- */
static int
ocmpr( struct tmp_slice_t *a,
	   struct tmp_slice_t *b )
{
	if( RAD_DIST1(a->angle) < RAD_DIST2(b->angle) )
		return 1;
	if( RAD_DIST1(a->angle) > RAD_DIST2(b->angle) )
		return -1;

	/* a tie (will happen between each slice) */
	/* are we within pie_3D_rad */
	if( (a->angle < pie_3D_rad) && (pie_3D_rad < a->slice) ||
		(a->slice < pie_3D_rad) && (pie_3D_rad < a->angle) )
		return 1;
	if( (b->slice < pie_3D_rad) && (pie_3D_rad < b->angle) ||
		(b->angle < pie_3D_rad) && (pie_3D_rad < b->slice) )
		return -1;

	/* let slice angle decide */
	if( RAD_DIST1(a->slice) < RAD_DIST2(b->slice) )
		return 1;
	if( RAD_DIST1(a->slice) > RAD_DIST2(b->slice) )
		return -1;

	return 0;
}

/* ======================================================= *\ 
 * PIE
 * 
 * Notes:
 *  always drawn from 12:00 position clockwise
 *  'missing' slices don't get labels
 *  sum(val[0], ... val[num_points-1]) is assumed to be 100%
\* ======================================================= */
void
GDC_out_pie( short			IMGWIDTH,
			 short			IMGHEIGHT,
			 FILE			*img_fptr,			/* open file pointer */
			 GDCPIE_TYPE	type,
			 int			num_points,
			 char			*lbl[],				/* data labels */
			 float			val[] )				/* data */
{
	int			i;

	gdImagePtr	im;
	int			BGColor,
				LineColor,
				PlotColor,
				EdgeColor,
				EdgeColorShd;
	CREATE_ARRAY1( SliceColor, int, num_points );		/* int SliceColor[num_points] */
	CREATE_ARRAY1( SliceColorShd, int, num_points );	/* int SliceColorShd[num_points] */

	float		rad = 0.0;					/* radius */
	float		ellipsex = 1.0;
	float		ellipsey = 1.0 - (float)GDCPIE_perspective/100.0;
	float		tot_val = 0.0;
	float		pscl;
	int			cx,							/* affects PX() */
				cy;							/* affects PY() */
								/* ~ 1% for a size of 100 pixs */
								/* label sizes will more dictate this */
	float		min_grphable = ( GDCPIE_other_threshold < 0?
								  100.0/(float)MIN(IMGWIDTH,IMGHEIGHT):
								  (float)GDCPIE_other_threshold )/100.0;
	short		num_slices1 = 0,
				num_slices2 = 0;
	char		any_too_small = FALSE;
	CREATE_ARRAY1( others, char, num_points );			/* char others[num_points] */
	CREATE_ARRAY2( slice_angle, float, 3, num_points );	/* float slice_angle[3][num_points] */
														/* must be used with others[] */
	char		threeD = ( type == GDC_3DPIE );

	int			xdepth_3D      = 0,			/* affects PX() */
				ydepth_3D      = 0;			/* affects PY() */
	int			do3Dx = 0,					/* reserved for macro use */
				do3Dy = 0;

	CREATE_ARRAY2( pct_lbl, char, num_points, 16 );			/* sizeof or strlen (GDCPIE_percent_fmt)? */
	CREATE_ARRAY1( pct_ftsz, struct fnt_sz_t, num_points );	/* struct fnt_sz_t lbl_ftsz[num_points] */
	CREATE_ARRAY1( lbl_ftsz, struct fnt_sz_t, num_points );	/* struct fnt_sz_t lbl_ftsz[num_points] */


#ifdef HAVE_LIBFREETYPE
	char			*gdcpie_title_font  = GDCPIE_title_font;
	char			*gdcpie_label_font  = GDCPIE_label_font;
	double			gdcpie_title_ptsize = GDCPIE_title_ptsize;
	double			gdcpie_label_ptsize = GDCPIE_label_ptsize;
#else
	char			*gdcpie_title_font  = NULL;
	char			*gdcpie_label_font  = NULL;
	double			gdcpie_title_ptsize = 0.0;
	double			gdcpie_label_ptsize = 0.0;
#endif

/*	GDCPIE_3d_angle = MOD_360(90-GDCPIE_3d_angle+360); */
	pie_3D_rad = TO_RAD( GDCPIE_3d_angle );

	xdepth_3D      = threeD? (int)( cos((double)MOD_2PI(M_PI_2-pie_3D_rad+2.0*M_PI)) * GDCPIE_3d_depth ): 0;
	ydepth_3D      = threeD? (int)( sin((double)MOD_2PI(M_PI_2-pie_3D_rad+2.0*M_PI)) * GDCPIE_3d_depth ): 0;
/*	xdepth_3D      = threeD? (int)( cos(pie_3D_rad) * GDCPIE_3d_depth ): 0; */
/*	ydepth_3D      = threeD? (int)( sin(pie_3D_rad) * GDCPIE_3d_depth ): 0; */

	load_font_conversions();

	/* ----- get total value ----- */
	for( i=0; i<num_points; ++i )
		tot_val += val[i];

	/* ----- pie sizing ----- */
	/* ----- make width room for labels, depth, etc.: ----- */
	/* ----- determine pie's radius ----- */
	{
	int		title_hgt  = GDCPIE_title? 1			/*  title? horizontal text line */
									   + GDCfnt_sz( GDCPIE_title,
													GDCPIE_title_size,
													gdcpie_title_font, gdcpie_title_ptsize, 0.0, NULL ).h

									   + 2:
									   0;
	float	last = 0.0;
	float	label_explode_limit = 0.0;
	int		cheight,
			cwidth;

	/* maximum: no labels, explosions */
	/* gotta start somewhere */
	rad = (float)MIN( (IMGWIDTH/2)/ellipsex-(1+ABS(xdepth_3D)), (IMGHEIGHT/2)/ellipsey-(1+ABS(ydepth_3D))-title_hgt );

	/* ok fix center, i.e., no floating re labels, explosion, etc. */
	cx = IMGWIDTH/2 /* - xdepth_3D */ ;
	cy = (IMGHEIGHT-title_hgt)/2 + title_hgt /* + ydepth_3D */ ;

	cheight = (IMGHEIGHT- title_hgt)/2 /* - ydepth_3D */ ;
	cwidth  = cx;

	/* walk around pie. determine spacing to edge */
	for( i=0; i<num_points; ++i )
		{
		float	this_pct = val[i]/tot_val;						/* should never be > 100% */
		float	this = this_pct*(2.0*M_PI);						/* pie-portion */
		if( (this_pct > min_grphable) ||						/* too small */
			(!GDCPIE_missing || !GDCPIE_missing[i]) )			/* still want angles */
			{
			int this_explode = GDCPIE_explode? GDCPIE_explode[i]: 0;
			double	this_sin;
			double	this_cos;
			slice_angle[0][i] = this/2.0+last;				/* mid-point on full pie */
			slice_angle[1][i] = last;						/* 1st on full pie */
			slice_angle[2][i] = this+last;					/* 2nd on full pie */
			this_sin        = ellipsex*sin( (double)slice_angle[0][i] );
			this_cos        = ellipsey*cos( (double)slice_angle[0][i] );

			if( !GDCPIE_missing || !(GDCPIE_missing[i]) )
				{
				short	lbl_wdth = 0,
						lbl_hgt  = 0;
				float	this_y_explode_limit,
						this_x_explode_limit;

				/* start slice label height, width     */
				/*  accounting for PCT placement, font */
				pct_ftsz[i].h = 0;
				pct_ftsz[i].w = 0;
				if( GDCPIE_percent_fmt &&
					GDCPIE_percent_labels != GDCPIE_PCT_NONE )
					{
					sprintf( pct_lbl[i], GDCPIE_percent_fmt, this_pct * 100.0 );
					pct_ftsz[i] = GDCfnt_sz( pct_lbl[i],
											 GDCPIE_label_size,
											 gdcpie_label_font, gdcpie_label_ptsize, 0.0, NULL );
					lbl_wdth = pct_ftsz[i].w;
					lbl_hgt  = pct_ftsz[i].h;
					}

				if( lbl && lbl[i] )
					{
					lbl_ftsz[i] = GDCfnt_sz( lbl[i],
											 GDCPIE_label_size,
											 gdcpie_label_font, gdcpie_label_ptsize, 0.0, NULL );

					if( GDCPIE_percent_labels == GDCPIE_PCT_ABOVE ||
						GDCPIE_percent_labels == GDCPIE_PCT_BELOW )
						{
						lbl_wdth = MAX( pct_ftsz[i].w, lbl_ftsz[i].w );
						lbl_hgt  = pct_ftsz[i].h + lbl_ftsz[i].h + 1;
						}
					else
					if( GDCPIE_percent_labels == GDCPIE_PCT_RIGHT ||
						GDCPIE_percent_labels == GDCPIE_PCT_LEFT )
						{
						lbl_wdth = pct_ftsz[i].w + lbl_ftsz[i].w + 1;
						lbl_hgt  = MAX( pct_ftsz[i].h, lbl_ftsz[i].h );
						}
					else /* GDCPIE_PCT_NONE */
						{
						lbl_wdth = lbl_ftsz[i].w;
						lbl_hgt  = lbl_ftsz[i].h;
						}
					}
				else
					lbl_wdth = lbl_hgt = 0;
				/* end label height, width */
				
				/* diamiter limited by this point's: explosion, label                 */
				/* (radius to box @ slice_angle) - (explode) - (projected label size) */
				/* radius constraint due to labels */
				this_y_explode_limit = (float)this_cos==0.0? FLT_MAX:
										(	(float)( (double)cheight/ABS(this_cos) ) - 
											(float)( this_explode + (lbl&&lbl[i]? GDCPIE_label_dist: 0) ) -
											(float)( lbl_hgt/2 ) / (float)ABS(this_cos)	);
				this_x_explode_limit = (float)this_sin==0.0? FLT_MAX:
										(	(float)( (double)cwidth/ABS(this_sin) ) - 
											(float)( this_explode + (lbl&&lbl[i]? GDCPIE_label_dist: 0) ) -
											(float)( lbl_wdth ) / (float)ABS(this_sin)	);

				rad = MIN( rad, this_y_explode_limit );
				rad = MIN( rad, this_x_explode_limit );

				/* ok at this radius (which is most likely larger than final) */
				/* adjust for inter-label spacing */
/*				if( lbl[i] && *lbl[i] ) */
/*					{ */
/*					char which_edge = slice_angle[0][i] > M_PI? +1: -1;		// which semi */
/*					last_label_yedge = cheight - (int)( (rad +				// top or bottom of label */
/*														(float)(this_explode + */
/*														(float)GDCPIE_label_dist)) * (float)this_cos ) + */
/*											     ( (GDC_fontc[GDCPIE_label_size].h+1)/2 + */
/*													GDC_label_spacing )*which_edge; */
/*					} */

				/* radius constriant due to exploded depth */
				/* at each edge of the slice, and the middle */
				/* this is really stupid */
				/*  this section uses a different algorithm then above, but does the same thing */
				/*  could be combined, but each is ugly enough! */
/* PROTECT /0 */
				if( threeD )
					{
					short	j;
					int		this_y_explode_pos;
					int		this_x_explode_pos;

					/* first N E S W (actually no need for N) */
					if( (slice_angle[1][i] < M_PI_2 && M_PI_2 < slice_angle[2][i]) &&				/* E */
						(this_x_explode_pos=OX(i,M_PI_2,1)) > cx+cwidth )
						rad -= (float)ABS( (double)(1+this_x_explode_pos-(cx+cwidth))/sin(M_PI_2) );
					if( (slice_angle[1][i] < 3.0*M_PI_2 && 3.0*M_PI_2 < slice_angle[2][i]) &&		/* W */
						(this_x_explode_pos=OX(i,3.0*M_PI_2,1)) < cx-cwidth )
						rad -= (float)ABS( (double)(this_x_explode_pos-(cx+cwidth))/sin(3.0*M_PI_2) );
					if( (slice_angle[1][i] < M_PI && M_PI < slice_angle[2][i]) &&					/* S */
						(this_y_explode_pos=OY(i,M_PI,1)) > cy+cheight )
						rad -= (float)ABS( (double)(1+this_y_explode_pos-(cy+cheight))/cos(M_PI) );

					for( j=0; j<3; ++j )
						{
						this_y_explode_pos = IY(i,j,1);
						if( this_y_explode_pos < cy-cheight )
							rad -= (float)ABS( (double)((cy-cheight)-this_y_explode_pos)/cos((double)slice_angle[j][i]) );
						if( this_y_explode_pos > cy+cheight )
							rad -= (float)ABS( (double)(1+this_y_explode_pos-(cy+cheight))/cos((double)slice_angle[j][i]) );

						this_x_explode_pos = IX(i,j,1);
						if( this_x_explode_pos < cx-cwidth )
							rad -= (float)ABS( (double)((cx-cwidth)-this_x_explode_pos)/sin((double)slice_angle[j][i]) );
						if( this_x_explode_pos > cx+cwidth )
							rad -= (float)ABS( (double)(1+this_x_explode_pos-(cx+cwidth))/sin((double)slice_angle[j][i]) );
						}
					}
				}
			others[i] = FALSE;
			}
		else
			{
			others[i] = TRUE;
			slice_angle[0][i] = -FLT_MAX;
			}
		last += this;
		}
	}

	/* ----- go ahead and start the image ----- */
	im = gdImageCreate( IMGWIDTH, IMGHEIGHT );

	/* --- allocate the requested colors --- */
	BGColor   = clrallocate( im, GDCPIE_BGColor );
	LineColor = clrallocate( im, GDCPIE_LineColor );
	PlotColor = clrallocate( im, GDCPIE_PlotColor );
	if( GDCPIE_EdgeColor != GDC_NOCOLOR )
	 {
	 EdgeColor = clrallocate( im, GDCPIE_EdgeColor );
	 if( threeD )
	  EdgeColorShd = clrshdallocate( im, GDCPIE_EdgeColor );
	 }

	/* --- set color for each slice --- */
	for( i=0; i<num_points; ++i )
		if( GDCPIE_Color )
			{
			unsigned long	slc_clr = GDCPIE_Color[i];

			SliceColor[i]     = clrallocate( im, slc_clr );
			if( threeD )
			 SliceColorShd[i] = clrshdallocate( im, slc_clr );
			}
		else
			{
			SliceColor[i]     = PlotColor;
			if( threeD )
			 SliceColorShd[i] = clrshdallocate( im, GDCPIE_PlotColor );
			}

	pscl = (2.0*M_PI)/tot_val;
	
	/* ----- calc: smallest a slice can be ----- */
	/* 1/2 circum / num slices per side. */
	/*              determined by number of labels that'll fit (height) */
	/* scale to user values */
	/* ( M_PI / (IMGHEIGHT / (SFONTHGT+1)) ) */
/*	min_grphable = tot_val / */
/*				   ( 2.0 * (float)IMGHEIGHT / (float)(SFONTHGT+1+TFONTHGT+2) ); */


	if( threeD )
		{
		/* draw background shaded pie */
		{
		float	rad1 = rad * 3.0/4.0;
		for( i=0; i<num_points; ++i )
			if( !(others[i]) &&
				(!GDCPIE_missing || !GDCPIE_missing[i]) )
				{
				int		edge_color = GDCPIE_EdgeColor == GDC_NOCOLOR? SliceColorShd[i]:
				                                                      EdgeColorShd;

				gdImageLine( im, CX(i,1), CY(i,1), IX(i,1,1), IY(i,1,1), edge_color );
				gdImageLine( im, CX(i,1), CY(i,1), IX(i,2,1), IY(i,2,1), edge_color );
				gdImageArc( im, CX(i,1), CY(i,1),
								(int)(rad*ellipsex*2.0), (int)(rad*ellipsey*2.0),
								TO_INT_DEG_FLOOR(slice_angle[1][i])+270,
								TO_INT_DEG_CEIL(slice_angle[2][i])+270,
								edge_color );
					
/*				gdImageFilledArc( im, CX(i,1), CY(i,1), */
/*									  rad*ellipsex*2, rad*ellipsey*2, */
/*									  TO_INT_DEG_FLOOR(slice_angle[1][i])+270, */
/*									  TO_INT_DEG_CEIL(slice_angle[2][i])+270, */
/*									  SliceColorShd[i], */
/*									  gdPie ); */
				/* attempt to fill, if slice is wide enough */
				if( (ABS(IX(i,1,1)-IX(i,2,1)) + ABS(IY(i,1,1)-IY(i,2,1))) > 2 )
					{
					float	rad = rad1;										/* local override */
					gdImageFillToBorder( im, IX(i,0,1), IY(i,0,1), edge_color, SliceColorShd[i] );
					}
				}
		}
		/* fill in connection to foreground pie */
		/* this is where we earn our keep */
		{
		int					t,
							num_slice_angles = 0;
		CREATE_ARRAY1( tmp_slice, struct tmp_slice_t, 4*num_points+4 );		/* should only need 2*num_points+2 */

		for( i=0; i<num_points; ++i )
			if( !GDCPIE_missing || !GDCPIE_missing[i] )
				{
				if( RAD_DIST1(slice_angle[1][i]) < RAD_DIST2(slice_angle[0][i]) )
					tmp_slice[num_slice_angles].hidden = FALSE;
				else
					tmp_slice[num_slice_angles].hidden = TRUE;
				tmp_slice[num_slice_angles].i       = i;
				tmp_slice[num_slice_angles].slice   = slice_angle[0][i];
				tmp_slice[num_slice_angles++].angle = slice_angle[1][i];
				if( RAD_DIST1(slice_angle[2][i]) < RAD_DIST2(slice_angle[0][i]) )
					tmp_slice[num_slice_angles].hidden = FALSE;
				else
					tmp_slice[num_slice_angles].hidden = TRUE;
				tmp_slice[num_slice_angles].i       = i;
				tmp_slice[num_slice_angles].slice   = slice_angle[0][i];
				tmp_slice[num_slice_angles++].angle = slice_angle[2][i];
				/* identify which 2 slices (i) have a tangent parallel to depth angle  */
				if( slice_angle[1][i]<MOD_2PI(pie_3D_rad+M_PI_2) && slice_angle[2][i]>MOD_2PI(pie_3D_rad+M_PI_2) )
					{
					tmp_slice[num_slice_angles].i       = i;
					tmp_slice[num_slice_angles].hidden  = FALSE;
					tmp_slice[num_slice_angles].slice   = slice_angle[0][i];
					tmp_slice[num_slice_angles++].angle = MOD_2PI( pie_3D_rad+M_PI_2 );
					}
				if( slice_angle[1][i]<MOD_2PI(pie_3D_rad+3.0*M_PI_2) && slice_angle[2][i]>MOD_2PI(pie_3D_rad+3.0*M_PI_2) )
					{
					tmp_slice[num_slice_angles].i       = i;
					tmp_slice[num_slice_angles].hidden  = FALSE;
					tmp_slice[num_slice_angles].slice   = slice_angle[0][i];
					tmp_slice[num_slice_angles++].angle = MOD_2PI( pie_3D_rad+3.0*M_PI_2 );
					}
				}

		qsort( tmp_slice, num_slice_angles, sizeof(struct tmp_slice_t), ocmpr );
		for( t=0; t<num_slice_angles; ++t )
			{
			gdPoint	gdp[4];

			i = tmp_slice[t].i;

			gdp[0].x  = CX(i,0);					gdp[0].y = CY(i,0);
			gdp[1].x  = CX(i,1);					gdp[1].y = CY(i,1);
			gdp[2].x  = OX(i,tmp_slice[t].angle,1);	gdp[2].y = OY(i,tmp_slice[t].angle,1);
			gdp[3].x  = OX(i,tmp_slice[t].angle,0);	gdp[3].y = OY(i,tmp_slice[t].angle,0);

			if( !(tmp_slice[t].hidden) )
				gdImageFilledPolygon( im, gdp, 4, SliceColorShd[i] );
			else
				{
				rad -= 2.0;										/* no peeking */
				gdp[0].x  = OX(i,slice_angle[0][i],0);	gdp[0].y = OY(i,slice_angle[0][i],0);
				gdp[1].x  = OX(i,slice_angle[0][i],1);	gdp[1].y = OY(i,slice_angle[0][i],1);
				rad += 2.0;
				gdp[2].x  = OX(i,slice_angle[1][i],1);	gdp[2].y = OY(i,slice_angle[1][i],1);
				gdp[3].x  = OX(i,slice_angle[1][i],0);	gdp[3].y = OY(i,slice_angle[1][i],0);
				gdImageFilledPolygon( im, gdp, 4, SliceColorShd[i] );
				gdp[2].x  = OX(i,slice_angle[2][i],1);	gdp[2].y = OY(i,slice_angle[2][i],1);
				gdp[3].x  = OX(i,slice_angle[2][i],0);	gdp[3].y = OY(i,slice_angle[2][i],0);
				gdImageFilledPolygon( im, gdp, 4, SliceColorShd[i] );
				}
				

			if( GDCPIE_EdgeColor != GDC_NOCOLOR )
				{
				gdImageLine( im, CX(i,0), CY(i,0), CX(i,1), CY(i,1), EdgeColorShd );
				gdImageLine( im, OX(i,tmp_slice[t].angle,0), OY(i,tmp_slice[t].angle,0),
								 OX(i,tmp_slice[t].angle,1), OY(i,tmp_slice[t].angle,1),
								 EdgeColorShd );
				}
			}
		FREE_ARRAY1( tmp_slice );
		}
		}


	/* ----- pie face ----- */
	{
	/* float	last = 0.0; */
	float	rad1 = rad * 3.0/4.0;
	for( i=0; i<num_points; ++i )
		if( !others[i] &&
			(!GDCPIE_missing || !GDCPIE_missing[i]) )
			{
			int		edge_color = GDCPIE_EdgeColor == GDC_NOCOLOR? SliceColor[i]:
																  EdgeColorShd;
			/* last += val[i]; */
			/* EXPLODE_CX_CY( slice_angle[0][i], i ); */
			gdImageLine( im, CX(i,0), CY(i,0), IX(i,1,0), IY(i,1,0), edge_color );
			gdImageLine( im, CX(i,0), CY(i,0), IX(i,2,0), IY(i,2,0), edge_color );
			gdImageArc( im, CX(i,0), CY(i,0), 
							(int)(rad*ellipsex*2.0), (int)(rad*ellipsey*2.0),
							(TO_INT_DEG_FLOOR(slice_angle[1][i])+270)%360,
							(TO_INT_DEG_CEIL(slice_angle[2][i])+270)%360,
							edge_color );
			/* antialiasing here */
			/* likely only on the face? */
/* bugs in gd2.0.0 */
/*	arc doesn't honor deg>360 */
/*	arcs from gdImageFilledArc() don't match with gdImageArc() */
/*	angles are off */
/*	doesn't always fill completely */
/*			gdImageFilledArc( im, CX(i,0), CY(i,0),  */
/*								  (int)(rad*ellipsex*2.0), (int)(rad*ellipsey*2.0), */
/*								  (TO_INT_DEG_FLOOR(slice_angle[1][i])+270)%360, */
/*								  (TO_INT_DEG_CEIL(slice_angle[2][i])+270)%360, */
/*								  SliceColor[i], */
/*								  gdPie ); */
			/* attempt to fill, if slice is wide enough */
			{
			float	rad = rad1;										/* local override */
			if( (ABS(IX(i,1,1)-IX(i,2,1)) + ABS(IY(i,1,1)-IY(i,2,1))) > 2 )
				{
				gdImageFillToBorder( im, IX(i,0,0), IY(i,0,0), edge_color, SliceColor[i] );
				}
			/* catch missed pixels on narrow slices */
			gdImageLine( im, CX(i,0), CY(i,0), IX(i,0,0), IY(i,0,0), SliceColor[i] );
			}
			}
	}

	if( GDCPIE_title )
		{
		struct fnt_sz_t	tftsz      = GDCfnt_sz( GDCPIE_title,
												GDCPIE_title_size,
												gdcpie_title_font, gdcpie_title_ptsize, 0.0, NULL );
		GDCImageStringNL( im,
						  &GDC_fontc[GDCPIE_title_size],
						  gdcpie_title_font, gdcpie_title_ptsize,
						  0.0,
						  IMGWIDTH/2 - tftsz.w/2,
						  1,
						  GDCPIE_title,
						  LineColor,
						  GDC_JUSTIFY_CENTER,
						  NULL );
		}

	/* labels */
	if( lbl )
		{
		float	liner = rad;

		rad += GDCPIE_label_dist;
		for( i=0; i<num_points; ++i )
			{
			if( !others[i] &&
				(!GDCPIE_missing || !GDCPIE_missing[i]) )
				{
				int		lblx,  pctx,
						lbly,  pcty,
						linex, liney;

				lbly = (liney = IY(i,0,0))-lbl_ftsz[i].h / 2;
				lblx = pctx = linex = IX(i,0,0);

				if( slice_angle[0][i] > M_PI )								/* which semicircle */
					{
					lblx -= lbl_ftsz[i].w;
					pctx = lblx;
					++linex;
					}
				else
					--linex;

				switch( GDCPIE_percent_labels )
					{
					case GDCPIE_PCT_LEFT:	if( slice_angle[0][i] > M_PI )
												pctx -= lbl_ftsz[i].w-1;
											else
												lblx += pct_ftsz[i].w+1;
											pcty = IY(i,0,0) - ( 1+pct_ftsz[i].h ) / 2;
											break;
					case GDCPIE_PCT_RIGHT:	if( slice_angle[0][i] > M_PI )
												lblx -= pct_ftsz[i].w-1;
											else
												pctx += lbl_ftsz[i].w+1;
											pcty = IY(i,0,0) - ( 1+pct_ftsz[i].h ) / 2;
											break;
					case GDCPIE_PCT_ABOVE:	lbly += (1+pct_ftsz[i].h) / 2;
											pcty = lbly - pct_ftsz[i].h;
											break;
					case GDCPIE_PCT_BELOW:	lbly -= (1+pct_ftsz[i].h) / 2;
											pcty = lbly + lbl_ftsz[i].h;
											break;
					case GDCPIE_PCT_NONE:
					default:;
					}

				if( GDCPIE_percent_labels != GDCPIE_PCT_NONE )
					GDCImageStringNL( im,
									  &GDC_fontc[GDCPIE_label_size],
									  gdcpie_label_font, gdcpie_label_ptsize,
									  0.0,
									  slice_angle[0][i] <= M_PI? pctx:
																 pctx+lbl_ftsz[i].w-pct_ftsz[i].w,
									  pcty,
									  pct_lbl[i],
									  LineColor,
									  GDC_JUSTIFY_CENTER,
									  NULL );
				if( lbl[i] )
					GDCImageStringNL( im,
									  &GDC_fontc[GDCPIE_label_size],
									  gdcpie_label_font, gdcpie_label_ptsize,
									  0.0,
									  lblx,
									  lbly,
									  lbl[i],
									  LineColor,
									  slice_angle[0][i] <= M_PI? GDC_JUSTIFY_LEFT:
																 GDC_JUSTIFY_RIGHT,
									  NULL );
				if( GDCPIE_label_line )
					{
					float	rad = liner;
					gdImageLine( im, linex, liney, IX(i,0,0), IY(i,0,0), LineColor );
					}
				}
			}
		rad -= GDCPIE_label_dist;
		}

		fflush( img_fptr );
		switch( GDC_image_type )
			{
#ifdef HAVE_JPEG
			case GDC_JPEG:	gdImageJpeg( im, img_fptr, GDC_jpeg_quality );	break;
#endif
			case GDC_WBMP:	gdImageWBMP( im, PlotColor, img_fptr );			break;
			case GDC_GIF:	gdImageGif( im, img_fptr);						break;
			case GDC_PNG:
			default:		gdImagePng( im, img_fptr );
			}

	FREE_ARRAY1( lbl_ftsz );
	FREE_ARRAY1( pct_ftsz );
	FREE_ARRAY2( pct_lbl );

	FREE_ARRAY2( slice_angle );
	FREE_ARRAY1( others );

	FREE_ARRAY1( SliceColorShd );
	FREE_ARRAY1( SliceColor );
	gdImageDestroy(im);
	return;
}
