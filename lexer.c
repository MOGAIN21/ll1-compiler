#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "ParserHeader.h" // Get structs from the header!

#define MAX_LEX 99
#define MAXLEXEMELEN 100

//  Globals 
int lineptr = 1;

// Safe append 
int addChar(char *lexeme, int *lexIndex, char c) {
    if (*lexIndex >= MAX_LEX) return 0;
    lexeme[(*lexIndex)++] = c;
    lexeme[*lexIndex] = '\0';
    return 1;
}

void increment_lineptr() {
    lineptr++;
}

//Keyword checks 
TokenType check_FUNID(char* lexeme) {
    if (strcmp(lexeme, "_main") == 0)
        return TK_MAIN;
    return TK_FUNID;
}

TokenType check_keyword(char* lexeme) {
    if (strcmp(lexeme, "with") == 0) return TK_WITH;
    if (strcmp(lexeme, "parameters") == 0) return TK_PARAMETERS;
    if (strcmp(lexeme, "end") == 0) return TK_END;
    if (strcmp(lexeme, "while") == 0) return TK_WHILE;
    if (strcmp(lexeme, "union") == 0) return TK_UNION;
    if (strcmp(lexeme, "endunion") == 0) return TK_ENDUNION;
    if (strcmp(lexeme, "definetype") == 0) return TK_DEFINETYPE;
    if (strcmp(lexeme, "as") == 0) return TK_AS;
    if (strcmp(lexeme, "type") == 0) return TK_TYPE;
    if (strcmp(lexeme, "global") == 0) return TK_GLOBAL;
    if (strcmp(lexeme, "parameter") == 0) return TK_PARAMETER;
    if (strcmp(lexeme, "list") == 0) return TK_LIST;
    if (strcmp(lexeme, "input") == 0) return TK_INPUT;
    if (strcmp(lexeme, "output") == 0) return TK_OUTPUT;
    if (strcmp(lexeme, "int") == 0) return TK_INT;
    if (strcmp(lexeme, "real") == 0) return TK_REAL;
    if (strcmp(lexeme, "endwhile") == 0) return TK_ENDWHILE;
    if (strcmp(lexeme, "if") == 0) return TK_IF;
    if (strcmp(lexeme, "then") == 0) return TK_THEN;
    if (strcmp(lexeme, "endif") == 0) return TK_ENDIF;
    if (strcmp(lexeme, "read") == 0) return TK_READ;
    if (strcmp(lexeme, "write") == 0) return TK_WRITE;
    if (strcmp(lexeme, "return") == 0) return TK_RETURN;
    if (strcmp(lexeme, "call") == 0) return TK_CALL;
    if (strcmp(lexeme, "record") == 0) return TK_RECORD;
    if (strcmp(lexeme, "endrecord") == 0) return TK_ENDRECORD;
    if (strcmp(lexeme, "else") == 0) return TK_ELSE;
    return TK_FIELDID;
}

tokenInfo makeToken(TokenType type, char* lexeme) {
    tokenInfo t = (tokenInfo)malloc(sizeof(Token));
    t->type = type;
    strncpy(t->lexeme, lexeme, MAXLEXEMELEN - 1);
    t->lexeme[MAXLEXEMELEN - 1] = '\0'; 
    t->lineNo = lineptr;
    return t;
}

tokenInfo getNextToken(FILE *fp) {
    int state = 0;
    char c;
    char lexeme[100] = {0};
    int lexIndex = 0;

    while (1) {
        c = fgetc(fp);

        switch (state) {

        //START 
        case 0:
            lexIndex = 0;
            memset(lexeme, 0, sizeof(lexeme));

            if (c == EOF) { lineptr = 1; return makeToken(TK_EOF, "EOF"); }
            if (c == ' ' || c == '\t' || c == '\r') continue;
            if (c == '\n') { increment_lineptr(); continue; }

            addChar(lexeme, &lexIndex, c);

            if (c == '<') state = 27;
            else if (c == '>') state = 33;
            else if (c == '=') state = 60;
            else if (c == '!') state = 61;
            else if (c == '~') return makeToken(TK_NOT, lexeme);
            else if (c == '+') return makeToken(TK_PLUS, lexeme);
            else if (c == '-') return makeToken(TK_MINUS, lexeme);
            else if (c == '*') return makeToken(TK_MUL, lexeme);
            else if (c == '/') return makeToken(TK_DIV, lexeme);
            else if (c == '[') return makeToken(TK_SQL, lexeme);
            else if (c == ']') return makeToken(TK_SQR, lexeme);
            else if (c == '(') return makeToken(TK_OP, lexeme);
            else if (c == ')') return makeToken(TK_CL, lexeme);
            else if (c == '.') return makeToken(TK_DOT, lexeme);
            else if (c == ',') return makeToken(TK_COMMA, lexeme);
            else if (c == ';') return makeToken(TK_SEM, lexeme);
            else if (c == ':') return makeToken(TK_COLON, lexeme);
            else if (c == '&') state = 65;
            else if (c == '@') state = 67;
            else if (c == '%') state = 70;
            else if (c == '_') state = 38;
            else if (c == '#') state = 40;
            else if (isdigit(c)) state = 42;
            else if (isalpha(c)) state = 56; // <--- ALL letters go here now!
            else return makeToken(TK_ERROR, lexeme);
            break;

        case 27:
            if (c == '=') { addChar(lexeme,&lexIndex,c); return makeToken(TK_LE,lexeme); }
            else if (c == '-') { addChar(lexeme,&lexIndex,c); state = 28; }
            else { ungetc(c,fp); return makeToken(TK_LT,lexeme); }
            break;

        case 28:
            if (c == '-') { addChar(lexeme,&lexIndex,c); state = 29; }
            else { ungetc(c,fp); return makeToken(TK_ERROR,lexeme); }
            break;

        case 29:
            if (c == '-') { addChar(lexeme,&lexIndex,c); return makeToken(TK_ASSIGNOP,lexeme); }
            else { return makeToken(TK_ERROR,lexeme); }

        case 33:
            if (c == '=') { addChar(lexeme,&lexIndex,c); return makeToken(TK_GE,lexeme); }
            else { ungetc(c,fp); return makeToken(TK_GT,lexeme); }

        case 60:
            if (c == '=') { addChar(lexeme,&lexIndex,c); return makeToken(TK_EQ,lexeme); }
            ungetc(c,fp); return makeToken(TK_ERROR,lexeme);

        case 61:
            if (c == '=') { addChar(lexeme,&lexIndex,c); return makeToken(TK_NE,lexeme); }
            ungetc(c,fp); return makeToken(TK_ERROR,lexeme);

        case 65:
            if (c == '&') { addChar(lexeme,&lexIndex,c); state = 66; }
            else { ungetc(c,fp); return makeToken(TK_ERROR,lexeme); }
            break;

        case 66:
            if (c == '&') { addChar(lexeme,&lexIndex,c); return makeToken(TK_AND,lexeme); }
            ungetc(c,fp); return makeToken(TK_ERROR,lexeme);

        case 67:
            if (c == '@') { addChar(lexeme,&lexIndex,c); state = 68; }
            else { ungetc(c,fp); return makeToken(TK_ERROR,lexeme); }
            break;

        case 68:
            if (c == '@') { addChar(lexeme,&lexIndex,c); return makeToken(TK_OR,lexeme); }
            ungetc(c,fp); return makeToken(TK_ERROR,lexeme);

        case 70:
            if (c == '\n') { increment_lineptr(); state = 0; }
            else if (c == EOF) { lineptr = 1; return makeToken(TK_EOF,"EOF"); }
            break;

        case 38:
            if (isalpha(c)) { addChar(lexeme,&lexIndex,c); state = 39; }
            else { ungetc(c,fp); return makeToken(TK_ERROR,lexeme); }
            break;

        case 39:
            if (isalnum(c)) addChar(lexeme,&lexIndex,c);
            else { ungetc(c,fp); return makeToken(check_FUNID(lexeme),lexeme); }
            break;

        case 40:
            if (islower(c)) { addChar(lexeme,&lexIndex,c); state = 41; }
            else { ungetc(c,fp); return makeToken(TK_ERROR,lexeme); }
            break;

        case 41:
            if (islower(c)) addChar(lexeme,&lexIndex,c);
            else { ungetc(c,fp); return makeToken(TK_RUID,lexeme); }
            break;

        case 56:
            // This single state reads the ENTIRE word first (both letters and numbers)
            if (isalnum(c)) {
                addChar(lexeme, &lexIndex, c);
            } else {
                ungetc(c, fp);
                
                // Now classify the whole word!
                // Variables in your language start with b, c, or d, followed by a number 2-7.
                if (strlen(lexeme) >= 2 && 
                   (lexeme[0] >= 'b' && lexeme[0] <= 'd') &&
                   (lexeme[1] >= '2' && lexeme[1] <= '7')) {
                    
                    return makeToken(TK_ID, lexeme);
                } 
                // If it doesn't match the TK_ID rule, it must be a Keyword or Field ID
                else {
                    return makeToken(check_keyword(lexeme), lexeme);
                }
            }
            break;

        case 42:
            if (isdigit(c)) addChar(lexeme,&lexIndex,c);
            else if (c == '.') { addChar(lexeme,&lexIndex,c); state = 43; }
            else { ungetc(c,fp); return makeToken(TK_NUM,lexeme); }
            break;

        case 43:
            if (isdigit(c)) { addChar(lexeme,&lexIndex,c); state = 44; }
            else { ungetc(c,fp); ungetc('.',fp); lexeme[--lexIndex]='\0'; return makeToken(TK_NUM,lexeme); }
            break;

        case 44:
            if (isdigit(c)) { addChar(lexeme,&lexIndex,c); state = 45; }
            else { ungetc(c,fp); return makeToken(TK_ERROR,lexeme); }
            break;

        case 45:
            if (c == 'E') { addChar(lexeme,&lexIndex,c); state = 46; }
            else { ungetc(c,fp); return makeToken(TK_RNUM,lexeme); }
            break;

        case 46:
            if (c == '+' || c == '-' || isdigit(c)) {
                addChar(lexeme,&lexIndex,c);
                state = (isdigit(c)) ? 48 : 47;
            } else { ungetc(c,fp); return makeToken(TK_ERROR,lexeme); }
            break;

        case 47:
            if (isdigit(c)) { addChar(lexeme,&lexIndex,c); state = 48; }
            else { ungetc(c,fp); return makeToken(TK_ERROR,lexeme); }
            break;

        case 48:
            if (isdigit(c)) { addChar(lexeme,&lexIndex,c); state = 49; }
            else { ungetc(c,fp); return makeToken(TK_ERROR,lexeme); }
            break;

        case 49:
            ungetc(c,fp);
            return makeToken(TK_RNUM,lexeme);
        }
    }
}

tempBuffer initBuffer(FILE *fp) {
    (void)fp; 
    tempBuffer dummy = (tempBuffer)malloc(sizeof(BufferState));
    dummy->buffer = NULL;
    return dummy;
}

tokenInfo nextToken(tempBuffer buffer, FILE *fp) {
    (void)buffer; 
    return getNextToken(fp);
}