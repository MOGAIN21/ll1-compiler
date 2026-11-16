#ifndef PARSER_DEFINITION
#define PARSER_DEFINITION

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define NOOFNONTERMINALS 53
#define RULESIZE_MAX 100
#define TOTALTOKENS 63
#define STACKBOUND 1000

// 1. ADD THESE LEXER TYPES HERE AT THE TOP!
typedef enum {
    TK_ASSIGNOP, TK_COMMENT, TK_FIELDID, TK_ID, TK_NUM, TK_RNUM,
    TK_FUNID, TK_RUID, TK_WITH, TK_PARAMETERS, TK_END, TK_WHILE,
    TK_UNION, TK_ENDUNION, TK_DEFINETYPE, TK_AS, TK_TYPE, TK_MAIN,
    TK_GLOBAL, TK_PARAMETER, TK_LIST, TK_SQL, TK_SQR, TK_INPUT,
    TK_OUTPUT, TK_INT, TK_REAL, TK_COMMA, TK_SEM, TK_COLON, TK_DOT,
    TK_ENDWHILE, TK_OP, TK_CL, TK_IF, TK_THEN, TK_ENDIF, TK_READ,
    TK_WRITE, TK_RETURN, TK_PLUS, TK_MINUS, TK_MUL, TK_DIV, TK_CALL,
    TK_RECORD, TK_ENDRECORD, TK_ELSE, TK_AND, TK_OR, TK_NOT, TK_LT,
    TK_LE, TK_EQ, TK_GT, TK_GE, TK_NE, TK_ERROR, EPSILLON, DOLLAR, TK_EOF
} TokenType;

typedef struct {
    TokenType type;
    char lexeme[100];
    int lineNo;
} Token;

typedef Token* tokenInfo;

typedef struct{
    char* buffer;
    int bufferSize;
    int forward;
    int lexemeStart;
    int currentLine;
} BufferState;
typedef BufferState* tempBuffer;

// 2. NOW leave your existing NonTerminal enum and grammar structs here...

typedef enum {
  NT_PROGRAM,
  NT_MAINFUNCTION,
  NT_OTHERFUNCTIONS,
  NT_FUNCTION,
  NT_INPUT_PAR,
  NT_OUTPUT_PAR,
  NT_PARAMETER_LIST,
  NT_DATATYPE,
  NT_PRIMITIVEDATATYPE,
  NT_CONSTRUCTEDDATATYPE,
  NT_REMAINING_LIST,
  NT_STMTS,
  NT_TYPEDEFINITIONS,
  NT_ACTUALORREDEFINED,
  NT_TYPEDEFINITION,
  NT_FIELDDEFINITIONS,
  NT_FIELDDEFINITION,
  NT_FIELDTYPE,
  NT_MOREFIELDS,
  NT_DECLATRATIONS,
  NT_DECLATRATION,
  NT_GLOBAL_OR_NOT,
  NT_OTHERSTMTS,
  NT_STMT,
  NT_ASSIGNMENTSTMT,
  NT_SINGLEORRECID,
  NT_OPTION_SINGLE_CONSTRUCTED,
  NT_ONEEXPANSION,
  NT_MOREEXPANSIONS,
  NT_FUNCALLSTMT,
  NT_OUTPUTPARAMETERS,
  NT_INPUTPARAMETERS,
  NT_ITERATIVESTMT,
  NT_CONDITIONALSTMT,
  NT_ELSEPART,
  NT_IOSTMT,
  NT_ARITHMETICEXPRESSION,
  NT_EXPPRIME,
  NT_TERM,
  NT_TERMPRIME,
  NT_FACTOR,
  NT_HIGHPRECEDENCEOPERATORS,
  NT_LOWPRECEDENCEOPERATORS,
  NT_BOOLEANEXPRESSION,
  NT_VAR,
  NT_LOGICALOP,
  NT_RELATIONALOP,
  NT_RETURNSTMT,
  NT_OPTIONALRETURN,
  NT_IDLIST,
  NT_MORE_IDS,
  NT_DEFINETYPESTMT,
  NT_A,
} NonTerminal;

typedef struct{
    enum {
        isTerminal,
        isNonTerminal
    }type;
    union{
        TokenType t;
        NonTerminal nt;
    }element;
}grammar_element;

typedef struct ruleNode{
    int elementCount;
    grammar_element ruleSpecs[RULESIZE_MAX];
    struct ruleNode* next;
}ruleNode;

typedef struct{
    int ruleCount;
    ruleNode* next;
    bool containsEpsilonRule;
}RuleHead;
typedef struct {
    RuleHead productionRules[NOOFNONTERMINALS];
}grammar;
typedef struct {
     TokenType first[NOOFNONTERMINALS][RULESIZE_MAX];
     ruleNode* firstRulePointers[NOOFNONTERMINALS][RULESIZE_MAX];
     bool isEpsilonPresent[NOOFNONTERMINALS];
     int firstCount[NOOFNONTERMINALS];
     

     TokenType follow[NOOFNONTERMINALS][RULESIZE_MAX];
     ruleNode* epsilonRulePointers[NOOFNONTERMINALS];
     int followCount[NOOFNONTERMINALS];
     

}FirstFollowSet;

typedef struct{
    ruleNode* table[NOOFNONTERMINALS][TOTALTOKENS];
}parserTable;

typedef struct {
  grammar_element symbol;
  int line;
  char lexeme[100];
  int lexemeSize;
} parseTreeElement;



typedef struct parseTree {
  parseTreeElement ele;
  struct parseTree *parent;
  int noOfChildren;
  struct parseTree *children[RULESIZE_MAX];

}parseTree;

// Add these at the bottom of ParserHeader.h
grammar initializeGrammar(void);
void computeFirstFollowSet(grammar G, FirstFollowSet *ff);
void generateParseTable(FirstFollowSet FF, parserTable *pTable);
parseTree* runParser(parserTable PT, FirstFollowSet FF, grammar G, FILE *fp);
void printParseTree(parseTree *PT, FILE *outfile);
void removeComments(char *testcaseFile);
void getStream(FILE *fp);
tempBuffer initBuffer(FILE *fp);
tokenInfo nextToken(tempBuffer buffer, FILE *fp);
const char* getTokenName(TokenType t);
char* getNonTerminal(NonTerminal nt);

// Parser Helpers (Used inside Parser.c)
bool addToBegg(FirstFollowSet *ff, NonTerminal nt, TokenType token, ruleNode* current);
bool addToFollow(FirstFollowSet *ff, NonTerminal nt, TokenType token);
void initializeLookupTable(void);
parseTree* createParseNode(bool isLeaf, NonTerminal nt, tokenInfo tk);
void pushSymbol(grammar_element** stack, int *top, grammar_element* sym);
grammar_element* createTerminal(TokenType t);
grammar_element* createNonTerminal(NonTerminal nt);
void attachTerminalNode(parseTree* node, tokenInfo tk);
void popSymbol(grammar_element** stack, int *top);
void reportMismatch(grammar_element* top, tokenInfo lookahead);
void reportInvalidEntry(NonTerminal A, tokenInfo lookahead);
void expandRule(parseTree* parent, grammar G, NonTerminal A, ruleNode* rule, grammar_element** symStack, int* topSym, parseTree** nodeStack, int* topNode);
#endif
