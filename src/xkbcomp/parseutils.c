/************************************************************
 Copyright (c) 1994 by Silicon Graphics Computer Systems, Inc.

 Permission to use, copy, modify, and distribute this
 software and its documentation for any purpose and without
 fee is hereby granted, provided that the above copyright
 notice appear in all copies and that both that copyright
 notice and this permission notice appear in supporting
 documentation, and that the name of Silicon Graphics not be
 used in advertising or publicity pertaining to distribution
 of the software without specific prior written permission.
 Silicon Graphics makes no representation about the suitability
 of this software for any purpose. It is provided "as is"
 without any express or implied warranty.

 SILICON GRAPHICS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL SILICON
 GRAPHICS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
 DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION  WITH
 THE USE OR PERFORMANCE OF THIS SOFTWARE.

 ********************************************************/

#include "parseutils.h"
#include "xkbmisc.h"
#include "xkbpath.h"
#include "xkbparse.h"

#include <X11/keysym.h>

ParseCommon *
AppendStmt(ParseCommon * to, ParseCommon * append)
{
    ParseCommon *start = to;

    if (append == NULL)
        return to;
    while ((to != NULL) && (to->next != NULL))
    {
        to = to->next;
    }
    if (to)
    {
        to->next = append;
        return start;
    }
    return append;
}

ExprDef *
ExprCreate(unsigned op, unsigned type)
{
    ExprDef *expr;
    expr = uTypedAlloc(ExprDef);
    if (expr)
    {
        expr->common.stmtType = StmtExpr;
        expr->common.next = NULL;
        expr->op = op;
        expr->type = type;
    }
    else
    {
        FATAL("Couldn't allocate expression in parser\n");
        /* NOTREACHED */
    }
    return expr;
}

ExprDef *
ExprCreateUnary(unsigned op, unsigned type, ExprDef * child)
{
    ExprDef *expr;
    expr = uTypedAlloc(ExprDef);
    if (expr)
    {
        expr->common.stmtType = StmtExpr;
        expr->common.next = NULL;
        expr->op = op;
        expr->type = type;
        expr->value.child = child;
    }
    else
    {
        FATAL("Couldn't allocate expression in parser\n");
        /* NOTREACHED */
    }
    return expr;
}

ExprDef *
ExprCreateBinary(unsigned op, ExprDef * left, ExprDef * right)
{
    ExprDef *expr;
    expr = uTypedAlloc(ExprDef);
    if (expr)
    {
        expr->common.stmtType = StmtExpr;
        expr->common.next = NULL;
        expr->op = op;
        if ((op == OpAssign) || (left->type == TypeUnknown))
            expr->type = right->type;
        else if ((left->type == right->type) || (right->type == TypeUnknown))
            expr->type = left->type;
        else
            expr->type = TypeUnknown;
        expr->value.binary.left = left;
        expr->value.binary.right = right;
    }
    else
    {
        FATAL("Couldn't allocate expression in parser\n");
        /* NOTREACHED */
    }
    return expr;
}

KeycodeDef *
KeycodeCreate(const char *name, unsigned long value)
{
    KeycodeDef *def;

    def = uTypedAlloc(KeycodeDef);
    if (def)
    {
        def->common.stmtType = StmtKeycodeDef;
        def->common.next = NULL;
        strncpy(def->name, name, XkbKeyNameLength);
        def->name[XkbKeyNameLength] = '\0';
        def->value = value;
    }
    else
    {
        FATAL("Couldn't allocate key name definition in parser\n");
        /* NOTREACHED */
    }
    return def;
}

KeyAliasDef *
KeyAliasCreate(const char *alias, const char *real)
{
    KeyAliasDef *def;

    def = uTypedAlloc(KeyAliasDef);
    if (def)
    {
        def->common.stmtType = StmtKeyAliasDef;
        def->common.next = NULL;
        strncpy(def->alias, alias, XkbKeyNameLength);
        def->alias[XkbKeyNameLength] = '\0';
        strncpy(def->real, real, XkbKeyNameLength);
        def->real[XkbKeyNameLength] = '\0';
    }
    else
    {
        FATAL("Couldn't allocate key alias definition in parser\n");
        /* NOTREACHED */
    }
    return def;
}

VModDef *
VModCreate(xkb_atom_t name, ExprDef * value)
{
    VModDef *def;
    def = uTypedAlloc(VModDef);
    if (def)
    {
        def->common.stmtType = StmtVModDef;
        def->common.next = NULL;
        def->name = name;
        def->value = value;
    }
    else
    {
        FATAL("Couldn't allocate variable definition in parser\n");
        /* NOTREACHED */
    }
    return def;
}

VarDef *
VarCreate(ExprDef * name, ExprDef * value)
{
    VarDef *def;
    def = uTypedAlloc(VarDef);
    if (def)
    {
        def->common.stmtType = StmtVarDef;
        def->common.next = NULL;
        def->name = name;
        def->value = value;
    }
    else
    {
        FATAL("Couldn't allocate variable definition in parser\n");
        /* NOTREACHED */
    }
    return def;
}

VarDef *
BoolVarCreate(xkb_atom_t nameToken, unsigned set)
{
    ExprDef *name, *value;

    name = ExprCreate(ExprIdent, TypeUnknown);
    name->value.str = nameToken;
    value = ExprCreate(ExprValue, TypeBoolean);
    value->value.uval = set;
    return VarCreate(name, value);
}

InterpDef *
InterpCreate(char *sym, ExprDef * match)
{
    InterpDef *def;

    def = uTypedAlloc(InterpDef);
    if (def)
    {
        def->common.stmtType = StmtInterpDef;
        def->common.next = NULL;
        def->sym = sym;
        def->match = match;
    }
    else
    {
        FATAL("Couldn't allocate interp definition in parser\n");
        /* NOTREACHED */
    }
    return def;
}

KeyTypeDef *
KeyTypeCreate(xkb_atom_t name, VarDef * body)
{
    KeyTypeDef *def;

    def = uTypedAlloc(KeyTypeDef);
    if (def)
    {
        def->common.stmtType = StmtKeyTypeDef;
        def->common.next = NULL;
        def->merge = MergeDefault;
        def->name = name;
        def->body = body;
    }
    else
    {
        FATAL("Couldn't allocate key type definition in parser\n");
        /* NOTREACHED */
    }
    return def;
}

SymbolsDef *
SymbolsCreate(const char *keyName, ExprDef *symbols)
{
    SymbolsDef *def;

    def = uTypedAlloc(SymbolsDef);
    if (def)
    {
        def->common.stmtType = StmtSymbolsDef;
        def->common.next = NULL;
        def->merge = MergeDefault;
        memset(def->keyName, 0, 5);
        strncpy(def->keyName, keyName, 4);
        def->symbols = symbols;
    }
    else
    {
        FATAL("Couldn't allocate symbols definition in parser\n");
        /* NOTREACHED */
    }
    return def;
}

GroupCompatDef *
GroupCompatCreate(int group, ExprDef * val)
{
    GroupCompatDef *def;

    def = uTypedAlloc(GroupCompatDef);
    if (def)
    {
        def->common.stmtType = StmtGroupCompatDef;
        def->common.next = NULL;
        def->merge = MergeDefault;
        def->group = group;
        def->def = val;
    }
    else
    {
        FATAL("Couldn't allocate group compat definition in parser\n");
        /* NOTREACHED */
    }
    return def;
}

ModMapDef *
ModMapCreate(uint32_t modifier, ExprDef * keys)
{
    ModMapDef *def;

    def = uTypedAlloc(ModMapDef);
    if (def)
    {
        def->common.stmtType = StmtModMapDef;
        def->common.next = NULL;
        def->merge = MergeDefault;
        def->modifier = modifier;
        def->keys = keys;
    }
    else
    {
        FATAL("Couldn't allocate mod mask definition in parser\n");
        /* NOTREACHED */
    }
    return def;
}

IndicatorMapDef *
IndicatorMapCreate(xkb_atom_t name, VarDef * body)
{
    IndicatorMapDef *def;

    def = uTypedAlloc(IndicatorMapDef);
    if (def)
    {
        def->common.stmtType = StmtIndicatorMapDef;
        def->common.next = NULL;
        def->merge = MergeDefault;
        def->name = name;
        def->body = body;
    }
    else
    {
        FATAL("Couldn't allocate indicator map definition in parser\n");
        /* NOTREACHED */
    }
    return def;
}

IndicatorNameDef *
IndicatorNameCreate(int ndx, ExprDef * name, bool virtual)
{
    IndicatorNameDef *def;

    def = uTypedAlloc(IndicatorNameDef);
    if (def)
    {
        def->common.stmtType = StmtIndicatorNameDef;
        def->common.next = NULL;
        def->merge = MergeDefault;
        def->ndx = ndx;
        def->name = name;
        def->virtual = virtual;
    }
    else
    {
        FATAL("Couldn't allocate indicator index definition in parser\n");
        /* NOTREACHED */
    }
    return def;
}

ExprDef *
ActionCreate(xkb_atom_t name, ExprDef * args)
{
    ExprDef *act;

    act = uTypedAlloc(ExprDef);
    if (act)
    {
        act->common.stmtType = StmtExpr;
        act->common.next = NULL;
        act->op = ExprActionDecl;
        act->value.action.name = name;
        act->value.action.args = args;
        return act;
    }
    FATAL("Couldn't allocate ActionDef in parser\n");
    return NULL;
}

static bool
ResizeKeysymList(ExprDef *list, unsigned int extra)
{
    int i;

    if (list->value.list.nSyms + extra > list->value.list.szSyms)
    {
        list->value.list.szSyms *= 2;
        list->value.list.szSyms += extra;
        if (list->value.list.szSyms == 1)
            list->value.list.szSyms = 4;
        list->value.list.syms = uTypedRecalloc(list->value.list.syms,
                                               list->value.list.nSyms,
                                               list->value.list.szSyms,
                                               char *);
        if (list->value.list.syms == NULL)
        {
            FATAL("Couldn't resize list of symbols for append\n");
            return false;
        }
    }
    if (list->value.list.nLevels >= list->value.list.szLevels)
    {
        list->value.list.szLevels *= 2;
        if (list->value.list.szLevels == 0)
            list->value.list.szLevels = 4;
        list->value.list.symsMapIndex =
            uTypedRecalloc(list->value.list.symsMapIndex,
                           list->value.list.nLevels,
                           list->value.list.szLevels,
                           int);
        if (list->value.list.symsMapIndex == NULL)
        {
            FATAL("Couldn't resize keysym index map for append\n");
            return false;
        }
        list->value.list.symsNumEntries =
            uTypedRecalloc(list->value.list.symsNumEntries,
                           list->value.list.nLevels,
                           list->value.list.szLevels,
                           unsigned int);
        if (list->value.list.symsNumEntries == NULL)
        {
            FATAL("Couldn't resize num keysym entries for append\n");
            return false;
        }
        for (i = list->value.list.nLevels; i < list->value.list.szLevels; i++)
            list->value.list.symsMapIndex[i] = -1;
    }

    return true;
}

ExprDef *
CreateKeysymList(char *sym)
{
    ExprDef *def;

    def = ExprCreate(ExprKeysymList, TypeSymbols);
    if (!def)
    {
        FATAL("Couldn't allocate expression for keysym list in parser\n");
        return NULL;
    }

    def->value.list.nSyms = 0;
    def->value.list.szSyms = 0;
    def->value.list.nLevels = 0;
    def->value.list.szLevels = 0;
    def->value.list.syms = NULL;
    def->value.list.symsMapIndex = NULL;
    def->value.list.symsNumEntries = NULL;

    if (!ResizeKeysymList(def, 1))
    {
        FreeStmt(&def->common);
        return NULL;
    }

    def->value.list.syms[0] = sym;
    def->value.list.symsMapIndex[0] = 0;
    def->value.list.symsNumEntries[0] = 1;
    def->value.list.nLevels = 1;
    def->value.list.nSyms = 1;

    return def;
}

ExprDef *
CreateMultiKeysymList(ExprDef *list)
{
    int i;

    for (i = 1; i < list->value.list.szLevels; i++)
    {
        list->value.list.symsMapIndex[i] = -1;
        list->value.list.symsNumEntries[i] = 0;
    }
    list->value.list.symsMapIndex[0] = 0;
    list->value.list.symsNumEntries[0] = list->value.list.nLevels;
    list->value.list.nLevels = 1;

    return list;
}

ExprDef *
AppendKeysymList(ExprDef * list, char *sym)
{
    if (!ResizeKeysymList(list, 1))
        return NULL;

    list->value.list.symsMapIndex[list->value.list.nLevels] =
        list->value.list.nSyms;
    list->value.list.symsNumEntries[list->value.list.nLevels] = 1;
    list->value.list.syms[list->value.list.nSyms++] = sym;
    list->value.list.nLevels++;
    return list;
}

ExprDef *
AppendMultiKeysymList(ExprDef * list, ExprDef * append)
{
    int i;

    if (!ResizeKeysymList(list, append->value.list.nSyms))
        return NULL;

    list->value.list.symsMapIndex[list->value.list.nLevels] =
        list->value.list.nSyms;
    list->value.list.symsNumEntries[list->value.list.nLevels] =
        append->value.list.nSyms;
    for (i = 0; i < append->value.list.nSyms; i++) {
        list->value.list.syms[list->value.list.nSyms++] =
            append->value.list.syms[i];
        append->value.list.syms[i] = NULL;
    }
    list->value.list.nLevels++;

    FreeStmt(&append->common);

    return list;
}

int
LookupKeysym(const char *str, xkb_keysym_t *sym_rtrn)
{
    xkb_keysym_t sym;

    if ((!str) || (strcasecmp(str, "any") == 0) ||
        (strcasecmp(str, "nosymbol") == 0))
    {
        *sym_rtrn = XKB_KEYSYM_NO_SYMBOL;
        return 1;
    }
    else if ((strcasecmp(str, "none") == 0) ||
             (strcasecmp(str, "voidsymbol") == 0))
    {
        *sym_rtrn = XK_VoidSymbol;
        return 1;
    }
    sym = xkb_string_to_keysym(str);
    if (sym != XKB_KEYSYM_NO_SYMBOL)
    {
        *sym_rtrn = sym;
        return 1;
    }
    return 0;
}

static void
FreeInclude(IncludeStmt *incl);

IncludeStmt *
IncludeCreate(char *str, unsigned merge)
{
    IncludeStmt *incl, *first;
    char *file, *map, *stmt, *tmp, *extra_data;
    char nextop;
    bool haveSelf;

    haveSelf = false;
    incl = first = NULL;
    file = map = NULL;
    tmp = str;
    stmt = uDupString(str);
    while ((tmp) && (*tmp))
    {
        if (XkbParseIncludeMap(&tmp, &file, &map, &nextop, &extra_data))
        {
            if ((file == NULL) && (map == NULL))
            {
                if (haveSelf)
                    goto BAIL;
                haveSelf = true;
            }
            if (first == NULL)
                first = incl = uTypedAlloc(IncludeStmt);
            else
            {
                incl->next = uTypedAlloc(IncludeStmt);
                incl = incl->next;
            }
            if (incl)
            {
                incl->common.stmtType = StmtInclude;
                incl->common.next = NULL;
                incl->merge = merge;
                incl->stmt = NULL;
                incl->file = file;
                incl->map = map;
                incl->modifier = extra_data;
                incl->path = NULL;
                incl->next = NULL;
            }
            else
            {
                WSGO("Allocation failure in IncludeCreate\n");
                ACTION("Using only part of the include\n");
                break;
            }
            if (nextop == '|')
                merge = MergeAugment;
            else
                merge = MergeOverride;
        }
        else
        {
            goto BAIL;
        }
    }
    if (first)
        first->stmt = stmt;
    else
        free(stmt);
    return first;

BAIL:
    ERROR("Illegal include statement \"%s\"\n", stmt);
    ACTION("Ignored\n");
    FreeInclude(first);
    free(stmt);
    return NULL;
}

void
CheckDefaultMap(XkbFile * maps, const char *fileName)
{
    XkbFile *dflt, *tmp;

    dflt = NULL;
    for (tmp = maps, dflt = NULL; tmp != NULL;
         tmp = (XkbFile *) tmp->common.next)
    {
        if (tmp->flags & XkbLC_Default)
        {
            if (dflt == NULL)
                dflt = tmp;
            else
            {
                if (warningLevel > 2)
                {
                    WARN("Multiple default components in %s\n",
                          (fileName ? fileName : "(unknown)"));
                    ACTION("Using %s, ignoring %s\n",
                            (dflt->name ? dflt->name : "(first)"),
                            (tmp->name ? tmp->name : "(subsequent)"));
                }
                tmp->flags &= (~XkbLC_Default);
            }
        }
    }
}

XkbFile *
CreateXKBFile(int type, char *name, ParseCommon * defs, unsigned flags)
{
    XkbFile *file;
    static int fileID;

    file = uTypedAlloc(XkbFile);
    if (file)
    {
        XkbcEnsureSafeMapName(name);
        memset(file, 0, sizeof(XkbFile));
        file->type = type;
        file->topName = uDupString(name);
        file->name = name;
        file->defs = defs;
        file->id = fileID++;
        file->flags = flags;
    }
    return file;
}

unsigned
StmtSetMerge(ParseCommon * stmt, unsigned merge, YYLTYPE *loc, void *scanner)
{
    if ((merge == MergeAltForm) && (stmt->stmtType != StmtKeycodeDef))
    {
        yyerror(loc, scanner, "illegal use of 'alternate' merge mode");
        merge = MergeDefault;
    }
    return merge;
}

static void
FreeExpr(ExprDef *expr)
{
    int i;

    if (!expr)
        return;

    switch (expr->op)
    {
    case ExprActionList:
    case OpNegate:
    case OpUnaryPlus:
    case OpNot:
    case OpInvert:
        FreeStmt(&expr->value.child->common);
        break;
    case OpDivide:
    case OpAdd:
    case OpSubtract:
    case OpMultiply:
    case OpAssign:
        FreeStmt(&expr->value.binary.left->common);
        FreeStmt(&expr->value.binary.right->common);
        break;
    case ExprActionDecl:
        FreeStmt(&expr->value.action.args->common);
        break;
    case ExprArrayRef:
        FreeStmt(&expr->value.array.entry->common);
        break;
    case ExprKeysymList:
        for (i = 0; i < expr->value.list.nSyms; i++)
            free(expr->value.list.syms[i]);
        free(expr->value.list.syms);
        free(expr->value.list.symsMapIndex);
        free(expr->value.list.symsNumEntries);
        break;
    default:
        break;
    }
}

static void
FreeInclude(IncludeStmt *incl)
{
    IncludeStmt *next;

    while (incl)
    {
        next = incl->next;

        free(incl->file);
        free(incl->map);
        free(incl->modifier);
        free(incl->path);
        free(incl->stmt);

        free(incl);
        incl = next;
    }
}

void
FreeStmt(ParseCommon *stmt)
{
    ParseCommon *next;
    YYSTYPE u;

    while (stmt)
    {
        next = stmt->next;
        u.any = stmt;

        switch (stmt->stmtType)
        {
        case StmtInclude:
            FreeInclude((IncludeStmt *)stmt);
            /* stmt is already free'd here. */
            stmt = NULL;
            break;
        case StmtExpr:
            FreeExpr(u.expr);
            break;
        case StmtVarDef:
            FreeStmt(&u.var->name->common);
            FreeStmt(&u.var->value->common);
            break;
        case StmtKeyTypeDef:
            FreeStmt(&u.keyType->body->common);
            break;
        case StmtInterpDef:
            free(u.interp->sym);
            FreeStmt(&u.interp->match->common);
            FreeStmt(&u.interp->def->common);
            break;
        case StmtVModDef:
            FreeStmt(&u.vmod->value->common);
            break;
        case StmtSymbolsDef:
            FreeStmt(&u.syms->symbols->common);
            break;
        case StmtModMapDef:
            FreeStmt(&u.modMask->keys->common);
            break;
        case StmtGroupCompatDef:
            FreeStmt(&u.groupCompat->def->common);
            break;
        case StmtIndicatorMapDef:
            FreeStmt(&u.ledMap->body->common);
            break;
        case StmtIndicatorNameDef:
            FreeStmt(&u.ledName->name->common);
            break;
        default:
            break;
        }

        free(stmt);
        stmt = next;
    }
}

void
FreeXKBFile(XkbFile *file)
{
    XkbFile *next;

    while (file)
    {
        next = (XkbFile *)file->common.next;

        switch (file->type)
        {
        case XkmKeymapFile:
        case XkmSemanticsFile:
        case XkmLayoutFile:
            FreeXKBFile((XkbFile *)file->defs);
            break;
        case XkmTypesIndex:
        case XkmCompatMapIndex:
        case XkmSymbolsIndex:
        case XkmKeyNamesIndex:
        case XkmGeometryIndex:
        case XkmGeometryFile:
            FreeStmt(file->defs);
            break;
        }

        free(file->name);
        free(file->topName);
        free(file);
        file = next;
    }
}
