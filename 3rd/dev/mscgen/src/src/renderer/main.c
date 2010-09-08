/***************************************************************************
 * This file is part of mscgen, a message sequence chart renderer.
 * Copyright (C) 2007 Michael C McTernan, Michael.McTernan.2001@cs.bris.ac.uk
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

/***************************************************************************
 * Include Files
 ***************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "cmdparse.h"
#include "adraw.h"
#include "msc.h"

/***************************************************************************
 * Types
 ***************************************************************************/

/** Structure for holding global options.
 * This structure groups all the options that affect the text output into
 * one structure.
 */
typedef struct GlobalOptionsTag
{
    /** Ideal width of output canvas.
     * If this value allows the entitySpacing to be increased, then
     * entitySpacing will be set to the larger value of it's original
     * value and idealCanvasWidth / number of entities.
     */
    unsigned int idealCanvasWidth;

    /** Horizontal spacing between entities. */
    unsigned int entitySpacing;

    /** Gap at the top of the page. */
    unsigned int entityHeadGap;

    /** Vertical spacing between arc. */
    unsigned int arcSpacing;

    /** Horizontal width of the arrow heads. */
    unsigned int arrowWidth;

    /** Vertical depth of the arrow heads. */
    unsigned int arrowHeight;

    /** Horizontal gap between text and horizonal lines. */
    unsigned int textHGapPre;

    /** Horizontal gap between text and horizonal lines. */
    unsigned int textHGapPost;
}
GlobalOptions;


/***************************************************************************
 * Local Variables.
 ***************************************************************************/

static Boolean gInputFilePresent = FALSE;
static char    gInputFile[4096];

static Boolean gOutputFilePresent = FALSE;
static char    gOutputFile[4096];

static Boolean gOutTypePresent = FALSE;
static char    gOutType[10];

static Boolean gDumpLicencePresent = FALSE;

static Boolean gPrintParsePresent = FALSE;

/** Command line switches.
 * This gives the command line switches that can be interpreted by mscgen.
 */
static CmdSwitch gClSwitches[] =
{
    {"-i",     &gInputFilePresent,  "%4096[^?]", gInputFile },
    {"-o",     &gOutputFilePresent, "%4096[^?]", gOutputFile },
    {"-T",     &gOutTypePresent,    "%10[^?]",   gOutType },
    {"-l",     &gDumpLicencePresent,NULL,        NULL },
    {"-p",     &gPrintParsePresent, NULL,        NULL }
};


static GlobalOptions gOpts =
{
    600,    /* idealCanvasWidth */

    80,     /* entitySpacing */
    20,     /* entityHeadGap */
    25,     /* arcSpacing */

    /* Arrow options */
    10, 6,

    /* textHGapPre, textHGapPost */
    2, 2
};

/** The drawing. */
static ADraw drw;

/** Name of a file to be removed by deleteTmp(). */
static char *deleteTmpFilename = NULL;

/***************************************************************************
 * Functions
 ***************************************************************************/

/** Delete the file named in deleteTmpFilename.
 * This function is registered with atexit() to delete a possible temporary
 * file used when generating image map files.
 */
static void deleteTmp()
{
    if(deleteTmpFilename)
    {
        unlink(deleteTmpFilename);
    }
}


/** Add a point to the output imagemap.
 * If \a ismap and \a url are non-NULL, this function will add a rectangle
 * to the imagemap according to the parameters passed.
 *
 * \param ismap  The file to which the imagemap should be rendered.
 * \param url    The URL to which the imagemap area should link.
 * \param x1     The x coordinate for the upper left point.
 * \param y2     The y coordinate for the upper left point.
 * \param x2     The x coordinate for the lower right point.
 * \param y2     The y coordinate for the lower right point.
 */
static void ismapRect(FILE        *ismap,
                      const char  *url,
                      unsigned int x1,
                      unsigned int y1,
                      unsigned int x2,
                      unsigned int y2)
{
    if(ismap && url)
    {
        assert(x1 <= x2); assert(y1 <= y2);

        fprintf(ismap,
                "rect %s %d,%d %d,%d\n",
                url,
                x1, y1,
                x2, y2);
    }
#if 0
    /* For debug render a cross onto the output */
    drw.line(&drw, x1, y1, x2, y2);
    drw.line(&drw, x2, y1, x1, y2);
#endif
}


/** Draw an arrow pointing to the right.
 * \param x     The x co-ordinate for the end point for the arrow head.
 * \param y     The y co-ordinate for the end point for the arrow head.
 * \param type  The arc type, which controls the format of the arrow head.
 */
static void arrowR(unsigned int x,
                   unsigned int y,
                   MscArcType   type)
{
    switch(type)
    {
        case MSC_ARC_SIGNAL: /* Unfilled half */
            drw.line(&drw,
                     x, y,
                     x - gOpts.arrowWidth, y + gOpts.arrowHeight);
            break;

        case MSC_ARC_METHOD: /* Filled */
        case MSC_ARC_RETVAL: /* Filled, dotted arc (not rendered here) */
            drw.filledTriangle(&drw,
                               x, y,
                               x - gOpts.arrowWidth, y + gOpts.arrowHeight,
                               x - gOpts.arrowWidth, y - gOpts.arrowHeight);
            break;

        case MSC_ARC_CALLBACK: /* Non-filled */
            drw.line(&drw,
                     x, y,
                     x - gOpts.arrowWidth, y + gOpts.arrowHeight);
            drw.line(&drw,
                     x, y,
                     x - gOpts.arrowWidth, y - gOpts.arrowHeight);
            break;

        default:
            assert(0);
            break;
    }
}


/** Draw an arrow pointing to the left.
 * \param x     The x co-ordinate for the end point for the arrow head.
 * \param y     The y co-ordinate for the end point for the arrow head.
 * \param type  The arc type, which controls the format of the arrow head.
 */
static void arrowL(unsigned int x,
                   unsigned int y,
                   MscArcType   type)
{
    switch(type)
    {
        case MSC_ARC_SIGNAL: /* Unfilled half */
            drw.line(&drw,
                     x, y,
                     x + gOpts.arrowWidth, y + gOpts.arrowHeight);
            break;

        case MSC_ARC_METHOD: /* Filled */
        case MSC_ARC_RETVAL: /* Filled, dotted arc (not rendered here) */
            drw.filledTriangle(&drw,
                               x, y,
                               x + gOpts.arrowWidth, y + gOpts.arrowHeight,
                               x + gOpts.arrowWidth, y - gOpts.arrowHeight);
            break;

        case MSC_ARC_CALLBACK: /* Non-filled */
            drw.line(&drw,
                     x, y,
                     x + gOpts.arrowWidth, y + gOpts.arrowHeight);
            drw.line(&drw,
                     x, y,
                     x + gOpts.arrowWidth, y - gOpts.arrowHeight);
            break;

        default:
            assert(0);
            break;
    }
}


/** Render some entity text.
 * Draw the text for some entity.
 * \param  m         The Msc for which the text is being rendered.
 * \param  ismap     If not \a NULL, write an ismap description here.
 * \param  x         The x position at which the entity text should be centered.
 * \param  y         The y position where the text should be placed
 * \param  entLabel  The label to render, which maybe \a NULL in which case
 *                     no ouput is produced.
 * \param  entUrl    The URL for rendering the label as a hyperlink.  This
 *                     maybe \a NULL if not required.
 * \param  entId     The text identifier for the arc.
 * \param  entIdUrl  The URL for rendering the test identifier as a hyperlink.
 *                     This maybe \a NULL if not required.
 */
static void entityText(Msc               m,
                       FILE             *ismap,
                       unsigned int      x,
                       unsigned int      y,
                       const char       *entLabel,
                       const char       *entUrl,
                       const char       *entId,
                       const char       *entIdUrl)
{
    if(entLabel)
    {
        unsigned int width = drw.textWidth(&drw, entLabel);

        /* Check if a URL is associated */
        if(entUrl)
        {
            drw.setPen(&drw, ADRAW_COL_BLUE);
            drw.textC (&drw, x, y, entLabel);
            drw.setPen(&drw, ADRAW_COL_BLACK);

            /* Image map output */
            ismapRect(ismap,
                      entUrl,
                      x - (width / 2), y - drw.textHeight(&drw),
                      x + (width / 2), y);
        }
        else
        {
            drw.textC(&drw, x, y, entLabel);
        }

        /* Render the Id of the arc, if specified */
        if(entId)
        {
            unsigned int idwidth;
            int          idx, idy;

            idy = y - drw.textHeight(&drw);
            idx = x + (width / 2);

            drw.setFontSize(&drw, ADRAW_FONT_TINY);

            idwidth = drw.textWidth(&drw, entId);
            idy    += (drw.textHeight(&drw) + 1) / 2;

            if(entIdUrl)
            {
                drw.setPen(&drw, ADRAW_COL_BLUE);
                drw.textR (&drw, idx, idy, entId);
                drw.setPen(&drw, ADRAW_COL_BLACK);

                /* Image map output */
                ismapRect(ismap,
                          entIdUrl,
                          idx, idy - drw.textHeight(&drw),
                          idx + idwidth, idy);
            }
            else
            {
                drw.textR(&drw, idx, idy, entId);
            }

            drw.setFontSize(&drw, ADRAW_FONT_SMALL);
        }
    }
}


/** Draw vertical lines stemming from entites.
 * This function will draw a single segment of the vertical line that
 * drops from an entity.
 * \param m      The \a Msc for which the lines are drawn
 * \param row    The row number indentifying the segment
 * \param dotted If #TRUE, produce a dotted line, otherwise solid.
 */
static void entityLines(Msc m, unsigned int row, Boolean dotted)
{
    const unsigned int ymin = (gOpts.arcSpacing * row) + gOpts.entityHeadGap;
    const unsigned int ymax = ymin + gOpts.arcSpacing;
    unsigned int t;

    for(t = 0; t < MscGetNumEntities(m); t++)
    {
        unsigned int x = (gOpts.entitySpacing / 2) + (gOpts.entitySpacing * t);

        if(dotted)
        {
            drw.dottedLine(&drw, x, ymin, x, ymax);
        }
        else
        {
            drw.line(&drw, x, ymin, x, ymax);
        }
    }

}


/** Count the number of lines in some string.
 * This counts line breaks that are written as a litteral '\n' in the line.
 *
 * \param[in] l  Pointer to the input string to inspect.
 * \retuns       The count of lines that should be output for the given string.
 */
static unsigned int countLines(const char *l)
{
    unsigned int c = 0;

    do
    {
        c++;

        l = strstr(l, "\\n");
        if(l) l += 2;
    }
    while(l != NULL);

    return c;
}


/** Get some line from a string containing '\n' delimiters.
 * Given a string that contains literal '\n' delimiters, return a subset in
 * a passed buffer that gives the nth line.
 *
 * \param[in] string  The string to parse.
 * \param[in] line    The line number to return from the string, which should
 *                     count from 0.
 * \param[in] out     Pointer to a buffer to fill with line data.
 * \param[in] outLen  The length of the buffer pointed to by \a out, in bytes.
 * \returns  A pointer to \a out.
 */
static char *getLine(const char        *string,
                     unsigned int       line,
                     char *const        out,
                     const unsigned int outLen)
{
    const char  *lineStart, *lineEnd;
    unsigned int lineLen;

    /* Setup for the loop */
    lineEnd = NULL;
    line++;

    do
    {
        /* Check if this is the first or a repeat iteration */
        if(lineEnd)
        {
            lineStart = lineEnd + 2;
        }
        else
        {
            lineStart = string;
        }

        /* Search for next delimited */
        lineEnd = strstr(lineStart, "\\n");

        line--;
    }
    while(line > 0 && lineEnd != NULL);

    /* Determine the length of the line */
    if(lineEnd != NULL)
    {
        lineLen = lineEnd - lineStart;
    }
    else
    {
        lineLen = strlen(string) - (lineStart - string);
    }

    /* Clamp the length to the buffer */
    if(lineLen > outLen - 1)
    {
        lineLen = outLen - 1;
    }

    /* Copy desired characters */
    memcpy(out, lineStart, lineLen);

    /* NULL terminate */
    out[lineLen] = '\0';

    return out;
}


/** Render text on an arc.
 * Draw the text on some arc.
 * \param  m         The Msc for which the text is being rendered.
 * \param  ismap     If not \a NULL, write an ismap description here.
 * \param  outwidth  Width of the output image.
 * \param  row       The row on which the text should be placed.
 * \param  startCol  The column at which the arc being labelled starts.
 * \param  endCol    The column at which the arc being labelled ends.
 * \param  arcLabel  The label to render.
 * \param  arcUrl    The URL for rendering the label as a hyperlink.  This
 *                     maybe \a NULL if not required.
 * \param  arcId     The text identifier for the arc.
 * \param  arcIdUrl  The URL for rendering the test identifier as a hyperlink.
 *                     This maybe \a NULL if not required.
 * \param  arcType   The type of arc, used to control output semantics.
 */
static void arcText(Msc               m,
                    FILE             *ismap,
                    unsigned int      outwidth,
                    unsigned int      row,
                    unsigned int      startCol,
                    unsigned int      endCol,
                    const char       *arcLabel,
                    const char       *arcUrl,
                    const char       *arcId,
                    const char       *arcIdUrl,
                    const MscArcType  arcType)
{
    unsigned int lines = countLines(arcLabel);
    unsigned int l;
    char         lineBuffer[1024];

    for(l = 0; l < lines; l++)
    {
        char *lineLabel = getLine(arcLabel, l, lineBuffer, sizeof(lineBuffer));
        unsigned int y = (gOpts.arcSpacing * row) +
                          gOpts.entityHeadGap + (gOpts.arcSpacing / 2) +
                          (l * drw.textHeight(&drw)) - l;
        unsigned int width = drw.textWidth(&drw, lineLabel);
        int x = ((startCol + endCol + 1) * gOpts.entitySpacing) / 2;

        /* Discontinunity arcs have central text, otherwise the
         *  label is above the rendered arc (or horizontally in the
         *  centre of a non-straight arc).
         */
        if(arcType == MSC_ARC_DISCO || arcType == MSC_ARC_DIVIDER)
        {
            y += drw.textHeight(&drw) / 2;
        }

        if(startCol != endCol)
        {
            /* Produce central aligned text */
            x -= width / 2;
        }
        else if(startCol < (MscGetNumEntities(m) / 2))
        {
            /* Form text to the right */
            x += gOpts.textHGapPre;
        }
        else
        {
            /* Form text to the left */
            x -= width + gOpts.textHGapPost;
        }

        /* Clip against edges of image */
        if(x + width > outwidth)
        {
            x = outwidth - width;
        }

        if(x < 0)
        {
            x = 0;
        }

        /* Check if a URL is associated */
        if(arcUrl)
        {
            drw.setPen(&drw, ADRAW_COL_BLUE);
            drw.textR (&drw, x, y, lineLabel);
            drw.setPen(&drw, ADRAW_COL_BLACK);

            /* Image map output */
            ismapRect(ismap,
                      arcUrl,
                      x, y - drw.textHeight(&drw),
                      x + width, y);
        }
        else
        {
            drw.textR(&drw, x, y, lineLabel);
        }

        /* Render the Id of the arc, if specified */
        if(arcId)
        {
            unsigned int idwidth;
            int          idx, idy;

            idy = y - drw.textHeight(&drw);
            idx = x + width;

            drw.setFontSize(&drw, ADRAW_FONT_TINY);

            idwidth = drw.textWidth(&drw, arcId);
            idy    += (drw.textHeight(&drw) + 1) / 2;

            if(arcIdUrl)
            {
                drw.setPen(&drw, ADRAW_COL_BLUE);
                drw.textR (&drw, idx, idy, arcId);
                drw.setPen(&drw, ADRAW_COL_BLACK);

                /* Image map output */
                ismapRect(ismap,
                          arcIdUrl,
                          idx, idy - drw.textHeight(&drw),
                          idx + idwidth, idy);
            }
            else
            {
                drw.textR(&drw, idx, idy, arcId);
            }

            drw.setFontSize(&drw, ADRAW_FONT_SMALL);
        }

        /* Dividers also have a horizontal line */
        if(arcType == MSC_ARC_DIVIDER)
        {
            const unsigned int margin = gOpts.entitySpacing / 4;

            y -= drw.textHeight(&drw) / 2;

            drw.dottedLine(&drw,
                           margin, y,
                           x - gOpts.textHGapPre,  y);
            drw.dottedLine(&drw,
                           x + width + gOpts.textHGapPost,
                           y,
                           (MscGetNumEntities(m) * gOpts.entitySpacing) - margin,
                           y);
        }
    }
}


/** Render the line and arrow head for some arc.
 * This will draw the arc line and arrow head between two columns,
 * noting that if the start and end column are the same, an arc is
 * rendered.
 * \param  m        The Msc for which the text is being rendered.
 * \param  row      Row in the output at which the arc should be rendered.
 * \param  startCol Starting column for the arc.
 * \param  endCol   Column at which the arc terminates.
 * \param  arcType  The type of the arc, which dictates its rendered style.
 */
static void arcLine(Msc               m,
                    unsigned int      row,
                    unsigned int      startCol,
                    unsigned int      endCol,
                    const MscArcType  arcType)
{
    const unsigned int y  = (gOpts.arcSpacing * row) +
                             gOpts.entityHeadGap + (gOpts.arcSpacing / 2);
    const unsigned int sx = (startCol * gOpts.entitySpacing) +
                             (gOpts.entitySpacing / 2);
    const unsigned int dx = (endCol * gOpts.entitySpacing) +
                             (gOpts.entitySpacing / 2);

    if(startCol != endCol)
    {
        /* Draw the line */
        if(arcType == MSC_ARC_RETVAL)
        {
            drw.dottedLine(&drw, sx, y, dx, y);
        }
        else
        {
            drw.line(&drw, sx, y, dx, y);
        }

        /* Now the arrow heads */
        if(startCol < endCol)
        {
            arrowR(dx, y, arcType);
        }
        else
        {
            arrowL(dx, y, arcType);
        }
    }
    else if(startCol < (MscGetNumEntities(m) / 2))
    {
        /* Arc looping to the left */
        if(arcType == MSC_ARC_RETVAL)
        {
            drw.dottedArc(&drw,
                          sx, y,
                          gOpts.entitySpacing,
                          gOpts.arcSpacing / 2,
                          90,
                          270);
        }
        else
        {
            drw.arc(&drw,
                    sx, y,
                    gOpts.entitySpacing,
                    gOpts.arcSpacing / 2,
                    90,
                    270);
        }

        arrowR(dx, y + (gOpts.arcSpacing / 4), arcType);
    }
    else
    {
        /* Arc looping to right */
        if(arcType == MSC_ARC_RETVAL)
        {
            drw.dottedArc(&drw,
                          sx, y,
                          gOpts.entitySpacing,
                          gOpts.arcSpacing / 2,
                          270,
                          90);
        }
        else
        {
            drw.arc(&drw,
                    sx, y,
                    gOpts.entitySpacing,
                    gOpts.arcSpacing / 2,
                    270,
                    90);
        }

        arrowL(dx, y + (gOpts.arcSpacing / 4), arcType);
    }
}


/** Print program usage and return.
 */
static void usage()
{
    printf(
"Usage: mscgen -T <type> [-i <infile>] -o <file>\n"
"       mscgen -l\n"
"\n"
"Where:\n"
" -T <type>   Specifies the output file type, which maybe one of 'png', 'eps',\n"
"             'svg' or 'ismap'\n"
" -i <infile> The file from which to read input.  If omitted or specified as\n"
"             '-', input will be read from stdin.\n"
" -o <file>   Write output to the named file.  This option must be specified.\n"
" -p          Print parsed msc output (for parser debug).\n"
" -l          Display program licence and exit.\n"
"\n"
"Mscgen version %1.01f, Copyright (C) 2007 Michael C McTernan,\n"
"                                       Michael.McTernan.2001@cs.bris.ac.uk\n"
"Mscgen comes with ABSOLUTELY NO WARRANTY.  This is free software, and you are\n"
"welcome to redistribute it under certain conditions; type `mscgen -l' for\n"
"details.\n",
VERSION);
}


/** Print program licence and return.
 */
static void licence()
{
    printf(
"Mscgen, a message sequence chart renderer.\n"
"Copyright (C) 2007 Michael C McTernan, Michael.McTernan.2001@cs.bris.ac.uk\n"
"\n"
"TTPCom Ltd., hereby disclaims all copyright interest in the program `mscgen'\n"
"(which renders message sequence charts) written by Michael McTernan.\n"
"\n"
"Rob Meades of TTPCom Ltd, 1 August 2005\n"
"Rob Meades, director of Software\n"
"\n"
"This program is free software; you can redistribute it and/or modify\n"
"it under the terms of the GNU General Public License as published by\n"
"the Free Software Foundation; either version 2 of the License, or\n"
"(at your option) any later version.\n"
"\n"
"This program is distributed in the hope that it will be useful,\n"
"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
"GNU General Public License for more details.\n"
"\n"
"You should have received a copy of the GNU General Public License\n"
"along with this program; if not, write to the Free Software\n"
"Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA\n");
}

int main(const int argc, const char *argv[])
{
    FILE            *ismap = NULL;
    ADrawOutputType  outType;
    char            *outImage;
    Msc              m;
    unsigned int     w, h, row, col;
    float            f;

    /* Parse the command line options */
    if(!CmdParse(gClSwitches, sizeof(gClSwitches) / sizeof(CmdSwitch), argc - 1, &argv[1]))
    {
        usage();
        return EXIT_FAILURE;
    }

    if(gDumpLicencePresent)
    {
        licence();
        return EXIT_SUCCESS;
    }

    /* Check that the ouput type was specified */
    if(!gOutTypePresent || !gOutputFilePresent)
    {
        fprintf(stderr, "-T <type> must be specified on the command line\n"
                        "-o <file> must be specified on the command line\n");
        usage();
        return EXIT_FAILURE;
    }

    /* Determine the output type */
    if(strcmp(gOutType, "png") == 0)
    {
        outType  = ADRAW_FMT_PNG;
        outImage = gOutputFile;
    }
    else if(strcmp(gOutType, "eps") == 0)
    {
        outType  = ADRAW_FMT_EPS;
        outImage = gOutputFile;
    }
    else if(strcmp(gOutType, "svg") == 0)
    {
        outType  = ADRAW_FMT_SVG;
        outImage = gOutputFile;
    }
    else if(strcmp(gOutType, "ismap") == 0)
    {
#ifdef __WIN32__
        /* When building for Windows , use the 'dangerous' tempnam()
         *  as mkstemp() isn't present.
         */
        outType  = ADRAW_FMT_PNG;
        outImage = tempnam(NULL, "png");
        if(!outImage)
        {
            perror("tempnam() failed");
            return EXIT_FAILURE;
        }

        /* Schedule the temp file to be deleted */
        deleteTmpFilename = outImage;
        atexit(deleteTmp);
#else
        static char tmpTemplate[] = "/tmp/mscgenXXXXXX";
        int h;

        outType  = ADRAW_FMT_PNG;
        outImage = tmpTemplate;

        /* Create temporary file */
        h = mkstemp(tmpTemplate);
        if(h == -1)
        {
            perror("mkstemp() failed");
            return EXIT_FAILURE;
        }

        /* Close the file handle */
        close(h);

        /* Schedule the temp file to be deleted */
        deleteTmpFilename = outImage;
        atexit(deleteTmp);
#endif
    }
    else
    {
        fprintf(stderr, "Unknown output format '%s'\n", gOutType);
        usage();
        return EXIT_FAILURE;
    }

    /* Parse input, either from a file, or stdin */
    if(gInputFilePresent && !strcmp(gInputFile, "-") == 0)
    {
        FILE *in = fopen(gInputFile, "r");

        if(!in)
        {
            fprintf(stderr, "Failed to open input file '%s'\n", gInputFile);
            return EXIT_FAILURE;
        }
        m = MscParse(in);
        fclose(in);
    }
    else
    {
        m = MscParse(stdin);
    }

    /* Check if the parse was okay */
    if(!m)
    {
        return EXIT_FAILURE;
    }

    /* Print the parse output if requested */
    if(gPrintParsePresent)
    {
      MscPrint(m);
    }

    /* Check if an ismap file should also be generated */
    if(strcmp(gOutType, "ismap") == 0)
    {
        ismap = fopen(gOutputFile, "w");
        if(!ismap)
        {
            fprintf(stderr, "Failed to open output file '%s':\n", gOutputFile);
            perror(NULL);
            return EXIT_FAILURE;
        }
    }


    if(MscGetOptAsFloat(m, MSC_OPT_HSCALE, &f))
    {
        gOpts.idealCanvasWidth *= f;
    }

    /* Work out the entitySpacing */
    if(gOpts.idealCanvasWidth / MscGetNumEntities(m) > gOpts.entitySpacing)
    {
        gOpts.entitySpacing = gOpts.idealCanvasWidth / MscGetNumEntities(m);
    }

    /* Work out the width and height of the canvas */
    w = MscGetNumEntities(m) * gOpts.entitySpacing;
    h = (MscGetNumArcs(m) * gOpts.arcSpacing) +
        gOpts.entityHeadGap;

    /* Open the output */
    if(!ADrawOpen(w, h, outImage, outType, &drw))
    {
        fprintf(stderr, "Failed to create output context\n");
        return EXIT_FAILURE;
    }


    /* Draw the entity headings */
    for(col = 0; col < MscGetNumEntities(m); col++)
    {
        unsigned int x = (gOpts.entitySpacing / 2) + (gOpts.entitySpacing * col);

        /* Titles */
        entityText(m,
                   ismap,
                   x,
                   (gOpts.entityHeadGap / 2) + (drw.textHeight(&drw) / 2),
                   MscGetCurrentEntAttrib(m, MSC_ATTR_LABEL),
                   MscGetCurrentEntAttrib(m, MSC_ATTR_URL),
                   MscGetCurrentEntAttrib(m, MSC_ATTR_ID),
                   MscGetCurrentEntAttrib(m, MSC_ATTR_IDURL));

        MscNextEntity(m);
    }

    /* Draw the arcs */
    for(row = 0; row < MscGetNumArcs(m); row++)
    {
        const MscArcType   arcType  = MscGetCurrentArcType(m);
        const char        *arcUrl   = MscGetCurrentArcAttrib(m, MSC_ATTR_URL);
        const char        *arcLabel = MscGetCurrentArcAttrib(m, MSC_ATTR_LABEL);
        const char        *arcId    = MscGetCurrentArcAttrib(m, MSC_ATTR_ID);
        const char        *arcIdUrl = MscGetCurrentArcAttrib(m, MSC_ATTR_IDURL);
        int                startCol, endCol;

        /* Get the entitiy indices */
        if(arcType != MSC_ARC_DISCO && arcType != MSC_ARC_DIVIDER)
        {
            startCol = MscGetEntityIndex(m, MscGetCurrentArcSource(m));
            endCol   = MscGetEntityIndex(m, MscGetCurrentArcDest(m));
        }
        else
        {
            /* Discontinuity arc spans whole chart */
            startCol = 0;
            endCol   = MscGetNumEntities(m) - 1;
        }


        /* Check that the start and end columns are known */
        if(startCol == -1)
        {
            fprintf(stderr,
                    "Unknown source entity '%s'\n",
                    MscGetCurrentArcSource(m));
            return EXIT_FAILURE;
        }

        if(endCol == -1)
        {
            fprintf(stderr,
                    "Unknown destination entity '%s'\n",
                    MscGetCurrentArcDest(m));
            return EXIT_FAILURE;
        }

        /* Check if it is a discontinunity arc or not */
        if(arcType == MSC_ARC_DISCO)
        {
            entityLines(m, row, TRUE /* dotted */);
        }
        else if (arcType == MSC_ARC_DIVIDER)
        {
            entityLines(m, row, FALSE);
        }
        else
        {
            entityLines(m, row, FALSE);
            arcLine    (m, row, startCol, endCol, arcType);
        }

        /* All may have text */
        if(arcLabel)
        {
            arcText(m, ismap, w, row, startCol, endCol, arcLabel,
                    arcUrl, arcId, arcIdUrl, arcType);
        }

        MscNextArc(m);
    }

    /* Close the image map if needed */
    if(ismap)
    {
        fclose(ismap);
    }

    /* Close the context */
    drw.close(&drw);

    return EXIT_SUCCESS;
}

/* END OF FILE */
