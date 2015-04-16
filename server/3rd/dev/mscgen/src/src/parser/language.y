%{
/***************************************************************************
 * language.y : Grammar and parser for the mscgen language.
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
 * along with Foobar; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <msc.h>

/* Lexer prototype to prevent compiler warning */
int yylex(void);

/* Use verbose error reporting such that the expected token names are dumped */
#define YYERROR_VERBOSE

/* Name of parameter that is passed to yyparse() */
#define YYPARSE_PARAM yyparse_result

unsigned long lex_getlinenum(void);

/* yyerror
 *  Error handling function.  The TOK_XXX names are substituted for more
 *  understandable values that make more sense to the user.
 */
void yyerror(const char *str)
{
    static const char *tokNames[] = { "TOK_OCBRACKET", "TOK_CCBRACKET",
                                      "TOK_OSBRACKET", "TOK_CSBRACKET",
                                      "TOK_RELTO",     "TOK_RELFROM",
                                      "TOK_EQUAL",     "TOK_COMMA",
                                      "TOK_SEMICOLON", "TOK_MSC",
                                      "TOK_ATTR_LABEL","TOK_ATTR_URL",
                                      "TOK_ATTR_IDURL","TOK_ATTR_ID",
                                      "TOK_SPECIAL_ARC",
                                      "TOK_STRING",    "TOK_QSTRING",
                                      "TOK_OPT_HSCALE" };

    static const char *tokRepl[] =  { "{",             "}",
                                      "[",             "]",
                                      "->",            "<-",
                                      "=",             ",",
                                      ";",             "msc",
                                      "label",         "url",
                                      "idurl",         "id",
                                      "'...', '---'",
                                      "a string",      "a quoted string",
                                      "hscale" };
    static const int tokArrayLen = sizeof(tokNames) / sizeof(char *);

    char *s;
    int   t;

    /* Print standard message part */
    fprintf(stderr,"Error detected at line %lu: ", lex_getlinenum());

    /* Search for TOK */
    s = strstr(str, "TOK_");
    while(s != NULL)
    {
        int found = 0;

        /* Print out message until start of the token is found */
        while(str < s)
        {
            fprintf(stderr, "%c", *str);
            str++;
        }

        /* Look for the token name */
        for(t = 0; t < tokArrayLen && !found; t++)
        {
            if(strncmp(tokNames[t], str, strlen(tokNames[t])) == 0)
            {
                /* Dump the replacement string */
                fprintf(stderr, "'%s'", tokRepl[t]);

                /* Skip the token name */
                str += strlen(tokNames[t]);

                /* Exit the loop */
                found = 1;
            }
        }

        /* Check if a replacement was found */
        if(!found)
        {
            /* Dump the next char and skip it so that TOK doesn't match again */
            fprintf(stderr, "%c", *str);
            str++;
        }

        s = strstr(str, "TOK_");
    }

    fprintf(stderr, "%s.\n", str);

}

int yywrap()
{
        return 1;
}


extern FILE *yyin;
extern int   yyparse (void *YYPARSE_PARAM);


Msc MscParse(FILE *in)
{
    Msc m;

    yyin = in;

    /* Parse, and check that no errors are found */
    if(yyparse((void *)&m) == 0)
    {
        return m;
    }
    else
    {
        return NULL;
    }
}


%}

%token TOK_STRING TOK_QSTRING TOK_EQUAL TOK_COMMA TOK_SEMICOLON TOK_OCBRACKET TOK_CCBRACKET
       TOK_OSBRACKET TOK_CSBRACKET TOK_MSC
       TOK_ATTR_LABEL TOK_ATTR_URL TOK_ATTR_ID TOK_ATTR_IDURL
       TOK_REL_SIG_TO      TOK_REL_SIG_FROM
       TOK_REL_METHOD_TO   TOK_REL_METHOD_FROM
       TOK_REL_RETVAL_TO   TOK_REL_RETVAL_FROM
       TOK_REL_CALLBACK_TO TOK_REL_CALLBACK_FROM
       TOK_SPECIAL_ARC     TOK_OPT_HSCALE

%union
{
    char *string;
    Msc           msc;
    MscOpt        opt;
    MscOptType    optType;
    MscArc        arc;
    MscArcList    arclist;
    MscArcType    arctype;
    MscEntity     entity;
    MscEntityList entitylist;
    MscAttrib     attrib;
    MscAttribType attribType;
};

%type <msc>        msc
%type <opt>        optlist opt
%type <optType>    optval TOK_OPT_HSCALE
%type <arc>        arc arcrel
%type <arclist>    arclist
%type <entity>     entity
%type <entitylist> entitylist
%type <arctype>    relation_to relation_from
                   TOK_REL_SIG_TO TOK_REL_METHOD_TO TOK_REL_RETVAL_TO TOK_REL_CALLBACK_TO
                   TOK_REL_SIG_FROM TOK_REL_METHOD_FROM TOK_REL_RETVAL_FROM TOK_REL_CALLBACK_FROM
                   TOK_SPECIAL_ARC
%type <attrib>     attrlist attr
%type <attribType> attrval TOK_ATTR_LABEL TOK_ATTR_URL TOK_ATTR_ID TOK_ATTR_IDURL
%type <string>     string TOK_STRING TOK_QSTRING


%%
msc:          TOK_MSC TOK_OCBRACKET optlist TOK_SEMICOLON entitylist TOK_SEMICOLON arclist TOK_SEMICOLON TOK_CCBRACKET
{
    $$ = MscAlloc($3, $5, $7);
    *(Msc *)yyparse_result = $$;

}
           | TOK_MSC TOK_OCBRACKET entitylist TOK_SEMICOLON arclist TOK_SEMICOLON TOK_CCBRACKET
{
    $$ = MscAlloc(NULL, $3, $5);
    *(Msc *)yyparse_result = $$;

};

optlist:     opt
           | optlist TOK_COMMA opt
{
    $$ = MscLinkOpt($1, $3);
};

opt:         optval TOK_EQUAL string
{
    $$ = MscAllocOpt($1, $3)
};

optval:      TOK_OPT_HSCALE;

entitylist:   entity
{
    $$ = MscLinkEntity(NULL, $1);   /* Create new list */
}
            | entitylist TOK_COMMA entity
{
    $$ = MscLinkEntity($1, $3);     /* Add to existing list */
};



entity:       string
{
    $$ = MscAllocEntity($1);
}
            | entity TOK_OSBRACKET attrlist TOK_CSBRACKET
{
    MscEntityLinkAttrib($1, $3);
}
;

arclist:      arc
{
    $$ = MscLinkArc(NULL, $1);      /* Create new list */
}
              | arclist TOK_SEMICOLON arc
{
    $$ = MscLinkArc($1, $3);     /* Add to existing list */
};



arc:          arcrel TOK_OSBRACKET attrlist TOK_CSBRACKET
{
    MscArcLinkAttrib($1, $3);
}
              | arcrel;

arcrel:       TOK_SPECIAL_ARC
{
    $$ = MscAllocArc(NULL, NULL, $1);
}
            | string relation_to string
{
    $$ = MscAllocArc($1, $3, $2);
}
            | string relation_from string
{
    $$ = MscAllocArc($3, $1, $2);
};

relation_to:   TOK_REL_SIG_TO | TOK_REL_METHOD_TO | TOK_REL_RETVAL_TO | TOK_REL_CALLBACK_TO;
relation_from: TOK_REL_SIG_FROM | TOK_REL_METHOD_FROM | TOK_REL_RETVAL_FROM | TOK_REL_CALLBACK_FROM;

attrlist:    attr
           | attrlist TOK_COMMA attr
{
    $$ = MscLinkAttrib($1, $3);
};

attr:         attrval TOK_EQUAL string
{
    $$ = MscAllocAttrib($1, $3)
};

attrval:      TOK_ATTR_LABEL | TOK_ATTR_URL | TOK_ATTR_ID | TOK_ATTR_IDURL;


string: TOK_QSTRING | TOK_STRING
{
    $$ = $1;
};
%%


/* END OF FILE */
