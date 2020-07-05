#include "GetToken.h"

#ifndef SICASSEMBLER_H
#define SICASSEMBLER_H

using namespace std;

class SicAssembler {
public:
	SicAssembler(vector<Line> lexicalCode); // initial
	
private:
	int sB, sP; // Base Register & Promgram Counter
	vector<Line> sLexicalCode; // code from GetToken
	int getOpcode(Token token); // get opcode
};

SicAssembler::SicAssembler(vector<Line> lexicalCode) {
	sB = 0;
	sP = 0;
	sLexicalCode = lexicalCode;
}

int SicAssembler::getOpcode(Token token) {
	if (token.tokenType == 1) return 0X18; else if (token.tokenType == 2) return 0X58; else if (token.tokenType == 3) return 0X90;
	else if (token.tokenType == 4) return 0X40; else if (token.tokenType == 5) return 0XB4; else if (token.tokenType == 6) return 0X28;
	else if (token.tokenType == 7) return 0X88; else if (token.tokenType == 8) return 0XA0; else if (token.tokenType == 9) return 0X24;
	else if (token.tokenType == 10) return 0X64; else if (token.tokenType == 11) return 0X9C; else if (token.tokenType == 12) return 0XC4;
	else if (token.tokenType == 13) return 0XC0; else if (token.tokenType == 14) return 0XF4; else if (token.tokenType == 15) return 0X3C;
	else if (token.tokenType == 16) return 0X30; else if (token.tokenType == 17) return 0X34; else if (token.tokenType == 18) return 0X38;
	else if (token.tokenType == 19) return 0X48; else if (token.tokenType == 20) return 0X00; else if (token.tokenType == 21) return 0X68;
	else if (token.tokenType == 22) return 0X50; else if (token.tokenType == 23) return 0X70; else if (token.tokenType == 24) return 0X08;
	else if (token.tokenType == 25) return 0X6C; else if (token.tokenType == 26) return 0X74; else if (token.tokenType == 27) return 0X04;
	else if (token.tokenType == 28) return 0XD0; else if (token.tokenType == 29) return 0X20; else if (token.tokenType == 30) return 0X60;
	else if (token.tokenType == 31) return 0X98; else if (token.tokenType == 32) return 0XC8; else if (token.tokenType == 33) return 0X44;
	else if (token.tokenType == 34) return 0XD8; else if (token.tokenType == 35) return 0XAC; else if (token.tokenType == 36) return 0X4C;
	else if (token.tokenType == 37) return 0XA4; else if (token.tokenType == 38) return 0XA8; else if (token.tokenType == 39) return 0XF0;
	else if (token.tokenType == 40) return 0XEC; else if (token.tokenType == 41) return 0X0C; else if (token.tokenType == 42) return 0X78;
	else if (token.tokenType == 43) return 0X54; else if (token.tokenType == 44) return 0X80; else if (token.tokenType == 45) return 0XD4;
	else if (token.tokenType == 46) return 0X14; else if (token.tokenType == 47) return 0X7C; else if (token.tokenType == 48) return 0XE8;
	else if (token.tokenType == 49) return 0X84; else if (token.tokenType == 50) return 0X10; else if (token.tokenType == 51) return 0X1C;
	else if (token.tokenType == 52) return 0X5C; else if (token.tokenType == 53) return 0X94; else if (token.tokenType == 54) return 0XB0;
	else if (token.tokenType == 55) return 0XE0; else if (token.tokenType == 56) return 0XF8; else if (token.tokenType == 57) return 0X2C;
	else if (token.tokenType == 58) return 0XB8; else if (token.tokenType == 59) return 0XDC;
}

#endif
