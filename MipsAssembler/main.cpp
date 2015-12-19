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

enum CommandIDCode { JR, JLR, ADD, SUB, SLT, SLTU, BEQ, LIS, MFLO, MFHI, BNE, MULT, MULTU, DIV, DIVU, SW, LW, NOTFOUND };

CommandIDCode getIDCode(string commandName) {
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
    } else if(commandName.compare("lis") == 0){
        return LIS;
    } else if(commandName.compare("mflo") == 0){
        return MFLO;
    } else if(commandName.compare("mfhi") == 0){
        return MFHI;
    } else if(commandName.compare("mult") == 0){
        return MULT;
    } else if(commandName.compare("multu") == 0){
        return MULTU;
    } else if(commandName.compare("div") == 0){
        return DIV;
    } else if(commandName.compare("divu") == 0){
        return DIVU;
    } else if(commandName.compare("sw") == 0){
        return SW;
    } else if(commandName.compare("lw") == 0){
        return LW;
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

int negativeNumberCheck(int valueToCheck, int numberOfBitsToReduceTo) {

    if(valueToCheck < 0) {
        switch (numberOfBitsToReduceTo) {
            case 4:
                valueToCheck = valueToCheck & 0xf;
                break;
            case 8:
                valueToCheck = valueToCheck & 0xff;
            case 12:
                valueToCheck = valueToCheck & 0xfff;
                break;
            case 16:
                valueToCheck = valueToCheck & 0xffff;
                break;
            default:
                break;
        }
        
    }
    return valueToCheck;
}

void loadOrStoreWord(int firstValue, int secondValue, int thirdValue, int commandValue) {
    secondValue = negativeNumberCheck(secondValue, 16);
    int value = secondValue;
    value = (thirdValue << 21) | value;
    value = (firstValue << 16) | value;
    value = (commandValue << 26) | value;
    printHexString(value);
}

void mutliplyOrDivide(int firstRegisterValue, int secondRegisterValue, int commandValue) {
    int value = commandValue;
    value = (firstRegisterValue << 21) | value;
    value = (secondRegisterValue << 16) | value;
    printHexString(value);
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
    third = negativeNumberCheck(third, 16);
    int value = (first << 21);
    value = value | (second << 16);
    value = value | (commandConstant << 26);
    value = value | third;
    printHexString(value);
}

void oneVariableAndACommand(int first, int commandConstant) {
    int value = (first << 11);
    value = value | commandConstant;
    printHexString(value);
}


int main(int argc, const char * argv[]) {

    map<string, int> labelMap;
    vector<vector<Token*> > tokensVector = *new vector<vector<Token*> >();
    
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
            continue;
        } else {
            emptyLineCount = 0;
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
                if(x >= 5){
                    
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
                    } else if ((idCode == LW || idCode == SW) && (tokens.at(x)->getKind() == RPAREN && tokens.at(x-1)->getKind() == REGISTER && tokens.at(x-2)->getKind() == LPAREN && tokens.at(x-5)->getKind() == REGISTER)){
                        
                        int thirdValue = tokens.at(x-1)->toInt();
                        int secondValue = tokens.at(x-3)->toInt();
                        int firstValue = tokens.at(x-5)->toInt();
                        
                        int commandValue = 0;
                        switch (idCode) {
                            case LW:
                                commandValue = 35;
                                break;
                            case SW:
                                commandValue = 43;
                                break;
                        }
                        loadOrStoreWord(firstValue, secondValue, thirdValue, commandValue);
                        
                    }

                    
                } else if(x >= 2 && (idCode == MULT || idCode == MULTU || idCode == DIV || idCode == DIVU || idCode == LW || idCode == SW)) {
                    
                        if(tokens.at(x)->getKind() == REGISTER && tokens.at(x-2)->getKind() == REGISTER && tokens.at(x-3)->getKind() == ID){
                            int secondValue = tokens.at(x)->toInt();
                            int firstValue = tokens.at(x-2)->toInt();
                            int commandValue = 0;
                            switch (idCode) {
                                case MULT:
                                    commandValue = 24;
                                    break;
                                case MULTU:
                                    commandValue = 25;
                                    break;
                                case DIV:
                                    commandValue = 26;
                                    break;
                                case DIVU:
                                    commandValue = 27;
                                    break;
                            }
                            mutliplyOrDivide(firstValue, secondValue, commandValue);
                        }
                }
                else if(x >= 1 && (idCode == DOTWORD || idCode == JR || idCode == JLR || idCode == MFLO || idCode == MFHI || idCode == LIS)) {
                    
                    if(idCode == DOTWORD){
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
                        
                        if((*tokens.at(x)).getKind() == REGISTER){
                            if(idCode == JR || idCode == JLR) {
                                    string lexeme = (*tokens.at(x)).getLexeme();
                                    int value = atoi(lexeme.substr(1, lexeme.size() - 1).c_str());
                                    
                                    if(idCode == JR) {
                                        jumpAndRegister(value, false);
                                    } else if(idCode == JLR) {
                                        jumpAndRegister(value, true);
                                    }
                            } else if(idCode == LIS || idCode == MFLO || idCode == MFHI) {
                                int firstValue = tokens.at(x)->toInt();
                                switch (idCode) {
                                    case LIS:
                                        oneVariableAndACommand(firstValue, 20);
                                        break;
                                    case MFLO:
                                        oneVariableAndACommand(firstValue, 18);
                                        break;
                                    case MFHI:
                                        oneVariableAndACommand(firstValue, 16);
                                        break;
                                    default:
                                        break;
                                }
                                
                            }
                        }
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
