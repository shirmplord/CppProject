// Cpp_Project.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include <sstream>
#define maxC 50
using namespace std;

struct Item {
	friend ostream &operator<<(ostream &out, const Item &i);
	string name;
	float price;
	int available;
	int sold;
};

// Abstract interface of Observer
class Observer {
public:
	virtual void Update(const string msg = "", const string name = "", const float price = 0.0, const int amount = 0) = 0;
};

class Subject {
public:
	virtual void Attach(Observer *o) = 0;
	virtual void Detach(Observer *o) = 0;
	virtual void Notify(const string msg = "", const string name = "", const float price = 0.0, const int amount = 0) = 0;
};

class StockManager : public Subject {
public:
	StockManager();
	~StockManager();
	//subject commands
	void ClearStockList();
	void AddItem(const Item item);
	void AddItem(const string name = "", const float price = 0.0, const int available = 0, const int sold = 0);
	void SellItem(const string name, const int amount);
	void RepStock(const string name, const int amount);
	void ReadStock();
	void UpdatePrice(const string name, float price);
	void GetReport() const;
	void SaveStock() const;
	void Attach(Observer* o);
	void Detach(Observer* o);
	void Notify(const string msg = "",const string name = "", const float price = 0.0, const int amount = 0);
private:
	vector<Observer*> observers;
	vector<Item> itemList;
	float revenue;
};

class ConsoleManager : public Observer {
public:
	ConsoleManager(StockManager *p);
	~ConsoleManager();
	void Update(const string msg = "", const string name = "", const float price = 0.0, const int amount = 0);
	void PrintMenu();
	void ExecuteCmd();
private:
	StockManager *psm;
};

/*
Save + retrieve from files
The program uses an array of records to hold the salesman’s stock of item.
The program must have at least the following commands
1. Initialize(clear stock listing) - Subject
2. Save stock listing to disk - Observer
3. Read stock listing from disk - Subject
4. Add a new item to list - Subject
5. Sell an item(decrease items for sale & increase number of sold items) - Subject
6. Replenish stock(increase items for sale) - Subject
7. Print a report which lists all items in the stock and in the end prints total revenue(total amount of
	money received from selling items) - Observer
8. Update price for items
9. Remove item from stock
*/
//---------------------------------------------------------------//
//Main of program
int main()
{
	{
		cout << "Welcome to the travelling salesman companion app!" << endl;
		Item newEntry;
		StockManager sm;
		ConsoleManager cm(&sm);
		sm.Notify();
	}
#ifdef _DEBUG
	if (_CrtDumpMemoryLeaks())
		cout << "WARNING: There was a memory leak in your program" << endl;
	else
		cout << "Your program has no memory leaks, congratulations!" << endl;
#endif
	return 0;
}
//-----------------------------------------------------------//
//Implementation of class StockManager
StockManager::StockManager() {
	itemList.clear();
	revenue = 0;
}

StockManager::~StockManager() {}

void StockManager::AddItem(const Item item) {
	bool exist = false;
	auto i = itemList.begin();
	while (i != itemList.end()) {
		if (i->name == item.name) {
			exist = true;
			break;
		}
		i++;
	}
	if (!exist) itemList.push_back(item);
}

void StockManager::AddItem(const string name, float price, int available, int sold) {
	bool exist = false;
	auto i = itemList.begin();
	while (i != itemList.end()) {
		if (i->name == name) {
			Notify("Item existed");
			unsigned short int option;
			cin >> option;
			if (option == 1) RepStock(name, available);
			else if (option == 2) break;
			exist = true;
		}
		i++;
	}
	if (!exist) {
		Item newItem;
		newItem.name = name;
		newItem.price = price;
		newItem.available = available;
		newItem.sold = sold;
		itemList.push_back(newItem);
	}
}

void StockManager::SellItem(const string name, int amount) {
	bool exist = false;
	auto i = itemList.begin();
	while (i != itemList.end()) {
		if (i->name == name) {
			exist = true;
			if (i->available == 0) {
				exist = false;
				break;
			}
			else if (amount < i->available) {
				i->available -= amount;
				i->sold += amount;
				revenue += amount * i->price;
			}
			else {
				amount = i->available;
				i->sold += i->available;
				revenue += i->price * i->available;
				i->available = 0;
			}
		}
		i++;
	}
	if (exist) Notify("Sold item", name, 0.0, amount);
	else Notify("Not in stock");
}

void StockManager::UpdatePrice(const string name, float price) {
	bool exist = false;
	auto i = itemList.begin();
	while (i != itemList.end()) {
		if (i->name == name) {
			exist = true;
			i->price = price;
		}
		i++;
	}
	if (!exist) Notify("Not in stock");
}

void StockManager::RepStock(const string name, int amount) {
	auto i = itemList.begin();
	while (i != itemList.end()) {
		if (i->name == name) {
			i->available += amount;
		}
		i++;
	}
}

void StockManager::ClearStockList() {
	itemList.clear();
	revenue = 0;
	SaveStock();
}

void StockManager::GetReport() const {
	cout << "Items available in stock:" << endl;
	cout << setiosflags(ios::left);
	cout << setfill(' ') << setw(25) << "Item Name";
	cout << setfill(' ') << setw(8) << "Price";
	cout << setfill(' ') << setw(12) << "Available";
	cout << setfill(' ') << setw(6) << "Sold" << endl;
	auto i = itemList.begin();
	while (i != itemList.end()) {
		cout << *i;
		i++;
	}
	cout << "Total revenue: " << revenue << endl << endl;
}

void StockManager::SaveStock() const {
	ofstream stock;
	stock.open("stock.txt");
	auto i = itemList.begin();
	while (i != itemList.end()) {
		stock << *i;
		i++;
	}
	stock << revenue;
}

Item ToItem(string input) {
	unsigned int i = 0;
	string temp;
	Item item;
	//initialize the item for the first time
	//reading from the file (empty file)
	item.name = "";
	item.price = 0.0;
	item.available = 0;
	item.sold = 0;
	//change string -> Item
	while (i < input.length()) {
		//get item name
		while (input[i] == ' ' && i < input.length()) i++;
		while (input[i] != ' ' && i < input.length()) {
			item.name += input[i];
			i++;
		}
		//get item price
		while (input[i] == ' ' && i < input.length()) i++;
		while (input[i] != ' ' && i < input.length()) {
			temp += input[i];
			i++;
		}
		stringstream(temp) >> item.price;
		temp = "";
		//get number of available items
		while (input[i] == ' ' && i < input.length()) i++;
		while (input[i] != ' ' && i < input.length()) {
			temp += input[i];
			i++;
		}
		stringstream(temp) >> item.available;
		temp = "";
		//get number of sold items
		while (input[i] == ' ' && i < input.length()) i++;
		while (input[i] != ' ' && i < input.length()) {
			temp += input[i];
			i++;
		}
		stringstream(temp) >> item.sold;
	}
	return item;
}

void StockManager::ReadStock() {
	char c;
	string temp, result("");
	ifstream stock("stock.txt");
	while (stock.get(c)) {
		temp = c;
		if (c != '\n') {
			result.append(temp);
		}
		else {
			AddItem(ToItem(result));
			result = "";
		}
	}
	if (result != "") stringstream(result) >> revenue;
}

void StockManager::Attach(Observer* o) {
	observers.push_back(o);
}

void StockManager::Detach(Observer* o) {
	auto i = find(observers.begin(), observers.end(), o);
	if (i != observers.end()) {
		observers.erase(i);
	}
}

void StockManager::Notify(const string msg, const string name, const float price, const int amount) {
	for (unsigned int i = 0; i < observers.size(); i++) {
		observers[i]->Update(msg,name,price,amount);
	}
}
//-------------------------------------------------------//
//Implementation of ConsoleManger
ConsoleManager::ConsoleManager(StockManager* p) {
	this->psm = p;
	psm->Attach(this);
	PrintMenu();
}

ConsoleManager::~ConsoleManager() {}

void ConsoleManager::Update(const string msg, const string name, const float price, const int amount) {
	if (msg == "Item existed") {
		cout << endl << "Item with the same name already in the list" << endl;
		cout << "Would you like to replenish stock with the said amout or add new entry? (Replenish (1)/New (2))";
	}
	else if (msg == "Not in stock") {
		cout << "Item not present in stock" << endl;
	}
	else if (msg == "Sold item") {
		cout << "Sold " << amount << " " << name << endl;
	} else ExecuteCmd(); 
}

void ConsoleManager::PrintMenu() {
	cout << endl;
	cout << "What would you like to do?: (Enter option number)" << endl;
	cout << "1.  Clear stock listing" << endl;
	cout << "2.  Save stock listing" << endl;
	cout << "3.  Read stock listing" << endl;
	cout << "4.  Add new item" << endl;
	cout << "5.  Sell item" << endl;
	cout << "6.  Replenish stock" << endl;
	cout << "7.  Update price" << endl;
	cout << "8.  Get current stock report for recent sales" << endl;
	cout << "9.  Print menu" << endl;
	cout << "10. Exit program" << endl;
}

void ConsoleManager::ExecuteCmd() {
	int input;
	bool finish = false;
	string name;
	float price;
	int amount;
	cin >> input;
	switch (input)
	{
	case 1: { //Clear stock listing
		psm->ClearStockList();
		break; 
	}
	case 2: { //Save stock listing
		psm->SaveStock();
		break;
	}
	case 3: { //Read stock listing
		psm->ReadStock();
		break;
	}
	case 4: { //Add new item
		cout << "Type the item name, price, and amount: ";
		cin >> name >> price >> amount;
		psm->AddItem(name, price, amount);
		break;
	}
	case 5: { //Sell an item
		cout << "Type the item name and amount: ";
		cin >> name >> amount;
		psm->SellItem(name, amount);
		break;
	}
	case 6: { //Replenish stock
		cout << "Type the item name and amount: ";
		cin >> name >> amount;
		psm->RepStock(name, amount);
		break;
	}
	case 7: { //Update price
		cout << "Type the item name and new price: ";
		cin >> name >> price;
		psm->UpdatePrice(name, price);
		break;
	}
	case 8: { //Print report
		psm->GetReport();
		break;
	}
	case 9: { //Print menu
		PrintMenu();
		break;
	}
	default:
		finish = true;
		break;
	}
	if (!finish) this->Update();
}

ostream &operator<<(ostream &out, const Item &i) {
	out << setfill(' ') << setw(25) << i.name;
	out << setfill(' ') << setw(8) << i.price;
	out << setfill(' ') << setw(12) << i.available;
	out << setfill(' ') << setw(6) << i.sold << endl;
	return out;
}
// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
