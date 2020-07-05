
#include "pch.h"

#ifndef GETTOKEN_H
#define GETTOKEN_H

using namespace std;

class Table {
public:
	bool inputTable(string fileName);
	int findToken(string token);
	int insertHashTable(string token);
	string getTokenStr(int tokenValue);

private:
	vector<string> tTable;
};

bool Table::inputTable(string fileName) {
	fstream infile;
	string input;
	infile.open(fileName.c_str(), ios::in);
	if (infile) {
		while (!infile.eof()) {
			infile >> input;
			tTable.push_back(input);
		}
		infile.close();

		return true;
	} // ×x.table Œ¢string·Åµ½tTable
	else return false;
}

int Table::findToken(string token) {
	bool found = false;
	int index;
	for (index = 0; index < tTable.size(); index++) {
		if (tTable[index] == token) {
			found = true;
			break;
		}
	}
	if (found)
		return index+1;
	else return -1;
}

int Table::insertHashTable(string token) {
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
		while (tTable[index] != token && i < 101) {
			index++;
			if (index > 99) 
				index = 0;

			if (tTable[index] == "") {
				tTable[index] = token;
				break;
			}
			else if (tTable[index] == token)
				break;

			i++;
		}
	}

	return index;
}

string Table::getTokenStr(int tokenValue) {
	return tTable[tokenValue];
}

struct AllTable {
	Table table1;
	Table table2;
	Table table3;
	Table table4;
	Table table5;
	Table table6;
	Table table7;
};

struct Token {
	int tokenType;
	int tokenValue;
};

struct Line {
	vector<Token> line;
};

class GetToken {
public:
	GetToken(Table table1, Table table2, Table table3, Table table4);
	bool inputCode(string fileName, vector<string> &sourceCode);
	vector<Line> lexicalAnalysis();
	void outputCode();
	AllTable getTable();
private:
	vector<string> gSourceCode; // source code
	vector<Line> gLexicalCode; // code after GetToken
	Table gTable1, gTable2, gTable3, gTable4, gTable5, gTable6, gTable7; // all table
	// ----------------------------------------------------
	bool whiteSpace(string str);
	Token tokenAnalysis(string tokenStr);
	string toLower(string token);
	string toBigger(string token);
	bool isInt(string token);
};

bool GetToken:: inputCode(string fileName, vector<string> &sourceCode) {
	ifstream infile;
	string input;
	fileName = fileName + ".txt";
	infile.open(fileName.c_str());
	if (infile) {
		while (!infile.eof()) {
			getline(infile, input);
			gSourceCode.push_back(input);
		}
		infile.close();
		sourceCode = gSourceCode;
		return true;
	}
	else return false;
}

GetToken::GetToken(Table table1, Table table2, Table table3, Table table4) {
	gTable1 = table1;
	gTable2 = table2;
	gTable3 = table3;
	gTable4 = table4;
}

vector<Line> GetToken::lexicalAnalysis() {
	int currentLine, currentChar;
	string tokenStr = "", lineString, currentStr, temp;
	Token tokenTV ;
	Line line;
	for (currentLine = 0; currentLine < gSourceCode.size(); currentLine++) {
		lineString = gSourceCode[currentLine];
		for (currentChar = 0; currentChar < lineString.length(); currentChar++) {
			currentStr = lineString.at(currentChar);
			if (currentStr == "'" &&  (tokenStr == "X" || tokenStr == "C" || tokenStr == "x" || tokenStr == "c")) {
				tokenTV = tokenAnalysis(currentStr);
				line.line.push_back(tokenTV);
				if (tokenStr == "X") 
					temp = "X";
				else 
					temp = "C";

				currentChar++;
				currentStr = lineString.at(currentChar);
				tokenStr.clear();
				while (!whiteSpace(currentStr) && gTable4.findToken(currentStr) == -1) {
					tokenStr = tokenStr + currentStr;
					currentChar++;
					if(currentChar < lineString.length())
						currentStr = lineString.at(currentChar);
					else currentStr = "\n";
				}
				if (currentStr == "'") {
					if (temp == "X") {
						tokenTV.tokenType = 6;
						tokenTV.tokenValue = gTable6.insertHashTable(tokenStr);
					}
					else {
						tokenTV.tokenType = 7;
						tokenTV.tokenValue = gTable7.insertHashTable(tokenStr);
					}
				}
				else{
					tokenTV = tokenAnalysis(tokenStr);
				}
				line.line.push_back(tokenTV);
				if (!whiteSpace(currentStr) && currentStr != ".") {
					tokenTV = tokenAnalysis(currentStr);
					line.line.push_back(tokenTV);
					
				}
				else if (currentStr == ".") {
					tokenStr.clear();
					break;
				}
				tokenStr.clear();
			}
			else if (currentStr == ".") {
				if (tokenStr != "") {
					tokenTV = tokenAnalysis(tokenStr);
					line.line.push_back(tokenTV);
				}
				/*tokenTV = tokenAnalysis(currentStr);
				line.line.push_back(tokenTV);*/
				tokenStr.clear();
				break;
			}
			else if (gTable4.findToken(currentStr) != -1 || whiteSpace(currentStr)) {
				if (tokenStr != "") {
					tokenTV = tokenAnalysis(tokenStr);
					line.line.push_back(tokenTV);
				}
				if (gTable4.findToken(currentStr) != -1) {
					tokenTV = tokenAnalysis(currentStr);
					line.line.push_back(tokenTV);
				}
				
				tokenStr.clear();
			}
			else if (currentChar == lineString.length() - 1) {
				tokenStr = tokenStr + currentStr;
				tokenTV = tokenAnalysis(tokenStr);
				line.line.push_back(tokenTV);
				tokenStr.clear();
			}
			else if (currentStr != " ")
				tokenStr = tokenStr + currentStr;
		}
		gLexicalCode.push_back(line);
		line.line.clear();

	}
	return gLexicalCode;
}

bool GetToken::whiteSpace(string str) {
	if (str == "\t" || str == "\n" || str == " ")
		return true;
	else return false;
}

Token GetToken::tokenAnalysis(string tokenStr) {
	Token tokenTV ;
	if (tokenStr != "") {
		if (gTable1.findToken(toLower(tokenStr)) != -1) {
			tokenTV.tokenType = 1;
			tokenTV.tokenValue = gTable1.findToken(toLower(tokenStr));
		}
		else if (gTable2.findToken(toBigger(tokenStr)) != -1) {
			tokenTV.tokenType = 2;
			tokenTV.tokenValue = gTable2.findToken(toBigger(tokenStr));
		}
		else if (gTable3.findToken(toBigger(tokenStr)) != -1) {
			tokenTV.tokenType = 3;
			tokenTV.tokenValue = gTable3.findToken(toBigger(tokenStr));
		}
		else if (gTable4.findToken(tokenStr) != -1) {
			tokenTV.tokenType = 4;
			tokenTV.tokenValue = gTable4.findToken(tokenStr);
		}
		else if (isInt(tokenStr)) {
			tokenTV.tokenType = 6;
			tokenTV.tokenValue = gTable6.insertHashTable(tokenStr);
		}
		else {
			tokenTV.tokenType = 5;
			tokenTV.tokenValue = gTable5.insertHashTable(tokenStr);
		}	
	}
	return tokenTV;

}

string GetToken::toLower(string token) {
	transform(token.begin(), token.end(), token.begin(), ::tolower);
	return token;
}

string GetToken::toBigger(string token) {
	transform(token.begin(), token.end(), token.begin(), ::toupper);
	return token;
}

bool GetToken::isInt(string token) {
	bool isInt = true;
	int i;
	for (i = 0; i < token.length(); i++) {
		if (token.at(i) < '0' || token.at(i) > '9') {
			isInt = false;
			break;
		}
	}
		
	return isInt;
}

void GetToken::outputCode() {
	int currentLine, currentToken;
	for (currentLine = 0; currentLine < gSourceCode.size(); currentLine++) {
		cout << gSourceCode[currentLine] << endl;
		for (currentToken = 0; currentToken < gLexicalCode[currentLine].line.size(); currentToken++) {
			cout << "(" << gLexicalCode[currentLine].line[currentToken].tokenType << ",";
			cout << gLexicalCode[currentLine].line[currentToken].tokenValue << ")";
		} 
		cout << endl;
	}
	cout << "=============================" << endl;
}

AllTable GetToken::getTable() {
	AllTable allTable;
	allTable.table1 = gTable1;
	allTable.table2 = gTable2; 
	allTable.table3 = gTable3;
	allTable.table4 = gTable4;
	allTable.table5 = gTable5;
	allTable.table6 = gTable6;
	allTable.table7 = gTable7;
	return allTable;
}

#endif
