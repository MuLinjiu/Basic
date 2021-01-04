/*
 * File: program.cpp
 * -----------------
 * This file is a stub implementation of the program.h interface
 * in which none of the methods do anything beyond returning a
 * value of the correct type.  Your job is to fill in the bodies
 * of each of these methods with an implementation that satisfies
 * the performance guarantees specified in the assignment.
 */

#include <string>
#include "program.h"
#include "../StanfordCPPLib/error.h"
#include "../StanfordCPPLib/tokenscanner.h"
#include "statement.h"
using namespace std;

Program::Program() {
}

Program::~Program() {
    this->clear();
}

void Program::clear() {
    for(auto & it : maptable){
            delete it.second.exp;
            it.second.exp = nullptr;
    }
    maptable.clear();
}

void Program::addSourceLine(int lineNumber, string line1) {
    if(!this->myfind(lineNumber)){
        maptable.insert(make_pair(lineNumber,line(line1)));
    }
    else {
        maptable[lineNumber].information = line1 ;
        if(maptable[lineNumber].exp!=nullptr)maptable[lineNumber].exp=nullptr ;
    }
}

void Program::removeSourceLine(int lineNumber) {
    if(!this->myfind(lineNumber))error("SYNTAX ERROR");
    else{
        delete maptable[lineNumber].exp;
        maptable.erase(lineNumber);
    }
}

string Program::getSourceLine(int lineNumber) {
    if(!this->myfind(lineNumber))error("SYNTAX ERROR");
    else{
        return maptable[lineNumber].information;
    }
}

void Program::setParsedStatement(int lineNumber, Statement *stmt) {
    if(!this->myfind(lineNumber))error("SYNTAX ERROR");
    else{

        delete maptable[lineNumber].exp;
        maptable[lineNumber].exp = stmt;
    }
}

Statement *Program::getParsedStatement(int lineNumber) {
    if(!this->myfind(lineNumber))error("SYNTAX ERROR");
    else{
        return maptable[lineNumber].exp;
    }
}

int Program::getFirstLineNumber() {
    if(maptable.empty())error("SYNTAX ERROR");
   auto it = maptable.begin();
   return it->first;
}
int Program::getNextLineNumber(int lineNumber) {
   for(auto & it : maptable){
       if(it.first > lineNumber)return it.first;
   }
   error("SYNTAX ERROR");
}
bool Program::myfind(int a){
    for(auto & it : maptable){
        if(it.first == a)return true;
    }
    return false;
}
void Program::my_show_list(){
    for(auto & it : maptable){
        cout<<it.second.information<<endl;
    }
}
void Program::my_run_programme(EvalState &state){
    if(maptable.empty()){
        error("error run");
        return;
    }
    auto it = maptable.begin();
    while(it!=maptable.end()){
        try{
            (it->second.exp)->execute(state);
            it++;
        }
        catch(ErrorException &a) {
            TokenScanner scanner;
            if(scanner.getTokenType(a.getMessage()) == NUMBER){
                if(!this->myfind(stringToInteger(a.getMessage()))){
                    error("goto");
                    return;
                }
                else {
                    int number = stringToInteger(a.getMessage());
                    it = this->maptable.find(number);
                    continue;
                }
            }else{
                error(a.getMessage());
            }
        }
    }
}