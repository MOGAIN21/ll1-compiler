#define ERROR_DEFAULT NULL
#define SYN ((ruleNode *)-1)

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ParserHeader.h"

static void computeFirstSets(grammar G, FirstFollowSet *ff);
static void computeFollowSets(grammar G, FirstFollowSet *ff);

void generateParseTable(FirstFollowSet FF, parserTable *pTable) {
    for (int i = 0; i < NOOFNONTERMINALS; i++) {
        for (int j = 0; j < TOTALTOKENS; j++) {
            pTable->table[i][j] = ERROR_DEFAULT;
        }
    }

    for (int i = 0; i < NOOFNONTERMINALS; i++) {
        for (int j = 0; j < FF.firstCount[i]; j++) {
            TokenType t = FF.first[i][j];
            if (t >= 0 && t < TOTALTOKENS) {
                pTable->table[i][t] = FF.firstRulePointers[i][j];
            }
        }

        if (FF.isEpsilonPresent[i]) {
            for (int j = 0; j < FF.followCount[i]; j++) {
                TokenType t = FF.follow[i][j];
                if (t >= 0 && t < TOTALTOKENS) {
                    pTable->table[i][t] = FF.epsilonRulePointers[i];
                }
            }
        } else {
            for (int j = 0; j < FF.followCount[i]; j++) {
                TokenType t = FF.follow[i][j];
                if (t >= 0 && t < TOTALTOKENS && pTable->table[i][t] == ERROR_DEFAULT) {
                    pTable->table[i][t] = SYN;
                }
            }
        }
    }
}

static void computeFirstSets(grammar G, FirstFollowSet *ff) {
    bool changed = true;

    while (changed) {
        changed = false;

        for (int A = 0; A < NOOFNONTERMINALS; A++) {
            for (ruleNode *current = G.productionRules[A].next; current != NULL; current = current->next) {
                bool epsilonPresent = true;

                if (current->elementCount == 0) {
                    if (!ff->isEpsilonPresent[A]) {
                        ff->isEpsilonPresent[A] = true;
                        ff->epsilonRulePointers[A] = current;
                        changed = true;
                    }
                    continue;
                }

                for (int i = 0; i < current->elementCount; i++) {
                    grammar_element elem = current->ruleSpecs[i];

                    if (elem.type == isTerminal) {
                        if (elem.element.t == EPSILLON) {
                            epsilonPresent = true;
                        } else if (addToBegg(ff, (NonTerminal)A, elem.element.t, current)) {
                            changed = true;
                            epsilonPresent = false;
                        } else {
                            epsilonPresent = false;
                        }
                        break;
                    }

                    NonTerminal target = elem.element.nt;
                    for (int f = 0; f < ff->firstCount[target]; f++) {
                        TokenType tokenInTargetFirst = ff->first[target][f];
                        if (tokenInTargetFirst == EPSILLON) {
                            continue;
                        }
                        if (addToBegg(ff, (NonTerminal)A, tokenInTargetFirst, current)) {
                            changed = true;
                        }
                    }

                    if (!ff->isEpsilonPresent[target]) {
                        epsilonPresent = false;
                        break;
                    }
                }

                if (epsilonPresent && !ff->isEpsilonPresent[A]) {
                    ff->isEpsilonPresent[A] = true;
                    ff->epsilonRulePointers[A] = current;
                    changed = true;
                }
            }
        }
    }
}

bool addToBegg(FirstFollowSet *ff, NonTerminal nt, TokenType token, ruleNode *current) {
    for (int i = 0; i < ff->firstCount[nt]; i++) {
        if (ff->first[nt][i] == token) {
            return false;
        }
    }

    if (ff->firstCount[nt] >= RULESIZE_MAX) {
        printf("Warning: First set for NT %d exceeded RULESIZE_MAX\n", nt);
        return false;
    }

    int idx = ff->firstCount[nt];
    ff->first[nt][idx] = token;
    ff->firstRulePointers[nt][idx] = current;
    ff->firstCount[nt]++;
    return true;
}

static void computeFollowSets(grammar G, FirstFollowSet *ff) {
    addToFollow(ff, NT_PROGRAM, DOLLAR);

    bool changed = true;
    while (changed) {
        changed = false;
        for (int A = 0; A < NOOFNONTERMINALS; A++) {
            for (ruleNode *current = G.productionRules[A].next; current != NULL; current = current->next) {
                for (int i = 0; i < current->elementCount; i++) {
                    if (current->ruleSpecs[i].type != isNonTerminal) {
                        continue;
                    }

                    NonTerminal B = current->ruleSpecs[i].element.nt;
                    bool epsilonInTail = true;

                    for (int j = i + 1; j < current->elementCount; j++) {
                        grammar_element nextElem = current->ruleSpecs[j];
                        if (nextElem.type == isTerminal) {
                            if (nextElem.element.t != EPSILLON &&
                                addToFollow(ff, B, nextElem.element.t)) {
                                changed = true;
                            }
                            epsilonInTail = false;
                            break;
                        }

                        NonTerminal C = nextElem.element.nt;
                        for (int k = 0; k < ff->firstCount[C]; k++) {
                            TokenType tok = ff->first[C][k];
                            if (tok != EPSILLON && addToFollow(ff, B, tok)) {
                                changed = true;
                            }
                        }
                        if (!ff->isEpsilonPresent[C]) {
                            epsilonInTail = false;
                            break;
                        }
                    }

                    if (epsilonInTail) {
                        for (int k = 0; k < ff->followCount[A]; k++) {
                            if (addToFollow(ff, B, ff->follow[A][k])) {
                                changed = true;
                            }
                        }
                    }
                }
            }
        }
    }
}

void computeFirstFollowSet(grammar G, FirstFollowSet *ff) {
    computeFirstSets(G, ff);
    computeFollowSets(G, ff);
}

parseTree *runParser(parserTable PT, FirstFollowSet FF, grammar G, FILE *fp) {
    (void)FF;
    grammar_element *symStack[STACKBOUND];
    parseTree *nodeStack[STACKBOUND];
    int topSymbol = -1;
    int topNode = -1;
    bool hasError = false;

    tempBuffer buffer = initBuffer(fp);
    initializeLookupTable();

    tokenInfo lookahead = nextToken(buffer, fp);
    if (lookahead->type == TK_EOF) {
        lookahead->type = DOLLAR;
    }

    parseTree *root = createParseNode(false, NT_PROGRAM, NULL);
    nodeStack[++topNode] = root;

    pushSymbol(symStack, &topSymbol, createTerminal(DOLLAR));
    pushSymbol(symStack, &topSymbol, createNonTerminal(NT_PROGRAM));

    int loopGuard = 0;
    while (topSymbol >= 0 && loopGuard++ < 10000) {
        grammar_element *top = symStack[topSymbol];

        if (top->type == isTerminal && top->element.t == EPSILLON) {
            popSymbol(symStack, &topSymbol);
            if (topNode >= 0) {
                topNode--;
            }
            continue;
        }

        if (top->type == isTerminal) {
            if (top->element.t == lookahead->type) {
                if (topNode >= 0) {
                    attachTerminalNode(nodeStack[topNode], lookahead);
                    topNode--;
                }
                popSymbol(symStack, &topSymbol);

                free(lookahead);
                lookahead = nextToken(buffer, fp);
                if (lookahead->type == TK_EOF) {
                    lookahead->type = DOLLAR;
                }
            } else {
                reportMismatch(top, lookahead);
                hasError = true;
                popSymbol(symStack, &topSymbol);
                if (topNode >= 0) {
                    topNode--;
                }
            }
            continue;
        }

        NonTerminal A = top->element.nt;
        ruleNode *nextRuleToApply = ERROR_DEFAULT;
        if (lookahead->type >= 0 && lookahead->type < TOTALTOKENS) {
            nextRuleToApply = PT.table[A][lookahead->type];
        }

        if (nextRuleToApply == SYN) {
            reportInvalidEntry(A, lookahead);
            hasError = true;
            popSymbol(symStack, &topSymbol);
            if (topNode >= 0) {
                topNode--;
            }
            continue;
        }

        if (nextRuleToApply == ERROR_DEFAULT) {
            reportInvalidEntry(A, lookahead);
            hasError = true;
            if (lookahead->type == DOLLAR) {
                break;
            }
            free(lookahead);
            lookahead = nextToken(buffer, fp);
            if (lookahead->type == TK_EOF) {
                lookahead->type = DOLLAR;
            }
            continue;
        }

        popSymbol(symStack, &topSymbol);
        parseTree *parent = (topNode >= 0) ? nodeStack[topNode--] : NULL;

        if (parent == NULL) {
            hasError = true;
            break;
        }

        if (nextRuleToApply->elementCount == 0) {
            parent->noOfChildren = 1;
            parseTree *epsNode = createParseNode(true, NT_PROGRAM, NULL);
            epsNode->ele.symbol.type = isTerminal;
            epsNode->ele.symbol.element.t = EPSILLON;
            strcpy(epsNode->ele.lexeme, "eps");
            epsNode->parent = parent;
            parent->children[0] = epsNode;
            continue;
        }

        expandRule(parent, G, A, nextRuleToApply, symStack, &topSymbol, nodeStack, &topNode);
    }

    if (loopGuard >= 10000) {
        printf("Parser safety break: potential infinite loop detected.\n");
        hasError = true;
    }

    while (topSymbol >= 0) {
        popSymbol(symStack, &topSymbol);
    }
    free(lookahead);
    free(buffer);

    if (!hasError) {
        printf("\nCOMPILATION SUCCESS!\n");
    } else {
        printf("\nCOMPILATION FAILED\n");
    }

    return root;
}

void printHeaders(FILE *out) {
    fprintf(out,
            "%-30s%-30s%-30s%-30s%-30s%-30s%-30s\n\n",
            "lexeme", "lineno", "token", "valueIfNumber",
            "parentNodeSymbol", "isLeafNode(yes/no)", "NodeSymbol");
}

void printNode(parseTree *node, FILE *out) {
    const char *lex = (node->ele.lexeme[0] != '\0') ? node->ele.lexeme : "----";
    int lineNo = node->ele.line;

    const char *tokenName = (node->ele.symbol.type == isTerminal)
                                ? getTokenName(node->ele.symbol.element.t)
                                : getNonTerminal(node->ele.symbol.element.nt);

    const char *valueField = "----";
    if (node->ele.symbol.type == isTerminal &&
        (node->ele.symbol.element.t == TK_NUM || node->ele.symbol.element.t == TK_RNUM)) {
        valueField = lex;
    }

    const char *parentName = "----";
    if (node->parent != NULL) {
        if (node->parent->ele.symbol.type == isNonTerminal) {
            parentName = getNonTerminal(node->parent->ele.symbol.element.nt);
        } else {
            parentName = getTokenName(node->parent->ele.symbol.element.t);
        }
    }

    const char *leafStatus = (node->noOfChildren == 0) ? "YES" : "NO";
    const char *nodeSym = tokenName;

    fprintf(out, "%-30s%-30d%-30s%-30s%-30s%-30s%-30s\n",
            lex, lineNo, tokenName, valueField, parentName, leafStatus, nodeSym);
}

void traverseParseTree(parseTree *root, FILE *out) {
    if (!root) {
        return;
    }

    printNode(root, out);
    for (int i = 0; i < root->noOfChildren; i++) {
        if (root->children[i]) {
            traverseParseTree(root->children[i], out);
        }
    }
}

void printParseTree(parseTree *PT, FILE *outfile) {
    if (!PT || !outfile) {
        return;
    }
    printHeaders(outfile);
    traverseParseTree(PT, outfile);
}

bool addToFollow(FirstFollowSet *ff, NonTerminal nt, TokenType token) {
    for (int i = 0; i < ff->followCount[nt]; i++) {
        if (ff->follow[nt][i] == token) {
            return false;
        }
    }
    if (ff->followCount[nt] >= RULESIZE_MAX) {
        return false;
    }
    ff->follow[nt][ff->followCount[nt]++] = token;
    return true;
}

void pushSymbol(grammar_element **stack, int *top, grammar_element *sym) {
    if (*top + 1 >= STACKBOUND) {
        free(sym);
        return;
    }
    stack[++(*top)] = sym;
}

void popSymbol(grammar_element **stack, int *top) {
    if (*top < 0) {
        return;
    }
    free(stack[*top]);
    (*top)--;
}

grammar_element *createTerminal(TokenType t) {
    grammar_element *e = (grammar_element *)malloc(sizeof(grammar_element));
    if (!e) {
        return NULL;
    }
    e->type = isTerminal;
    e->element.t = t;
    return e;
}

grammar_element *createNonTerminal(NonTerminal nt) {
    grammar_element *e = (grammar_element *)malloc(sizeof(grammar_element));
    if (!e) {
        return NULL;
    }
    e->type = isNonTerminal;
    e->element.nt = nt;
    return e;
}

parseTree *createParseNode(bool isLeaf, NonTerminal nt, tokenInfo tk) {
    parseTree *node = (parseTree *)malloc(sizeof(parseTree));
    if (!node) {
        return NULL;
    }

    node->parent = NULL;
    node->noOfChildren = 0;
    for (int i = 0; i < RULESIZE_MAX; i++) {
        node->children[i] = NULL;
    }
    node->ele.lexeme[0] = '\0';
    node->ele.lexemeSize = 0;
    node->ele.line = -1;

    if (isLeaf) {
        node->ele.symbol.type = isTerminal;
        node->ele.symbol.element.t = tk ? tk->type : EPSILLON;
        if (tk) {
            node->ele.line = tk->lineNo;
            strncpy(node->ele.lexeme, tk->lexeme, sizeof(node->ele.lexeme) - 1);
            node->ele.lexeme[sizeof(node->ele.lexeme) - 1] = '\0';
            node->ele.lexemeSize = (int)strlen(node->ele.lexeme);
        }
    } else {
        node->ele.symbol.type = isNonTerminal;
        node->ele.symbol.element.nt = nt;
    }

    return node;
}

void attachTerminalNode(parseTree *node, tokenInfo tk) {
    if (!node || !tk) {
        return;
    }

    node->noOfChildren = 0;
    node->ele.symbol.type = isTerminal;
    node->ele.symbol.element.t = tk->type;
    node->ele.line = tk->lineNo;
    strncpy(node->ele.lexeme, tk->lexeme, sizeof(node->ele.lexeme) - 1);
    node->ele.lexeme[sizeof(node->ele.lexeme) - 1] = '\0';
    node->ele.lexemeSize = (int)strlen(node->ele.lexeme);
}

void reportMismatch(grammar_element *top, tokenInfo lookahead) {
    printf("Syntax Error: Expected %s but got %s at line %d\n",
           getTokenName(top->element.t), getTokenName(lookahead->type), lookahead->lineNo);
}

void reportInvalidEntry(NonTerminal A, tokenInfo lookahead) {
    printf("Table Error: No rule for %s with %s at line %d\n",
           getNonTerminal(A), getTokenName(lookahead->type), lookahead->lineNo);
}

void expandRule(parseTree *parent, grammar G, NonTerminal A, ruleNode *rule,
                grammar_element **symStack, int *topSym, parseTree **nodeStack, int *topNode) {
    (void)G;
    (void)A;

    parent->noOfChildren = rule->elementCount;
    for (int i = 0; i < rule->elementCount; i++) {
        grammar_element sym = rule->ruleSpecs[i];

        parseTree *child = createParseNode(sym.type == isTerminal, NT_PROGRAM, NULL);
        if (!child) {
            continue;
        }

        child->parent = parent;
        child->ele.symbol = sym;
        parent->children[i] = child;
    }

    for (int i = rule->elementCount - 1; i >= 0; i--) {
        grammar_element *sym = (grammar_element *)malloc(sizeof(grammar_element));
        if (!sym) {
            continue;
        }
        *sym = rule->ruleSpecs[i];
        pushSymbol(symStack, topSym, sym);
        nodeStack[++(*topNode)] = parent->children[i];
    }
}

void initializeLookupTable(void) {
}

const char *getTokenName(TokenType t) {
    switch (t) {
        case TK_ASSIGNOP: return "TK_ASSIGNOP";
        case TK_COMMENT: return "TK_COMMENT";
        case TK_FIELDID: return "TK_FIELDID";
        case TK_ID: return "TK_ID";
        case TK_NUM: return "TK_NUM";
        case TK_RNUM: return "TK_RNUM";
        case TK_FUNID: return "TK_FUNID";
        case TK_RUID: return "TK_RUID";
        case TK_WITH: return "TK_WITH";
        case TK_PARAMETERS: return "TK_PARAMETERS";
        case TK_END: return "TK_END";
        case TK_WHILE: return "TK_WHILE";
        case TK_UNION: return "TK_UNION";
        case TK_ENDUNION: return "TK_ENDUNION";
        case TK_DEFINETYPE: return "TK_DEFINETYPE";
        case TK_AS: return "TK_AS";
        case TK_TYPE: return "TK_TYPE";
        case TK_MAIN: return "TK_MAIN";
        case TK_GLOBAL: return "TK_GLOBAL";
        case TK_PARAMETER: return "TK_PARAMETER";
        case TK_LIST: return "TK_LIST";
        case TK_SQL: return "TK_SQL";
        case TK_SQR: return "TK_SQR";
        case TK_INPUT: return "TK_INPUT";
        case TK_OUTPUT: return "TK_OUTPUT";
        case TK_INT: return "TK_INT";
        case TK_REAL: return "TK_REAL";
        case TK_COMMA: return "TK_COMMA";
        case TK_SEM: return "TK_SEM";
        case TK_COLON: return "TK_COLON";
        case TK_DOT: return "TK_DOT";
        case TK_ENDWHILE: return "TK_ENDWHILE";
        case TK_OP: return "TK_OP";
        case TK_CL: return "TK_CL";
        case TK_IF: return "TK_IF";
        case TK_THEN: return "TK_THEN";
        case TK_ENDIF: return "TK_ENDIF";
        case TK_READ: return "TK_READ";
        case TK_WRITE: return "TK_WRITE";
        case TK_RETURN: return "TK_RETURN";
        case TK_PLUS: return "TK_PLUS";
        case TK_MINUS: return "TK_MINUS";
        case TK_MUL: return "TK_MUL";
        case TK_DIV: return "TK_DIV";
        case TK_CALL: return "TK_CALL";
        case TK_RECORD: return "TK_RECORD";
        case TK_ENDRECORD: return "TK_ENDRECORD";
        case TK_ELSE: return "TK_ELSE";
        case TK_AND: return "TK_AND";
        case TK_OR: return "TK_OR";
        case TK_NOT: return "TK_NOT";
        case TK_LT: return "TK_LT";
        case TK_LE: return "TK_LE";
        case TK_EQ: return "TK_EQ";
        case TK_GT: return "TK_GT";
        case TK_GE: return "TK_GE";
        case TK_NE: return "TK_NE";
        case TK_ERROR: return "TK_ERROR";
        case EPSILLON: return "EPSILLON";
        case DOLLAR: return "$ (EOF)";
        case TK_EOF: return "TK_EOF";
        default: {
            static char buf[20];
            snprintf(buf, sizeof(buf), "TOKEN_%d", t);
            return buf;
        }
    }
}

void removeComments(char *testcaseFile) {
    (void)testcaseFile;
    printf("Comments removed (Dummy implementation).\n");
}

void getStream(FILE *fp) {
    (void)fp;
}
