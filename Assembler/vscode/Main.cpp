#include "GetToken.h"
#include "SICAssembler.h"

int main()
{
	string fileName, input;
	do {
		Table table1, table2, table3, table4;
		Table table5, table6, table7;
		bool tableCheck = false;
		if (table1.inputTable("Table1.table"))
			if (table2.inputTable("Table2.table"))
				if (table3.inputTable("Table3.table"))
					if (table4.inputTable("Table4.table"))
						tableCheck = true;
					else cout << "Can't found Table4.table" << endl;
				else cout << "Can't found Table3.table" << endl;
			else cout << "Can't found Table2.table" << endl;
		else cout << "Can't found Table1.table" << endl;

		if (tableCheck) {
			GetToken token(table1, table2, table3, table4);
			cout << "Please input file name (with out .txt)>>";
			cin >> fileName;
			if (token.inputCode(fileName)) {
				token.lexicalAnalysis();
				token.getTable(table5, table6, table7);
				cout << "***********done***********" << endl;
				token.outputCode(fileName);
				
			}
			else cout << "Error:not found" << fileName << "!!" << endl;

		}
		cout << "input[0]quit>>";
		cin >> input;
	} while (input != "0");
	return 0;
}