#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <set>

using namespace std;
const int max_word = 10000;
const int maxTableRows = 50;
const int maxSureGap = 20;
const int minAcceptableLen = 3;
const int maxColumnMismatch = 1;
const int minAcceptableValids = 3;
const float abundantWordRatio = 20.0/1000.0;

struct Word {
	string content;
	string address;

	Word(string cnt = "", string addr = "") : content(cnt), address(addr) {}
};
int word_cnt = 0;
Word words[max_word];
set<string> abundantWords;
map<string, vector<int> > wordReverseIndex;

void loadWords(ifstream& fi) {
	for (string cnt, addr; fi >> cnt >> addr;) {
		words[word_cnt++] = Word(cnt, addr);
	}
	map<string, int> wordAbudance;
	for (int i=0; i<word_cnt; i++) {
		wordReverseIndex[words[i].content].push_back(i);
		wordAbudance[words[i].content]++;
	}
	for (map<string, int>::iterator i=wordAbudance.begin(); i!= wordAbudance.end(); i++)
		if (i->second > abundantWordRatio * word_cnt)
			abundantWords.insert(i->first);
	
}

int firstOccuranceAfter(string w, int st) {
	vector<int>& v = wordReverseIndex[w];
	vector<int>::iterator it = lower_bound(v.begin(), v.end(), st);
	if (it == v.end())
		return -1;
	return *it;
}





struct TableColumn {
	int len, gapCnt;
	int wordIndex[maxTableRows];
	vector<int> historyAction;

	TableColumn() : len(), gapCnt() {}
	TableColumn(int firstIndex) : len(1), gapCnt(0) {
		wordIndex[0] = firstIndex;
	}
	bool operator<(const TableColumn &t) const {
		for (int i=0; i<len; i++)
			if (i >= t.len)
				return false;
			else if (wordIndex[i] < t.wordIndex[i]) 
				return true;
			else if (wordIndex[i] > t.wordIndex[i])
				return false;
		return len < t.len;
	}

	int last() const {
		if (len <= 0)
			return -1;
		return wordIndex[len-1];
	}

	int first() const {
		return wordIndex[0];
	}

	bool valid() const {
		return gapCnt <= maxColumnMismatch;
	}

	bool addWord(string w) {
		if (!valid()) {
			historyAction.push_back(0);
			return false;
		}
		int f = firstOccuranceAfter(w, last()+1);
		if (f == -1 || f - last() > maxSureGap) {
			gapCnt++;
			historyAction.push_back(1);
			return false;
		}
		wordIndex[len++] = f;
		historyAction.push_back(2);
		return true;
	}

	void removeLast() {
		int b = historyAction.back();
		historyAction.pop_back();
		if (b == 0) {
			return;
		}
		if (b == 1) {
			gapCnt--;
			return;
		}
		if (b == 2) {
			len--;
		}
	}
};

void printResult(const vector<TableColumn*>& columns, const vector<string>& consensus) {
	//TODO: minimize number of crossing lines here
	cout << columns.size() << " " << consensus.size() << " ";
	for (int i=0; i<consensus.size(); i++)
		cout << consensus[i] << " ";
	cout << endl;
	for (vector<TableColumn*>::const_iterator i = columns.begin(); i!= columns.end(); i++) {
		int cns = 0;
		for (int j=0; j<(*i)->len; j++) {
			Word w = words[(*i)->wordIndex[j]];
			while (consensus[cns] != w.content)
				cns++;
			cout << w.address << "|" << cns << " ";
		}
		cout << endl;
	}
}

bool bt(const vector<TableColumn*>& prevColumns, int depth, vector<string>& consensus) {
	vector<TableColumn*> columns;
	int validCnt = 0;
	for (vector<TableColumn*>::const_iterator k = prevColumns.begin(); k!=prevColumns.end(); k++) {
		if ((*k)->valid() && (columns.size() == 0 || (*k)->first() > columns.back()->last() ) ) {
			validCnt++;
			columns.push_back(*k);
		}
	}
	if (validCnt < minAcceptableValids)
		return false;
	cerr << "bt " << depth << " " << validCnt << endl;

	set<string> addedWords;
	bool goodDeepers = false;
	for (int i=0, j=prevColumns[i]->last()+1; i < columns.size() && j < word_cnt ; ) {
		if (j < prevColumns[i]->last() + 1)
			j = prevColumns[i]->last() + 1;
		if (j > prevColumns[i]->last() + maxSureGap) {
			i++;
			continue;
		}
		string w = words[j].content;
		if (abundantWords.find(w) != abundantWords.end() || addedWords.find(w) != addedWords.end()) {
			j++;
			continue;
		}
		addedWords.insert(w);
		int addedWordCnt = 0;
		for (vector<TableColumn*>::iterator k = columns.begin(); k!=columns.end(); k++) {
			addedWordCnt += (*k)->addWord(w);
		}
		consensus.push_back(w);
		//cerr << "Going to bt" << endl;
		if (addedWordCnt > 1)
			goodDeepers |= bt(columns, depth+1, consensus);
		consensus.pop_back();
		//cerr << "  Coming back from bt" << endl;
		for (vector<TableColumn*>::iterator k = columns.begin(); k!=columns.end(); k++) {
			(*k)->removeLast();
		}
		//cerr << "  rel bt" << endl;
	}
	cerr << "~bt " << depth << " " << validCnt << endl;
	if (depth > maxColumnMismatch + minAcceptableLen && !goodDeepers)
	{
		cerr << " ================== SOLUTION (" << depth << ") ======================" << endl;
		for (vector<TableColumn*>::const_iterator k = prevColumns.begin(); k!=prevColumns.end(); k++) {
			for (int i=0; i<(*k)->len; i++)
				cerr << words[(*k)->wordIndex[i]].content << " [" << words[(*k)->wordIndex[i]].address << "] ";
			cerr << (*k)->len << " " << (*k)->gapCnt << endl;
		}
		printResult(columns, consensus);
	}
}

void bt_start() {
	set<string> addedWords;
	vector<string> consensus;
	for (int i=0; i<word_cnt; i++) {
		if (addedWords.find(words[i].content) == addedWords.end()) {
			string w = words[i].content;
			addedWords.insert(w);
			vector<TableColumn*> columns;

			//cerr << "bts ";
			for (int f=-1; (f=firstOccuranceAfter(w, f+1)) != -1; ) {
			//	cerr << f << " ";
				columns.push_back(new TableColumn(f));
			}
			//cerr << endl;
			consensus.push_back(w);
			bt(columns, 1, consensus);
			consensus.pop_back();

			for (vector<TableColumn*>::iterator k=columns.begin(); k!=columns.end(); k++) {
				delete *k;
			}
		}
	}
}

int main(int argc, char* argv[]){
	ifstream fi(argv[1]);
	loadWords(fi);
	cerr << "Word # " << word_cnt << endl;
	bt_start();
	return 0;
}
