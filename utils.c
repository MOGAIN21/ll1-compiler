#include "utils.h"
#include <stdlib.h>

char* getNonTerminal(NonTerminal nt) {
    switch (nt) {
        case NT_PROGRAM: return "NT_PROGRAM";
        case NT_MAINFUNCTION: return "NT_MAINFUNCTION";
        case NT_OTHERFUNCTIONS: return "NT_OTHERFUNCTIONS";
        case NT_FUNCTION: return "NT_FUNCTION";
        case NT_INPUT_PAR: return "NT_INPUT_PAR";
        case NT_OUTPUT_PAR: return "NT_OUTPUT_PAR";
        case NT_PARAMETER_LIST: return "NT_PARAMETER_LIST";
        case NT_DATATYPE: return "NT_DATATYPE";
        case NT_PRIMITIVEDATATYPE: return "NT_PRIMITIVEDATATYPE";
        case NT_CONSTRUCTEDDATATYPE: return "NT_CONSTRUCTEDDATATYPE";
        case NT_REMAINING_LIST: return "NT_REMAINING_LIST";
        case NT_STMTS: return "NT_STMTS";
        case NT_TYPEDEFINITIONS: return "NT_TYPEDEFINITIONS";
        case NT_ACTUALORREDEFINED: return "NT_ACTUALORREDEFINED";
        case NT_TYPEDEFINITION: return "NT_TYPEDEFINITION";
        case NT_FIELDDEFINITIONS: return "NT_FIELDDEFINITIONS";
        case NT_FIELDDEFINITION: return "NT_FIELDDEFINITION";
        case NT_FIELDTYPE: return "NT_FIELDTYPE";
        case NT_MOREFIELDS: return "NT_MOREFIELDS";
        case NT_DECLATRATIONS: return "NT_DECLATRATIONS";
        case NT_DECLATRATION: return "NT_DECLATRATION";
        case NT_GLOBAL_OR_NOT: return "NT_GLOBAL_OR_NOT";
        case NT_OTHERSTMTS: return "NT_OTHERSTMTS";
        case NT_STMT: return "NT_STMT";
        case NT_ASSIGNMENTSTMT: return "NT_ASSIGNMENTSTMT";
        case NT_SINGLEORRECID: return "NT_SINGLEORRECID";
        case NT_OPTION_SINGLE_CONSTRUCTED: return "NT_OPTION_SINGLE_CONSTRUCTED";
        case NT_ONEEXPANSION: return "NT_ONEEXPANSION";
        case NT_MOREEXPANSIONS: return "NT_MOREEXPANSIONS";
        case NT_FUNCALLSTMT: return "NT_FUNCALLSTMT";
        case NT_OUTPUTPARAMETERS: return "NT_OUTPUTPARAMETERS";
        case NT_INPUTPARAMETERS: return "NT_INPUTPARAMETERS";
        case NT_ITERATIVESTMT: return "NT_ITERATIVESTMT";
        case NT_CONDITIONALSTMT: return "NT_CONDITIONALSTMT";
        case NT_ELSEPART: return "NT_ELSEPART";
        case NT_IOSTMT: return "NT_IOSTMT";
        case NT_ARITHMETICEXPRESSION: return "NT_ARITHMETICEXPRESSION";
        case NT_EXPPRIME: return "NT_EXPPRIME";
        case NT_TERM: return "NT_TERM";
        case NT_TERMPRIME: return "NT_TERMPRIME";
        case NT_FACTOR: return "NT_FACTOR";
        case NT_HIGHPRECEDENCEOPERATORS: return "NT_HIGHPRECEDENCEOPERATORS";
        case NT_LOWPRECEDENCEOPERATORS: return "NT_LOWPRECEDENCEOPERATORS";
        case NT_BOOLEANEXPRESSION: return "NT_BOOLEANEXPRESSION";
        case NT_VAR: return "NT_VAR";
        case NT_LOGICALOP: return "NT_LOGICALOP";
        case NT_RELATIONALOP: return "NT_RELATIONALOP";
        case NT_RETURNSTMT: return "NT_RETURNSTMT";
        case NT_OPTIONALRETURN: return "NT_OPTIONALRETURN";
        case NT_IDLIST: return "NT_IDLIST";
        case NT_MORE_IDS: return "NT_MORE_IDS";
        case NT_DEFINETYPESTMT: return "NT_DEFINETYPESTMT";
        case NT_A: return "NT_A";
        default: return "UNKNOWN_NT";
    }
}

static ruleNode *makeRule(const grammar_element *specs, int count) {
    ruleNode *node = (ruleNode *)malloc(sizeof(ruleNode));
    if (!node) return NULL;
    node->elementCount = count;
    for (int i = 0; i < count; ++i) node->ruleSpecs[i] = specs[i];
    node->next = NULL;
    return node;
}

static void appendRule(grammar *G, NonTerminal A, const grammar_element *specs, int count) {
    ruleNode *node = makeRule(specs, count);
    if (!node) return;
    RuleHead *head = &G->productionRules[A];
    if (head->next == NULL) {
        head->next = node;
    } else {
        ruleNode *cur = head->next;
        while (cur->next) cur = cur->next;
        cur->next = node;
    }
    head->ruleCount += 1;
}

grammar initializeGrammar(void) {
    grammar G;
    for (int i = 0; i < NOOFNONTERMINALS; ++i) {
        G.productionRules[i].ruleCount = 0;
        G.productionRules[i].next = NULL;
        G.productionRules[i].containsEpsilonRule = false;
    }

    // Rules for NT_A
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_RECORD}) };
        appendRule(&G, NT_A, specs, 1);
    }
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_UNION}) };
        appendRule(&G, NT_A, specs, 1);
    }

    // Rules for NT_ACTUALORREDEFINED
    {
        const grammar_element specs[] = { ((grammar_element){.type=isNonTerminal, .element.nt=NT_TYPEDEFINITION}) };
        appendRule(&G, NT_ACTUALORREDEFINED, specs, 1);
    }
    {
        const grammar_element specs[] = { ((grammar_element){.type=isNonTerminal, .element.nt=NT_DEFINETYPESTMT}) };
        appendRule(&G, NT_ACTUALORREDEFINED, specs, 1);
    }

    // Rules for NT_ARITHMETICEXPRESSION
    {
        const grammar_element specs[] = { ((grammar_element){.type=isNonTerminal, .element.nt=NT_TERM}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_EXPPRIME}) };
        appendRule(&G, NT_ARITHMETICEXPRESSION, specs, 2);
    }

    // Rules for NT_ASSIGNMENTSTMT
    {
        const grammar_element specs[] = { ((grammar_element){.type=isNonTerminal, .element.nt=NT_SINGLEORRECID}), ((grammar_element){.type=isTerminal, .element.t=TK_ASSIGNOP}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_ARITHMETICEXPRESSION}), ((grammar_element){.type=isTerminal, .element.t=TK_SEM}) };
        appendRule(&G, NT_ASSIGNMENTSTMT, specs, 4);
    }

    // Rules for NT_BOOLEANEXPRESSION
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_OP}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_BOOLEANEXPRESSION}), ((grammar_element){.type=isTerminal, .element.t=TK_CL}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_LOGICALOP}), ((grammar_element){.type=isTerminal, .element.t=TK_OP}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_BOOLEANEXPRESSION}), ((grammar_element){.type=isTerminal, .element.t=TK_CL}) };
        appendRule(&G, NT_BOOLEANEXPRESSION, specs, 7);
    }
    {
        const grammar_element specs[] = { ((grammar_element){.type=isNonTerminal, .element.nt=NT_VAR}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_RELATIONALOP}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_VAR}) };
        appendRule(&G, NT_BOOLEANEXPRESSION, specs, 3);
    }
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_NOT}), ((grammar_element){.type=isTerminal, .element.t=TK_OP}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_BOOLEANEXPRESSION}), ((grammar_element){.type=isTerminal, .element.t=TK_CL}) };
        appendRule(&G, NT_BOOLEANEXPRESSION, specs, 4);
    }

    // Rules for NT_CONDITIONALSTMT
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_IF}), ((grammar_element){.type=isTerminal, .element.t=TK_OP}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_BOOLEANEXPRESSION}), ((grammar_element){.type=isTerminal, .element.t=TK_CL}), ((grammar_element){.type=isTerminal, .element.t=TK_THEN}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_STMT}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_OTHERSTMTS}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_ELSEPART}) };
        appendRule(&G, NT_CONDITIONALSTMT, specs, 8);
    }

    // Rules for NT_CONSTRUCTEDDATATYPE
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_RECORD}), ((grammar_element){.type=isTerminal, .element.t=TK_RUID}) };
        appendRule(&G, NT_CONSTRUCTEDDATATYPE, specs, 2);
    }
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_UNION}), ((grammar_element){.type=isTerminal, .element.t=TK_RUID}) };
        appendRule(&G, NT_CONSTRUCTEDDATATYPE, specs, 2);
    }
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_RUID}) };
        appendRule(&G, NT_CONSTRUCTEDDATATYPE, specs, 1);
    }

    // Rules for NT_DATATYPE
    {
        const grammar_element specs[] = { ((grammar_element){.type=isNonTerminal, .element.nt=NT_PRIMITIVEDATATYPE}) };
        appendRule(&G, NT_DATATYPE, specs, 1);
    }
    {
        const grammar_element specs[] = { ((grammar_element){.type=isNonTerminal, .element.nt=NT_CONSTRUCTEDDATATYPE}) };
        appendRule(&G, NT_DATATYPE, specs, 1);
    }

    // Rules for NT_DECLATRATION
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_TYPE}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_DATATYPE}), ((grammar_element){.type=isTerminal, .element.t=TK_COLON}), ((grammar_element){.type=isTerminal, .element.t=TK_ID}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_GLOBAL_OR_NOT}), ((grammar_element){.type=isTerminal, .element.t=TK_SEM}) };
        appendRule(&G, NT_DECLATRATION, specs, 6);
    }

    // Rules for NT_DECLATRATIONS
    {
        const grammar_element specs[] = { ((grammar_element){.type=isNonTerminal, .element.nt=NT_DECLATRATION}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_DECLATRATIONS}) };
        appendRule(&G, NT_DECLATRATIONS, specs, 2);
    }
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=EPSILLON}) };
        //appendRule(&G, NT_DECLATRATIONS, specs, 1);
    }
    // Epsilon rule for NT_DECLATRATIONS
    appendRule(&G, NT_DECLATRATIONS, NULL, 0);
    G.productionRules[NT_DECLATRATIONS].containsEpsilonRule = true;

    // Rules for NT_DEFINETYPESTMT
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_DEFINETYPE}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_A}), ((grammar_element){.type=isTerminal, .element.t=TK_RUID}), ((grammar_element){.type=isTerminal, .element.t=TK_AS}), ((grammar_element){.type=isTerminal, .element.t=TK_RUID}) };
        appendRule(&G, NT_DEFINETYPESTMT, specs, 5);
    }

    // Rules for NT_ELSEPART
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_ELSE}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_STMT}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_OTHERSTMTS}), ((grammar_element){.type=isTerminal, .element.t=TK_ENDIF}) };
        appendRule(&G, NT_ELSEPART, specs, 4);
    }
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_ENDIF}) };
        appendRule(&G, NT_ELSEPART, specs, 1);
    }

    // Rules for NT_EXPPRIME
    {
        const grammar_element specs[] = { ((grammar_element){.type=isNonTerminal, .element.nt=NT_LOWPRECEDENCEOPERATORS}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_TERM}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_EXPPRIME}) };
        appendRule(&G, NT_EXPPRIME, specs, 3);
    }
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=EPSILLON}) };
        //appendRule(&G, NT_EXPPRIME, specs, 1);
    }
    // Epsilon rule for NT_EXPPRIME
    appendRule(&G, NT_EXPPRIME, NULL, 0);
    G.productionRules[NT_EXPPRIME].containsEpsilonRule = true;

    // Rules for NT_FACTOR
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_OP}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_ARITHMETICEXPRESSION}), ((grammar_element){.type=isTerminal, .element.t=TK_CL}) };
        appendRule(&G, NT_FACTOR, specs, 3);
    }
    {
        const grammar_element specs[] = { ((grammar_element){.type=isNonTerminal, .element.nt=NT_VAR}) };
        appendRule(&G, NT_FACTOR, specs, 1);
    }

    // Rules for NT_FIELDDEFINITION
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_TYPE}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_FIELDTYPE}), ((grammar_element){.type=isTerminal, .element.t=TK_COLON}), ((grammar_element){.type=isTerminal, .element.t=TK_FIELDID}), ((grammar_element){.type=isTerminal, .element.t=TK_SEM}) };
        appendRule(&G, NT_FIELDDEFINITION, specs, 5);
    }

    // Rules for NT_FIELDDEFINITIONS
    {
        const grammar_element specs[] = { ((grammar_element){.type=isNonTerminal, .element.nt=NT_FIELDDEFINITION}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_FIELDDEFINITION}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_MOREFIELDS}) };
        appendRule(&G, NT_FIELDDEFINITIONS, specs, 3);
    }

    // Rules for NT_FIELDTYPE
    {
        const grammar_element specs[] = { ((grammar_element){.type=isNonTerminal, .element.nt=NT_PRIMITIVEDATATYPE}) };
        appendRule(&G, NT_FIELDTYPE, specs, 1);
    }
    {
        const grammar_element specs[] = { ((grammar_element){.type=isNonTerminal, .element.nt=NT_CONSTRUCTEDDATATYPE}) };
        appendRule(&G, NT_FIELDTYPE, specs, 1);
    }

    // Rules for NT_FUNCALLSTMT
    {
        const grammar_element specs[] = { ((grammar_element){.type=isNonTerminal, .element.nt=NT_OUTPUTPARAMETERS}), ((grammar_element){.type=isTerminal, .element.t=TK_CALL}), ((grammar_element){.type=isTerminal, .element.t=TK_FUNID}), ((grammar_element){.type=isTerminal, .element.t=TK_WITH}), ((grammar_element){.type=isTerminal, .element.t=TK_PARAMETERS}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_INPUTPARAMETERS}), ((grammar_element){.type=isTerminal, .element.t=TK_SEM}) };
        appendRule(&G, NT_FUNCALLSTMT, specs, 7);
    }

    // Rules for NT_FUNCTION
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_FUNID}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_INPUT_PAR}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_OUTPUT_PAR}), ((grammar_element){.type=isTerminal, .element.t=TK_SEM}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_STMTS}), ((grammar_element){.type=isTerminal, .element.t=TK_END}) };
        appendRule(&G, NT_FUNCTION, specs, 6);
    }

    // Rules for NT_GLOBAL_OR_NOT
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_COLON}), ((grammar_element){.type=isTerminal, .element.t=TK_GLOBAL}) };
        appendRule(&G, NT_GLOBAL_OR_NOT, specs, 2);
    }
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=EPSILLON}) };
        //appendRule(&G, NT_GLOBAL_OR_NOT, specs, 1);
    }
    // Epsilon rule for NT_GLOBAL_OR_NOT
    appendRule(&G, NT_GLOBAL_OR_NOT, NULL, 0);
    G.productionRules[NT_GLOBAL_OR_NOT].containsEpsilonRule = true;

    // Rules for NT_HIGHPRECEDENCEOPERATORS
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_MUL}) };
        appendRule(&G, NT_HIGHPRECEDENCEOPERATORS, specs, 1);
    }
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_DIV}) };
        appendRule(&G, NT_HIGHPRECEDENCEOPERATORS, specs, 1);
    }

    // Rules for NT_IDLIST
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_ID}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_MORE_IDS}) };
        appendRule(&G, NT_IDLIST, specs, 2);
    }

    // Rules for NT_INPUTPARAMETERS
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_SQL}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_IDLIST}), ((grammar_element){.type=isTerminal, .element.t=TK_SQR}) };
        appendRule(&G, NT_INPUTPARAMETERS, specs, 3);
    }

    // Rules for NT_INPUT_PAR
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_INPUT}), ((grammar_element){.type=isTerminal, .element.t=TK_PARAMETER}), ((grammar_element){.type=isTerminal, .element.t=TK_LIST}), ((grammar_element){.type=isTerminal, .element.t=TK_SQL}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_PARAMETER_LIST}), ((grammar_element){.type=isTerminal, .element.t=TK_SQR}) };
        appendRule(&G, NT_INPUT_PAR, specs, 6);
    }

    // Rules for NT_IOSTMT
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_READ}), ((grammar_element){.type=isTerminal, .element.t=TK_OP}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_VAR}), ((grammar_element){.type=isTerminal, .element.t=TK_CL}), ((grammar_element){.type=isTerminal, .element.t=TK_SEM}) };
        appendRule(&G, NT_IOSTMT, specs, 5);
    }
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_WRITE}), ((grammar_element){.type=isTerminal, .element.t=TK_OP}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_VAR}), ((grammar_element){.type=isTerminal, .element.t=TK_CL}), ((grammar_element){.type=isTerminal, .element.t=TK_SEM}) };
        appendRule(&G, NT_IOSTMT, specs, 5);
    }

    // Rules for NT_ITERATIVESTMT
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_WHILE}), ((grammar_element){.type=isTerminal, .element.t=TK_OP}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_BOOLEANEXPRESSION}), ((grammar_element){.type=isTerminal, .element.t=TK_CL}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_STMT}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_OTHERSTMTS}), ((grammar_element){.type=isTerminal, .element.t=TK_ENDWHILE}) };
        appendRule(&G, NT_ITERATIVESTMT, specs, 7);
    }

    // Rules for NT_LOGICALOP
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_AND}) };
        appendRule(&G, NT_LOGICALOP, specs, 1);
    }
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_OR}) };
        appendRule(&G, NT_LOGICALOP, specs, 1);
    }

    // Rules for NT_LOWPRECEDENCEOPERATORS
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_PLUS}) };
        appendRule(&G, NT_LOWPRECEDENCEOPERATORS, specs, 1);
    }
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_MINUS}) };
        appendRule(&G, NT_LOWPRECEDENCEOPERATORS, specs, 1);
    }

    // Rules for NT_MAINFUNCTION
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_MAIN}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_STMTS}), ((grammar_element){.type=isTerminal, .element.t=TK_END}) };
        appendRule(&G, NT_MAINFUNCTION, specs, 3);
    }

    // Rules for NT_MOREEXPANSIONS
    {
        const grammar_element specs[] = { ((grammar_element){.type=isNonTerminal, .element.nt=NT_ONEEXPANSION}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_MOREEXPANSIONS}) };
        appendRule(&G, NT_MOREEXPANSIONS, specs, 2);
    }
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=EPSILLON}) };
        //appendRule(&G, NT_MOREEXPANSIONS, specs, 1);
    }
    // Epsilon rule for NT_MOREEXPANSIONS
    appendRule(&G, NT_MOREEXPANSIONS, NULL, 0);
    G.productionRules[NT_MOREEXPANSIONS].containsEpsilonRule = true;

    // Rules for NT_MOREFIELDS
    {
        const grammar_element specs[] = { ((grammar_element){.type=isNonTerminal, .element.nt=NT_FIELDDEFINITION}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_MOREFIELDS}) };
        appendRule(&G, NT_MOREFIELDS, specs, 2);
    }
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=EPSILLON}) };
        //appendRule(&G, NT_MOREFIELDS, specs, 1);
    }
    // Epsilon rule for NT_MOREFIELDS
    appendRule(&G, NT_MOREFIELDS, NULL, 0);
    G.productionRules[NT_MOREFIELDS].containsEpsilonRule = true;

    // Rules for NT_MORE_IDS
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_COMMA}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_IDLIST}) };
        appendRule(&G, NT_MORE_IDS, specs, 2);
    }
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=EPSILLON}) };
        //appendRule(&G, NT_MORE_IDS, specs, 1);
    }
    // Epsilon rule for NT_MORE_IDS
    appendRule(&G, NT_MORE_IDS, NULL, 0);
    G.productionRules[NT_MORE_IDS].containsEpsilonRule = true;

    // Rules for NT_ONEEXPANSION
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_DOT}), ((grammar_element){.type=isTerminal, .element.t=TK_FIELDID}) };
        appendRule(&G, NT_ONEEXPANSION, specs, 2);
    }

    // Rules for NT_OPTIONALRETURN
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_SQL}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_IDLIST}), ((grammar_element){.type=isTerminal, .element.t=TK_SQR}) };
        appendRule(&G, NT_OPTIONALRETURN, specs, 3);
    }
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=EPSILLON}) };
        //appendRule(&G, NT_OPTIONALRETURN, specs, 1);
    }
    // Epsilon rule for NT_OPTIONALRETURN
    appendRule(&G, NT_OPTIONALRETURN, NULL, 0);
    G.productionRules[NT_OPTIONALRETURN].containsEpsilonRule = true;

    // Rules for NT_OPTION_SINGLE_CONSTRUCTED
    {
        const grammar_element specs[] = { ((grammar_element){.type=isNonTerminal, .element.nt=NT_ONEEXPANSION}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_MOREEXPANSIONS}) };
        appendRule(&G, NT_OPTION_SINGLE_CONSTRUCTED, specs, 2);
    }
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=EPSILLON}) };
        //appendRule(&G, NT_OPTION_SINGLE_CONSTRUCTED, specs, 1);
    }
    // Epsilon rule for NT_OPTION_SINGLE_CONSTRUCTED
    appendRule(&G, NT_OPTION_SINGLE_CONSTRUCTED, NULL, 0);
    G.productionRules[NT_OPTION_SINGLE_CONSTRUCTED].containsEpsilonRule = true;

    // Rules for NT_OTHERFUNCTIONS
    {
        const grammar_element specs[] = { ((grammar_element){.type=isNonTerminal, .element.nt=NT_FUNCTION}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_OTHERFUNCTIONS}) };
        appendRule(&G, NT_OTHERFUNCTIONS, specs, 2);
    }
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=EPSILLON}) };
        //appendRule(&G, NT_OTHERFUNCTIONS, specs, 1);
    }
    // Epsilon rule for NT_OTHERFUNCTIONS
    appendRule(&G, NT_OTHERFUNCTIONS, NULL, 0);
    G.productionRules[NT_OTHERFUNCTIONS].containsEpsilonRule = true;

    // Rules for NT_OTHERSTMTS
    {
        const grammar_element specs[] = { ((grammar_element){.type=isNonTerminal, .element.nt=NT_STMT}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_OTHERSTMTS}) };
        appendRule(&G, NT_OTHERSTMTS, specs, 2);
    }
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=EPSILLON}) };
        //appendRule(&G, NT_OTHERSTMTS, specs, 1);
    }
    // Epsilon rule for NT_OTHERSTMTS
    appendRule(&G, NT_OTHERSTMTS, NULL, 0);
    G.productionRules[NT_OTHERSTMTS].containsEpsilonRule = true;

    // Rules for NT_OUTPUTPARAMETERS
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_SQL}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_IDLIST}), ((grammar_element){.type=isTerminal, .element.t=TK_SQR}), ((grammar_element){.type=isTerminal, .element.t=TK_ASSIGNOP}) };
        appendRule(&G, NT_OUTPUTPARAMETERS, specs, 4);
    }
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=EPSILLON}) };
        //appendRule(&G, NT_OUTPUTPARAMETERS, specs, 1);
    }
    // Epsilon rule for NT_OUTPUTPARAMETERS
    appendRule(&G, NT_OUTPUTPARAMETERS, NULL, 0);
    G.productionRules[NT_OUTPUTPARAMETERS].containsEpsilonRule = true;

    // Rules for NT_OUTPUT_PAR
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_OUTPUT}), ((grammar_element){.type=isTerminal, .element.t=TK_PARAMETER}), ((grammar_element){.type=isTerminal, .element.t=TK_LIST}), ((grammar_element){.type=isTerminal, .element.t=TK_SQL}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_PARAMETER_LIST}), ((grammar_element){.type=isTerminal, .element.t=TK_SQR}) };
        appendRule(&G, NT_OUTPUT_PAR, specs, 6);
    }
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=EPSILLON}) };
        //appendRule(&G, NT_OUTPUT_PAR, specs, 1);
    }
    // Epsilon rule for NT_OUTPUT_PAR
    appendRule(&G, NT_OUTPUT_PAR, NULL, 0);
    G.productionRules[NT_OUTPUT_PAR].containsEpsilonRule = true;

    // Rules for NT_PARAMETER_LIST
    {
        const grammar_element specs[] = { ((grammar_element){.type=isNonTerminal, .element.nt=NT_DATATYPE}), ((grammar_element){.type=isTerminal, .element.t=TK_ID}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_REMAINING_LIST}) };
        appendRule(&G, NT_PARAMETER_LIST, specs, 3);
    }

    // Rules for NT_PRIMITIVEDATATYPE
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_INT}) };
        appendRule(&G, NT_PRIMITIVEDATATYPE, specs, 1);
    }
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_REAL}) };
        appendRule(&G, NT_PRIMITIVEDATATYPE, specs, 1);
    }

    // Rules for NT_PROGRAM
    {
        const grammar_element specs[] = { ((grammar_element){.type=isNonTerminal, .element.nt=NT_OTHERFUNCTIONS}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_MAINFUNCTION}) };
        appendRule(&G, NT_PROGRAM, specs, 2);
    }

    // Rules for NT_RELATIONALOP
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_LT}) };
        appendRule(&G, NT_RELATIONALOP, specs, 1);
    }
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_LE}) };
        appendRule(&G, NT_RELATIONALOP, specs, 1);
    }
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_EQ}) };
        appendRule(&G, NT_RELATIONALOP, specs, 1);
    }
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_GT}) };
        appendRule(&G, NT_RELATIONALOP, specs, 1);
    }
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_GE}) };
        appendRule(&G, NT_RELATIONALOP, specs, 1);
    }
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_NE}) };
        appendRule(&G, NT_RELATIONALOP, specs, 1);
    }

    // Rules for NT_REMAINING_LIST
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_COMMA}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_PARAMETER_LIST}) };
        appendRule(&G, NT_REMAINING_LIST, specs, 2);
    }
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=EPSILLON}) };
        //appendRule(&G, NT_REMAINING_LIST, specs, 1);
    }
    // Epsilon rule for NT_REMAINING_LIST
    appendRule(&G, NT_REMAINING_LIST, NULL, 0);
    G.productionRules[NT_REMAINING_LIST].containsEpsilonRule = true;

    // Rules for NT_RETURNSTMT
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_RETURN}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_OPTIONALRETURN}), ((grammar_element){.type=isTerminal, .element.t=TK_SEM}) };
        appendRule(&G, NT_RETURNSTMT, specs, 3);
    }

    // Rules for NT_SINGLEORRECID
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_ID}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_OPTION_SINGLE_CONSTRUCTED}) };
        appendRule(&G, NT_SINGLEORRECID, specs, 2);
    }

    // Rules for NT_STMT
    {
        const grammar_element specs[] = { ((grammar_element){.type=isNonTerminal, .element.nt=NT_ASSIGNMENTSTMT}) };
        appendRule(&G, NT_STMT, specs, 1);
    }
    {
        const grammar_element specs[] = { ((grammar_element){.type=isNonTerminal, .element.nt=NT_ITERATIVESTMT}) };
        appendRule(&G, NT_STMT, specs, 1);
    }
    {
        const grammar_element specs[] = { ((grammar_element){.type=isNonTerminal, .element.nt=NT_CONDITIONALSTMT}) };
        appendRule(&G, NT_STMT, specs, 1);
    }
    {
        const grammar_element specs[] = { ((grammar_element){.type=isNonTerminal, .element.nt=NT_IOSTMT}) };
        appendRule(&G, NT_STMT, specs, 1);
    }
    {
        const grammar_element specs[] = { ((grammar_element){.type=isNonTerminal, .element.nt=NT_FUNCALLSTMT}) };
        appendRule(&G, NT_STMT, specs, 1);
    }

    // Rules for NT_STMTS
    {
        const grammar_element specs[] = { ((grammar_element){.type=isNonTerminal, .element.nt=NT_TYPEDEFINITIONS}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_DECLATRATIONS}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_OTHERSTMTS}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_RETURNSTMT}) };
        appendRule(&G, NT_STMTS, specs, 4);
    }

    // Rules for NT_TERM
    {
        const grammar_element specs[] = { ((grammar_element){.type=isNonTerminal, .element.nt=NT_FACTOR}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_TERMPRIME}) };
        appendRule(&G, NT_TERM, specs, 2);
    }

    // Rules for NT_TERMPRIME
    {
        const grammar_element specs[] = { ((grammar_element){.type=isNonTerminal, .element.nt=NT_HIGHPRECEDENCEOPERATORS}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_FACTOR}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_TERMPRIME}) };
        appendRule(&G, NT_TERMPRIME, specs, 3);
    }
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=EPSILLON}) };
        //appendRule(&G, NT_TERMPRIME, specs, 1);
    }
    // Epsilon rule for NT_TERMPRIME
    appendRule(&G, NT_TERMPRIME, NULL, 0);
    G.productionRules[NT_TERMPRIME].containsEpsilonRule = true;

    // Rules for NT_TYPEDEFINITION
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_RECORD}), ((grammar_element){.type=isTerminal, .element.t=TK_RUID}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_FIELDDEFINITIONS}), ((grammar_element){.type=isTerminal, .element.t=TK_ENDRECORD}) };
        appendRule(&G, NT_TYPEDEFINITION, specs, 4);
    }
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_UNION}), ((grammar_element){.type=isTerminal, .element.t=TK_RUID}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_FIELDDEFINITIONS}), ((grammar_element){.type=isTerminal, .element.t=TK_ENDUNION}) };
        appendRule(&G, NT_TYPEDEFINITION, specs, 4);
    }

    // Rules for NT_TYPEDEFINITIONS
    {
        const grammar_element specs[] = { ((grammar_element){.type=isNonTerminal, .element.nt=NT_ACTUALORREDEFINED}), ((grammar_element){.type=isNonTerminal, .element.nt=NT_TYPEDEFINITIONS}) };
        appendRule(&G, NT_TYPEDEFINITIONS, specs, 2);
    }
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=EPSILLON}) };
        //appendRule(&G, NT_TYPEDEFINITIONS, specs, 1);
    }
    // Epsilon rule for NT_TYPEDEFINITIONS
    appendRule(&G, NT_TYPEDEFINITIONS, NULL, 0);
    G.productionRules[NT_TYPEDEFINITIONS].containsEpsilonRule = true;

    // Rules for NT_VAR
    {
        const grammar_element specs[] = { ((grammar_element){.type=isNonTerminal, .element.nt=NT_SINGLEORRECID}) };
        appendRule(&G, NT_VAR, specs, 1);
    }
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_NUM}) };
        appendRule(&G, NT_VAR, specs, 1);
    }
    {
        const grammar_element specs[] = { ((grammar_element){.type=isTerminal, .element.t=TK_RNUM}) };
        appendRule(&G, NT_VAR, specs, 1);
    }

    return G;
}