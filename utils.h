#ifndef UTILS_H
#define UTILS_H

#include "ParserHeader.h"

char *getNonTerminal(NonTerminal nt);
tokenInfo getNextToken(FILE *fp);
TokenType check_FUNID(char* lexeme);
TokenType check_keyword(char* lexeme);
tokenInfo makeToken(TokenType type, char* lexeme);
tempBuffer initBuffer(FILE *fp);
tokenInfo nextToken(tempBuffer buffer, FILE *fp) ;

#endif