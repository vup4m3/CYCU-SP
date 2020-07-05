#include "pch.h"

#ifndef CONFTABLE_H
#define CONFTABLE_H

using namespace std;

struct Objectcode {
	long long objectValue;
};

struct UndefineAddress {
	Objectcode* objectCode;
	int pc;
	int type; //  <format 3> = 1, <format4> = 2, <BASE> = 3, <BASE> = use Base
	UndefineAddress* next;
};

struct ConfList {
	int symbolType;
	int symbolValue;
	int address;
	UndefineAddress* undefineAddress;
};

struct BaseList {
	int address;
	UndefineAddress* undefineAddress;
};

class ConfTable {
public:
	int getSymbolAdress(int symbolValue);
	bool insertSymbol(int symbolValue, int address, int index); // symbol insert to table
	void link(int symbolValue, Objectcode* objectCode, int type); // type = 1 this symbol is undefine, type = 2 this symbol not even in the table (oprend)
	void coverAdress(int symbolValue, int address, int index);
	bool insertSymbolXE(int symbolValue, int address, int base);
	void confProcess(int symbolValue, int pc, int base, Objectcode* objectCode, int type);
	
private:
	vector<ConfList> cConfTable;
	UndefineAddress* cBaseListHead = NULL,* cBaseListTail = NULL;
	// ==========================================================================================
	void configuration(int address, UndefineAddress* undefineAdress);
	UndefineAddress* patch(UndefineAddress* node, int address, int base);
	UndefineAddress* baseProcess(UndefineAddress* node, int address);
	UndefineAddress* linkXE(UndefineAddress* node, int pc, Objectcode* objectCode, int type);
};

int ConfTable::getSymbolAdress( int symbolValue) { //  list exist return -1; list unexist return -2 
	for (int i = 0; i < cConfTable.size(); i++) {
		if (cConfTable[i].symbolValue == symbolValue) {
			return cConfTable[i].address;
		}	
	}

	return -2; // list unexist 
}

bool ConfTable::insertSymbol(int symbolValue, int address, int index) {
	ConfList input;
	input.symbolValue = symbolValue;
	input.address = address;
	input.undefineAddress = NULL;
	if (cConfTable.size() == index) {
		cConfTable.push_back(input);
		return true;
	}	
	else if (cConfTable[index].symbolValue > symbolValue) { // symbol isn't in the table
		cConfTable.insert(cConfTable.begin() + index, input);
		return true;
	}
	else if (cConfTable[index].symbolValue == symbolValue) {
		if (cConfTable[index].address != -1) { // multiple defination
			return false;
		}
		else { // symbol is already in table but without address
			cConfTable[index].address = address; // give address
			configuration(address, cConfTable[index].undefineAddress);
			return true;
		}
	}
	else return insertSymbol(symbolValue, address, index + 1);
}

void ConfTable::configuration(int address, UndefineAddress* undefineAddress) {
	if (undefineAddress != NULL) {
		undefineAddress->objectCode->objectValue += address;
		configuration(address, undefineAddress->next);
	}
}

void ConfTable::link(int symbolValue, Objectcode* objectCode, int type) { // type = 1: this symble is undefine, and in the table. type = 2: this symble not even in the table
	int index;
	UndefineAddress* newPt = new UndefineAddress;
	UndefineAddress* currentPtr;
	newPt->objectCode = objectCode;
	newPt->next = NULL;

	if (type == 1) {
		for (index = 0; index < cConfTable.size(); index++) {
			if (cConfTable[index].symbolValue == symbolValue)
				break;
		}
		for (currentPtr = cConfTable[index].undefineAddress; currentPtr->next != NULL; currentPtr = currentPtr->next)
			;
		currentPtr->next = newPt;

	}
	else {
		ConfList newList;
		newList.symbolValue = symbolValue;
		newList.address = -1;
		newList.undefineAddress = newPt;
		for (index = 0; index < cConfTable.size(); index++) {
			if (cConfTable[index].symbolValue > symbolValue)
				break;
		}
		cConfTable.insert(cConfTable.begin() + index, newList);
	}
}

void ConfTable::coverAdress(int symbolValue, int address, int index) {
	if (index < cConfTable.size()) {
		if (cConfTable[index].symbolValue == symbolValue)
			cConfTable[index].address = address;
		else 
			coverAdress(symbolValue, address, index + 1);
	}
}

bool ConfTable::insertSymbolXE(int symbolValue, int address, int base) {
	ConfList input;
	int index;
	for (index = 0; index < cConfTable.size(); index++) {
		if (cConfTable[index].symbolValue == symbolValue || cConfTable[index].symbolValue > symbolValue) {
			break;
		}
	}

	if (index > cConfTable.size() || cConfTable[index].symbolValue > symbolValue) {
		input.address = address;
		input.symbolValue = symbolValue;
		input.undefineAddress = NULL;
		if (index > cConfTable.size()) 
			cConfTable.push_back(input);
		else 
			cConfTable.insert(cConfTable.begin() + index, input);

		return true;
	}
	else if (cConfTable[index].address == -1) {
		cConfTable[index].address = address;
		cConfTable[index].undefineAddress = patch(cConfTable[index].undefineAddress, address, base);

		return true;
	}	
	else 
		return false;
	

}

void ConfTable::confProcess(int symbolValue, int pc, int base, Objectcode* objectCode, int type) { // //  <format 3> = 1, <format4> = 2, <BASE> = 3, <BASE> = use Base
	int index, disp, address;
	for (index = 0; index < cConfTable.size(); index++) 
		if (cConfTable[index].symbolValue == symbolValue || cConfTable[index].symbolValue > symbolValue) 
			break;
	
	if (index == cConfTable.size() || cConfTable[index].symbolValue > symbolValue) {
		ConfList newSymbol;
		newSymbol.address = -1;
		newSymbol.symbolValue = symbolValue;
		newSymbol.undefineAddress = NULL;
		newSymbol.undefineAddress = linkXE(newSymbol.undefineAddress, pc, objectCode, type);
		if (index == cConfTable.size()) {
			cConfTable.push_back(newSymbol);
		}
		else if (cConfTable[index].symbolValue > symbolValue) {
			cConfTable.insert(cConfTable.begin(), newSymbol);
		}
	}
	else if (index< cConfTable.size() && cConfTable[index].address != -1) {
		address = cConfTable[index].address;
		if (type == 1) {
			disp = address - pc;
			if (disp >= -2048 && disp <= 2047) {
				disp &= 0xfff;
				objectCode->objectValue += 0x2000; // p = 1
				objectCode->objectValue += disp;

			}
			else { // base relative
				objectCode->objectValue += 0x4000; // b = 1
				if (base != -1) { // base already got address
					disp = address - base;
					disp &= 0xfff;
					objectCode->objectValue += disp;

				}
				else {
					objectCode->objectValue += address;
					if (cBaseListTail == NULL)
						cBaseListTail = linkXE(cBaseListTail, pc, objectCode, type);
					else cBaseListTail->next = linkXE(cBaseListTail->next, pc, objectCode, type);

				}
			}
		}
		else {
			objectCode->objectValue += address;

		}
	}
	else {
		cConfTable[index].undefineAddress = linkXE(cConfTable[index].undefineAddress, pc, objectCode, type);
		objectCode->objectValue = 0;
	}
}

UndefineAddress* ConfTable::patch(UndefineAddress* node, int address, int base) { //  <format 3> = 1, <format4> = 2, <BASE> = 3
	int disp;
	if (node == NULL) {
		return node;
	}
	//  <format 3> = 1, <format4> = 2, <BASE> = 3, <BASE> = use Base
	else if (node->type == 1) {
		disp = address - node->pc;
		if (disp >= -2048 && disp <= 2047) {
			node->objectCode->objectValue += 0x2000;
			node->objectCode->objectValue += disp;
		}
		else { // base relative
			node->objectCode->objectValue += 0x4000;
			if (base != -1) { // base already got address
				disp = address - base;
				node->objectCode->objectValue += 0x4000;
			}
			else {
				node->objectCode->objectValue += address;
				cBaseListTail = node;
			}
		}

		 return node->next = patch(node->next, address, base);
	}
	else if (node->type == 2) { // format4
		node->objectCode->objectValue += address;
		return node->next = patch(node->next, address, base);
	}
	else { // base
		cBaseListTail = baseProcess(cBaseListTail, address);
		return node->next = patch(node->next, address, base);
	}
}

UndefineAddress* ConfTable::baseProcess(UndefineAddress* node, int address) {
	if (node == NULL) 
		return node;

	else {
		node->objectCode->objectValue -= address;
		return node->next = baseProcess(node->next, address);
	}
}

UndefineAddress* ConfTable::linkXE(UndefineAddress* node, int pc, Objectcode* objectCode, int type) {
	if (node == NULL) {
		UndefineAddress* newPtr = new UndefineAddress;
		newPtr->objectCode = objectCode;
		newPtr->pc = pc;
		newPtr->type = type;
		newPtr->next = NULL;

		return newPtr;
	}
	else return node->next = linkXE(node->next, pc, objectCode, type);
}

#endif


