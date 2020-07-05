#include "GetToken.h"
#include "ConfTable.h"
#include "LiteralTable.h"
#include "SICAssembler.h"

#ifndef XEASSEMBLER_H
#define XEASSEMBLER_H

class XEAssembler: public SicAssembler {
public:
	XEAssembler(vector<Line> lexicalCode, vector<string> sourceCode, AllTable allTable);
	vector<AssemblrData> assembler(vector<string> &sourceCode);
private:
//==================================================================================================================
	AssemblrData xeInst(AssemblrData currentData, bool isFormat4, Token opcode, Opereand operand1, Token operand2);
	AssemblrData format1(AssemblrData currentData, long long opValue);
	AssemblrData format2(AssemblrData currentData, long long opValue, Opereand operand1, Token operand2);
	AssemblrData format3(AssemblrData currentData, long long opValue, Opereand operand1, Token operand2);
	AssemblrData format4(AssemblrData currentData, long long opValue, Opereand operand1, Token operand2);
};
XEAssembler::XEAssembler(vector<Line> lexicalCode, vector<string> sourceCode, AllTable allTable) {
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

vector<AssemblrData> XEAssembler::assembler(vector<string> &sourceCode) {
	int j;
	int symbolValue;
	string token;
	AssemblrData currentData;
	bool isFormat4;
	Token symbol, opcode, operand2;
	vector<Token> currenLine;
	Opereand operand1;
	Objectcode* newObjCode;
	for (sLine = 0; sLine < sLexicalCode.size(); sLine++) {
		operand2.tokenType = 0;
		operand1.operand.tokenType = 0;
		currenLine = sLexicalCode[sLine].line;
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
			}
			if (opcode.tokenType == 2) { // pseudo instruction
				currentData = pseudo(currentData, symbol, opcode, operand1, operand2);
			}
			else if (opcode.tokenType == 1) { // Instruction
				currentData = xeInst(currentData, isFormat4, opcode, operand1, operand2);
			}
			else {
				newObjCode = new Objectcode;
				newObjCode->objectValue = -1;
				currentData.objectCode = newObjCode;
			}
		}
		else if (currenLine.size() != 0) 
			currentData.errorMsg = "Error: Syntactic error !!";

	end:
		sAssemblerOutput.push_back(currentData);
	}
	sourceCode = sSourceCode;
	return sAssemblerOutput;
}

AssemblrData XEAssembler::xeInst(AssemblrData currentData, bool isFormat4, Token opcode, Opereand operand1, Token operand2) {
	InstructionSet format = getInsSet(opcode);
	long long op = format.opcode;
	if (format.format != 34 && isFormat4) {
		currentData.errorMsg = "Error: Error instruction !!";
	}
	else {
		if (format.format == 1 && operand1.operand.tokenType == 0) {
			currentData = format1(currentData, op);
		}
		else if (format.format == 2 && ( operand1.operand.tokenType == 3 || operand1.operand.tokenType == 0)) {
			op *= 256;
			currentData = format2(currentData, op, operand1, operand2);
		}
		else {
			if (isFormat4) {
				op *= 0x1000000;
				currentData = format4(currentData, op, operand1, operand2);
			}
			else {
				op *= 0x10000;
				currentData = format3(currentData, op, operand1, operand2);
			}
		}

	}

	return currentData;
}

AssemblrData XEAssembler::format1(AssemblrData currentData, long long opValue) {
	sP++;
	Objectcode* newObjCode;
	newObjCode = new Objectcode;
	newObjCode->objectValue = opValue;
	currentData.codeSize = 2;
	currentData.location = sLocationCounter;
	currentData.objectCode = newObjCode;

	return currentData;
}

AssemblrData XEAssembler::format2(AssemblrData currentData, long long opValue, Opereand operand1, Token operand2) {
	sP += 2;
	int r1 = 0, r2 = 0;
	if (operand1.operand.tokenType == 0 || (operand1.operand.tokenType == 6 && sTable6.getTokenStr(operand1.operand.tokenValue) == "0")) {
		r1 = 0;
	}
	else if (operand1.operand.tokenType == 3) {
		r1 =( operand1.operand.tokenValue-1) * 0x10;
	}
	else {
		currentData.errorMsg = "Error: Error operand !!";
		return currentData;
	}

	if (operand2.tokenType == 0 || (operand2.tokenType == 6 && sTable6.getTokenStr(operand2.tokenValue) == "0")) {
		r2 = 0;
	}
	else if (operand2.tokenType == 3) {
		r2 = operand2.tokenValue-1;
	}
	else {
		currentData.errorMsg = "Error: Error operand !!";
		return currentData;
	}
	Objectcode* newObjCode;
	newObjCode = new Objectcode;
	newObjCode->objectValue = opValue;
	newObjCode->objectValue += r1;
	newObjCode->objectValue += r2;
	currentData.codeSize = 2;
	currentData.location = sLocationCounter;
	currentData.objectCode = newObjCode;
	return currentData;
	
}

AssemblrData XEAssembler::format3(AssemblrData currentData, long long opValue, Opereand operand1, Token operand2) {
	Token m = operand1.operand;
	string tokenStr;
	if (operand1.operand.tokenType != 0) {
		Objectcode* newObjCode;
		newObjCode = new Objectcode;
		newObjCode->objectValue = 0;
		sP += 3;
		if (operand1.operandMode1 == 11) { // =
			newObjCode = literalProcess(operand1);
		}
		else if (operand1.operandMode1 == 13) { // @

		}
		else { // # or no mode
			if (m.tokenType == 6) { // interger
				tokenStr = sTable6.getTokenStr(m.tokenValue);
				newObjCode->objectValue += stoi(tokenStr, 0, 10);
			}
			else { // m.tokenType == 5 (symbol)
				sConfTable.confProcess(m.tokenValue, sP, sB, newObjCode, 1);
			}
		}
		if (operand1.operandMode1 == 12) // i = 1 #
			newObjCode->objectValue += 0x10000;
		else if (operand1.operandMode1 == 13) // n = 1 @
			newObjCode->objectValue += 0x20000;
		else
			newObjCode->objectValue += 0x30000; // i = 1 n = 1

		if (operand2.tokenType == 3 && operand2.tokenValue == 2) {
			newObjCode->objectValue += 0x8000; // x = 1 index register

		}
		currentData.objectCode = newObjCode;
		currentData.location = sLocationCounter;
		currentData.objectCode->objectValue += opValue;
		currentData.codeSize = 6;
	}

	return currentData;
}

AssemblrData XEAssembler::format4(AssemblrData currentData, long long opValue, Opereand operand1, Token operand2) {
	Token m = operand1.operand;
	string tokenStr;
	if (operand1.operand.tokenType != 0) {
		Objectcode* newObjCode;
		newObjCode = new Objectcode;
		newObjCode->objectValue = 0;
		sP += 4;
		if (operand1.operandMode1 == 11) { // =
			newObjCode = literalProcess(operand1);
		}
		else if (operand1.operandMode1 == 13) { // @

		}
		else { // # or no mode
			if (m.tokenType == 6) { // interger
				tokenStr = sTable6.getTokenStr(m.tokenValue);
				newObjCode->objectValue += stoi(tokenStr, 0, 10);
			}
			else { // m.tokenType == 5 (symbol)
				sConfTable.confProcess(m.tokenValue, sP, sB, newObjCode, 2);
			}
		}
		if (operand1.operandMode1 == 12) // i = 1 #
			newObjCode->objectValue += 0x1000000;
		else if (operand1.operandMode1 == 13) // n = 1 @
			newObjCode->objectValue += 0x2000000;
		else 
			newObjCode->objectValue += 0x3000000;

		if (operand2.tokenType == 3 && operand2.tokenValue == 2) {
			newObjCode->objectValue += 0x800000; // x = 1 index register
		}
		newObjCode->objectValue += 0x100000; // e = 1 format4
		currentData.location = sLocationCounter;
		currentData.objectCode = newObjCode;
		currentData.objectCode->objectValue += opValue;
		currentData.codeSize = 8;
	}

	return currentData;
}
#endif