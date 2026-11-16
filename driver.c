#include "ParserHeader.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

static const char *MAIN_MENU =
    "What do you want to do:\n"
    "\t0. Exit\n"
    "\t1. Remove Comments\n"
    "\t2. Print Lexer Token List\n"
    "\t3. Parse Code and Print Parse Tree\n"
    "\t4. Parse Code and Print Time Taken\n";

static void printMenu(void) {
    printf("%s", MAIN_MENU);
    printf("==> ");
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("Invalid No. of Arguments required 2 got %d\n", argc - 1);
        return 1;
    }

    printf("DEBUG: Initializing grammar...\n");
    grammar G = initializeGrammar();
    
    // FIX: Allocate memory for FirstFollowSet and initialize it to zero!
    printf("DEBUG: Computing First and Follow sets...\n");
    FirstFollowSet *ff = (FirstFollowSet*)malloc(sizeof(FirstFollowSet));
    memset(ff, 0, sizeof(FirstFollowSet)); // Clear out garbage data
    computeFirstFollowSet(G, ff);

    printf("DEBUG: Generating Parse Table...\n");
    parserTable T;
    generateParseTable(*ff, &T);

    printf("DEBUG: Starting Compiler Menu...\n\n");
    for (;;) {
        int choice = -1;
        printMenu();

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input - Exiting!\n");
            return 0;
        }

        if (choice == 0) {
            return 0;
        }

        if (choice == 1) {
            printf("==== Comment Free Code ====\n");
            removeComments(argv[1]);
            printf("==== Comment Removal Complete ====\n\n\n");
            continue;
        }
        if (choice == 2) {
            printf("==== Lexer Tokens ====\n");
            FILE *fp = fopen(argv[1], "r");
            if (!fp) {
                printf("Invalid Input File!\n");
                continue;
            }
            
            // This loop asks the lexer for tokens one by one and prints them!
            while (1) {
                tokenInfo tk = getNextToken(fp); 
                if (tk == NULL) break; 
                
                // Print the Token's Line, ID, and Lexeme (the actual string)
                printf("Line: %d | Token ID: %d | Lexeme: %s\n", tk->lineNo, tk->type, tk->lexeme);
                
                if (tk->type == TK_EOF) {
                    free(tk);
                    break;
                }
                free(tk);
            }
            
            fclose(fp);
            printf("==== Lexer Tokens Complete ====\n\n\n");
            continue;
        }

       
        if (choice == 3) {
            printf("==== Parsing... ====\n");

            FILE *in = fopen(argv[1], "r");
            if (!in) {
                printf("Invalid Input File!\n");
                continue;
            }

            FILE *out = fopen(argv[2], "w");
            if (!out) {
                printf("Invalid Output File!\n");
                fclose(in);
                continue;
            }

            parseTree *root = runParser(T, *ff, G, in);
            printParseTree(root, out);

            fclose(in);
            fclose(out);

            printf("Source Code Parsed and parse tree printed into %s\n", argv[2]);
            continue;
        }

        if (choice == 4) {
            printf("==== Parsing... ====\n");

            FILE *in = fopen(argv[1], "r");
            if (!in) {
                printf("Invalid Input File!\n");
                continue;
            }

            clock_t start = clock();
            parseTree *pt = runParser(T, *ff, G, in);
            (void)pt; // pt kept to match original behavior (parse executed)
            clock_t end = clock();

            fclose(in);

            printf("Source Code Parsed\n");
            printf("Time Taken to Parse: %ld\n", (long)(end - start));
            printf("Time Taken to Parse (seconds): %lf\n",
                   (double)(end - start) / CLOCKS_PER_SEC);

            continue;
        }

        printf("Invalid Input - Exiting!\n");
        return 0;
    }
}
