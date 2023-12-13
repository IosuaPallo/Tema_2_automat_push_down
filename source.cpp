#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <sstream>
#include <vector>
#include <time.h>
#include <stack>
using namespace std;

class Tuple {
public: string first;
		string second;
		Tuple(string first, string second) {
			this->first = first;
			this->second = second; 
		}
};

void citire(vector<Tuple>& grammar, vector<string>& nonTerminals, vector<string>& terminals, string& startSymbol, map<string, vector<int>>& jumpTable, map<string, vector<string>>& actionTable, string& startString);
void citire3(vector<Tuple>& grammar, vector<string>& nonTerminals, vector<string>& terminals, string& startSymbol, string& startString);


string createStringFromNumber(int number);

string remakeString(string result, int position);
bool egal(vector<Tuple> el1, vector<Tuple>  el2);
bool contains(vector<string> nonTerminals, string symbol);
string urmator(string nonTerminal, vector<Tuple> grammar);
vector<string> elementSalt(vector<Tuple>i);
vector<Tuple> genInc(vector<Tuple>i, vector<string> nonTerminals, vector<Tuple> grammar);
vector<Tuple> genSalt(vector<Tuple>i, string x, vector<string> nonTerminals, vector<Tuple>grammar);
string completeazaReducerea(int nrProd);


string findOperatorInProduction(string production, vector<string>operators);
int verifyString(vector<Tuple>& grammar, vector<string>& nonTerminals, vector<string>& terminals, string& startSymbol, map<string, vector<int>>& jumpTable, map<string, vector<string>>& actionTable, string& startString, vector<vector<string>>& codIntermediar, vector<string>& historyStack);
void genColectieSiRelatii(vector<string> nonTerminals, vector<Tuple> grammar, vector<vector<Tuple>>& colectie, vector<vector<string>>& relatii);

void initializareTabele(map<string, vector<string>>& actionTable, map<string, vector<int>>& jumpTable, vector<string> nonTerminals, vector<string>terminals, int colectieLength);
void generareTabele(vector<string> nonTerminals, vector<string>terminals, vector<Tuple> grammar, vector<vector<Tuple>> colectie, vector<vector<string>> relatii, map<string, vector<string>>& actionTable, map<string, vector<int>>& jumpTable);

void afisareTabele(map<string, vector<string>> actionTable, map<string, vector<int>> jumpTable);

ofstream fout("date.out");

int main() {
	srand(time(0));
	vector<string> nonTerminals, terminals;
	vector<Tuple> grammar;	
	string startSymbol, startString;
	map<string, vector<string>> actionTable;
	map<string, vector<int>> jumpTable;
	vector<vector<Tuple>> colectie;
	vector<vector<string>> relatii;
	vector<vector<string>> codIntermediar; 
	vector<string> historyStack; 


	citire3(grammar, nonTerminals, terminals, startSymbol, startString);
	genColectieSiRelatii(nonTerminals, grammar, colectie, relatii); 
	generareTabele(nonTerminals, terminals, grammar, colectie, relatii, actionTable, jumpTable);
	afisareTabele(actionTable, jumpTable); 


	//citire(grammar, nonTerminals, terminals, startSymbol, jumpTable, actionTable, startString);
	int sem = verifyString(grammar, nonTerminals, terminals, startSymbol, jumpTable, actionTable, startString, codIntermediar, historyStack);

	string result;	
	sem == 1 ? result = "Sirul este corect\n" : result = "Sirul NU este corect\n";
	fout <<"\n" << result;

	fout << "\nCod Intermediar\n";

	for (int i = 0; i < codIntermediar.size();i++) {
		fout << "t" << i+1 << " = ";
		for (string term : codIntermediar[i]) {
			fout << term << " "; 
		}
		fout << "\n"; 
	}
	/*fout << "\n"; 

	for (string historyString : historyStack) {
		fout << historyString << "\n";
	}*/

	fout.close(); 
}

void generareTabele(vector<string> nonTerminals, vector<string>terminals, vector<Tuple> grammar, vector<vector<Tuple>> colectie, vector<vector<string>> relatii, map<string, vector<string>> &actionTable, map<string, vector<int>> &jumpTable) {

	initializareTabele(actionTable, jumpTable, nonTerminals, terminals, colectie.size()); 


	for (vector<string> relatie : relatii)
	{
		if (contains(nonTerminals, relatie[1]) == 1) {
			jumpTable[relatie[1]].at(stoi(relatie[0])) = stoi(relatie[2]);
		}
		else {
			actionTable[relatie[1]].at(stoi(relatie[0])) = "d" + relatie[2];
		}
	}

	for (int i = 0; i < colectie.size(); i++) {
		vector<Tuple> col = colectie[i];
		for (Tuple prod : col) {
			int poz = prod.second.find('.');
			
			if (poz + 1 == prod.second.size()) {
				if (prod.first.compare("S") == 0) {
					actionTable["$"].at(i) = "acc"; 
				}
				else {
					string urm = urmator(prod.first, grammar);
					prod.second = prod.second.substr(0, poz) + prod.second.substr(poz + 1);


					int nrProd = 0;

					for (int j = 0; j < grammar.size(); j++) {
						if (grammar[j].second.compare(prod.second) == 0 && grammar[j].first.compare(prod.first) == 0) {
							nrProd = j;
						}
					}

					for (char c : urm) {
						string elem = "";
						elem += c; 
						string reducere = completeazaReducerea(nrProd+1); 
						actionTable[elem].at(i) = reducere; 
					}
				}
			}
		}
	}
}

string completeazaReducerea(int nrProd) {
	string reducere; 
	while (nrProd > 0) {
		reducere += nrProd%10 + '0';
		nrProd /= 10;
	}
	reducere += "r";
	reverse(reducere.begin(), reducere.end()); 
	return reducere;
}

void initializareTabele(map<string, vector<string>>& actionTable, map<string, vector<int>>& jumpTable, vector<string> nonTerminals, vector<string>terminals, int colectieLength) {
	
	for (string terminal : terminals) {
		vector<string> actionTableLinie; 
		for (int i = 0; i < colectieLength; i++) {
			actionTableLinie.push_back("-1");
		}
		actionTable[terminal] = actionTableLinie; 
	}

	for (string nonTerminal : nonTerminals) {
		vector<int>jumpTableLinie; 
		for (int i = 0; i < colectieLength; i++) {
			jumpTableLinie.push_back(-1); 
		}
		jumpTable[nonTerminal] = jumpTableLinie;
	}
}

void afisareTabele(map<string, vector<string>> actionTable, map<string, vector<int>> jumpTable) {
	fout << "Jump Table : \n";
	for (pair<string, vector<int>> symbol : jumpTable) {
		fout << symbol.first << " ";
		for (int i = 0; i < symbol.second.size() - 1; i++) {
			fout << symbol.second[i] << " ";
		}
		fout << symbol.second[symbol.second.size() - 1] << "\n";
	}

	fout << "\nAction Table : \n";

	for (pair<string, vector<string>> symbol: actionTable) {
		fout << symbol.first << " "; 
		for (int i = 0; i < symbol.second.size()-1; i++) {
			fout << symbol.second[i] << " ";
		}
		fout << symbol.second[symbol.second.size() - 1] << "\n";
	}
	fout << "\n";
}


string urmator(string nonTerminal, vector<Tuple> grammar) {
	string symbols = "", finalSymbols = "";

	for (Tuple prod : grammar) {
		if (prod.second.size() == 1 && prod.second.compare(nonTerminal) == 0) {
			symbols += urmator(prod.first, grammar);
		}

		int poz = prod.second.find(nonTerminal);
		if (poz != string::npos && poz + 1 < prod.second.size()) {
			symbols += prod.second.substr(poz + 1, 1); 
		}
	}

	symbols += "$";
	for (char c : symbols) {
		if (finalSymbols.find(c) == string::npos) {
			finalSymbols += c; 
		}
	}

	return finalSymbols;
}


bool contains(vector<string> nonTerminals, string symbol) {
	for (string nonTerminal : nonTerminals)
	{
		if (nonTerminal.compare(symbol) == 0) {
			return true; 
		}
	}
	return false; 
}

void genColectieSiRelatii(vector<string> nonTerminals, vector<Tuple> grammar, vector<vector<Tuple>>& colectie, vector<vector<string>> &relatii ) {

	vector<Tuple>start, inchidere;

	start.push_back(Tuple("S", ".E"));

	inchidere = genInc(start, nonTerminals, grammar);

	colectie.push_back(inchidere); 
	
	int n = 1; 

	for (int j = 0; j < n; j++) {
		vector<Tuple> elementLinie = colectie[j]; 
		vector<string> elemSalt = elementSalt(elementLinie);
		vector<Tuple> m;


		for (string element : elemSalt) {

			m = genSalt(elementLinie, element, nonTerminals, grammar);
			bool sem = 1;

			for (int i = 0; i < colectie.size() - 1; i++) {
				if (egal(colectie[i], m)) {
					sem = 0;
					vector<string>relatie;
					relatie.push_back(to_string(j));
					relatie.push_back(element);
					relatie.push_back(to_string(i));
					relatii.push_back(relatie);
				}
			}

			if (sem == 1) {
				colectie.push_back(m);
				vector<string>relatie;
				relatie.push_back(to_string(j));
				relatie.push_back(element);
				relatie.push_back(to_string(colectie.size() - 1));
				relatii.push_back(relatie);
				n++; 
			}
		}
	}
}

bool egal(vector<Tuple> el1, vector<Tuple>  el2) {
	if (el1.size() != el2.size()) {
		return false;
	}
	for (int i = 0; i < el1.size(); i++) {
		if ((el1[i].first != el2[i].first) || (el1[i].second != el2[i].second)) {
			return false;
		}
	}
	return true; 
}


vector<string> elementSalt(vector<Tuple>inchidere) {
	vector<string> elemSalt; 

	for (Tuple alfa : inchidere) {
		int pozPunct = alfa.second.find('.');
		if (pozPunct + 1 < alfa.second.size()) {
			int pozAddString = string::npos;
			string addString = alfa.second.substr(pozPunct + 1, 1); 
			for (int i = 0; i < elemSalt.size() && pozAddString == string::npos; i++) {
				if (elemSalt[i].compare(addString) == 0) {
					pozAddString = i;
				}
			}
			if (pozAddString == string::npos) {
				elemSalt.push_back(addString);
			}
		}
	}

	return elemSalt; 
}



vector<Tuple> genInc(vector<Tuple>inchidere, vector<string> nonTerminals, vector<Tuple> grammar ) {

	vector<Tuple> m, m0;

	for (Tuple prod : inchidere) {
		m.push_back(prod); 
		m0.push_back(prod);
	}

	while (m0.size() > 0) {
		for (Tuple prod : m0) {
			int pozPunct = prod.second.find('.');
			string elDupaPunct = "ex";
			if (pozPunct != prod.second.size()) {
				elDupaPunct = prod.second.substr(pozPunct + 1, 1);
			}

			int pozNeterminal = string::npos;
			for (int i = 0; i < nonTerminals.size() && pozNeterminal == string::npos; i++) {
				if (nonTerminals[i].compare(elDupaPunct) == 0) {
					pozNeterminal = i;
				}
			}

			if (pozNeterminal != string::npos) {
				nonTerminals.erase(nonTerminals.begin() + pozNeterminal);
				for (Tuple productie : grammar) {
					if (productie.first.compare(elDupaPunct) == 0) {
						m.push_back(Tuple(productie.first, "." + productie.second));
						m0.push_back(Tuple(productie.first, "." + productie.second));
					}
				}
			}

			for (int i = 0; i < m0.size(); i++) {
				if (m0[i].first == prod.first && m0[i].second == m0[i].second) {
					m0.erase(m0.begin() + i);
					break;
				}
			}
		}
	}
	return m;
}

vector<Tuple> genSalt(vector<Tuple>inchidere, string symbol, vector<string> nonTerminals, vector<Tuple>grammar) {
	vector<Tuple> m; 
	for (Tuple alfa : inchidere) {
		int pozPunct = alfa.second.find('.'); 

		if (pozPunct + 1 < alfa.second.size()) {
			if (alfa.second.substr(pozPunct + 1, 1).compare(symbol) == 0) {
				string second = remakeString(alfa.second, pozPunct); 
				m.push_back(Tuple(alfa.first, second));
			}
		}
	}
	m = genInc(m,nonTerminals,grammar); 
	return m;
}

string remakeString(string result, int position) {
	return result.substr(0, position) + result.substr(position + 1, 1) + "." + result.substr(position + 2);
}

string createStringFromNumber(int number) {
	string term;
	while (number != 0) {
		term += number % 10 + '0';
		number /= 10;
	}
	reverse(term.begin(), term.end());
	return term;
}



void actiuneGenerare(int actiune, stack<string> &stivaAtribuire,  vector<vector<string>>& codIntermediar,  string terminal) {
	if (actiune == 0) {
		stivaAtribuire.push(terminal); 
	}
	else if (actiune == 1) {
		string last = stivaAtribuire.top();
		stivaAtribuire.pop(); 
		string almostLast = stivaAtribuire.top();
		stivaAtribuire.pop();
		codIntermediar.push_back({ almostLast,terminal,last});
		string term;
		term += "t" + createStringFromNumber(codIntermediar.size()); 
		stivaAtribuire.push(term); 
	}
}



int verifyString(vector<Tuple>& grammar, vector<string>& nonTerminals, vector<string>& terminals, string& startSymbol, map<string, vector<int>>& jumpTable, map<string, vector<string>>& actionTable, string& startString, vector<vector<string>>&codIntermediar, vector<string>&historyStack) {
	int sem = 0;
	stack<string> stiva;
	stack<string> stivaAtribuire; 
	stiva.push("$");
	stiva.push("0");
	int index = 0; 
	while (sem == 0) {
		stack<string>stiva2 = stiva;
		vector<string> vectorStiva; 
		while (!stiva2.empty()) {
			vectorStiva.insert(vectorStiva.begin(), stiva2.top());
			stiva2.pop();
		}

		for (string term : vectorStiva) {
			fout << term << " ";
		}
		fout << "\n";

		vector<int> position(terminals.size(), -1);
		int min = startString.size() + 1;
		string key = "";


		int stare;
		stare = stoi(stiva.top());

		for (int k = 0; k < terminals.size(); k++) {
			position[k] = startString.find(terminals[k]);
			if (position[k] < min && position[k] != -1) {
				min = position[k];
				key = terminals[k];
			}
		}

		if (min != 0) {
			sem = -1;
		}
		else {
			vector<string> actionTableStates = actionTable[key];
			string action = actionTableStates[stare];
			fout << "Action = " << action << "\n";
			if (action.compare("-1") != 0) {
				if (action == "acc") {
					sem = 1;
				}
				else {
					int stareUrmatoare = stoi(action.substr(1));
					if (action.find("d") != string::npos) {
						stiva.push(key);
						stiva.push(to_string(stareUrmatoare));
						startString = startString.substr(key.size());
					}
					else if (action.find("r") != string::npos) {

						string second = grammar[stareUrmatoare - 1].second;

						vector<int> nonTerminalPosition(nonTerminals.size(), -1);

						vector<string> grammarOperators = { "+","*"};

						int min = second.size() + 1;
						string reductionKey = "";
						for (int k = 0; k < nonTerminals.size(); k++) {
							position[k] = second.find(nonTerminals[k]);
							if (position[k] < min && position[k] != -1) {
								min = position[k];
								reductionKey = nonTerminals[k];
							}
						}
						for (int k = 0; k < terminals.size(); k++) {
							position[k] = second.find(terminals[k]);
							if (position[k] < min && position[k] != -1) {
								min = position[k];
								reductionKey = terminals[k];
							}
						}

						while (stiva.top() != reductionKey) {
							stiva.pop();
						}
						stiva.pop();

						int stareInitiala = stoi(stiva.top());
						string first = grammar[stareUrmatoare - 1].first;
						stiva.push(first);
						vector<int>jumpTableStates = jumpTable[first];
						int jumpTableState = jumpTableStates[stareInitiala];
						if (jumpTableState == -1) {
							sem = -1;
							continue;
						}
						stiva.push(to_string(jumpTableState));

						string terminal = findOperatorInProduction(second, grammarOperators);
						if (terminal == "") {
							if (reductionKey == "a") {
								index++; 
								terminal = reductionKey + createStringFromNumber(index);
								actiuneGenerare(0, stivaAtribuire, codIntermediar,terminal);
							}
						}
						else {
							actiuneGenerare(1, stivaAtribuire, codIntermediar,terminal);
						}

					}
				}
			}
			else {
				sem = -1;
			}
			stack<string> stivaDuplicat = stivaAtribuire;
			vector<string> vectorTerms;
			string history;
			while (!stivaDuplicat.empty()) {
				vectorTerms.insert(vectorTerms.begin(), stivaDuplicat.top());
				stivaDuplicat.pop();
			}
			for (string term : vectorTerms) {
				history += term + " ";
			}
			historyStack.push_back(history);
		}
	}
	return sem; 
}

string findOperatorInProduction(string production, vector<string>operators) {
	for (int i = 0; i < operators.size(); i++) {
		if (production.find(operators[i]) != string::npos) {
			return operators[i]; 
		}
	}
	return ""; 
}

void citire(vector<Tuple>& grammar, vector<string>& nonTerminals, vector<string>& terminals, string& startSymbol, map<string, vector<int>>& jumpTable, map<string,vector<string>> &actionTable ,string &startString) {
	ifstream fin("date.in");
	string line;
	getline(fin, line);
	istringstream stream(line);
	while (getline(stream, line, ' ')) {
		nonTerminals.push_back(line);
	}
	getline(fin, line);
	stream.clear(); 
	stream.str(line);
	while (getline(stream, line, ' ')) {
		terminals.push_back(line);
	}
	bool sem = 0;
	string key;
	while (getline(fin, line) && sem == 0) {
		string value; 
		if (line.find("=") == string::npos) {
			startSymbol.append(line);
			sem = 1;
		}
		else {
			istringstream stream(line);
			getline(stream, key, '=');
			getline(stream, value);
			grammar.push_back(Tuple(key, value));
		}
	}
	startString.assign(line);
	int i = 0;
	while (i < nonTerminals.size()) {
		getline(fin, line); 
		stream.clear();
		stream.str(line);
		getline(stream, key, ' ');
		int integer;
		while (stream >> integer) {
			jumpTable[key].push_back(integer);
		}
		i++;
	}
	while (getline(fin, line)){
		string value; 
		stream.clear();
		stream.str(line);
		getline(stream, key, ' ');
		while (stream >> value) {
			actionTable[key].push_back(value);
		}
	}

	fin.close();
}

void citire3(vector<Tuple>& grammar, vector<string>& nonTerminals, vector<string>& terminals, string& startSymbol, string& startString) {
	ifstream fin("date3.in");
	string line;
	getline(fin, line);
	istringstream stream(line);
	while (getline(stream, line, ' ')) {
		nonTerminals.push_back(line);
	}
	getline(fin, line);
	stream.clear();
	stream.str(line);
	while (getline(stream, line, ' ')) {
		terminals.push_back(line);
	}
	bool sem = 0;
	string key;
	while (getline(fin, line) && sem == 0) {
		string value;
		if (line.find("=") == string::npos) {
			startSymbol.append(line);
			sem = 1;
		}
		else {
			istringstream stream(line);
			getline(stream, key, '=');
			getline(stream, value);
			grammar.push_back(Tuple(key, value));
		}
	}
	startString.assign(line);
	fin.close();
}