//
//  main.cpp
//  assignment3
//
//  Created by Saif Khan on 2015-11-11.
//  Copyright © 2015 Saif Khan. All rights reserved.
//
#include <stdio.h>
#include <stdlib.h>
#include <map>

#include <iostream>
#include <sstream>
#include <cstring>
#include "./lexer.h"
#include "kind.h"
#include <bitset>

using std::vector;
using namespace std;
using namespace ASM;

bool stringContainsLable(string stringToCheck, map<string, int> labelMap) {
    if(labelMap.find(stringToCheck) != labelMap.end()) {
        return true;
    }
    return false;
}

int valueOfLabel(string stringToCheck, map<string, int> labelMap) {
    return ((*labelMap.find(stringToCheck).operator->()).second);
}

enum IDCode { JR, JLR, ADD, SUB, SLT, NOTFOUND };

IDCode getIDCode(string commandName) {
    if(commandName.compare("jr") == 0) {
        return JR;
    } else if(commandName.compare("jalr") == 0){
        return JLR;
    } else if(commandName.compare("add") == 0){
        return ADD;
    } else if(commandName.compare("sub") == 0){
        return SUB;
    } else if(commandName.compare("slt") == 0){
        return SLT;
    } else {
        cerr << "No Such Code " << commandName;
        return NOTFOUND;
    }
}

void printHexString(int value) {
    string bitValue = std::bitset< 32 >(value).to_string();
    char * ptr;
    
    for(int y = 0; y < 4; y++) {
        string first = bitValue.substr(y*8,8);
        long firstLong = strtol(first.c_str(), & ptr , 2);
        int intTest = (int)firstLong;
        char charRepresentation = intTest;
        cout << charRepresentation;
    }
}

void jumpAndRegister(int registerValue, bool linkRegister) {
    int value = (registerValue << 21);
    if(linkRegister) {
        value = value | 9;
    } else {
        value = value | 8;
    }
    printHexString(value);
}

void threeVariablesAndACommand(int first, int second, int third, int commandConstant) {
    int value = (first << 21);
    value = value | (second << 16);
    value = value | (third << 11);
    
    value = third | commandConstant;
    printHexString(value);
}

int main(int argc, const char * argv[]) {
    
    
    std::string input;
    std::getline (std::cin,input);
    map<string, int> labelMap;
    int lineNumber = 0;
    
    Lexer* lexer = new Lexer();
    while(input.size() > 0) {
        bool isRealLine= false;
        
        std::vector<Token*> tokens = (*lexer).scan(input);
        for(int x = 0; x < tokens.size(); x++) {
            Token token = (*tokens[x]);
            if ((*tokens.at(x)).getKind() == LABEL){
                pair<string, int> pair = *new ::pair<string, int>();
                pair.second = lineNumber;
                pair.first = token.getLexeme().substr(0, token.getLexeme().size() - 1);
                labelMap.insert(pair);
            }
            else {
                isRealLine = true;
            }
            
            if (x > 0) {
                
                if((*tokens[x -1]).getKind() == DOTWORD){
                    if((*tokens[x]).getKind() == INT){
                        const string lexeme = token.getLexeme();
                        int value = atoi(lexeme.c_str());
                        printHexString(value);
                    } else if((*tokens[x]).getKind() == ID) {
                        const string lexeme = token.getLexeme();
                    
                        if(stringContainsLable(lexeme, labelMap)) {
                            int value = valueOfLabel(lexeme, labelMap);
                        
                            printHexString(value);
                        } else {
                            cerr << "Label not found: " << lexeme;
                        }
                        
                    }
                }
                else if((*tokens[x -1]).getKind() == ID){
                    int idCode = getIDCode((*tokens[x -1]).getLexeme());
                    
                    if(idCode == JR || idCode == JLR) {
                        if((*tokens[x]).getKind() == REGISTER){
                            string lexeme = (*tokens[x]).getLexeme();
                            int value = atoi(lexeme.substr(1, lexeme.size() - 1).c_str());
                            
                            if(idCode == JR) {
                                jumpAndRegister(value, false);
                            } else if(idCode == JLR) {
                                jumpAndRegister(value, true);
                            }
                        }
                    }
                }
                else if((*tokens[x -4]).getKind() == ID){
                    
                }
                
            }
            //            cout << "size is fuckiiing " << tokens.size();
            
        }
        if(isRealLine) {
            lineNumber = lineNumber + 4;
        }
        
        std::getline (std::cin,input);
    }
    
    typedef std::map<string, int>::iterator it_type;
    for(it_type iterator = labelMap.begin(); iterator != labelMap.end(); iterator++) {
        cerr << iterator->second << " " << iterator->first << endl;
    }
    return 0;
    
}
