#include "pch.h"
#include "GetToken.h"
#include "SICAssembler.h"
#include "XEAssembler.h"

using namespace std;

void print(vector<string> sourceCode, vector<AssemblrData> machineCode, string fileName);

int main()
{
	string fileName, input;
	AllTable allTable;
	vector<Line> lexicalCode;
	vector<string> sourceCode;
	vector<AssemblrData> machineCode;
	do {
		Table table1, table2, table3, table4, table5, table6, table7;
		bool tableCheck = false;
		if (table1.inputTable("Table1.table"))
			if (table2.inputTable("Table2.table"))
				if (table3.inputTable("Table3.table"))
					if (table4.inputTable("Table4.table"))
						tableCheck = true;
					else cout << "Can't found Table4.table" << endl;
				else cout << "Can't found Table3.table" << endl;
			else cout << "Can't found tableTable2.table" << endl;
		else cout << "Can't found Table1.table" << endl;

		if (tableCheck) {
			GetToken token(table1, table2, table3, table4);
			cout << "***************************************************" << endl;
			cout << "* System Program Project2                         *" << endl;
			cout << "* 1. SIC                                          *" << endl;
			cout << "* 1. SIC/XE                                       *" << endl;
			cout << "***************************************************" << endl;
			cout << "Input file name(with out [.txt] ex. SIC_input)\n>>";
			cin >> fileName;
			if (token.inputCode(fileName, sourceCode)) {
				lexicalCode = token.lexicalAnalysis();
				allTable = token.getTable();
				cout << "***********done***********" << endl;
				token.outputCode();
				cout << "[1]sic or [2]sic/xe (Input 1 or 2) \n>>";
				cin >> input;
				if (input == "1") {
					SicAssembler sic(lexicalCode, sourceCode, allTable);
					machineCode = sic.assembler(sourceCode);
				}
				else {
					XEAssembler xeAss(lexicalCode, sourceCode, allTable);
					machineCode = xeAss.assembler(sourceCode);
				}
				
				print(sourceCode, machineCode, fileName);
				cout << "*********done**********" << endl;
			}
			else cout << "Can't found:" << fileName << "!!" << endl;

		}
		cout << "Enter[0]quit\n>>";
		cin >> input;
	} while (input != "0");
	return 0;
}

void print(vector<string> sourceCode, vector<AssemblrData> machineCode, string fileName) {
	AssemblrData currentData;
	ofstream outfile;
	fileName = fileName + "_output.txt";
	outfile.open(fileName.c_str());
	for (int k= 0; k < sourceCode.size(); k++) {
		currentData = machineCode[k];
		if (sourceCode.size() != 0) {
			if (currentData.location != -1)
				outfile << hex << uppercase << right << setw(4) << setfill('0') << currentData.location << "\t";
			else outfile << "\t";
			outfile << left << setw(25) << setfill(' ') <<  sourceCode[k] << "\t";
			
			if (currentData.codeSize != 0) 
				outfile << hex << uppercase << right << setw(currentData.codeSize) << setfill('0') << currentData.objectCode->objectValue;
			else outfile << "\t";
			outfile << currentData.errorMsg;
		}
		outfile << endl;
	}
}
