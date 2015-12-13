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

enum IDCode { JR, JLR, ADD, SUB, SLT, SLTU, BEQ, BNE, NOTFOUND };

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
    } else if(commandName.compare("sltu") == 0){
        return SLTU;
    } else if(commandName.compare("beq") == 0){
        return BEQ;
    } else if(commandName.compare("bne") == 0){
        return BNE;
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

void branch(int first, int second, int third, int commandConstant) {
    int value = (first << 21);
    value = value | (second << 16);
    value = value | (third << 11);
    value = value | commandConstant;
    printHexString(value);
}


void threeVariablesAndACommand(int first, int second, int third, int commandConstant) {
    int value = (first << 21);
    value = value | (second << 16);
    value = value | (commandConstant << 26);
    value = value | third;
    printHexString(value);
}

int main(int argc, const char * argv[]) {

    map<string, int> labelMap;
    vector<std::vector<Token*>> tokensVector = *new vector<std::vector<Token*>>();
    
    int lineNumber = 0;
    
    Lexer* lexer = new Lexer();
    std::string line;
    int emptyLineCount = 0;
    while (std::getline(std::cin, line))
    {
        if(line.size() <= 0) {
            emptyLineCount ++;
            if(emptyLineCount > 2) {
                break;
            }
            cout << "here and count is " << emptyLineCount << endl;
            continue;
        } else {
            emptyLineCount = 0;
            cout << "not here and count is " << emptyLineCount << endl;
        }
        std::vector<Token*> tokens = (*lexer).scan(line);
        tokensVector.insert(tokensVector.end(), tokens);
        if ((*tokens.at(0)).getKind() == LABEL){
            pair<string, int> pair = *new ::pair<string, int>();
            pair.second = lineNumber;
            pair.first = tokens.at(0)->getLexeme().substr(0, tokens.at(0)->getLexeme().size() - 1);
            labelMap.insert(pair);
        }
        for(int x = 1; x < tokens.size(); x++) {
            Token token = (*tokens[x]);
            if(token.getKind() != LABEL) {
                lineNumber++;
                break;
            }
        }
        std::getline (std::cin,line);
    }
    
    
    for (int v = 0; v < tokensVector.size(); v++) {
        
        std::vector<Token*> tokens = tokensVector.at(v);
        int idCode = NOTFOUND;
        if(tokens.size() > 0 && tokens.at(0)->getKind() != LABEL) {
            idCode = getIDCode((*tokens.at(0)).getLexeme());
        }
        for(int x = 0; x < tokens.size(); x++) {
            Token token = (*tokens[x]);
            
            if (x > 0) {
                
                if((*tokens[x -1]).getKind() == DOTWORD){
                    if(tokens.at(x)->getKind() == INT){
                        printHexString((*tokens.at(x)).toInt());
                    } else if(tokens.at(x)->getKind() == ID) {
                        const string lexeme = token.getLexeme();
                        if(stringContainsLable(lexeme, labelMap)) {
                            int value = valueOfLabel(lexeme, labelMap);
                            printHexString(value);
                        } else {
                            cerr << "Label not found: " << lexeme;
                        }
                        
                    }
                }
                else if((*tokens.at(x -1)).getKind() == ID){
                    
                    if(idCode == JR || idCode == JLR) {
                        if((*tokens.at(x)).getKind() == REGISTER){
                            string lexeme = (*tokens.at(x)).getLexeme();
                            int value = atoi(lexeme.substr(1, lexeme.size() - 1).c_str());
                            
                            if(idCode == JR) {
                                jumpAndRegister(value, false);
                            } else if(idCode == JLR) {
                                jumpAndRegister(value, true);
                            }
                        }
                    }
                }
                else if(x >= 5 && (*tokens.at(x - 5)).getKind() == ID){
                    
                    if(idCode == ADD || idCode == SUB || idCode == SLT || idCode == SLTU) {
                        for(int b = 0; b < 3; b++) {
                            if((*tokens[x - b  * 2]).getKind() != REGISTER) {
                                cerr << "Command had id code " << idCode << "but did not have all register as arguments";
                                return -1;
                            }
                        }
                        int firstValue = tokens.at(x-4)->toInt();
                        int secondValue = tokens.at(x-2)->toInt();
                        int thirdValue = tokens.at(x)->toInt();
                    
                        int commandValue = 0;
                        switch (idCode) {
                            case ADD:
                                commandValue = 32;
                                break;
                            case SUB:
                                commandValue = 34;
                                break;
                            case SLT:
                                commandValue = 42;
                                break;
                            case SLTU:
                                commandValue = 43;
                                break;
                        }
                        
                        threeVariablesAndACommand(secondValue, thirdValue, firstValue, commandValue);
                    } else if(idCode == BEQ || idCode == BNE) {
                        
                        int thirdValue =  0;
                        
                        const string lexeme = tokens.at(x)->getLexeme();
                        if(tokens.at(x)->getKind() == INT || tokens.at(x)->getKind() == HEXINT) {
                            thirdValue = tokens.at(x)->toInt();
                        }
                        else if(stringContainsLable(lexeme, labelMap)) {
                            thirdValue = valueOfLabel(lexeme, labelMap);
                            thirdValue = thirdValue - lineNumber;
                        }

                        
                        int secondValue = tokens.at(x-2)->toInt();
                        int firstValue = tokens.at(x-4)->toInt();
                        
                        if(thirdValue < 0) {
                            thirdValue = thirdValue & 0xffff;
                        }
                        
                        int commandValue = 0;
                        switch (idCode) {
                            case BEQ:
                                commandValue = 4;
                                break;
                            case BNE:
                                commandValue = 5;
                                break;
                        }
                        
                        threeVariablesAndACommand(firstValue, secondValue, thirdValue, commandValue);
                    }

                }
                
            }
            
        }
    }
    
    typedef std::map<string, int>::iterator it_type;
    for(it_type iterator = labelMap.begin(); iterator != labelMap.end(); iterator++) {
        cerr << iterator->second << " " << iterator->first << endl;
    }
    return 0;
    
}
