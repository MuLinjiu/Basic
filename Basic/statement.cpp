/*
 * File: statement.cpp
 * -------------------
 * This file implements the constructor and destructor for
 * the Statement class itself.  Your implementation must do
 * the same for the subclasses you define for each of the
 * BASIC statements.
 */

#include <string>
#include "statement.h"
#include "../StanfordCPPLib/tokenscanner.h"
#include "../StanfordCPPLib/simpio.h"
using namespace std;

/* Implementation of the Statement class */

Statement::Statement() {
   /* Empty */
}

Statement::~Statement() {
   /* Empty */
}
REM::REM() = default;
REM::~REM() = default;
void REM::execute(EvalState &state) {
}
LET::LET(Expression *exp):expression(exp){};
LET::~LET(){
    delete expression;
}
void LET::execute(EvalState &state) {
    expression->eval(state);//compound expression s eval
}
PRINT::PRINT(Expression *a) :expression(a){}
PRINT::~PRINT(){
    delete expression;
}
void PRINT::execute(EvalState &state) {
    cout<<expression->eval(state)<<endl;//const expression s eval
}
END::END() = default;
END::~END() = default;
void END::execute(EvalState &state) {
    error("end");
}
GOTO::GOTO(int a):linenumber(a){}
GOTO::~GOTO() = default;
void GOTO::execute(EvalState &state) {
    error(integerToString(linenumber));//throw the exact number//存疑
}
INPUT::INPUT(string &a):name(a) {}
INPUT::~INPUT() noexcept {}
void INPUT::execute(EvalState &state){
    cout<<"?";
    string token;TokenScanner scanner;TokenType token_Type;
    scanner.ignoreWhitespace();
    scanner.ignoreComments();
    while(true){
        int value;
        scanner.setInput(getLine());//
        if(!scanner.hasMoreTokens()){//后面跟东西
            //cout<<"INVALID NUMBER"<<endl<<"?";
            continue;
        }
        token = scanner.nextToken();
        token_Type = scanner.getTokenType(token);
        if(token !="-") {
            if (token_Type != NUMBER) {
                cout << "INVALID NUMBER" << endl << "?";
                continue;
            }
            if (scanner.hasMoreTokens()) {//后面不跟东西
                cout << "INVALID NUMBER" << endl << "?";
                continue;
            }
            try {
                value = stringToInteger(token);
            } catch (...) {
                cout << "INVALID NUMBER" << endl << " ? ";
                continue;
            }
        }
            else {//-------
                token = scanner.nextToken();
                token_Type = scanner.getTokenType(token);
                if(token_Type!=NUMBER){
                    cout<<"INVALID NUMBER"<<endl<<"?";
                    continue;
                }
                if(scanner.hasMoreTokens()){//后面不跟东西
                    cout<<"INVALID NUMBER"<<endl<<"?";
                    continue;
                }
                try{
                    value = -stringToInteger(token);
                }catch(...){
                    cout << "INVALID NUMBER" << endl << " ? ";
                    continue;
            }
        }
            state.setValue(name,value);//存入table
            break;
    }
}
IF::IF(Expression *a, string &b, Expression *c, GOTO *d) :a(a),b(c),d(d),sign(b){}
IF::~IF(){
    delete a;
    delete b;
    delete d;
}
void IF::execute(EvalState &state) {
    int x = a->eval(state);
    int y = b->eval(state);
    bool flag;
    if(sign == "<"){
        if(x < y)flag = true;
        else flag = false;
    }
    if(sign == ">"){
        if(x > y)flag = true;
        else flag = false;
    }
    if(sign == "="){
        if(x == y)flag = true;
        else flag = false;
    }
    if(!flag)return;
    else d->execute(state);
}