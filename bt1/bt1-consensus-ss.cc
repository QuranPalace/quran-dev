#include <iostream>
#include <fstream>
#include<string>
#include <cstdio>
#include<vector>
#include <cstring>
#include <boost/algorithm/string.hpp>
#include <map>
using namespace std;

const int maxSiblingError = 2;

map<string, int> rootHash;

struct TableCell {
	string address;
	int row;
	TableCell(string _address, int _row) : address(_address), row(_row) {}
	TableCell(string loadString="") {
		char addressCharStar[100];
		sscanf(loadString.c_str(), "(%[^)])|%d", addressCharStar, &row);
		address = addressCharStar;
	}
};

struct Table {
	int rowCnt, colCnt;
	vector<string> consensus;
	vector<int> consensusHash;
	string id;
	vector<vector<TableCell> > columns;

	bool isSuper(const vector<string>& tconsensus) const {
		Table t;
		t.consensus = tconsensus;
		return t.isSub(consensus);
	}

	bool isSibling(const vector<string>& tconsensus) const {
		if (consensus.size() != tconsensus.size()) return false;
		int i=0, j=0, err=0;
		for (; i<consensus.size()&& j<tconsensus.size(); j++, i++)
			if (consensus[i] != tconsensus[j])
				err++;
		return err <= maxSiblingError;
	}

	bool isSub(const vector<string>& tconsensus) const {
		int i=0, j=0;
		for (; i<consensus.size()&& j<tconsensus.size(); j++)
			if (consensus[i] == tconsensus[j])
				i++;
		return i == consensus.size();
	}

	void init() {
	}
};

istream& operator>>(istream& os, Table& t) {
	os >> t.colCnt >> t.rowCnt;
	t.consensus.clear();
	for (int i=0; i<t.rowCnt; i++) {
		string s;
		os >> s;
		t.consensus.push_back(s);
	}
	os >> t.id;
	char line[1000];
	os.getline(line, sizeof line);
	t.columns.clear();
	for (int i=0; i<t.colCnt; i++) {
		os.getline(line, sizeof line);
		std::vector<std::string> strs;
		boost::split(strs, line, boost::is_any_of(" "));
		t.columns.push_back(vector<TableCell>());
		for (int j=0; j<strs.size(); j++)
			t.columns.back().push_back(TableCell(strs[j]));
	}
	t.init();
	return os;
}

ostream& operator<<(ostream& os, const TableCell& c) {
	return os << "(" << c.address << ")|" << c.row;
}

ostream& operator<<(ostream& os, const Table& t) {
	os << t.colCnt << " " << t.rowCnt;
	for (int i=0; i<t.rowCnt; i++) {
		os << " " << t.consensus[i];
	}
	os << " " << t.id;
	os << endl;
	for (int i=0; i<t.colCnt; i++) {
		for (int j=0; j<t.columns[i].size(); j++)
			os << t.columns[i][j] << " ";
		os << endl;
	}
	return os;
}

int main() {
	char inp[1000];
	cin.getline(inp, sizeof inp);
	vector<string> consensus;
	boost::split(consensus, inp, boost::is_any_of(" "));
	ifstream fi("res-bt1/out-mrgd");
	vector<Table> tables, subTables, superTables, siblingTables;
	Table thisTable;
	bool thisFound = false;
	for (Table t; fi >> t; ) {
		//tables.push_back(t);
		//cerr << t.id << endl;
		if (t.consensus == consensus) {
			thisTable = t;
			thisFound = true;
		} else {
			if (t.isSub(consensus))
				subTables.push_back(t);
			if (t.isSuper(consensus))
				superTables.push_back(t);
			if (t.isSibling(consensus))
				siblingTables.push_back(t);
		}
	}
	if (!thisFound) {
		cout << 0 << " " << consensus.size();
		for (int i=0; i<consensus.size(); i++)
			cout << " " << consensus[i];
		cout << " " << -1 << endl;
	} else {
		cout << thisTable;
	}
	cout << subTables.size() << endl;
	for (int i=0; i<subTables.size(); i++)
		cout << subTables[i];
	cout << superTables.size() << endl;
	for (int i=0; i<superTables.size(); i++)
		cout << superTables[i];
	cout << siblingTables.size() << endl;
	for (int i=0; i<siblingTables.size(); i++)
		cout << siblingTables[i];
	return 0;
}
