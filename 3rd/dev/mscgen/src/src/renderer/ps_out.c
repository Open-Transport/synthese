/***************************************************************************
 * This file is part of mscgen, a message sequence chart renderer.
 * Copyright (C) 2005 Michael C McTernan, Michael.McTernan.2001@cs.bris.ac.uk
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 **************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "gd.h"
#include "gdfontt.h"  /* Tiny font */
#include "gdfonts.h"  /* Small font */
#include "adraw_int.h"

/***************************************************************************
 * Local types
 ***************************************************************************/

typedef struct PsContextTag
{
    /** Output file. */
    FILE        *of;

    /** Point size of the current font. */
    int          fontPoints;
}
PsContext;

typedef struct
{
    int capheight, xheight, ascender, descender;
    int widths[256];
}
PsCharMetric;

/** Helvetica character widths.
 * This gives the width of each character is 1/1000ths of a point.
 * The values are taken from the Adobe Font Metric file for Hevletica.
 */
static const PsCharMetric PsHelvetica =
{
    718, 523, 718, -207,
    {
       -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
       -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
       -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
       -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      278,  278,  355,  556,  556,  889,  667,  222,
      333,  333,  389,  584,  278,  333,  278,  278,
      556,  556,  556,  556,  556,  556,  556,  556,
      556,  556,  278,  278,  584,  584,  584,  556,
     1015,  667,  667,  722,  722,  667,  611,  778,
      722,  278,  500,  667,  556,  833,  722,  778,
      667,  778,  722,  667,  611,  722,  667,  944,
      667,  667,  611,  278,  278,  278,  469,  556,
      222,  556,  556,  500,  556,  556,  278,  556,
      556,  222,  222,  500,  222,  833,  556,  556,
      556,  556,  333,  500,  278,  556,  500,  722,
      500,  500,  500,  334,  260,  334,  584,   -1,
       -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
       -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
       -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
       -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
       -1,  333,  556,  556,  167,  556,  556,  556,
      556,  191,  333,  556,  333,  333,  500,  500,
       -1,  556,  556,  556,  278,   -1,  537,  350,
      222,  333,  333,  556, 1000, 1000,   -1,  611,
       -1,  333,  333,  333,  333,  333,  333,  333,
      333,   -1,  333,  333,   -1,  333,  333,  333,
     1000,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
       -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
       -1, 1000,   -1,  370,   -1,   -1,   -1,   -1,
      556,  778, 1000,  365,   -1,   -1,   -1,   -1,
       -1,  889,   -1,   -1,   -1,  278,   -1,   -1,
      222,  611,  944,  611,   -1,   -1,   -1,   -1
    }
};

/***************************************************************************
 * Helper functions
 ***************************************************************************/


/** Get the context pointer from an ADraw structure.
 */
static PsContext *getPsCtx(struct ADrawTag *ctx)
{
    return (PsContext *)ctx->internal;
}

/** Get the context pointer from an ADraw structure.
 */
static FILE *getPsFile(struct ADrawTag *ctx)
{
    return getPsCtx(ctx)->of;
}

/** Given a font metric measurement, return device dependent units.
 * Font metric data is stored as 1/1000th of a point, and therefore
 * needs to be multiplied by the font point size and divided by
 * 1000 to give a value in device dependent units.
 */
static int getSpace(struct ADrawTag *ctx, long thousanths)
{
    return (thousanths * getPsCtx(ctx)->fontPoints) / 1000;
}

/***************************************************************************
 * API Functions
 ***************************************************************************/

unsigned int PsTextWidth(struct ADrawTag *ctx,
                          const char *string)
{
    unsigned long width = 0;

    while(*string != '\0')
    {
        width += PsHelvetica.widths[(int)*string];
        string++;
    }

    return getSpace(ctx, width);
}


int PsTextHeight(struct ADrawTag *ctx)
{
    return getSpace(ctx, PsHelvetica.ascender - PsHelvetica.descender);
}


void PsLine(struct ADrawTag *ctx,
             unsigned int     x1,
             unsigned int     y1,
             unsigned int     x2,
             unsigned int     y2)
{
    fprintf(getPsFile(ctx),
            "newpath %d %d moveto %d %d lineto stroke\n",
            x1, -y1, x2, -y2);

}

void PsDottedLine(struct ADrawTag *ctx,
                   unsigned int     x1,
                   unsigned int     y1,
                   unsigned int     x2,
                   unsigned int     y2)
{
    fprintf(getPsFile(ctx), "[2] 0 setdash\n");
    PsLine(ctx, x1, y1, x2, y2);
    fprintf(getPsFile(ctx), "[] 0 setdash\n");
}


void PsTextR(struct ADrawTag *ctx,
              unsigned int     x,
              unsigned int     y,
              const char      *string)
{
    fprintf(getPsFile(ctx),
            "%d %d moveto (%s) show\n",
            x, -y - getSpace(ctx, PsHelvetica.descender), string);

}


void PsTextL(struct ADrawTag *ctx,
              unsigned int     x,
              unsigned int     y,
              const char      *string)
{
    fprintf(getPsFile(ctx),
            "%d %d moveto "
            "(%s) dup stringwidth "
            "pop "  /* Ignore y value */
            "neg "  /* Invert x value */
            "0 "
            "rmoveto "
            "show\n",
            x, -y - getSpace(ctx, PsHelvetica.descender), string);
}

void PsTextC(struct ADrawTag *ctx,
              unsigned int     x,
              unsigned int     y,
              const char      *string)
{
    fprintf(getPsFile(ctx),
                "%d %d moveto "
                "(%s) dup stringwidth "
                "pop "        /* Ignore y value */
                "2 div neg "  /* Invert and halve x value */
                "0 "
                "rmoveto "
                "show\n",
                x, -y - getSpace(ctx, PsHelvetica.descender), string);
}

void PsFilledTriangle(struct ADrawTag *ctx,
                       unsigned int x1,
                       unsigned int y1,
                       unsigned int x2,
                       unsigned int y2,
                       unsigned int x3,
                       unsigned int y3)
{
    fprintf(getPsFile(ctx),
            "newpath "
            "%d %d moveto "
            "%d %d lineto "
            "%d %d lineto "
            "closepath "
            "fill\n",
            x1, -y1,
            x2, -y2,
            x3, -y3);
}


void PsArc(struct ADrawTag *ctx,
            unsigned int cx,
            unsigned int cy,
            unsigned int w,
            unsigned int h,
            unsigned int s,
            unsigned int e)
{
    fprintf(getPsFile(ctx),
            "newpath "
            "%d %d %d %d %d %d ellipse "
            "stroke\n",
            cx, -cy, w, h, s, e);
}


void PsDottedArc(struct ADrawTag *ctx,
                  unsigned int cx,
                  unsigned int cy,
                  unsigned int w,
                  unsigned int h,
                  unsigned int s,
                  unsigned int e)
{
    fprintf(getPsFile(ctx), "[2] 0 setdash\n");
    PsArc(ctx, cx, cy, w, h, s, e);
    fprintf(getPsFile(ctx), "[] 0 setdash\n");
}


void PsSetPen (struct ADrawTag *ctx,
               ADrawColour      col)
{
    float r, g, b;

    switch(col)
    {
        case ADRAW_COL_WHITE:
            r = g = b = 1.0f;
            break;

        case ADRAW_COL_BLACK:
            r = g = b = 0.0f;
            break;

        case ADRAW_COL_BLUE:
            r = g = 0.0f;
            b = 1.0f;
            break;

        default:
            r = g = b = 0.0f;
            assert(0);

    }

    fprintf(getPsFile(ctx), "%f %f %f setrgbcolor\n", r ,g ,b);
}


void PsSetFontSize(struct ADrawTag *ctx,
                   ADrawFontSize    size)
{
    PsContext *context = getPsCtx(ctx);

    switch(size)
    {
        case ADRAW_FONT_TINY:
            getPsCtx(ctx)->fontPoints = 8;
            break;

        case ADRAW_FONT_SMALL:
            getPsCtx(ctx)->fontPoints = 10;
            break;

        default:
            assert(0);
    }

    fprintf(context->of, "/Helvetica findfont\n");
    fprintf(context->of, "%d scalefont\n", getPsCtx(ctx)->fontPoints);
    fprintf(context->of, "setfont\n");
}


Boolean PsClose(struct ADrawTag *ctx)
{
    PsContext *context = getPsCtx(ctx);

    /* Close the output file */
    fclose(context->of);

    /* Free and destroy context */
    free(context);
    ctx->internal = NULL;

    return TRUE;
}



Boolean PsInit(unsigned int     w,
               unsigned int     h,
               const char      *file,
               struct ADrawTag *outContext)
{
    PsContext *context;

    /* Create context */
    context = outContext->internal = malloc(sizeof(PsContext));
    if(context == NULL)
    {
        return FALSE;
    }

    /* Open the output file */
    context->of = fopen(file, "wb");
    if(!context->of)
    {
        fprintf(stderr, "PsInit: Failed to open output file '%s':\n", file);
        perror(NULL);
        return FALSE;
    }

    /* Write the header */
    fprintf(context->of, "%%!PS-Adobe-3.0 EPSF-2.0\n"
                         "%%%%BoundingBox: 0 0 %u %u\n", w, h);
    fprintf(context->of, "%%%%Creator: mscgen %0.0f\n", VERSION);
    fprintf(context->of, "%%%%EndComments\n");

    /* Shrink everything by 70% */
    fprintf(context->of, "0.70 0.70 scale\n");

    /* Create clipping rectangle to constrain dimensions */
    fprintf(context->of, "0 0 moveto\n");
    fprintf(context->of, "0 %u lineto\n", h);
    fprintf(context->of, "%u %u lineto\n", w, h);
    fprintf(context->of, "%u 0 lineto\n", w);
    fprintf(context->of, "closepath\n");
    fprintf(context->of, "clip\n");
    fprintf(context->of, "%%PageTrailer\n");
    fprintf(context->of, "%%Page: 1 1\n");

    /* Set default font */
    fprintf(context->of, "/Helvetica findfont\n");
    fprintf(context->of, "10 scalefont\n");
    fprintf(context->of, "setfont\n");

    /* Default to 10 point font */
    context->fontPoints = 10;

    /* Translate up by the height, y-axis will be inverted */
    fprintf(context->of, "0 %d translate\n", h);

    /* Arc drawing function */
    fprintf(context->of, "/mtrx matrix def\n"
                         "/ellipse\n"
                         "  { /endangle exch def\n"
                         "    /startangle exch def\n"
                         "    /ydia exch def\n"
                         "    /xdia exch def\n"
                         "    /y exch def\n"
                         "    /x exch def\n"
                         "    /savematrix mtrx currentmatrix def\n"
                         "    x y translate\n"
                         "    xdia 2 div ydia 2 div scale\n"
                         "    0 0 1 startangle endangle arc\n"
                         "    savematrix setmatrix\n"
                         "} def\n");

    /* Now fill in the function pointers */
    outContext->line           = PsLine;
    outContext->dottedLine     = PsDottedLine;
    outContext->textL          = PsTextL;
    outContext->textC          = PsTextC;
    outContext->textR          = PsTextR;
    outContext->textWidth      = PsTextWidth;
    outContext->textHeight     = PsTextHeight;
    outContext->filledTriangle = PsFilledTriangle;
    outContext->arc            = PsArc;
    outContext->dottedArc      = PsDottedArc;
    outContext->setPen         = PsSetPen;
    outContext->setFontSize    = PsSetFontSize;
    outContext->close          = PsClose;

    return TRUE;
}

/* END OF FILE */
