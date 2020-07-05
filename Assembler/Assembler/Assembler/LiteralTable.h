
#include "pch.h"
#include "ConfTable.h"

#ifndef LITERALRABLE_H
#define LITERALRABLE_H

using namespace std;

struct UndefineAdress {
	Objectcode* objectCode;
	UndefineAddress* next;
};

struct LiteralList {
	char format;
	string token;
	int literalType;
	int literalValue;
	int address;
	UndefineAddress* undefineAddress;
	LiteralList* next;
};

struct LiteralReturn {
	string newSourceCode;
	vector<Token> newCode;
	int codeSize;
};

class LiteralTable {
public:
	void literalProcess(char format, string token, int literalType, int literalValue, Objectcode* objectCode);
	LiteralReturn extendTable(int location);
	int undefineNum();

private:
	vector<LiteralList> lLiteralTable;
	int unextend = 0;
	
// ========================================================================
	UndefineAddress* link(UndefineAddress* head, Objectcode* objectCode);
	UndefineAddress* patch(UndefineAddress* head, int location);
	string getSourceCodeLine();
	vector<Token> getTokenLine();
	
};

void LiteralTable::literalProcess(char format, string token, int literalType, int literalValue, Objectcode* objectCode) {
	int index;
	LiteralList newLiteral;
	for (index = 0; index < lLiteralTable.size(); index++) {
		if (format == lLiteralTable[index].format && token == lLiteralTable[index].token && literalType == lLiteralTable[index].literalType && literalValue == lLiteralTable[index].literalValue)
			break;
	}

	if (index == lLiteralTable.size()) {
		newLiteral.format = format;
		newLiteral.token = token;
		newLiteral.literalType = literalType;
		newLiteral.literalValue = literalValue;
		newLiteral.address = -1;
		UndefineAddress* newPtr = new UndefineAddress;
		newPtr = NULL;
		newPtr = link(newPtr, objectCode);
		newLiteral.undefineAddress = newPtr;
		lLiteralTable.push_back(newLiteral);
	}
	else if (lLiteralTable[index].address == -1) {
		lLiteralTable[index].undefineAddress = link(lLiteralTable[index].undefineAddress, objectCode);
	}
	else {
		objectCode->objectValue += lLiteralTable[index].address;
	}
	
}

LiteralReturn LiteralTable::extendTable(int location) { 
	LiteralReturn returnBack;
	string str;
	LiteralList currrent = lLiteralTable[unextend];
	currrent.undefineAddress = patch(currrent.undefineAddress, location);
	returnBack.newCode = getTokenLine();
	returnBack.newSourceCode = getSourceCodeLine();
	if (currrent.format == 'w') {
		returnBack.codeSize = 3;
	}
	else {
		str = currrent.token;
		returnBack.codeSize = str.length();
	}

	unextend++;

	return returnBack;
}

UndefineAddress* LiteralTable::link(UndefineAddress* head, Objectcode* objectCode) {
	UndefineAddress* newPtr;
	if (head == NULL) {
		newPtr = new UndefineAddress;
		newPtr->objectCode = objectCode;
		newPtr->next = NULL;

		return newPtr;
	}
	else 
		return head->next = link(head->next, objectCode);
	
}

UndefineAddress* LiteralTable::patch(UndefineAddress* head, int location) {
	if (head == NULL)
		return head;
	else {
		head->objectCode->objectValue += location;
		return head->next = patch(head->next, location);
	}
}

string LiteralTable::getSourceCodeLine() {
	LiteralReturn returnBack;
	string returnStr = "";
	LiteralList currrent = lLiteralTable[unextend];
	returnStr = "=";
	if (currrent.format == 'x' || currrent.format == 'c') {
		returnStr += currrent.format;
		returnStr += "'";
		returnStr += currrent.token;
		returnStr += "'";
		returnStr += "\tBYTE\t=";
		returnStr += currrent.format;
		returnStr += "'";
		returnStr += currrent.token;
		returnStr += currrent.token;
		returnStr += "'";
	}
	else {
		returnStr += currrent.token;
		returnStr += "\tWORD\t=";
		returnStr += currrent.token;
		
	}
	
	return returnStr;
}
vector<Token> LiteralTable::getTokenLine() {
	vector<Token> returnLine;
	Token token, intruction, a;
	LiteralList currrent = lLiteralTable[unextend];
	token.tokenType = currrent.literalType;
	token.tokenValue = currrent.literalValue;
	if (currrent.format == 'x' || currrent.format == 'c') {
		intruction = { 2, 3 }; // BYTE
		returnLine.push_back(intruction);
		a = { 4, 9 };
		returnLine.push_back(a);
		returnLine.push_back(token);
		returnLine.push_back(a);
	}
	else {
		intruction = { 2, 4 }; // WORD
		returnLine.push_back(intruction);
		returnLine.push_back(token);
	}
	
	return returnLine;
}

int LiteralTable::undefineNum() {
	return lLiteralTable.size() - unextend;
}

#endif
