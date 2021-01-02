/*
 * File: parser.cpp
 * ----------------
 * Implements the parser.h interface.
 */

#include <iostream>
#include <string>

#include "exp.h"
#include "parser.h"

#include "../StanfordCPPLib/error.h"
#include "../StanfordCPPLib/strlib.h"
#include "../StanfordCPPLib/tokenscanner.h"
#include "statement.h"

using namespace std;

/*
 * Implementation notes: parseExp
 * ------------------------------
 * This code just reads an expression and then checks for extra tokens.
 */

Expression *parseExp(TokenScanner & scanner) {
   Expression *exp = readE(scanner);
   if (scanner.hasMoreTokens()) {
      error("parseExp: Found extra token: " + scanner.nextToken());
   }
   return exp;
}
Statement *parsestatment(TokenScanner & scanner,string line){
    string token;
    TokenType token_type;
    token = scanner.nextToken();
    token_type = scanner.getTokenType(token);
    if(token_type != WORD){
        error("SYNTAX ERROR");
    }
    else{
        if(token == "REM"){
            if(!scanner.hasMoreTokens()){
                error("SYNTAX ERROR");
            }
            else return new REM();
        }
        else if(token == "END"){
            if(scanner.hasMoreTokens()){
                error("SYNTAX ERROR");
            }
            else return new END();
        }
        else if(token == "PRINT"){
            if(!scanner.hasMoreTokens()){
                error("SYNTAX ERROR");
            }
            else {
                Expression *exp = parseExp(scanner);
                if(exp->getType() == COMPOUND){
                    if(((CompoundExp*)exp)->getOp()=="="){
                        error("SYNTAX ERROR");
                        delete exp;
                    }
                }
                else if(scanner.hasMoreTokens()){
                    error("SYNTAX ERROR");
                    delete exp;
                }
                    return new PRINT(exp);
            }
        }
        else if(token == "GOTO"){
            if(!scanner.hasMoreTokens()){
                error("SYNTAX ERROR");
            }
            else {
                token = scanner.nextToken();
                token_type = scanner.getTokenType(token);
                if(scanner.hasMoreTokens()){
                    error("SYNTAX ERROR");
                }
                else {
                    if(token_type != NUMBER){
                        error("SYNTAX ERROR");
                    }
                        int linenumbe = stringToInteger(token);
                        return new GOTO(linenumbe);
                }
            }
        }
        else if(token == "LET"){
            Expression *exp = parseExp(scanner);
            if(((IdentifierExp*)(((CompoundExp*)exp)->getLHS()))->getName()=="LET"){
                error("SYNTAX ERROR");
                delete exp;
            }
            if(exp->getType() != COMPOUND){
                error("SYNTAX ERROR");
                delete exp;
            }
            if(((CompoundExp*)exp)->getOp()!="="){
                error("SYNTAX ERROR");
                delete exp;
            }
            if(((Expression*)(((CompoundExp*)exp)->getLHS()))->getType()!=IDENTIFIER){
                error("SYNTAX ERROR");
                delete exp;
            }
            return new LET(exp);
        }
        else if(token == "INPUT"){
            if(!scanner.hasMoreTokens()){
                error("SYNTAX ERROR");
            }
            else {
                token = scanner.nextToken();
                token_type = scanner.getTokenType(token);
                if(token_type != WORD){
                    error("SYNTAX ERROR1");
                }
                else if(scanner.hasMoreTokens()){
                    error("SYNTAX ERROR2");
                }
                return new INPUT(token);
            }
        }
        else if(token == "IF"){
            Expression *a,*b;
            a = readE(scanner);
            token = scanner.nextToken();
            if(line.find('=')==string::npos){//借鉴赵一龙，即end();
                //a = readE(scanner);
                string tmp = token;
                if(token != "<" &&token != ">" && token != "="){
                    error("SYNTAX ERROR");
                    delete a;
                }
                b = readE(scanner);
                token = scanner.nextToken();
                if(token != "THEN"){
                    error("SYNTAX ERROR");
                    delete a;
                    delete b;
                }
                else {
                    if (!scanner.hasMoreTokens()) {
                        error("SYNTAX ERROR");
                        delete a;
                        delete b;
                    } else {
                        token = scanner.nextToken();
                        token_type = scanner.getTokenType(token);
                        if (token_type != NUMBER) {
                            error("SYNTAX ERROR");
                            delete a;
                            delete b;
                        }
                        GOTO *goto1 = new GOTO(stringToInteger(token));
                        return new IF(a,tmp,b,goto1);
                    }
                }
            }
            else {
                string tmp = "=";
                string newa,newb;
                scanner.setInput(line);
                token = scanner.nextToken();
                token = scanner.nextToken();
                while(scanner.hasMoreTokens()){
                    token = scanner.nextToken();
                    if(token == "=")break;
                    newa += (token+" ");
                }
                b = readE(scanner);
                token = scanner.nextToken();
                if(token != "THEN"){
                    error("SYNTAX ERROR");
                }
                token = scanner.nextToken();
                token_type = scanner.getTokenType(token);
                if(token_type!=NUMBER){
                    error("SYNTAX ERROR");
                }
                GOTO *goto_ =new GOTO(stringToInteger(token));
                scanner.setInput(newa);
                a = readE(scanner);
                return new IF(a,tmp,b,goto_);
            }
        }
    }
    error("SYNTAX ERROR");
}
/*
 * Implementation notes: readE
 * Usage: exp = readE(scanner, prec);
 * ----------------------------------
 * This version of readE uses precedence to resolve the ambiguity in
 * the grammar.  At each recursive level, the parser reads operators and
 * subexpressions until it finds an operator whose precedence is greater
 * than the prevailing one.  When a higher-precedence operator is found,
 * readE calls itself recursively to read in that subexpression as a unit.
 */

Expression *readE(TokenScanner & scanner, int prec) {
   Expression *exp = readT(scanner);
   string token;
   while (true) {
      token = scanner.nextToken();
      int newPrec = precedence(token);
      if (newPrec <= prec) break;
      Expression *rhs = readE(scanner, newPrec);
      exp = new CompoundExp(token, exp, rhs);
   }
   scanner.saveToken(token);
   return exp;
}

/*
 * Implementation notes: readT
 * ---------------------------
 * This function scans a term, which is either an integer, an identifier,
 * or a parenthesized subexpression.
 */

Expression *readT(TokenScanner & scanner) {
   string token = scanner.nextToken();
   TokenType type = scanner.getTokenType(token);
   if (type == WORD) return new IdentifierExp(token);
   if (type == NUMBER) return new ConstantExp(stringToInteger(token));
   if (token != "(") error("Illegal term in expression");
   Expression *exp = readE(scanner);
   if (scanner.nextToken() != ")") {
      error("Unbalanced parentheses in expression");
   }
   return exp;
}

/*
 * Implementation notes: precedence
 * --------------------------------
 * This function checks the token against each of the defined operators
 * and returns the appropriate precedence value.
 */

int precedence(string token) {
   if (token == "=") return 1;
   if (token == "+" || token == "-") return 2;
   if (token == "*" || token == "/") return 3;
   return 0;
}
