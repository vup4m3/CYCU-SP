#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;


struct Token
{
    int tokenType;
    int tokenValue;
};

class Table {
private:
    vector<string> tTable;

public:
    int insertToken(string token);
    int getDelimiterValue(string token);
    int getReservedWordValue(string token);
    string getElement(int tokenValue);
};

int Table::insertToken(string token) {
    int i, ascii, index;
	string str = "";
	for (i = 0, ascii = 0; i < token.length(); i++) 
		ascii = ascii + token.at(i);
	
	index = ascii % 100;
	if (tTable.size() == 0) 
		for (i = 0; i < 100; i++) 
			tTable.push_back(str);
	
	if (tTable[index] == "") 
		tTable[index] = token;
	else if (tTable[index] == token) 
		return index;
	else {
		i = 0;
		while (tTable[index] != token && i < 100) {
			index++;
			if (index == 100) 
				index = 0;

			if (tTable[index] == "") {
				tTable[index] = token;
				break;
			}

			i++;
		}
	}

	return index;
}

int Table::getDelimiterValue(string token) {
    if (token == ";") return 1; else if (token == "(") return 2;
    else if (token == ")") return 3; else if (token == "=") return 4;
    else if (token == "+") return 5; else if (token == "-") return 6;
    else if (token == "*") return 7; else if (token == "/") return 8;
    else if (token == "↑") return 9; else if (token == "‘") return 10;
    else if (token == ",") return 11; else if (token == ":") return 12;
    else return -1;
}

int Table::getReservedWordValue(string token) {
    if (token == "AND") return 1; else if (token == "BOOLEAN") return 2;
    else if (token == "CALL") return 3; else if (token == "DIMENSION") return 4;
    else if (token == "ELSE") return 5; else if (token == "ENP") return 6;
    else if (token == "ENS") return 7; else if (token == "EQ") return 8;
    else if (token == "GE") return 9; else if (token == "GT") return 10;
    else if (token == "GTO") return 11; else if (token == "IF") return 12;
    else if (token == "INPUT") return 13; else if (token == "INTEGER") return 14;
    else if (token == "LABEL") return 15; else if (token == "LE") return 16;
    else if (token == "LT") return 17; else if (token == "NE") return 18;
    else if (token == "OR") return 19; else if (token == "OUTPUT") return 20;
    else if (token == "PROGRAM") return 21; else if (token == "REAL") return 22;
    else if (token == "SUBROUTINE") return 23; else if (token == "THEN") return 24;
    else if (token == "VARIABLE") return 25; else if (token == "ARRAY") return 26;
    else if (token == "CHARACTER") return 27; else return -1;                  
}

string Table::getElement(int tokenValue) {
    return tTable[tokenValue];
}

struct Identifer
{
    string identifer;
    int subroutine;
    int type;
    int pointer;
};

class IdTable {
private:
    vector<Identifer> idTable;

public:
    int insertToken(string tokenStr, int subroutine);
    void addType(int tokenValue, int type);
    void addPointer(int tokenValue, int pointer);
    int getDataType(int tokenValue);
    string getIdentifer(int tokenValue);

};

int IdTable::insertToken(string tokenStr, int subroutine) {
    int i, ascii, index;
	Identifer element = {"", -1, -1, -1};
    Identifer newId;
	for (i = 0, ascii = 0; i < tokenStr.length(); i++) 
		ascii = ascii + tokenStr.at(i);
	
	index = ascii % 100;
	if (idTable.size() == 0) 
		idTable.assign(100, element);
	
	if (idTable[index].identifer == "") {
        idTable[index].identifer = tokenStr;
        idTable[index].subroutine = subroutine;
    }
	else if (idTable[index].identifer == tokenStr && (idTable[index].subroutine == subroutine || idTable[index].subroutine == -1)) // Identifer and Subroutine must be same
		return index;
	else {
		i = 0;
		while (!(idTable[index].identifer == tokenStr && idTable[index].subroutine == subroutine) && i < 100) {
			index++;
			if (index == 100) 
				index = 0;

			if (idTable[index].identifer == "") {
				idTable[index].identifer = tokenStr;
                idTable[index].subroutine = subroutine;
				break;
			}

			i++; // for sure that loop will be end even if hash is full
		}
	}

	return index;
}

void IdTable::addType(int tokenValue, int type) {
    idTable[tokenValue].type = type;
}

void IdTable::addPointer(int tokenValue, int pointer) {
    idTable[tokenValue].pointer = pointer;
}

int IdTable::getDataType(int tokenValue) {
    return idTable[tokenValue].type;
}

string IdTable::getIdentifer(int tokenValue) {
    return idTable[tokenValue].identifer;
}

class SyntaxAnalysis {
private:
    vector<Token> sLexicalCode;
    int sCurrIndex;
    Token sCurToken;
    // ===============================
    void nextToken();
    int stateMent(Table &intTable, Table &realNumTable, IdTable &idTable, vector<int> &inforTable);
    int stmSubroutine(IdTable &idTable);
    int stmProgram(IdTable &idTable);
    int stmVariable(IdTable &idTable);
    int stmLabel(IdTable &idTable);
    int stmDimension(IdTable &idTable, vector<int> &inforTable, Table intTable);
    int stmIf_than(Table &intTable, Table &realNumTable, IdTable &idTable, vector<int> &inforTable);
    int stmGto(IdTable idTable);
    int stmAssignment();
    int stmCall(vector<int> &inforTable);
    int stmIO();
    int getDataType();
    bool isId(IdTable &idTable, int dataType);
    bool isArray(int dataType, IdTable &idTable, vector<int> &inforTable, Table intTable);
    bool isNum(vector<int> &inforTable, Table intTable, int &dimensionality);
    bool isCondition();
    bool isCondVarialbe();
    bool isRelationalOp();
    bool isCallArgument(vector<int> &inforTable, int &amount);
    bool isExp();
    bool isOperater();
    bool isVariable();
    bool isNumber();
public:
    int syntaxAnalysis(vector<Token> lexicalCode, Table &intTable, Table &realNumTable, IdTable &idTable, vector<int> &inforTable);
};

void SyntaxAnalysis::nextToken() {
    sCurrIndex++;
    if (sCurrIndex < sLexicalCode.size()) 
        sCurToken = sLexicalCode[sCurrIndex];
    else 
        sCurToken = { -1, -1 };
}

int SyntaxAnalysis::stateMent(Table &intTable, Table &realNumTable, IdTable &idTable, vector<int> &inforTable) {
    Token reservedWord = sCurToken;
    nextToken();
    int returnBack;
    if (reservedWord.tokenType == 2 && reservedWord.tokenValue == 21) { // PROGRAM
        return stmProgram(idTable);
    }
    else if (reservedWord.tokenType == 2 && reservedWord.tokenValue == 23) { // SUBROUTINE
        return stmSubroutine(idTable);
    }
    else if (reservedWord.tokenType == 2 && reservedWord.tokenValue == 25) { // VARIBLE 
        return stmVariable(idTable);
    }
    else if (reservedWord.tokenType == 2 && reservedWord.tokenValue == 15) { // LABEL
        return stmLabel(idTable);
    }
    else if (reservedWord.tokenType == 2 && reservedWord.tokenValue == 4) { // DIMENSION
        return stmDimension(idTable, inforTable, intTable);
    }
    else if (reservedWord.tokenType == 2 && reservedWord.tokenValue == 12) { // IF...THAN
        return stmIf_than(intTable, realNumTable, idTable, inforTable);
    }
    else if (reservedWord.tokenType == 2 && reservedWord.tokenValue == 11) { // GTO 
        return stmGto(idTable);
    }
    else if (reservedWord.tokenType == 2 && reservedWord.tokenValue == 3) { // CALL
        return stmCall(inforTable);
    }
    else if (reservedWord.tokenType == 2 && reservedWord.tokenValue == 13 || reservedWord.tokenType == 2 && reservedWord.tokenValue == 20) { // INPUT OUTPUT
        returnBack == stmIO();
        if (returnBack == 10) {
            if (reservedWord.tokenValue == 13) 
                return returnBack; // INPUT
            else return 11; // OUTPUT
        }
        else return stmIO();
    }
    else if (reservedWord.tokenType == 1 && reservedWord.tokenType == 1 || reservedWord.tokenType == 2 && (reservedWord.tokenValue == 6 || reservedWord.tokenValue == 7))
        return 0;
    else  { // assignment
        sCurrIndex--;
        sCurToken = reservedWord;
        return stmAssignment();
    }
}

int SyntaxAnalysis::stmProgram(IdTable &idTable) {
    // PROGRAM id
    if (sCurToken.tokenType == 5) { // id
        nextToken(); // ;
        return 1; // PROGRAM
    }
    else return 21; // Should be identifer
}

int SyntaxAnalysis::stmSubroutine(IdTable &idTable) {
    // SUBROUTINE id(<data type> : id{, id})
    int returnMsg = stmProgram(idTable);
    if (returnMsg != 1) 
        return returnMsg; // PROGRAM Error
    else {
        if (sCurToken.tokenType == 1 && sCurToken.tokenValue == 2) { // (
            nextToken();
            returnMsg = stmVariable(idTable);
            if (returnMsg != 3) 
                return returnMsg; // VARIABLE Error
            else {
                if (sCurToken.tokenType == 1 && sCurToken.tokenValue == 3) { // )
                    nextToken(); // ;
                    return 2; // SUBROUTINE
                }
                else return 14; // Wrong format
            }
        }
        else return 14; // with out '(', Wrong format
    }
}

int SyntaxAnalysis::stmVariable(IdTable &idTable) {
    // VARIABLE <data type> : id, {id}
    int dataType;
    dataType = getDataType(); // <data type>
    if ( dataType != -1) { // <data type> exist
        if (sCurToken.tokenType == 1 && sCurToken.tokenValue == 12) { // :
            nextToken();
            if (isId(idTable, dataType)) { // id, {id}
                return 3; // VARIABLE
            }
            else return 20; // Error arguement;
        }
        else return 14; // Error format
        
    }
    else return 22; // Data type Error
}

int SyntaxAnalysis::stmLabel(IdTable &idTable) {
    // LABEL id, {id}
    if (isId(idTable, 5)) {// id, {id}
        nextToken(); // ;
        return 4; // LABEL 
    }
    else return 13; // Error define
}

int SyntaxAnalysis::stmDimension(IdTable &idTable, vector<int> &inforTable, Table intTable ) {
    // DIMENSION <dara type>:<array>{, <array>}
    int dataType, dimensionality;
    vector<int> sizeArray;
    dataType = getDataType(); // <data type>
    if (dataType != -1) { // <data type> exist
        if (sCurToken.tokenType == 1 && sCurToken.tokenValue == 12) { // : exist
            nextToken(); // ;
            if (isArray(dataType, idTable, inforTable, intTable)) {// <array>
                
                return 5; // DIMESION
            }
            else return 13; // Error define
        }
        else return 14; // Error format
    }
    else return 13; // Error define
}

int SyntaxAnalysis::stmIf_than(Table &intTable, Table &realNumTable, IdTable &idTable, vector<int> &inforTable) {
    // IF <condition> THEN <statement> ELSE <statement> 
    if (isCondition()) {
        if (sCurToken.tokenType == 2 && sCurToken.tokenValue == 24) { // THEN
            nextToken();
            if (stateMent(intTable, realNumTable, idTable, inforTable) < 11) { // statement
                if (sCurToken.tokenType == 2 && sCurToken.tokenValue == 5) { // ELSE
                    nextToken();
                    if (stateMent(intTable, realNumTable, idTable, inforTable) < 11) // statement
                        return 6; // IF...THEN
                    else return 18; // Error statement
                }
                else return 6; // IF...THEN
            }
            else return 18; // Error statement
        }
        else return 14; // Error format
    }
    else return 17; // Error Condition
}

int SyntaxAnalysis::stmGto(IdTable idTable) {
    // GTO label
    int type;
    if (sCurToken.tokenType == 5) { // id
        if (idTable.getDataType(sCurToken.tokenValue) == 5) { // label
            nextToken(); // ;
            return 7; // GTO
        }
        else return 15; // Undefine symbol
    }
    else return 13; // Error define
}

int SyntaxAnalysis::stmAssignment() {
    if (isVariable()) {
        if (sCurToken.tokenType == 1 && sCurToken.tokenValue == 4) { // =
            nextToken();
            if (isExp()) {
                return 8; // assignment
            }
            else return 16; // Erro expression
        }
        else return 14; // Wrong format
    }
    else return 19; // Error variable
}

int SyntaxAnalysis::stmCall(vector<int> &inforTable) {
    // id(argument, {argument})
    int amount = 0, index;
    if (sCurToken.tokenType == 5) {
        index = inforTable.size();
        inforTable.push_back(0);
        nextToken();
        if (sCurToken.tokenType == 1 && sCurToken.tokenValue == 2) { // (
            nextToken();
            if (isCallArgument(inforTable, amount)) {
                if (sCurToken.tokenType == 1 && sCurToken.tokenValue == 3) { // )
                    nextToken();
                    inforTable[index] = amount;
                    return 9;
                }
                else return 14; // No ')', Error format 
            }
            else return 20; // Error argument
        }
        else return 14; // No '(', Error format
    }
    else return 21; // No 'id', Should be identifer

}

int SyntaxAnalysis::stmIO() {
    // <id> || <id> ( <id> || <integer> )
    if (sCurToken.tokenType == 5) {
        nextToken();
        if (sCurToken.tokenType == 1 && sCurToken.tokenType == 2) { // (
            if (sCurToken.tokenType == 5 || sCurToken.tokenType == 3) { // <id> || <integer>
                if (sCurToken.tokenType == 1 && sCurToken.tokenType == 3) { // )
                    nextToken();
                    return 10;
                }
                else return 14; // Error format
            }
            else return 14; // Error format
        }
        else return 10;
    }
    else return 14; // Error format
}

int SyntaxAnalysis::getDataType() {
    // <data type>
    Token token = sCurToken;
    nextToken();
    if (token.tokenType == 2) {
        if (token.tokenValue == 26)
            return 1; // ARRAY
        else if (token.tokenValue == 2)
            return 2; // BOOLEAN
        else if (token.tokenValue == 27)
            return 3; // CHARACTER
        else if (token.tokenValue == 14)
            return 4; // INTERGER
        else if (token.tokenValue == 15)
            return 5; // LABEL
        else if (token.tokenValue == 22)
            return 6; // REAL
        else 
            return -1; // NOT data type
    }
    else return -1; // NOT data type
}

bool SyntaxAnalysis::isId(IdTable &idTable, int dataType) {
    // id, {id}
    if (sCurToken.tokenType == 5) { // id
        idTable.addType(sCurToken.tokenValue, dataType);
        nextToken(); // ,
        if (sCurToken.tokenType == 1 && sCurToken.tokenValue == 11) { // , exist
            nextToken(); // id
            return isId(idTable, dataType); // id{, id}
        }
        else return true; 
    }
    else return false; // no id

}

bool SyntaxAnalysis::isArray(int dataType, IdTable &idTable, vector<int> &inforTable, Table intTable) {
    // <array> ::= id(number, {number})
    int dimensionalityIndex, num, dimensionality = 0;
    bool error = false;
    string numStr;
    if (sCurToken.tokenType == 5) { // id 
        idTable.addType(sCurToken.tokenValue, 1);
        idTable.addPointer(sCurToken.tokenValue, inforTable.size());
        nextToken(); // (
        if (sCurToken.tokenType == 1 && sCurToken.tokenValue == 2) { // (
            inforTable.push_back(dataType);
            dimensionalityIndex = inforTable.size();
            inforTable.push_back(0);
            nextToken(); // number
            if (isNum(inforTable, intTable, dimensionality)) { // number{, number}
                inforTable[dimensionalityIndex] = dimensionality; // number of {number}
                if (sCurToken.tokenType == 1 && sCurToken.tokenValue == 3) { // )
                    nextToken(); // ,
                    if (sCurToken.tokenType == 1 && sCurToken.tokenValue == 11) { // ,
                        nextToken(); // <array>
                        return isArray(dataType, idTable, inforTable, intTable); // <array>
                    }
                    else 
                        return true;
                }
                else return false;
            }  
            else return false;
        }
        else return false;
    }
    else return false; 
}

bool SyntaxAnalysis::isNum(vector<int> &inforTable, Table intTable, int &dimensionality) {
    // number{, number}
    int num;
    if (sCurToken.tokenType == 3) { // number
        dimensionality++;
        num = stoi(intTable.getElement(sCurToken.tokenValue), 0);
        inforTable.push_back(num);
        nextToken(); // ,
        if (sCurToken.tokenType == 1 && sCurToken.tokenValue == 11) { // ,
            nextToken(); // number
            return isNum(inforTable, intTable, dimensionality); // number
        } // {, number}
        else return true;
    }
    else return false;
}

bool SyntaxAnalysis::isCondition() {
    // <condition variable> <relations> <condition variable>
    if (isCondVarialbe()) { // 
        if (isRelationalOp() || (sCurToken.tokenType == 1 && sCurToken.tokenValue == 19) || (sCurToken.tokenType == 1 && sCurToken.tokenValue == 1)) {
        relations:
            nextToken();
            if (isCondVarialbe()) {
                if (isRelationalOp() || (sCurToken.tokenType == 1 && sCurToken.tokenValue == 19) || (sCurToken.tokenType == 1 && sCurToken.tokenValue == 1))
                    goto relations;
                else return true;
            }
            return false;
        }
        return false;
    }
    else return false;
}

bool SyntaxAnalysis::isCondVarialbe() {
    // <codition variable> := <id> || <sign> <integer> || <integer> || <id> ( <intege> || <id> )
    if (sCurToken.tokenType == 5) {
        nextToken();
        if (sCurToken.tokenType == 1 && sCurToken.tokenValue == 2) { // (
            nextToken();
            if (sCurToken.tokenType == 3 || sCurToken.tokenType == 5) { // <integer> || <id>
                nextToken();
                if (sCurToken.tokenType == 1 && sCurToken.tokenValue == 3) { // )
                    nextToken();
                    return true; // <id> ( <intege> || <id> )
                }
                return false;
            }
            else return false;
        }
        else return true; // <id> 
    }
    else if (sCurToken.tokenType == 1 && (sCurToken.tokenValue == 5 || sCurToken.tokenValue == 6)) { // + || -
        nextToken();
        if (sCurToken.tokenType == 3 || sCurToken.tokenType == 4 || sCurToken.tokenType == 5) { // <integer> || <real number> || <id>
            nextToken();
            return true; // <sign> <integer> || <real number> || <id>
        }
        else return false;
    }
    else if (sCurToken.tokenType == 3 || sCurToken.tokenType == 4) { // <integer> || <real number>
        nextToken();
        return true; // <integer> || <real number>
    }
    else return false;
}

bool SyntaxAnalysis::isRelationalOp() {
    // EQ NE GT GE LT LE
    if (sCurToken.tokenType == 2) {
        if(sCurToken.tokenValue == 8 || sCurToken.tokenValue == 18 || sCurToken.tokenValue == 10 || sCurToken.tokenValue == 9
        || sCurToken.tokenValue == 17 || sCurToken.tokenValue == 16) {
            return true;
        }
        else return false;
    }
    else return false;
}

bool SyntaxAnalysis::isCallArgument(vector<int> &inforTable, int &amount) {
    // id || integer || real number
    if (sCurToken.tokenType == 5 || sCurToken.tokenType == 3 || sCurToken.tokenType == 4) {
        amount++;
        inforTable.push_back(sCurToken.tokenType);
        inforTable.push_back(sCurToken.tokenValue);
        nextToken();
        if (sCurToken.tokenType == 1 && sCurToken.tokenValue == 11) { // ,
            nextToken();
            return isCallArgument(inforTable, amount);
        }
        else return true;
    }
    else return false;
}

bool SyntaxAnalysis::isExp() {
    if (isVariable() || isNumber()) {
        if (isOperater()) {
            return isExp();
        }
        else return true;
    }
    else if (sCurToken.tokenType == 1 && sCurToken.tokenValue == 2) { // (
        nextToken();
        if (isExp()) {
            if (sCurToken.tokenType == 1 && sCurToken.tokenValue ==3) {
                return true;
            }
            else return false;
        }
        else return false;
    }
    else return false;
}

bool SyntaxAnalysis::isOperater(){
    if (sCurToken.tokenType == 1 && sCurToken.tokenValue >= 5 && sCurToken.tokenValue <= 9) {
        nextToken();
        return true;
    }
    else if (sCurToken.tokenType == 2 && (sCurToken.tokenValue == 1 ||sCurToken.tokenValue == 19))
        return true;
    else return false;
}

bool SyntaxAnalysis::isVariable() {
    // <id> || <id> ( <argument> { , <argument> } )
    // argument := id || integer
    if (sCurToken.tokenType == 5) {
        nextToken();
        if (sCurToken.tokenType == 1 && sCurToken.tokenValue == 2) { // (
            nextToken();
        argument: // {argument}
            if (sCurToken.tokenType == 5 || sCurToken.tokenType == 3) { // id || integer
                nextToken();
                if (sCurToken.tokenType == 1 && sCurToken.tokenValue == 3) {// ) 
                    nextToken();
                    return true;
                }
                else if (sCurToken.tokenType == 1 && sCurToken.tokenValue == 11) { // ,
                    nextToken();
                    goto argument;
                }
                else return false;
            }
            else return false;
        }
        else return true;
    }
    else return false; // No Variable
}

bool SyntaxAnalysis::isNumber() {
    // <integer> || <real number>
    if (sCurToken.tokenType ==3 || sCurToken.tokenType == 4) {
        nextToken();
        return true;
    }
    else return false;
}

int SyntaxAnalysis::syntaxAnalysis(vector<Token> lexicalCode, Table &intTable, Table &realNumTable, IdTable &idTable, vector<int> &inforTable) {
    sLexicalCode = lexicalCode;
    int syntax = 0;
    sCurrIndex = 0;
    sCurToken = sLexicalCode[sCurrIndex];
    if (sCurToken.tokenType == 5) // LABEL
        if (idTable.getDataType(sCurToken.tokenValue) == 5) 
            syntax = 100;
        
    syntax += stateMent(intTable, realNumTable, idTable, inforTable);
    if (syntax % 100 < 12 && sCurToken.tokenType != 1 && sCurToken.tokenValue != 1)
        return 12; // with out ;
    else return syntax; 
}

struct Quadruple {
    Token token1;
    Token token2;
    Token token3;
    Token token4;
    string code;
};

class Intermediate
{
private:
    int temp = 0, iCurrIndex;
    vector<int> holeList;
    vector<Token> iLexicalCode;
    Token iCurrToken;
    // =============================================
    void nextToken();
    Quadruple buildQuadruple(Token t1, Token t2, Token t3, Token t4, string str);
    void program_subroutine(IdTable &idTable, vector<Quadruple> qdpTable);
    vector<Quadruple> variable(IdTable idTable, vector<Quadruple> qdpTable);
    vector<Quadruple> label(IdTable idTable, vector<Quadruple> qdpTable);
    vector<Quadruple> diemension(IdTable idTable, vector<Quadruple> qdpTable);
    vector<Quadruple> stmGto(IdTable idTable, vector<Quadruple> qdpTable);
    
public:

};

void Intermediate::nextToken() {
    iCurrIndex++;
    if (iCurrIndex < iLexicalCode.size()) 
        iCurrToken = iLexicalCode[iCurrIndex];
    else 
        iCurrToken = {-1, -1};
}

Quadruple Intermediate::buildQuadruple(Token t1, Token t2, Token t3, Token t4, string str) {
    Quadruple newQuadruple;
    newQuadruple.token1 = t1;
    newQuadruple.token2 = t2;
    newQuadruple.token3 = t3;
    newQuadruple.token4 = t4;
    newQuadruple.code = str;
}

void Intermediate::program_subroutine(IdTable &idTable, vector<Quadruple> qdpTable) {
    // PROGRAM id;
    // SUBROUTINE id();
    idTable.addPointer(iLexicalCode[1].tokenValue, qdpTable.size());
    if (holeList.size() != 0) {

    }
}

vector<Quadruple> Intermediate::variable(IdTable idTable, vector<Quadruple> qdpTable) {
    // VARIABLE <data type> : id {, id};
    string str;
    Quadruple newQuadruple;
    Token t1, t2, t3, t4;
    Token blank = {-1, -1};
    // pass 3 token  "VARIABLE <data type> :""
    iCurrIndex += 3;
    iCurrToken = iLexicalCode[iCurrIndex];
    // =======================================
    variable:
    str = idTable.getIdentifer(iCurrToken.tokenValue); // get id 
    newQuadruple = buildQuadruple(iCurrToken, blank, blank ,blank, str);
    qdpTable.push_back(newQuadruple);
    nextToken();
    if (iCurrToken.tokenType == 1 && iCurrToken.tokenValue == 11) { // ,
        nextToken();
        goto variable;
    }
    else return qdpTable;
}

vector<Quadruple> Intermediate::label(IdTable idTable, vector<Quadruple> qdpTable) {
    // LABEL id {, id};
    string str;
    Quadruple newQuadruple;
    Token t1, t2, t3, t4;
    Token blank = {-1, -1};
    // pass 1 token  "LABEL""
    iCurrIndex += 1;
    iCurrToken = iLexicalCode[iCurrIndex];
    // =======================================
variable:
    str = idTable.getIdentifer(iCurrToken.tokenValue); // get id 
    newQuadruple = buildQuadruple(iCurrToken, blank, blank ,blank, str);
    qdpTable.push_back(newQuadruple);
    nextToken();
    if (iCurrToken.tokenType == 1 && iCurrToken.tokenValue == 11) { // ,
        nextToken();
        goto variable;
    }
    else return qdpTable;
}

vector<Quadruple> Intermediate::diemension(IdTable idTable, vector<Quadruple> qdpTable){
    // DIMENSION <data type> id();
    string str;
    Quadruple newQuadruple;
    Token blank = {-1, -1};
    // pass 1 token  "DIMENSION <data type>"
    iCurrIndex += 2;
    iCurrToken = iLexicalCode[iCurrIndex];
    // =======================================
dimension:
    str = idTable.getIdentifer(iCurrToken.tokenValue); // get id 
    newQuadruple = buildQuadruple(iCurrToken, blank, blank ,blank, str);
    qdpTable.push_back(newQuadruple);
    nextToken();
    while (iCurrToken.tokenType != 1 && iCurrToken.tokenValue != 3)
        nextToken();
    
    nextToken();
    if (iCurrToken.tokenType == 1 && iCurrToken.tokenValue == 11) { // ,
        nextToken();
        goto dimension;
    }
    else return qdpTable;
}

vector<Quadruple> Intermediate::stmGto(IdTable idTable, vector<Quadruple> qdpTable) {
    string str;
    Quadruple newQuadruple;
    Token t1 = {1,11}, blank ={-1, -1};
    nextToken();
    str = "GTO " + idTable.getIdentifer(iCurrToken.tokenValue);

}

class Compiler
{
private:
    string cFileName;
    string cCurrentSourceCode;
    ifstream cInfile;
    Table cIntTable, cRealNumTable;
    IdTable cIdTable;
    vector<int> cInforTable;
    int cSubroutine, cCurLine;
    vector<Quadruple> cQdpTable;
    // -----------------------------------
    vector<Token> lexicalAnalysis();
    Token identify(string token, int subroutine);
    void syntaxErrorMsg(int errorCode);
    string getStateMent();
    
public:
    void initial();
    bool readFile(string input);
    void compiler();
};

vector<Token> Compiler::lexicalAnalysis() {
    vector<Token> tokenArray;
    string currentCh;
    string tokenStr = "";
    Token token;
    int tempSubroutine = cSubroutine;
    for (int i = 0; i < cCurrentSourceCode.length()+1; i++) {
        if (i < cCurrentSourceCode.length())
            currentCh = cCurrentSourceCode.at(i);
        else 
            currentCh = " ";

        if (currentCh == " " || currentCh == "\t" || currentCh == "\n") { // Blank ?
            if (tokenStr != "") { // identify the string in the queue, if it exists as a token
                if (tokenStr == "SUBROUTINE" || tokenStr == "PROGRAM")
                    tempSubroutine = -1;
                    
                token = identify(tokenStr, tempSubroutine);
                tokenArray.push_back(token);
                if (tempSubroutine == -1)
                    tempSubroutine = token.tokenValue;
                tokenStr = "";
            }
        }
        else if (cIntTable.getDelimiterValue(currentCh) != -1) { // Delimiter ?
            if (tokenStr != "") { // identify the string in the queue, if it exists as a token
                token = identify(tokenStr, tempSubroutine);
                tokenArray.push_back(token);
            }
            token.tokenType = 1;
            token.tokenValue = cIntTable.getDelimiterValue(currentCh);
            tokenArray.push_back(token);
            tokenStr = "";
        }
        else { // put this character in to the queue
            tokenStr = tokenStr + currentCh;
        }
        currentCh = "";
    }

    return tokenArray;
}

Token Compiler::identify(string tokenStr, int subroutine) {
    Token token;
    if (cIntTable.getDelimiterValue(tokenStr) != -1) { // Delimiter
        token.tokenType = 1;
        token.tokenValue = cIntTable.getDelimiterValue(tokenStr);
    }
    else if (cIntTable.getReservedWordValue(tokenStr) != -1) { // Reserved Word
        if (tokenStr == "PROGRAM")
            subroutine = -1;

        token.tokenType = 2;
        token.tokenValue = cIntTable.getReservedWordValue(tokenStr);
        if (tokenStr == "ENS")
            cSubroutine = -1;
    }
    else if (tokenStr.at(0) <= '9' && tokenStr.at(0) >= '0') { // Integer and Real Number
        token.tokenType = 3;
        for (int i = 0; i < tokenStr.length() && token.tokenType!= -1; i++) {
            if (tokenStr.at(i) == '.') {
                if (token.tokenType == 4) 
                    token.tokenType = -1;
                else 
                    token.tokenType =  4;

            }
            else if (tokenStr.at(i) > '9' && tokenStr.at(i) < '0')
                token.tokenType = -1;
                
        } // for
            
        if (token.tokenType = 3) 
            token.tokenValue = cIntTable.insertToken(tokenStr);
        else if (token.tokenType = 4)
            token.tokenValue = cRealNumTable.insertToken(tokenStr);
        else 
            token = {-1, -1}; // Error number
    }
    else { // Identifer
        token.tokenType = 5;
        token.tokenValue = cIdTable.insertToken(tokenStr, subroutine);
        if (cSubroutine == -1)
            cSubroutine = token.tokenValue;
    }

    return token;
}

void Compiler::syntaxErrorMsg(int errorCode){
    cout << "Line: " << cCurLine << " ";
    if (errorCode == 12) {
        cout << "The last character should be ;" << endl;
    } 
    else if (errorCode == 13) {
        cout << "Error define" << endl;
    }
    else if (errorCode == 14) {
        cout << "Error format" << endl;
    }
    else if (errorCode == 15) {
        cout << "Undefine symbol" << endl;
    }
    else if (errorCode == 16) {
        cout << "Error expression" << endl;
    }
    else if (errorCode == 17) {
        cout << "Error condition" << endl;
    }
    else if (errorCode == 18) {
        cout << "Error Statement" << endl;
    }
    else if (errorCode == 19) {
        cout << "Error Variable" << endl;
    }
    else if (errorCode == 20) {
        cout << "Error Argument" << endl;
    }
    else if (errorCode == 21) {
        cout << "Should be identifer !!" << endl;
    }
    else if (errorCode == 22) {
        cout << "Data type Error !!" << endl;
    }
    else cout << "Unknown Error" << endl;
}

string Compiler::getStateMent() {
    string str = "";
    char ch;
    do
    {
        cInfile.get(ch);
        if (str == "\n") {
            str = "";
            cCurLine++;
        }
            
        str += ch;
    } while (!cInfile.eof() && ch != ';');
    
    return str;
}

void Compiler::initial() {
    cCurrentSourceCode = "";
    cSubroutine = -1;
    cCurLine = 1;
    cInforTable.clear();
    cInforTable.push_back(0);
}

bool Compiler::readFile(string input) {
    string str;
    cFileName = input;
    str = cFileName + ".txt";
    cInfile.open(str.c_str(), ios::in);
	if (cInfile) 
		return true;

	else return false;
}

void Compiler::compiler() {
    vector<Token> lexicalCode;
    SyntaxAnalysis syn;
    int synResult;
    cCurrentSourceCode = getStateMent();
    cout << cCurrentSourceCode << "\t"; // debug
    while (!cInfile.eof()) {
        lexicalCode.clear();
        lexicalCode = lexicalAnalysis();
        synResult = syn.syntaxAnalysis(lexicalCode, cIntTable, cRealNumTable, cIdTable, cInforTable);
        if (synResult > 11) {
            syntaxErrorMsg(synResult);
            break;
        }
        else {
            cout << synResult << endl; // debug
        }
         /* // ==========================debug==========================
        for (int i = 0; i < lexicalCode.size(); i++)
            cout << "(" << lexicalCode[i].tokenType << ", " << lexicalCode[i].tokenValue << ")  ";
        cout << endl;
        // ==========================debug========================== */
        cCurrentSourceCode = getStateMent();
        cout << cCurrentSourceCode << "\t"; // debug
	}
    cInfile.close();
}

int main(int argc, char const *argv[])
{
    Compiler comp;
    string input;
    
    cout << "Please input file name: (ex: input.txt >input)" << endl << ">";
    cin >> input;
    comp.initial();
    if (comp.readFile(input)) {
        comp.compiler();
    }
    else {
        cout << "Can't find file " << input << ".txt !!" << endl;
    }
    cout << "\ninput 0 to quit:" << endl << ">"; 
    // =================
    input = "0"; // debug
    // =================
    // cin >> input;
    if (input != "0") {
        main(argc, argv);
    }
    else return 0;

}
