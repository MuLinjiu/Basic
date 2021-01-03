/*
 * File: Basic.cpp
 * ---------------
 * Name: [TODO: enter name here]
 * Section: [TODO: enter section leader here]
 * This file is the starter project for the BASIC interpreter from
 * Assignment #6.
 * [TODO: extend and correct the documentation]
 */

#include <cctype>
#include<ios>
#include <iostream>
#include <string>
#include "exp.h"
#include "parser.h"
#include "program.h"
#include "../StanfordCPPLib/error.h"
#include "../StanfordCPPLib/tokenscanner.h"

#include "../StanfordCPPLib/simpio.h"
#include "../StanfordCPPLib/strlib.h"
using namespace std;

/* Function prototypes */

void processLine(const string& line, Program & program, EvalState & state);
/* Main program */

int main() {
   EvalState state;
   Program program;
   while (true) {
      try {

         processLine(getLine(), program, state);
      } catch (ErrorException & ex) {
         cerr << "Error: " << ex.getMessage() << endl;
      }
   }
   return 0;
}

/*
 * Function: processLine
 * Usage: processLine(line, program, state);
 * -----------------------------------------
 * Processes a single line entered by the user.  In this version,
 * the implementation does exactly what the interpreter program
 * does in Chapter 19: read a line, parse it as an expression,
 * and then print the result.  In your implementation, you will
 * need to replace this method with one that can respond correctly
 * when the user enters a program line (which begins with a number)
 * or one of the BASIC commands, such as LIST or RUN.
 */

void processLine(const string& line, Program & program, EvalState & state) {
   TokenScanner scanner;
   scanner.ignoreWhitespace();
   scanner.scanNumbers();
   scanner.setInput(line);
   string token;
   TokenType token_type;
   if(scanner.hasMoreTokens())token = scanner.nextToken();
   else return;
   token_type =scanner.getTokenType(token);
   if(token_type == WORD){
       if(check(token)) {
           switch (token[0]) {
               case 'R':
                   if (scanner.hasMoreTokens()) {
                       error("SYNTAX ERROR");
                   } else
                       try {
                           program.my_run_programme(state);
                       } catch (ErrorException &a) {
                           if (a.getMessage() == "end")return;
                           if (a.getMessage() == "zero") {
                               cout << "DIVIDE BY ZERO" << endl;
                               return;
                           }
                           if (a.getMessage() == "goto") {
                               cout << "LINE NUMBER ERROR" << endl;
                           } else {
                               cout << "VARIABLE NOT DEFINED" << endl;
                               return;
                           }
                       }
                   return;
               case 'L':
                   if (token == "LIST") {
                       if (scanner.hasMoreTokens()) {
                           cout << "SYNTAX ERROR\n";
                           return;
                       } else program.my_show_list();
                       return;
                   }
               case 'P' :
               case 'I':
                   scanner.setInput(line);
                   Statement *sta;
                   if (!scanner.hasMoreTokens()) {
                       cout << "SYNTAX ERROR\n";
                       return;
                   } else {
                       try {
                           sta = parsestatment(scanner, line);
                       } catch (...) {
                           cout << "SYNTAX ERROR\n";
                           //delete sta;
                           return;
                       }
                       try {
                           sta->execute(state);
                       } catch (ErrorException &a) {
                           if (a.getMessage() == "zero") {
                               //error("DIVIDE BY ZERO");
                               cout << "DIVIDE BY ZERO\n";
                               delete sta;
                               return;
                           } else {
                               //error("VARIABLE NOT DEFINED");
                               cout << "VARIABLE NOT DEFINED\n";
                               delete sta;
                               return;
                           }
                       }
                       delete sta;
                   }
                   return;
               case 'C':
                   if (scanner.hasMoreTokens()) {
                       cout << "SYNTAX ERROR\n";
                       return;
                   } else {
                       program.clear();
                       state.clear();//不能少！
                   }
                   return;
               case 'Q':
                   if (scanner.hasMoreTokens()) {
                       cout << "SYNTAX ERROR\n";
                       return;
                   } else exit(0);
               case 'H':
                   if (scanner.hasMoreTokens()) {
                       cout << "SYNTAX ERROR\n";
                       return;
                   } else {
                       cout << "RUN : run the program from the lowest line number\n";
                       cout << "LIST : show the command lists in numerical sequence\n";
                       cout << "CLEAR : delete all of the command above \n";
                       cout << "QUIT : exit from the program\n";
                       cout << "HAVE FUN ! \n";
                   }
                   return;
               default:
                   cout << "SYNTAX ERROR\n";
                   return;
           }
       }
       else {
           cout << "SYNTAX ERROR\n";
           return;
       }
   }
   else if(token_type == NUMBER){
           int linenumber;
           try {
               linenumber = stringToInteger(token);
           } catch (...) {
               cout << "SYNTAX ERROR" << endl;
               return;
           }
           if (!scanner.hasMoreTokens()) {
               program.removeSourceLine(linenumber);
               return;
           }
           try {
               Statement *sta = parsestatment(scanner,line);//parser.cpp继续写函数
               scanner.nextToken();
               program.addSourceLine(linenumber, line);//与下一行一起存入信息
               program.setParsedStatement(linenumber, sta);//同上
           } catch (...) {
               cout << "SYNTAX ERROR\n";
               return;
           }
       }
   else {
       cout<<"SYNTAX ERROR\n";
   }
}
