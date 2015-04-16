%{
/***************************************************************************
 * language.l : Mscgen language lexer definition.
 *
 * This file is part of msclib.
 *
 * Msc is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * Msclib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with msclib; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 ***************************************************************************/

#include <stdio.h>
#include <string.h>
#include "msc.h"
#include "language.tab.h"  /* Token definitions from Yacc/Bison */
     
/* Counter for error reporting */
static unsigned long lex_linenum = 1;

%}

/* Not used, so prevent compiler warning */
%option nounput	          

%%

\n            lex_linenum++;

#.*$          /* Ignore lines after '#' */
\/\/.*$       /* Ignore lines after '//' */
"/*".*"*/"    /* Ignore C style comments */
msc           return TOK_MSC;
HSCALE|hscale yylval.optType = MSC_OPT_HSCALE;    return TOK_OPT_HSCALE; 
URL|url       yylval.attribType = MSC_ATTR_URL;   return TOK_ATTR_URL;
LABEL|label   yylval.attribType = MSC_ATTR_LABEL; return TOK_ATTR_LABEL;
IDURL|idurl   yylval.attribType = MSC_ATTR_IDURL; return TOK_ATTR_IDURL;
ID|id         yylval.attribType = MSC_ATTR_ID;    return TOK_ATTR_ID;
[A-Za-z0-9_]+ yylval.string = strdup(yytext); return TOK_STRING;
\"[^\"]+\"    yylval.string = strdup(yytext + 1); yylval.string[strlen(yylval.string) - 1] = '\0'; return TOK_QSTRING;
\.\.\.        yylval.arctype = MSC_ARC_DISCO;    return TOK_SPECIAL_ARC;        /* ... */
---           yylval.arctype = MSC_ARC_DIVIDER;  return TOK_SPECIAL_ARC;        /* --- */
-\>           yylval.arctype = MSC_ARC_SIGNAL;   return TOK_REL_SIG_TO;         /* -> */
\<-           yylval.arctype = MSC_ARC_SIGNAL;   return TOK_REL_SIG_FROM;       /* <- */
=\>           yylval.arctype = MSC_ARC_METHOD;   return TOK_REL_METHOD_TO;      /* => */
\<=           yylval.arctype = MSC_ARC_METHOD;   return TOK_REL_METHOD_FROM;    /* <= */
\>\>          yylval.arctype = MSC_ARC_RETVAL;   return TOK_REL_RETVAL_TO;      /* >> */
\<\<          yylval.arctype = MSC_ARC_RETVAL;   return TOK_REL_RETVAL_FROM;    /* << */
=\>\>         yylval.arctype = MSC_ARC_CALLBACK; return TOK_REL_CALLBACK_TO;    /* =>> */
\<\<=         yylval.arctype = MSC_ARC_CALLBACK; return TOK_REL_CALLBACK_FROM;  /* <<= */
=             return TOK_EQUAL;
,             return TOK_COMMA;
\;            return TOK_SEMICOLON;
\{            return TOK_OCBRACKET;
\}            return TOK_CCBRACKET;
\[            return TOK_OSBRACKET;
\]            return TOK_CSBRACKET;
[ \t]+        /* ignore whitespace */;
%%

unsigned long lex_getlinenum(void)
{
    return lex_linenum;
}

/* END OF FILE */
