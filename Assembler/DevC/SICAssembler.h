
#include "GetToken.h"
#include "ConfTable.h"
#include "LiteralTable.h"

#ifndef SICASSEMBLER_H
#define SICASSEMBLER_H

using namespace std;

struct InstructionSet {
	int format;
	int opcode;
};

struct AssemblrData{
	int codeSize;
	int location;
	Objectcode* objectCode;
	string errorMsg;
};

struct Opereand {
	int operandMode1; // @ # =
	int operandMode2;
	Token operand;
};

class SicAssembler {
public:
	SicAssembler();
	SicAssembler(vector<Line> lexicalCode, vector<string> sourceCode, AllTable allTable);
	vector<AssemblrData> assembler(vector<string> &sourceCode);
	
protected:
	int sLocationCounter, sLine;
	int sB, sP; // Base Register & Promgram Counter
	vector<Line> sLexicalCode; // code from GetToken
	vector<string> sSourceCode;
	vector<AssemblrData> sAssemblerOutput; // The result should assemblr output
	Table sTable1, sTable2, sTable3, sTable4, sTable5, sTable6, sTable7; // all of the table
	ConfTable sConfTable;
	LiteralTable sLiteralTable;
	//-----------------------------------------------------------------------------------------
	InstructionSet getInsSet(Token token); // get opcode
	bool parsing(vector<Token> currenLine, Token &symbol, bool &isFormat4, Token &opcode, Opereand &operand1, Token &operand2);
	AssemblrData pseudo(AssemblrData currentData, Token symbol, Token opcode, Opereand operand1, Token operand2);
	AssemblrData instStart(AssemblrData currentData, Token symbol, Opereand operand1);
	AssemblrData instEnd(AssemblrData currentData, Opereand operand1);
	AssemblrData instByte(AssemblrData currentData, Opereand operand1);
	AssemblrData instWord(AssemblrData currentData, Opereand operand1);
	AssemblrData instResb(AssemblrData currentData, Opereand operand1);
	AssemblrData instResw(AssemblrData currentData, Opereand operand1);
	AssemblrData instEqu(AssemblrData currentData, Token symbol, Opereand operand1);
	AssemblrData instBase(AssemblrData currentData, Opereand operand1);
	AssemblrData instLtorg(AssemblrData currentData);
	int stringToAscii(string str);
	Objectcode* literalProcess(Opereand operand1);
private:
	AssemblrData inst(AssemblrData currentData, Token opcode, Opereand operand1, Token operand2);
};

SicAssembler::SicAssembler(){
	int sLocationCounter = 0;
	sB = 0;
	sP = 0;
}

SicAssembler::SicAssembler(vector<Line> lexicalCode, vector<string> sourceCode, AllTable allTable) {
	int sLocationCounter = 0;
	sB = 0;
	sP = 0;
	sLexicalCode = lexicalCode;
	sSourceCode = sourceCode;
	sTable1 = allTable.table1; // Instruction
	sTable2 = allTable.table2; // Pseudo instruction 
	sTable3 = allTable.table3; // Register
	sTable4 = allTable.table4; // Delimiter
	sTable5 = allTable.table5; // Symbol
	sTable6 = allTable.table6; // Integer
	sTable7 = allTable.table7; // String
}

vector<AssemblrData> SicAssembler::assembler(vector<string> &sourceCode) {
	int j;
	int symbolValue;
	string token;
	AssemblrData currentData;
	bool isFormat4;
	Token symbol, opcode, operand2;
	Opereand operand1;
	vector<Token> currenLine;
	Objectcode* newObjCode;
	
	for (sLine = 0; sLine < sLexicalCode.size(); sLine++) {
		operand2.tokenType = 0;
		operand1.operand.tokenType = 0;
		currenLine = sLexicalCode[sLine].line;
		j = 0;
		symbolValue = -1;
		currentData.location = -1;
		currentData.codeSize = 0;
		currentData.objectCode = NULL;
		currentData.errorMsg = "";
		sLocationCounter = sP;
		if (parsing(currenLine, symbol, isFormat4, opcode, operand1, operand2)) {
			if (symbol.tokenType != 0) { // symbol
				if (!sConfTable.insertSymbol(symbol.tokenValue, sLocationCounter, 0)) {
					currentData.errorMsg = "Error: Multiple symbol Define !!";
					goto end;
				}
				j++;
			}
			if (opcode.tokenType == 2) { // pseudo instruction
				currentData = pseudo(currentData, symbol, opcode, operand1, operand2);
			}
			else if (opcode.tokenType == 1) { // Instruction
				currentData = inst(currentData, opcode, operand1, operand2);
			}
			else {
				newObjCode = new Objectcode;
				newObjCode->objectValue = -1;
				currentData.objectCode = newObjCode;
			}
		}
		else if (currenLine.size() != 0) {
			currentData.errorMsg = "Error: Syntactic error !!";
		}
		
	end:
		sAssemblerOutput.push_back(currentData);
	}
	sourceCode = sSourceCode;
	return sAssemblerOutput;
}

InstructionSet SicAssembler::getInsSet(Token token) {
	InstructionSet set;
	if (token.tokenValue == 1) { set.format = 34;set.opcode = 0x18;} else if (token.tokenValue == 2){ set.format = 34; set.opcode = 0x58; }
	else if (token.tokenValue == 3) { set.format = 2; set.opcode = 0x90; } else if (token.tokenValue == 4) { set.format = 34; set.opcode = 0x40; }
	else if (token.tokenValue == 5) { set.format = 2; set.opcode = 0xB4; } else if (token.tokenValue == 6) { set.format = 34; set.opcode = 0x28; }
	else if (token.tokenValue == 7) { set.format = 34; set.opcode = 0x88; } else if (token.tokenValue == 8) { set.format = 2; set.opcode = 0xA0; }
	else if (token.tokenValue == 9) { set.format = 34; set.opcode = 0x24; } else if (token.tokenValue == 10) { set.format = 34; set.opcode = 0x64; }
	else if (token.tokenValue == 11) { set.format = 2; set.opcode = 0x9C; } else if (token.tokenValue == 12) { set.format = 1; set.opcode = 0xC4; }
	else if (token.tokenValue == 13) { set.format = 1; set.opcode = 0xC0; } else if (token.tokenValue == 14) { set.format = 1; set.opcode = 0xF4; }
	else if (token.tokenValue == 15) { set.format = 34; set.opcode = 0x3C; } else if (token.tokenValue == 16) { set.format = 34; set.opcode = 0x30; }
	else if (token.tokenValue == 17) { set.format = 34; set.opcode = 0x34; } else if (token.tokenValue == 18) { set.format = 34; set.opcode = 0x38; }
	else if (token.tokenValue == 19) { set.format = 34; set.opcode = 0x48; }else if (token.tokenValue == 20) { set.format = 34; set.opcode = 0x00; }
	else if (token.tokenValue == 21) { set.format = 34; set.opcode = 0x68; }else if (token.tokenValue == 22) { set.format = 34; set.opcode = 0x50; }
	else if (token.tokenValue == 23) { set.format = 34; set.opcode = 0x70; }else if (token.tokenValue == 24) { set.format = 34; set.opcode = 0x08; }
	else if (token.tokenValue == 25) { set.format = 34; set.opcode = 0x6C; }else if (token.tokenValue == 26) { set.format = 34; set.opcode = 0x74; }
	else if (token.tokenValue == 27) { set.format = 34; set.opcode = 0x04; }else if (token.tokenValue == 28) { set.format = 34; set.opcode = 0xD0; }
	else if (token.tokenValue == 29) { set.format = 34; set.opcode = 0x20; }else if (token.tokenValue == 30) { set.format = 34; set.opcode = 0x60; }
	else if (token.tokenValue == 31) { set.format = 2; set.opcode = 0x98; }else if (token.tokenValue == 32) { set.format = 1; set.opcode = 0xC8; }
	else if (token.tokenValue == 33) { set.format = 34; set.opcode = 0x44; }else if (token.tokenValue == 34) { set.format = 34; set.opcode = 0xD8; }
	else if (token.tokenValue == 35) { set.format = 2; set.opcode = 0xAC; }else if (token.tokenValue == 36) { set.format = 34; set.opcode = 0x4C; }
	else if (token.tokenValue == 37) { set.format = 2; set.opcode = 0xA4; }	else if (token.tokenValue == 38) { set.format = 2; set.opcode = 0xA8; }
	else if (token.tokenValue == 39) { set.format = 1; set.opcode = 0xF0; }	else if (token.tokenValue == 40) { set.format = 34; set.opcode = 0xEC; }
	else if (token.tokenValue == 41) { set.format = 34; set.opcode = 0x0C; }else if (token.tokenValue == 42) { set.format = 34; set.opcode = 0x78; }
	else if (token.tokenValue == 43) { set.format = 34; set.opcode = 0x54; }else if (token.tokenValue == 44) { set.format = 34; set.opcode = 0x80; }
	else if (token.tokenValue == 45) { set.format = 34; set.opcode = 0xD4; }else if (token.tokenValue == 46) { set.format = 34; set.opcode = 0x14; }
	else if (token.tokenValue == 47) { set.format = 34; set.opcode = 0x7C; }else if (token.tokenValue == 48) { set.format = 34; set.opcode = 0xE8; }
	else if (token.tokenValue == 49) { set.format = 34; set.opcode = 0x84; }else if (token.tokenValue == 50) { set.format = 34; set.opcode = 0x10; }
	else if (token.tokenValue == 51) { set.format = 34; set.opcode = 0x1C; }else if (token.tokenValue == 52) { set.format = 34; set.opcode = 0x5C; }
	else if (token.tokenValue == 53) { set.format = 2; set.opcode = 0x94; }	else if (token.tokenValue == 54) { set.format = 2; set.opcode = 0xB0; }
	else if (token.tokenValue == 55) { set.format = 34; set.opcode = 0xE0; }else if (token.tokenValue == 56) { set.format = 1; set.opcode = 0xF8; }
	else if (token.tokenValue == 57) { set.format = 34; set.opcode = 0x2C; }else if (token.tokenValue == 58) { set.format = 2; set.opcode = 0xB8; }
	else if (token.tokenValue == 59) { set.format = 34; set.opcode = 0xDC; }
	return set;
}

bool  SicAssembler::parsing(vector<Token> currenLine, Token &symbol, bool &isFormat4, Token &opcode, Opereand &operand1, Token &operand2) {
	int index = 0;
	isFormat4 = false;
	symbol.tokenType = 0;
	opcode.tokenType = 0;
	operand1.operandMode1 = 0;
	operand1.operandMode2 = 0;
	operand2.tokenType = 0;
	if (currenLine.size() > index && currenLine[index].tokenType == 5) { // symbol
		symbol = currenLine[index];
		index++;
	}
	// =================instruction=================
	if (currenLine.size() > index &&currenLine[index].tokenType == 4 && currenLine[index].tokenValue == 2 && (currenLine[index+1].tokenType == 1 || currenLine[index+1].tokenType == 2)) {
		isFormat4 = true; // + instruction
		index++;
		goto inst;
	}
	else if (currenLine.size() > index && (currenLine[index].tokenType == 1 || currenLine[index].tokenType == 2)) { // instruction
	inst:
		opcode = currenLine[index];
		index++;
	// =================instruction=================
		// ================operand1================
	operand1:
		if (currenLine.size() > index && currenLine[index].tokenType == 4 && currenLine[index].tokenValue >= 11
			&& !(currenLine[index+1].tokenType == 4 && currenLine[index + 1].tokenValue >= 11)) { // = # @
			operand1.operandMode1 = currenLine[index].tokenValue;
			index++;
			goto operand1;
		}
		else if (currenLine.size() > index && (currenLine[index].tokenType == 5 || currenLine[index].tokenType == 6 || currenLine[index].tokenType == 3)) {
			operand1.operand = currenLine[index];
			index++;
		// ================operand1================
			goto operand2;
		}
		else if (currenLine.size() > index + 2 && currenLine[index].tokenType == 4 && currenLine[index].tokenValue == 9
			&& (currenLine[index + 1].tokenType == 6 || currenLine[index + 1].tokenType == 7) && currenLine[index+2].tokenType == 4 && currenLine[index+2].tokenValue == 9) {
			operand1.operandMode2 = currenLine[index].tokenValue; // ' '
			index++;
			operand1.operand = currenLine[index];
			index++;
			// ================operand2================
		operand2:
			if (currenLine.size() > index + 1 && currenLine[index].tokenType == 4 && currenLine[index].tokenValue == 1 && currenLine[index + 1].tokenType == 3) {
				operand2 = currenLine[index + 1];
				return true;
			}
			else if (currenLine.size() > index + 1) return false;
			else return true;
			// ================operand2================
			return true;
		}
		else if (currenLine.size() > index) 
			return false;
		else return true;
	}
	else return false;
}

AssemblrData SicAssembler::pseudo(AssemblrData currentData, Token symbol, Token opcode, Opereand operand1, Token operand2) {
	string tokenStr;
	currentData.codeSize = 0;
	currentData.errorMsg = "";
	Objectcode* newObj = new Objectcode;
	newObj->objectValue = -1;
	currentData.objectCode = newObj;
	if (operand2.tokenType == 0 && operand1.operand.tokenType != 0) {
		if (opcode.tokenValue == 1) { // START
			currentData = instStart(currentData, symbol, operand1);
		}
		else if (opcode.tokenValue == 2) { // END
			currentData = instEnd(currentData, operand1);
		}
		else if (opcode.tokenValue == 3) { // BYTE
			currentData = instByte(currentData, operand1);
		}
		else if (opcode.tokenValue == 4) { // WORD
			currentData = instWord(currentData, operand1);
		}
		else if (opcode.tokenValue == 5) { // RESB
			currentData = instResb(currentData, operand1);
		}
		else if (opcode.tokenValue == 6) { // RESW
			currentData = instResw(currentData, operand1);
		}
		else if (opcode.tokenValue == 7) { // EQU
			currentData = instEqu(currentData, symbol, operand1);
		}
		else { // BASE
			currentData = instBase(currentData, operand1);
		}
	}
	else if (opcode.tokenValue == 9) {// LTORG
		currentData = instLtorg(currentData);
	}
	else 
		currentData.errorMsg = "Error: No operand !!";

	return currentData;
}

int SicAssembler::stringToAscii(string str) {
	int ascii, i;
	for (i = 0, ascii = 0; i < str.length(); i++)
		ascii = ascii * 256 + str.at(i);

	return ascii;
}

AssemblrData SicAssembler::instStart(AssemblrData currentData, Token symbol, Opereand operand1) {
	Token operand = operand1.operand;
	if (operand.tokenType == 6) {
		sLocationCounter = stoi(sTable6.getTokenStr(operand.tokenValue), 0, 16);
		sP = sLocationCounter;
		currentData.location = sLocationCounter;
		sConfTable.coverAdress(symbol.tokenValue, sLocationCounter, 0);
	}
	else if (operand.tokenType == 5) {
		sLocationCounter = stoi(sTable5.getTokenStr(operand.tokenValue), 0, 16);
		sP = sLocationCounter;
		currentData.location = sLocationCounter;
		sConfTable.coverAdress(symbol.tokenValue, sLocationCounter, 0);
	}
	else currentData.errorMsg = "Error: Error operand";

	return currentData;
}

AssemblrData SicAssembler::instEnd(AssemblrData currentData, Opereand operand1) {
	Token operand = operand1.operand;
	LiteralReturn backLine;
	string newLine;
	Line lineOfToken;
	vector<Token> line;
	if (operand1.operandMode1 == 0 && operand.tokenType == 5) {
		int index = sLine+1;
		int location = sLocationCounter;
		while (sLiteralTable.undefineNum() != 0) {
			backLine = sLiteralTable.extendTable(location);
			location += backLine.codeSize;

			sSourceCode.insert(sSourceCode.begin() + index, backLine.newSourceCode);
			Line line;
			line.line = backLine.newCode;
			sLexicalCode.insert(sLexicalCode.begin() + index, line);
			index++;
		}
		sP = sLocationCounter;
		sLocationCounter = -1;
		currentData.location = sLocationCounter;
		
	}
	else currentData.errorMsg = "Error: Error operand";

	return currentData;
}

AssemblrData SicAssembler::instByte(AssemblrData currentData, Opereand operand1) {
	string tokenStr;
	Token operand = operand1.operand;
	if (operand1.operandMode2 == 9 && (operand.tokenType == 6 || operand.tokenType == 7)) { // BYTE
		if (operand.tokenType == 6) { // X'F5'
			tokenStr = sTable6.getTokenStr(operand.tokenValue);
			currentData.objectCode->objectValue = stoi(tokenStr, 0, 16);
			currentData.codeSize = tokenStr.length();
		}
		else { // C'EOF'
			tokenStr = sTable7.getTokenStr(operand.tokenValue);
			currentData.objectCode->objectValue = stringToAscii(tokenStr);
			currentData.codeSize = tokenStr.length() * 2;
		}
		
		currentData.location = sLocationCounter;
		sLocationCounter = sLocationCounter + currentData.codeSize / 2;
		sP = sLocationCounter;
	}
	else currentData.errorMsg = "Error: Error operand";

	return currentData;
}

AssemblrData SicAssembler::instWord(AssemblrData currentData, Opereand operand1) {
	currentData.codeSize = 6;
	string tokenStr;
	Token operand = operand1.operand;
	if (operand.tokenType == 5 || operand.tokenType == 6) {
		if (operand.tokenType == 5) { // operand is symbol
			if (sConfTable.getSymbolAdress(operand.tokenValue) == -2) { // no configuration list
				currentData.objectCode->objectValue = 0;
				sConfTable.link(operand.tokenValue, currentData.objectCode, 2);
			}
			else if (sConfTable.getSymbolAdress(operand.tokenValue) == -1) { // no adress
				currentData.objectCode->objectValue = 0;
				sConfTable.link(operand.tokenValue, currentData.objectCode, 1);
			}
			else
				currentData.objectCode->objectValue = sConfTable.getSymbolAdress(operand.tokenValue);
		}
		else { // operrand is integer
			tokenStr = sTable6.getTokenStr(operand.tokenValue);
			currentData.objectCode->objectValue = stoi(tokenStr, 0, 10);
		}
		currentData.codeSize = 6;
		currentData.location = sLocationCounter;
		sP += 3;
	}
	else currentData.errorMsg = "Error: Error operand";

	return currentData;
}

AssemblrData SicAssembler::instResb(AssemblrData currentData, Opereand operand1) {
	int value;
	Token operand = operand1.operand;
	if (operand.tokenType == 6) {
		value = stoi(sTable6.getTokenStr(operand.tokenValue), 0, 10);
		currentData.location = sLocationCounter;
		sP += value;
	}
	else currentData.errorMsg = "Error: Error operand";

	return currentData;
}

AssemblrData SicAssembler::instResw(AssemblrData currentData, Opereand operand1) {
	int value;
	Token operand = operand1.operand;
	if (operand.tokenType == 6) {
		value = stoi(sTable6.getTokenStr(operand.tokenValue), 0, 10);
		currentData.location = sLocationCounter;
		value *= 3;
		sP += value;
	}
	else currentData.errorMsg = "Error: Error operand";

	return currentData;
}

AssemblrData SicAssembler::inst(AssemblrData currentData, Token opcode, Opereand operand1, Token operand2) {
	Token operand = operand1.operand;
	string tokenStr;
	int i = 0;
	long long longlongOp;
	sP += 3;
	Objectcode* newPtr = new Objectcode;
	currentData.objectCode = newPtr;
	newPtr = 0;
	longlongOp = getInsSet(opcode).opcode * 65536;
	// // ===============Operand1=============
	if (operand1.operandMode1 == 11) { // =
		currentData.objectCode = literalProcess(operand1);
	}
	else if (operand1.operandMode1 == 13) { // @

	}
	else if (operand.tokenType == 5) {
		if (sConfTable.getSymbolAdress(operand.tokenValue) == -2) { // no configuration list
			currentData.objectCode->objectValue = longlongOp;
			sConfTable.link(operand.tokenValue, currentData.objectCode, 2);
		}
		else if (sConfTable.getSymbolAdress(operand.tokenValue) == -1) { // no adress
			currentData.objectCode->objectValue = longlongOp;
			sConfTable.link(operand.tokenValue, currentData.objectCode, 1);
		}
		else
			currentData.objectCode->objectValue = longlongOp + sConfTable.getSymbolAdress(operand.tokenValue);

	}
	else if (operand.tokenType == 6) { // operrand is integer
		tokenStr = sTable6.getTokenStr(operand.tokenValue);
		currentData.objectCode->objectValue = stoi(tokenStr, 0, 10) + longlongOp;
	}
	else currentData.objectCode->objectValue = longlongOp;
	
	// ===============Operand2=============
	if (operand2.tokenType != 0) {
		if (operand2.tokenType == 3 && operand2.tokenValue == 2)
			currentData.objectCode->objectValue = currentData.objectCode->objectValue + 32768;
		else
			currentData.errorMsg = "Error: Error operand !!";
	}
	// ===============Operand2=============
	currentData.location = sLocationCounter;
	currentData.codeSize = 6;

	return currentData;
}

AssemblrData SicAssembler::instEqu(AssemblrData currentData, Token symbol, Opereand operand1) {
	Token operand = operand1.operand;
	if (operand.tokenType == 5 || operand.tokenType == 6) {
		if (operand.tokenType == 6) {
			sLocationCounter = stoi(sTable6.getTokenStr(operand.tokenValue), 0, 10);
			sConfTable.coverAdress(symbol.tokenValue, sLocationCounter, 0);
		}
		else {
			sLocationCounter = sConfTable.getSymbolAdress(operand.tokenValue);
			sConfTable.coverAdress(symbol.tokenValue, sLocationCounter, 0);
		}
	}
	currentData.location = sLocationCounter;

	return currentData;
}

AssemblrData SicAssembler::instBase(AssemblrData currentData, Opereand operand1) {
	Token operand = operand1.operand;
	Objectcode* temp = new Objectcode;
	temp->objectValue = -1;
	if (operand.tokenType == 5 || operand.tokenType == 6) {
		if (operand.tokenType == 6) {
			sB = stoi(sTable6.getTokenStr(operand.tokenValue), 0, 16);
		}
		else {
			sConfTable.confProcess(operand.tokenValue, sP, sB, temp, 3);
		}
	}

	return currentData;
}

AssemblrData SicAssembler::instLtorg(AssemblrData currentData) {
	LiteralReturn backLine;
	string newLine;
	Line lineOfToken;
	vector<Token> line;
	int index = sLine + 1;
	int location = sLocationCounter;
	while (sLiteralTable.undefineNum() != 0) {
		backLine = sLiteralTable.extendTable(location);
		location += backLine.codeSize;

		sSourceCode.insert(sSourceCode.begin() + index, backLine.newSourceCode);
		Line line;
		line.line = backLine.newCode;
		sLexicalCode.insert(sLexicalCode.begin() + index, line);
		index++;
	}
	sP = sLocationCounter;
	sLocationCounter = -1;
	currentData.location = sLocationCounter;

	return currentData;
}

Objectcode* SicAssembler::literalProcess(Opereand operand1) {
	Objectcode* newObjCode = new Objectcode;
	Token m = operand1.operand;
	newObjCode = new Objectcode;
	newObjCode->objectValue = 0;
	if (operand1.operandMode2 == 9) { // =C'EOF', =X'5F'
		if (operand1.operand.tokenType == 6) { // X'5F'
			sLiteralTable.literalProcess('x', sTable6.getTokenStr(m.tokenValue), m.tokenType, m.tokenValue, newObjCode);
		}
		else { // C'EOF'
			sLiteralTable.literalProcess('c', sTable7.getTokenStr(m.tokenValue), m.tokenType, m.tokenValue, newObjCode);
		}
	}
	else { // =123
		sLiteralTable.literalProcess('w', sTable6.getTokenStr(m.tokenValue), m.tokenType, m.tokenValue, newObjCode);
	}

	return newObjCode;
}
#endif
