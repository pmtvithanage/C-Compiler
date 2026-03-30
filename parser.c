#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_TOKEN_LEN 256
#define MAX_TOKENS 5000
#define MAX_SYMBOLS 500


typedef enum {
    TOKEN_INT,         
    TOKEN_PRINT,        
    TOKEN_IDENTIFIER,   
    TOKEN_NUMBER,       
    TOKEN_PLUS,         
    TOKEN_MINUS,        
    TOKEN_MULTIPLY,     
    TOKEN_DIVIDE,       
    TOKEN_ASSIGN,       
    TOKEN_SEMICOLON,    
    TOKEN_LPAREN,       
    TOKEN_RPAREN,       
    TOKEN_LBRACE,       // {
    TOKEN_RBRACE,       // }
    TOKEN_COLON,        // :
    TOKEN_IF,           // if
    TOKEN_ELSE,         // else
    TOKEN_FOR,          // for
    TOKEN_WHILE,        // while
    TOKEN_DO,           // do
    TOKEN_SWITCH,       // switch
    TOKEN_CASE,         // case
    TOKEN_DEFAULT,      // default
    TOKEN_BREAK,        // break
    TOKEN_EQ,           // ==
    TOKEN_NE,           // !=
    TOKEN_LT,           // <
    TOKEN_GT,           // >
    TOKEN_LE,           // <=
    TOKEN_GE,           // >=
    TOKEN_AND,          // &&
    TOKEN_OR,           // ||
    TOKEN_NOT,          // !
    TOKEN_EOF,          
    TOKEN_UNKNOWN       
} TokenType;


typedef struct {
    TokenType type;
    char lexeme[256];
    int value;
} Token;

typedef struct {
    char name[256];
    int value;
    bool initialized;
} Symbol;

Token tokens[1000];
int tokenCount = 0;
int currentToken = 0;
Symbol symbolTable[100];
int symbolCount = 0;
bool hasError = false;

int c;

/* Forward declarations */
void addToken(TokenType type, const char* lexeme, int value);
bool check(TokenType type);
void parseStatement();
void parseBlock();
void parseDeclaration();
void parsePrintStatement();
void parseIfStatement();
void parseWhileStatement();
void parseForStatement();
void parseDoWhileStatement();
void parseSwitchStatement();
void parseBreakStatement();
int getVariableValue(const char* name, bool mustExist);
void parseAssignment(const char* identifier);
int parseExpression();
int parseLogicalOr();
int parseLogicalAnd();
int parseComparison();
int parseAdditive();
int parseTerm();
int parseFactor();
void setVariable(const char* name, int value);

/* Error Handling */
void error(const char* message) {
     fprintf(stderr, "\033[1;31mERROR: \033[0m%s\n", message);
    hasError = true;
}

void print(const char* message) {
  // printf("%s\n", message);
}

void tokenPrint(Token token) {
    //printf("Token: Type=%d, Lexeme='%s', Value=%d\n", token.type, token.lexeme, token.value);           
}

void printToken(){
     // Print all tokens
    for (int i = 0; i < tokenCount; i++) {
        tokenPrint(tokens[i]);
    }
}

/* Read entire file into memory */
char* readFile(const char* filename) {
    print("Reading source file...");
    FILE* file = fopen(filename, "r");
    if (!file) {
        error("Cannot open input file");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file); //ftell - Returns current file position (now at end, so this gives file size)
    rewind(file); //rewind(file); - Moves file position back to beginning

    char* buffer = (char*)malloc(fileSize + 1); // +1 for null terminator

    size_t bytesRead = fread(buffer, 1, fileSize, file); //fread - Reads data from file | buffer - Destination to store read data | 1 - Size of each element to read (1 byte) | fileSize - Number of elements to read | file - Source file stream
    buffer[bytesRead] = '\0'; //Add null terminator to end of buffer
    fclose(file);

    return buffer;
}

/* Add token to token array */
void addToken(TokenType type, const char* lexeme, int value) {
    print("Adding token to array");
    if (tokenCount >= MAX_TOKENS) {
        error("Too many tokens");
        return;
    }
    tokens[tokenCount].type = type;
    strcpy(tokens[tokenCount].lexeme, lexeme);
    tokens[tokenCount].value = value;
    tokenCount++;
    //printf("Added token(%d): Type=%d, Lexeme='%s', Value=%d\n", tokenCount - 1, type, lexeme, value);
}

/* Tokenizer - Lexical Analysis */
void tokenize(const char* source) {
    print("Starting tokenization...");
    int i = 0;
    
    int len = strlen(source);

    while (i < len) {
        // Skip whitespace
        while (i < len && isspace(source[i])) 
            i++;
        

        if (i >= len) break;

        // Check for keywords and identifiers
        if (isalpha(source[i]) || source[i] == '_') {
            char lexeme[MAX_TOKEN_LEN];
            int j = 0;

            while (i < len && (isalnum(source[i]) || source[i] == '_')) 
                lexeme[j++] = source[i++];

            lexeme[j] = '\0';

            if (strcmp(lexeme, "int") == 0) addToken(TOKEN_INT, lexeme, 0);
            else if (strcmp(lexeme, "print") == 0) addToken(TOKEN_PRINT, lexeme, 0);
            else if (strcmp(lexeme, "if") == 0) addToken(TOKEN_IF, lexeme, 0);
            else if (strcmp(lexeme, "else") == 0) addToken(TOKEN_ELSE, lexeme, 0);
            else if (strcmp(lexeme, "for") == 0) addToken(TOKEN_FOR, lexeme, 0);
            else if (strcmp(lexeme, "while") == 0) addToken(TOKEN_WHILE, lexeme, 0);
            else if (strcmp(lexeme, "do") == 0) addToken(TOKEN_DO, lexeme, 0);
            else if (strcmp(lexeme, "switch") == 0) addToken(TOKEN_SWITCH, lexeme, 0);
            else if (strcmp(lexeme, "case") == 0) addToken(TOKEN_CASE, lexeme, 0);
            else if (strcmp(lexeme, "default") == 0) addToken(TOKEN_DEFAULT, lexeme, 0);
            else if (strcmp(lexeme, "break") == 0) addToken(TOKEN_BREAK, lexeme, 0);
            else addToken(TOKEN_IDENTIFIER, lexeme, 0);
        }
        // Check for numbers
        else if (isdigit(source[i])) {
            char lexeme[MAX_TOKEN_LEN];
            
            int j = 0;
            int value = 0;
            while (i < len && isdigit(source[i])) {
                lexeme[j++] = source[i];
                value = value * 10 + (source[i] - '0');
                i++;
            }
            lexeme[j] = '\0';
            addToken(TOKEN_NUMBER, lexeme, value);
        }
        // Check for operators and punctuation
        else {
            char ch = source[i];
            char lexeme[3] = {ch, '\0', '\0'};

            if (ch == '+') addToken(TOKEN_PLUS, lexeme, 0);
            else if (ch == '-') addToken(TOKEN_MINUS, lexeme, 0);
            else if (ch == '*') addToken(TOKEN_MULTIPLY, lexeme, 0);
            else if (ch == '/') addToken(TOKEN_DIVIDE, lexeme, 0);
            else if (ch == '{') addToken(TOKEN_LBRACE, lexeme, 0);
            else if (ch == '}') addToken(TOKEN_RBRACE, lexeme, 0);
            else if (ch == ':') addToken(TOKEN_COLON, lexeme, 0);
            else if (ch == ';') addToken(TOKEN_SEMICOLON, lexeme, 0);
            else if (ch == '(') addToken(TOKEN_LPAREN, lexeme, 0);
            else if (ch == ')') addToken(TOKEN_RPAREN, lexeme, 0);
            else if (ch == '=') {
                if (i + 1 < len && source[i + 1] == '=') {
                    addToken(TOKEN_EQ, "==", 0);
                    i++;
                } else {
                    addToken(TOKEN_ASSIGN, lexeme, 0);
                }
            }
            else if (ch == '!') {
                if (i + 1 < len && source[i + 1] == '=') {
                    addToken(TOKEN_NE, "!=", 0);
                    i++;
                } else {
                    addToken(TOKEN_NOT, lexeme, 0);
                }
            }
            else if (ch == '<') {
                if (i + 1 < len && source[i + 1] == '=') {
                    addToken(TOKEN_LE, "<=", 0);
                    i++;
                } else {
                    addToken(TOKEN_LT, lexeme, 0);
                }
            }
            else if (ch == '>') {
                if (i + 1 < len && source[i + 1] == '=') {
                    addToken(TOKEN_GE, ">=", 0);
                    i++;
                } else {
                    addToken(TOKEN_GT, lexeme, 0);
                }
            }
            else if (ch == '&') {
                if (i + 1 < len && source[i + 1] == '&') {
                    addToken(TOKEN_AND, "&&", 0);
                    i++;
                } else {
                    error("Unknown character in source");
                }
            }
            else if (ch == '|') {
                if (i + 1 < len && source[i + 1] == '|') {
                    addToken(TOKEN_OR, "||", 0);
                    i++;
                } else {
                    error("Unknown character in source");
                }
            }
            else error("Unknown character in source");

            i++;
        }
    }

    addToken(TOKEN_EOF, "", 0);
    
    //printToken();
}

/* Consume and return current token */
Token advance() {
    print("Advancing to next token");
    if (currentToken < tokenCount) {
        return tokens[currentToken++];
    }
    return tokens[tokenCount - 1];
}

/* Check if current token matches type and consume if it does */
bool match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

/* Check if current token matches type without consuming */
bool check(TokenType type) {
    if (currentToken >= tokenCount) return false;
    return tokens[currentToken].type == type;
}

//=============================================================================================

/* Program -> StatementList */
void parseProgram() {
    print("Parsing program...");
    while (!check(TOKEN_EOF) && !hasError) 
        parseStatement();
    
}

/* Statement -> Declaration | Assignment | PrintStatement | IfStatement | WhileStatement 
              | ForStatement | DoWhileStatement | SwitchStatement | BreakStatement */
void parseStatement() {
    print("Parsing statement");
    if (check(TOKEN_INT)) parseDeclaration();
    else if (check(TOKEN_PRINT)) parsePrintStatement();
    else if (check(TOKEN_IF)) parseIfStatement();
    else if (check(TOKEN_WHILE)) parseWhileStatement();
    else if (check(TOKEN_FOR)) parseForStatement();
    else if (check(TOKEN_DO)) parseDoWhileStatement();
    else if (check(TOKEN_SWITCH)) parseSwitchStatement();
    else if (check(TOKEN_BREAK)) parseBreakStatement();
    else if (check(TOKEN_IDENTIFIER)) {
        Token id = advance();
        parseAssignment(id.lexeme);
    }
    else if (check(TOKEN_LBRACE)) {
        advance(); // consume '{'
        while (!check(TOKEN_RBRACE) && !check(TOKEN_EOF) && !hasError)
            parseStatement();
        if (!match(TOKEN_RBRACE)) {
            error("Syntax error: Expected '}'");
        }
    }
    else if (check(TOKEN_EOF)) return;
    else {
        error("Syntax error: Expected statement");
        advance(); // Skip invalid token
    }
}

/* Declaration -> 'int' IDENTIFIER '=' Expression ';' */
void parseDeclaration() {
    print("Parsing declaration");
    advance(); // consume 'int'

    if (!check(TOKEN_IDENTIFIER)) {
        error("Syntax error: Expected identifier after 'int'");
        return;
    }

    Token identifier = advance();
    //printf("Declaring variable: %s\n", identifier.lexeme);

    // Check if variable already declared
    for (int i = 0; i < symbolCount; i++) 
        if (strcmp(symbolTable[i].name, identifier.lexeme) == 0) {
            error("Semantic error: Variable already declared");
            return;
        }
    

    if (!match(TOKEN_ASSIGN)) {
        error("Syntax error: Expected '=' in declaration");
        return;
    }

    int value = parseExpression();

    if (!match(TOKEN_SEMICOLON)) {
        error("Syntax error: Expected ';' at end of statement");
        return;
    }

    if (!hasError) 
        setVariable(identifier.lexeme, value);
    
}

/* Assignment -> IDENTIFIER '=' Expression ';' */
void parseAssignment(const char* identifier) {
    print("Parsing assignment");
    // Check if variable is declared
    int index = -1;
    for (int i = 0; i < symbolCount; i++) 
        if (strcmp(symbolTable[i].name, identifier) == 0) {
            index = i;
            break;
        }
    

    if (index == -1) {
        error("Semantic error: Variable not declared");
        return;
    }

    if (!match(TOKEN_ASSIGN)) {
        error("Syntax error: Expected '=' in assignment");
        return;
    }

    int value = parseExpression();

    if (!match(TOKEN_SEMICOLON)) {
        error("Syntax error: Expected ';' at end of statement");
        return;
    }

    if (!hasError) {
        symbolTable[index].value = value;
        symbolTable[index].initialized = true;
    }
}

/* PrintStatement -> 'print' '(' Expression ')' ';' */
void parsePrintStatement() {
    print("Parsing print statement");
    advance(); // consume 'print'

    if (!match(TOKEN_LPAREN)) {
        error("Syntax error: Expected '(' after 'print'");
        return;
    }

    int value = parseExpression();

    if (!match(TOKEN_RPAREN)) {
        error("Syntax error: Expected ')' after expression");
        return;
    }

    if (!match(TOKEN_SEMICOLON)) {
        error("Syntax error: Expected ';' at end of statement");
        return;
    }

    if (!hasError) {
        printf("%d\n", value);
    }
}

/* Expression -> LogicalOr */
int parseExpression() {
    print("Parsing expression");
    return parseLogicalOr();
}

/* LogicalOr -> LogicalAnd ('||' LogicalAnd)* */
int parseLogicalOr() {
    print("Parsing logical or");
    int result = parseLogicalAnd();

    while (match(TOKEN_OR)) {
        int right = parseLogicalAnd();
        result = result || right;
    }

    return result;
}

/* LogicalAnd -> Comparison ('&&' Comparison)* */
int parseLogicalAnd() {
    print("Parsing logical and");
    int result = parseComparison();

    while (match(TOKEN_AND)) {
        int right = parseComparison();
        result = result && right;
    }

    return result;
}

/* Comparison -> Additive (('==' | '!=' | '<' | '>' | '<=' | '>=') Additive)* */
int parseComparison() {
    print("Parsing comparison");
    int result = parseAdditive();

    while (check(TOKEN_EQ) || check(TOKEN_NE) || check(TOKEN_LT) || 
           check(TOKEN_GT) || check(TOKEN_LE) || check(TOKEN_GE)) {
        TokenType op = tokens[currentToken].type;
        advance();
        int right = parseAdditive();

        switch(op) {
            case TOKEN_EQ: result = (result == right) ? 1 : 0; break;
            case TOKEN_NE: result = (result != right) ? 1 : 0; break;
            case TOKEN_LT: result = (result < right) ? 1 : 0; break;
            case TOKEN_GT: result = (result > right) ? 1 : 0; break;
            case TOKEN_LE: result = (result <= right) ? 1 : 0; break;
            case TOKEN_GE: result = (result >= right) ? 1 : 0; break;
            default: break;
        }
    }

    return result;
}

/* Additive -> Term (('+' | '-') Term)* */
int parseAdditive() {
    print("Parsing additive");
    int result = parseTerm();

    while (match(TOKEN_PLUS) || match(TOKEN_MINUS)) {
        TokenType op = tokens[currentToken - 1].type;
        int right = parseTerm();

        if (op == TOKEN_PLUS) {
            result += right;
        } else {
            result -= right;
        }
    }

    return result;
}

/* Term -> Factor (('*' | '/') Factor)* */
int parseTerm() {
    print("Parsing term");
    int result = parseFactor();

    while (match(TOKEN_MULTIPLY) || match(TOKEN_DIVIDE)) {
        TokenType op = tokens[currentToken - 1].type;
        int right = parseFactor();

        if (op == TOKEN_MULTIPLY) {
            result *= right;
        } else {
            if (right == 0) {
                error("Semantic error: Division by zero");
                return 0;
            }
            result /= right;
        }
    }

    return result;
}

/* Factor -> ('!' | '-')* (NUMBER | IDENTIFIER | '(' Expression ')') */
int parseFactor() {
    print("Parsing factor");
    int negate = 0;
    int notOp = 0;

    while (check(TOKEN_MINUS) || check(TOKEN_NOT)) {
        if (match(TOKEN_MINUS)) {
            negate = !negate;
        } else if (match(TOKEN_NOT)) {
            notOp = !notOp;
        }
    }

    int result = 0;

    if (check(TOKEN_NUMBER)) {
        Token num = advance();
        result = num.value;
    }
    else if (check(TOKEN_IDENTIFIER)) {
        Token id = advance();
        result = getVariableValue(id.lexeme, true);
    }
    else if (match(TOKEN_LPAREN)) {
        result = parseExpression();
        if (!match(TOKEN_RPAREN)) {
            error("Syntax error: Expected ')' after expression");
        }
    }
    else if (check(TOKEN_PLUS)) {
        advance();
        result = parseFactor();  // Handle unary plus
    }
    else {
        error("Syntax error: Expected expression");
        return 0;
    }

    if (negate) result = -result;
    if (notOp) result = result ? 0 : 1;

    return result;
}

//Find variable index in symbol table 
int findVariableIndex(const char* name) {
    for (int i = 0; i < symbolCount; i++) {
        if (strcmp(symbolTable[i].name, name) == 0) {
            return i;  // Return index if found
        }
    }
    return -1;  // Return -1 if not found
}

/* Parse a block of statements (used for control structures) */
void parseBlock() {
    print("Parsing block");
    if (!match(TOKEN_LBRACE)) {
        // Single statement without braces
        parseStatement();
    } else {
        // Multiple statements with braces
        while (!check(TOKEN_RBRACE) && !check(TOKEN_EOF) && !hasError) {
            parseStatement();
        }
        if (!match(TOKEN_RBRACE)) {
            error("Syntax error: Expected '}'");
        }
    }
}

/* IfStatement -> 'if' '(' Expression ')' Block ('else' Block)? */
void parseIfStatement() {
    print("Parsing if statement");
    advance(); // consume 'if'

    if (!match(TOKEN_LPAREN)) {
        error("Syntax error: Expected '(' after 'if'");
        return;
    }

    int condition = parseExpression();

    if (!match(TOKEN_RPAREN)) {
        error("Syntax error: Expected ')' after condition");
        return;
    }

    if (condition) {
        parseBlock();
        // Skip else block if present
        if (check(TOKEN_ELSE)) {
            advance();
            // Skip the else block by consuming tokens
            if (check(TOKEN_LBRACE)) {
                advance();
                int braceCount = 1;
                while (braceCount > 0 && !check(TOKEN_EOF)) {
                    if (check(TOKEN_LBRACE)) braceCount++;
                    if (check(TOKEN_RBRACE)) braceCount--;
                    advance();
                }
            } else {
                parseStatement();
            }
        }
    } else {
        // Skip if block
        if (check(TOKEN_LBRACE)) {
            advance();
            int braceCount = 1;
            while (braceCount > 0 && !check(TOKEN_EOF)) {
                if (check(TOKEN_LBRACE)) braceCount++;
                if (check(TOKEN_RBRACE)) braceCount--;
                advance();
            }
        } else {
            parseStatement();
        }
        
        // Parse else block if present
        if (match(TOKEN_ELSE)) {
            parseBlock();
        }
    }
}

/* WhileStatement -> 'while' '(' Expression ')' Block */
void parseWhileStatement() {
    print("Parsing while statement");
    advance(); // consume 'while'

    if (!match(TOKEN_LPAREN)) {
        error("Syntax error: Expected '(' after 'while'");
        return;
    }

    int conditionStart = currentToken;
    
    // Parse and evaluate condition
    int condition = parseExpression();

    if (!match(TOKEN_RPAREN)) {
        error("Syntax error: Expected ')' after condition");
        return;
    }

    // Now handle the loop body
    // For proper loop execution, we need to repeatedly execute the body
    // while the condition remains true
    
    // Save the body starting position
    int bodyStartToken = currentToken;
    
    if (!check(TOKEN_LBRACE)) {
        error("Syntax error: Expected '{' to start while block");
        return;
    }
    
    // Loop execution
    while (condition) {
        // Move to body start and execute it
        currentToken = bodyStartToken;
        advance(); // skip opening brace
        
        // Execute statements until closing brace
        while (!check(TOKEN_RBRACE) && !check(TOKEN_EOF)) {
            parseStatement();
        }
        
        if (check(TOKEN_RBRACE)) advance();
        
        // Re-evaluate condition
        currentToken = conditionStart;
        condition = parseExpression();
        
        // Skip past the closing paren
        while (!check(TOKEN_RPAREN) && !check(TOKEN_EOF)) {
            advance();
        }
        if (check(TOKEN_RPAREN)) advance();
    }
    
    // After condition becomes false, skip the body block
    currentToken = bodyStartToken;
    if (check(TOKEN_LBRACE)) {
        advance();
        int braceCount = 1;
        while (braceCount > 0 && !check(TOKEN_EOF)) {
            if (check(TOKEN_LBRACE)) braceCount++;
            if (check(TOKEN_RBRACE)) braceCount--;
            advance();
        }
    }
}

/* ForStatement -> 'for' '(' Declaration|Assignment ';' Expression ';' Assignment ')' Block */
void parseForStatement() {
    print("Parsing for statement");
    advance(); // consume 'for'

    if (!match(TOKEN_LPAREN)) {
        error("Syntax error: Expected '(' after 'for'");
        return;
    }

    // Parse initialization (declaration or assignment)
    if (check(TOKEN_INT)) {
        parseDeclaration();
    } else if (check(TOKEN_IDENTIFIER)) {
        Token id = advance();
        parseAssignment(id.lexeme);
    } else {
        error("Syntax error: Expected initialization in for loop");
        return;
    }

    // Parse condition
    int condition = parseExpression();

    if (!match(TOKEN_SEMICOLON)) {
        error("Syntax error: Expected ';' after condition in for loop");
        return;
    }

    // Parse increment (just parse it, don't execute)
    if (check(TOKEN_IDENTIFIER)) {
        advance();
        if (match(TOKEN_ASSIGN)) {
            parseExpression();
        }
    }

    if (!match(TOKEN_RPAREN)) {
        error("Syntax error: Expected ')' after for clauses");
        return;
    }

    // Parse loop body once
    if (condition) {
        parseBlock();
    } else {
        // Skip block if condition is false
        if (check(TOKEN_LBRACE)) {
            advance();
            int braceCount = 1;
            while (braceCount > 0 && !check(TOKEN_EOF)) {
                if (check(TOKEN_LBRACE)) braceCount++;
                if (check(TOKEN_RBRACE)) braceCount--;
                advance();
            }
        } else {
            advance();
        }
    }
}

/* DoWhileStatement -> 'do' Block 'while' '(' Expression ')' ';' */
void parseDoWhileStatement() {
    print("Parsing do-while statement");
    advance(); // consume 'do'

    parseBlock();

    if (!match(TOKEN_WHILE)) {
        error("Syntax error: Expected 'while' after do block");
        return;
    }

    if (!match(TOKEN_LPAREN)) {
        error("Syntax error: Expected '(' after 'while'");
        return;
    }

    parseExpression();  // Parse and discard condition

    if (!match(TOKEN_RPAREN)) {
        error("Syntax error: Expected ')' after condition");
        return;
    }

    if (!match(TOKEN_SEMICOLON)) {
        error("Syntax error: Expected ';' after do-while");
        return;
    }
}

/* SwitchStatement -> 'switch' '(' Expression ')' '{' CaseList '}' */
void parseSwitchStatement() {
    print("Parsing switch statement");
    advance(); // consume 'switch'

    if (!match(TOKEN_LPAREN)) {
        error("Syntax error: Expected '(' after 'switch'");
        return;
    }

    int switchValue = parseExpression();

    if (!match(TOKEN_RPAREN)) {
        error("Syntax error: Expected ')' after switch expression");
        return;
    }

    if (!match(TOKEN_LBRACE)) {
        error("Syntax error: Expected '{' after switch");
        return;
    }

    int caseMatched = 0;
    int switchDone = 0;

    while (!check(TOKEN_RBRACE) && !check(TOKEN_EOF) && !hasError) {
        if (check(TOKEN_CASE)) {
            advance(); // consume 'case'

            if (!check(TOKEN_NUMBER)) {
                error("Syntax error: Expected number after 'case'");
                return;
            }

            int caseValue = advance().value;

            if (!match(TOKEN_COLON)) {
                error("Syntax error: Expected ':' after case value");
                return;
            }

            if (!switchDone && switchValue == caseValue && !caseMatched) {
                caseMatched = 1;
            }

            // Execute statements until break or next case/default
            while (!check(TOKEN_CASE) && !check(TOKEN_DEFAULT) && 
                   !check(TOKEN_RBRACE) && !check(TOKEN_EOF) && !hasError) {
                if (check(TOKEN_BREAK)) {
                    if (caseMatched && !switchDone) {
                        advance(); // consume 'break'
                        if (!match(TOKEN_SEMICOLON)) {
                            error("Syntax error: Expected ';' after break");
                        }
                        switchDone = 1;
                    } else {
                        advance();
                    }
                    break;
                } else {
                    if (caseMatched && !switchDone) {
                        parseStatement();
                    } else {
                        // Skip statement
                        advance();
                    }
                }
            }
        } else if (check(TOKEN_DEFAULT)) {
            advance(); // consume 'default'

            if (!match(TOKEN_COLON)) {
                error("Syntax error: Expected ':' after 'default'");
                return;
            }

            if (!switchDone && !caseMatched) {
                caseMatched = 1;
            }

            // Execute default statements
            while (!check(TOKEN_RBRACE) && !check(TOKEN_EOF) && !hasError) {
                if (check(TOKEN_BREAK)) {
                    if (caseMatched && !switchDone) {
                        advance();
                        if (!match(TOKEN_SEMICOLON)) {
                            error("Syntax error: Expected ';' after break");
                        }
                        switchDone = 1;
                    } else {
                        advance();
                    }
                    break;
                } else {
                    if (caseMatched && !switchDone) {
                        parseStatement();
                    } else {
                        advance();
                    }
                }
            }
        } else if (switchDone) {
            // If break was executed, skip remaining tokens until closing brace
            advance();
        } else {
            advance();
        }
    }

    if (!match(TOKEN_RBRACE)) {
        error("Syntax error: Expected '}' after switch cases");
    }
}

/* BreakStatement -> 'break' ';' */
void parseBreakStatement() {
    print("Parsing break statement");
    advance(); // consume 'break'

    if (!match(TOKEN_SEMICOLON)) {
        error("Syntax error: Expected ';' after break");
    }
}

//Get variable value with semantic checks 
int getVariableValue(const char* name, bool mustExist) {
    print("Getting variable value");
    int index = findVariableIndex(name);
    
    // Variable not found
    if (index == -1) {
        if (mustExist) {
            error("Semantic error: Undeclared variable");
        }
        return 0;
    }
    
    // Variable found but not initialized
    if (!symbolTable[index].initialized) {
        error("Semantic error: Variable used before initialization");
        return 0;
    }
    
    // Variable found and initialized
    return symbolTable[index].value;
}

int countSymbols() {
    if (symbolCount >= 100) {
        error("Too many variables");
        return -1;
    }
    return symbolCount;
}

/* Set variable value in symbol table */
void setVariable(const char* name, int value) {
    print("Setting variable in symbol table");
    if (countSymbols() == -1) return;

    strcpy(symbolTable[symbolCount].name, name);
    symbolTable[symbolCount].value = value;
    symbolTable[symbolCount].initialized = true;
    symbolCount++;
}

int correctArguments(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 0;
    }
    return 1;
}

int logic(int argc, char* argv[]){
    print("Starting compiler logic...");

    if (!correctArguments(argc, argv)) return 1;

    // Read source file
    char* source = readFile(argv[1]);

    if (!source || hasError) return 1;
    

    // Tokenize
    tokenize(source);
    free(source);

    if (hasError) return 1;
    

    // Parse and Execute
    parseProgram();

    if (hasError) return 1;

    return 0;
}

/* Main Function */
int main(int argc, char* argv[]) {
    print("=== Compiler Started ===");
    return logic(argc, argv);
}