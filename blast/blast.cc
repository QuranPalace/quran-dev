#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <set>
#include <list>
#include <algorithm>
#include <functional>   // std::less
#include <cmath>

using namespace std;
const int max_word = 10000;
const int maxSureGap = 10;
const float abundantWordRatio = 20.0/1000.0;
const int mergingHSPScore=10;
const int minMergingHSPSize=4;

const int maxTableRows = 50;
const int minAcceptableLen = 3;
const int maxColumnMismatch = 1;
const int minAcceptableValids = 3;

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

template<class T>
ostream& operator<<(ostream& os, const list<T>& v) {
	os << "[";
	for (typename list<T>::const_iterator i=v.begin(); i != v.end(); i++)
		os << *i << " ";
	return os <<"]";
}

int firstOccuranceAfter(string w, int st, int dir) {
	vector<int>& v = wordReverseIndex[w];
	if (dir == 1) {
		vector<int>::iterator it = lower_bound(v.begin(), v.end(), st);
		if (it == v.end())
			return -1;
		return *it;
	} else {
		vector<int>::reverse_iterator it = lower_bound(v.rbegin(), v.rend(), st, greater<int>());
		if (it == v.rend())
			return -1;
		return *it;
	}
}

struct HSP {
	list<int> first, second;
	vector<int> vfirst, vsecond;
	int score;
	int end(int fs, int dir) const {
		const list<int>& v = (fs == 0)? first : second;
		return (dir == -1)? v.front() : v.back();
	}
	void push_end(int fs, int dir, int v) {
		list<int>& vv = (fs == 0)? first : second;
		if (dir == -1)
			vv.push_front(v);
		else
			vv.push_back(v);
	}

	set<int> first_indices, second_indices, indices;

	void build_indices() {
		indices.clear();
		first_indices.clear();
		vfirst.clear();
		for (typename list<int>::const_iterator i=first.begin(); i != first.end(); i++) {
			first_indices.insert(*i);
			indices.insert(*i);
			vfirst.push_back(*i);
		}
		second_indices.clear();
		vsecond.clear();
		for (typename list<int>::const_iterator i=second.begin(); i != second.end(); i++) {
			second_indices.insert(*i);
			indices.insert(*i);
			vsecond.push_back(*i);
		}
	}

	int commonScore(const HSP& h) const {
		float s = 0;
		for (set<int>::const_iterator i = indices.begin(), j=h.indices.begin(); i!=indices.end() && j != h.indices.end(); ) {
			if (*i == *j) {
				s+= log(word_cnt) - log(wordReverseIndex[words[*i].content].size());
				i++;
				j++;
			} else if (*i < *j) {
				i++;
			} else {
				j++;
			}
		}
		return int(s);
	}
	
};

ostream& operator<<(ostream& os, const HSP& hsp) {
	os << "<[";
	for (typename list<int>::const_iterator i=hsp.first.begin(); i != hsp.first.end(); i++)
		os << *i << ":" << words[*i].content << " ";
	os << "] [";
	for (typename list<int>::const_iterator i=hsp.second.begin(); i != hsp.second.end(); i++)
		os << *i << ":" << words[*i].content << " ";
	os << "]>";
	return os;
}

vector<HSP> hsps;

void build_seeds() {
	map<pair<string, string>, vector<pair<int, int> > > index;
	for (int i=0; i<word_cnt; i++)
		for (int j=i+1; j<i+maxSureGap; j++) 
			if (abundantWords.find(words[i].content) == abundantWords.end() && abundantWords.find(words[j].content) == abundantWords.end()) {
				vector<pair<int, int> >& v = index[make_pair(words[i].content, words[j].content)];
				for (int k=0; k<v.size(); k++) 
					if (v[k].first < i && v[k].second < j) {
						HSP h;
						h.first.push_back(v[k].first);
						h.first.push_back(v[k].second);
						h.second.push_back(i);
						h.second.push_back(j);
						h.score = 2;
						hsps.push_back(h);
					}
				v.push_back(make_pair(i, j));
			}
}

void extend_hsps() {
	cerr << "Extending HSPS..." << endl;
	for (int i=0; i<hsps.size(); i++) {
		HSP& h = hsps[i];
		for (int dir=-1; dir<2; dir += 2) {
			for (int best_value = 0, best_j = -1, best_k = -1; best_value < 3*maxSureGap; ) {
				best_value = 10*maxSureGap;
				for (int j=1; j<maxSureGap; j++) {
					int first_end = h.end(0, dir)+j*dir, second_end = h.end(1, dir)+1*dir;
					if (first_end >=0 && first_end < word_cnt && second_end >= 0 && second_end < word_cnt &&
							abundantWords.find(words[i].content) == abundantWords.end() && abundantWords.find(words[j].content) == abundantWords.end()) {
						int k = firstOccuranceAfter(words[first_end].content, second_end, dir);
						if (k >= 0 && abs(k-second_end) < maxSureGap && abs(k - second_end) + j < best_value && 
							first_end < h.second.front() && k > h.first.back()) {
							best_value = abs(k - second_end) + j;
							best_j = first_end;
							best_k = k;
						}
					}
				}
				if (best_value < 3 * maxSureGap) {
					h.push_end(0, dir, best_j);
					h.push_end(1, dir, best_k);
					h.score += 1;
				}
			}
		}
		//cerr << "  E: " << h << endl;
	}
}

vector<int> edges[max_word];
vector<vector<HSP> > tablesHSP;

int mark[max_word];
void dfs(int v) {
	mark[v] = true;
	tablesHSP.back().push_back(hsps[v]);
	for (int j=0; j<edges[v].size(); j++)
		if (!mark[edges[v][j]])
			dfs(edges[v][j]);
}

void merge_hsps() {
	cerr << "    +MS: Buiding Indices: " << endl;
	int newSize = 0;
	for (int i=0; i< hsps.size(); i++)
		if (hsps[i].first.size() >= minMergingHSPSize) {
			hsps[i].build_indices();
			hsps[newSize++] = hsps[i];
		}
	hsps.resize(newSize);
	cerr << "    +MS: Computing Edges: " << endl;
	for (int i=0; i< hsps.size(); i++)
		for (int j=0; j<hsps.size(); j++)
			if (hsps[i].commonScore(hsps[j]) >= mergingHSPScore) {
				edges[i].push_back(j);
				edges[j].push_back(i);
			}
	cerr << "    +MS: DFSing" << endl;
	for (int i=0; i<hsps.size(); i++)
		if (!mark[i]) {
			tablesHSP.push_back(vector<HSP>());
			dfs(i);
		}
}

vector<vector<vector<pair<int, int> > > > tables;
int tableMaxRow[max_word];

int points_dfs(int v, const vector<int>* points_edge, int* row, int& max_row) {
	if (row[v] != -1)
		return row[v];
	row[v] = max_row;
	for (int i=0; i<points_edge[v].size(); i++)
		points_dfs(points_edge[v][i], points_edge, row, max_row);
	return row[v];
}

void build_tables_1() {
	for (int i=0; i<tablesHSP.size(); i++) {
		vector<int> points;
		vector<int> points_edges[max_word];
		for (int j=0; j<tablesHSP[i].size(); j++) {
			HSP& h = tablesHSP[i][j];
			for (int k=0; k<h.first.size(); k++) {
				points.push_back(h.vfirst[k]);
				points.push_back(h.vsecond[k]);
				points_edges[h.vfirst[k]].push_back(h.vsecond[k]);
				points_edges[h.vsecond[k]].push_back(h.vfirst[k]);
			}
		}
		sort(points.begin(), points.end());
		points.resize(unique(points.begin(), points.end()) - points.begin());
		vector<vector<pair<int, int> > > columns;
		int row[max_word], max_row = 0;
		for (int j=0; j<word_cnt; j++)
			row[j] = -1;
		for (int j=0; j<points.size(); j++) {
			if (!(columns.size() > 0 && points[j] - columns.back().back().first < maxSureGap && points[j] > columns.back().back().first))
				columns.push_back(vector<pair<int, int> >());

			int r = points_dfs(points[j], points_edges, row, max_row);
			if (r == max_row) max_row++;
			columns.back().push_back(make_pair(points[j], r));
		}
		tableMaxRow[tables.size()] = max_row;
		tables.push_back(columns);

		
	}
}

struct HSPEndComparator {
	vector<HSP>& v;
	HSPEndComparator(vector<HSP>& _v) : v(_v) {}
	bool operator()(const pair<int, int>& a, const pair<int, int>& b) const {
		const list<int>& a_fl = (a.second == 0) ? v[a.first].first : v[a.first].second,
			b_fl = (b.second == 0)?  v[b.first].first : v[b.first].second;
		if (a_fl.front() != b_fl.front())
			return a_fl.front() < b_fl.front();
		return a_fl.back() < b_fl.back();
	}
};

void build_tables_2() {
	for (int i=0; i<tablesHSP.size(); i++) {

		vector<pair<int, int> > hspEnds;
		for (int j=0; j<tablesHSP[i].size(); j++) {
			hspEnds.push_back(make_pair(j, 0));
			hspEnds.push_back(make_pair(j, 1));
		}
		sort(hspEnds.begin(), hspEnds.end(), HSPEndComparator(tablesHSP[i]));
		int hspFirstColumn[max_word];
		for (int j=0; j<max_word; j++)
			hspFirstColumn[j] = -1;
		vector<vector<pair<int, int> > > columnsHSP;
		int lastColumnRight = -1;
		columnsHSP.push_back(vector<pair<int, int> >());
		for (int j=0; j<hspEnds.size(); j++) {
			if (lastColumnRight == -1 || lastColumnRight >= tablesHSP[i][hspEnds[j].first].end(hspEnds[j].second, -1)) {
				if (hspFirstColumn[hspEnds[j].first] == columnsHSP.size() - 1 ) {
					hspFirstColumn[hspEnds[j].first] = -1;
				} else {
					if (hspEnds[j].second == 0)
						hspFirstColumn[hspEnds[j].first] = columnsHSP.size() - 1;
					lastColumnRight = max(lastColumnRight, tablesHSP[i][hspEnds[j].first].end(hspEnds[j].second, 1));
					columnsHSP.back().push_back(hspEnds[j]);
				}
			} else {

				columnsHSP.push_back(vector<pair<int, int> >());
				columnsHSP.back().push_back(hspEnds[j]);
				lastColumnRight =  tablesHSP[i][hspEnds[j].first].end(hspEnds[j].second, 1);
				if (hspEnds[j].second == 0)
					hspFirstColumn[hspEnds[j].first] = columnsHSP.size() - 1;
			}
		}

		

		vector<int> points_edges[max_word];
		cerr << "    ce: ";
		for (int j=0; j<tablesHSP[i].size(); j++) 
			if (hspFirstColumn[j] != -1) {
				HSP& h = tablesHSP[i][j];
				for (int k=0; k<h.first.size(); k++) {
					cerr << "[" << h.vfirst[k] << "-" << h.vsecond[k] << "] " ;
					points_edges[h.vfirst[k]].push_back(h.vsecond[k]);
					points_edges[h.vsecond[k]].push_back(h.vfirst[k]);
				}
			}
		cerr << endl;
		int row[max_word], max_row = 0;
		for (int j=0; j<word_cnt; j++)
			row[j] = -1;
		for (int j=0; j<tablesHSP[i].size(); j++) {
			HSP& h = tablesHSP[i][j];
			for (int k=0; k<h.first.size(); k++) {
				int r = points_dfs(h.vfirst[k], points_edges, row, max_row);
				if (r == max_row) max_row++;
			}
		}





		cerr << "Final Table " << i << endl;
		vector<vector<pair<int, int> > > columnsTable;
		for (int j=0; j<columnsHSP.size(); j++) {
			columnsTable.push_back(vector<pair<int, int> >());
			for (int k=0; k<columnsHSP[j].size(); k++) {
				if (hspFirstColumn[columnsHSP[j][k].first] == -1)
					continue;
				vector<int>& lst =  (columnsHSP[j][k].second == 0) ? tablesHSP[i][columnsHSP[j][k].first].vfirst : tablesHSP[i][columnsHSP[j][k].first].vsecond;
				for (int l=0; l<lst.size(); l++)
					columnsTable.back().push_back(make_pair(lst[l], row[lst[l]]));
			}
			sort(columnsTable.back().begin(), columnsTable.back().end());

			cerr << "  FT ";
			for (int k=0; k< columnsTable.back().size(); k++)
				cerr << columnsTable.back()[k].first << ":" <<  columnsTable.back()[k].second << " ";
			cerr << endl;

		}
		tableMaxRow[tables.size()] = max_row;
		tables.push_back(columnsTable);

		
	}
}

void print_tables() {
	for (int i=0; i< tables.size(); i++) 
		if (tables[i].size() >= 3) {
			map<int, int> recolor;
			for (int j=0; j<tables[i].size(); j++) {
				int ff = 0;
				for (int k=0; k<tables[i][j].size(); k++) {
					//if (recolor.find(tables[i][j][k].second) == recolor.end()) {
					//	recolor[tables[i][j][k].second] = recolor.size()-1;
					//}
					//tables[i][j][k].second = recolor[tables[i][j][k].second];
					if (ff == 0 || tables[i][j][k].first != tables[i][j][ff-1].first)
						tables[i][j][ff++] = tables[i][j][k];
				}
				tables[i][j].resize(ff);
			}


			cout << tables[i].size() << " " << tableMaxRow[i] << endl;
			for (int j=0; j<tables[i].size(); j++) {
				for (int k=0; k<tables[i][j].size(); k++) {
					cout << words[tables[i][j][k].first].address << "|" << tables[i][j][k].second << " ";
				}
				cout << endl;
			}
		}
}



int main(int argc, char* argv[]){
	ifstream fi(argv[1]);
	loadWords(fi);
	cerr << "Word # " << word_cnt << endl;
	cerr << "Abundant words: " << endl;
	for (set<string>::iterator i = abundantWords.begin(); i != abundantWords.end(); i++)
		cerr << "  -w: " << *i << endl;

	build_seeds();
	cerr << "HSPS: " << hsps.size() << endl;
	for (int i=0; i<hsps.size(); i++) {
		//cerr << "  " << hsps[i].first << " " << hsps[i].second << endl;
		cerr << "  S " << hsps[i] << endl;
	}
	cerr << "  --" << endl;
	extend_hsps();
	cerr << "extented HSPS: " << hsps.size() << endl;
	for (int i=0; i<hsps.size(); i++) {
		//cerr << "  " << hsps[i].first << " " << hsps[i].second << endl;
		cerr << "  E " << hsps[i] << endl;
	}
	merge_hsps();
	cerr << "merged HSPS: " << tablesHSP.size() << endl;
	for (int i=0; i<tablesHSP.size(); i++) {
		cerr << "  Table " << i << " " << tablesHSP[i].size() << endl;
		for (int j=0; j<tablesHSP[i].size(); j++) {
			cerr << "   T " << i << ":" << tablesHSP[i][j] << endl;
		}
	}
	build_tables_2();
	print_tables();
	return 0;
}
