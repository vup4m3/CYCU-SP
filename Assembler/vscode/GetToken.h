#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>

#ifndef GETTOKEN_H
#define GETTOKEN_H

using namespace std;

class Table {
public:
	bool inputTable(string fileName);
	int findToken(string token);
	int insertHashTable(string token);

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
	} // 讀.table 將string放到tTable
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
	bool inputCode(string fileName);
	void lexicalAnalysis();
	void outputCode(string fileName);
	vector<Line>getLexicalCode();
	void getTable(Table &table5, Table &table6, Table &table7);
private:
	vector<string> gSourceCode;
	vector<Line> gLexicalCode;
	Table gTable1, gTable2, gTable3, gTable4;
	Table gTable5, gTable6, gTable7;
	bool whiteSpace(string str);
	Token tokenAnalysis(string tokenStr);
	string toLower(string token);
	string toBigger(string token);
	bool isInt(string token);
};

bool GetToken:: inputCode(string fileName) {
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

void GetToken::lexicalAnalysis() {
	int currentLine, currentChar;
	string tokenStr = "", lineString, currentStr, temp;
	Token tokenTV ;
	Line line;
	for (currentLine = 0; currentLine < gSourceCode.size(); currentLine++) {
		lineString = gSourceCode[currentLine];
		for (currentChar = 0; currentChar < lineString.length(); currentChar++) {
			currentStr = lineString.at(currentChar);
			if (currentStr == "'" &&  (tokenStr == "X" || tokenStr == "C")) {
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
				if (!whiteSpace(currentStr)) {
					tokenTV = tokenAnalysis(currentStr);
					line.line.push_back(tokenTV);
					if (currentStr == ".") {
						tokenStr.clear();
						break;
					}
				}
				tokenStr.clear();
			}
			else if (currentStr == ".") {
				if (tokenStr != "") {
					tokenTV = tokenAnalysis(tokenStr);
					line.line.push_back(tokenTV);
				}
				tokenTV = tokenAnalysis(currentStr);
				line.line.push_back(tokenTV);
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

void GetToken::outputCode(string fileName) {
	ofstream outfile;
	fileName = fileName + "_output.txt";
	outfile.open(fileName.c_str());
	int currentLine, currentToken;
	for (currentLine = 0; currentLine < gSourceCode.size(); currentLine++) {
		outfile << gSourceCode[currentLine] << endl;
		for (currentToken = 0; currentToken < gLexicalCode[currentLine].line.size(); currentToken++) {
			outfile << "(" << gLexicalCode[currentLine].line[currentToken].tokenType << ",";
			outfile << gLexicalCode[currentLine].line[currentToken].tokenValue << ")";
		} 
		outfile << endl;
	}
	outfile.close();
}

vector<Line> GetToken::getLexicalCode() {
	return gLexicalCode;
}

void GetToken::getTable(Table &table5, Table &table6, Table &table7) {
	table5 = gTable5;
	table6 = gTable6;
	table7 = gTable7;
}

#endif