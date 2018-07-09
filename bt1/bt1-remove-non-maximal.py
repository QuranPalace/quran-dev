import sys
import cStringIO

def addressToWordAddress(loc):
	loc = loc.replace('(', '').replace(')', '')
	return loc[:loc.rfind(':')]
	
def addressToAyeAddress(loc):
	loc = addressToWordAddress(loc)
	return loc[:loc.rfind(':')]
	
def addressToIndex(loc):
	loc = loc.replace('(', '').replace(')', '')
	return [int(s) for s in loc.split(':')]

aye_len = {}
corpus_words = {}
corpus_word_index = {}
def loadCorpus():
	h_corpus = open('data/quranic-corpus-morphology-0.4.txt')
	imageIndex = 1
	for l in h_corpus:
		if len(l) == 0 or l[0] != '(': continue
		x = l.split()
		loc = addressToWordAddress(x[0])
		if loc not in corpus_words: corpus_words[loc] = ''
		corpus_words[loc] += x[1]
		aye = addressToAyeAddress(x[0])
		wordNumber = addressToIndex(x[0])[2]
		if aye not in aye_len: aye_len[aye] = 0
		aye_len[aye] = max(aye_len[aye], wordNumber)
		if loc not in corpus_word_index:
			corpus_word_index[loc] = imageIndex;
			imageIndex += 1
	h_corpus.close()
	#print >>sys.stderr, imageIndex
class Table:
	pass
	def isSub(self, other):
		if self.rowCnt > other.rowCnt: return (False, 1)
		i, j = 0, 0
		while i < len(self.consensus) and j < len(other.consensus):
			if (self.consensus[i] == other.consensus[j]):
				i += 1
			j += 1
		#print >>sys.stderr, self.consensus, other.consensus, i, j
		return (i == len(self.consensus), len(other.consensus) - j)
		
def hasMinimunRequirements(t):
	rOcr = [0 for i in range(t.rowCnt)]
	for c in t.columns:
		for w in c:
			rOcr[int(w[1])] += 1
	for r in rOcr:
		if r < 2: return False
	return True

def printTable(t):
	print t.columnCnt, t.rowCnt, 
	for w in t.consensus:
		print w,
	print t.id,
	for sub in t.subTables:
		if not tables[sub].removed: print sub,
	print
	for col in t.columns:
		for w in col:
			print w[0] + '|' + w[1], 
		print


loadCorpus()
isNew = True
tables = []
for l in sys.stdin:
	if isNew:
		cnts = [x for x in l.split()]
		table = Table()
		table.columnCnt, table.rowCnt = int(cnts[0]), int(cnts[1])
		table.consensus = cnts[2:]
		table.columns = []
		isNew = False
		i = 0
	else:
		table.columns.append([])
		for s in l.split():
			table.columns[-1].append(s.split('|'))
		i += 1
		if i == table.columnCnt:
			isNew = True
			table.consensus_sorted = sorted(table.consensus)
			tables.append(table)

def cmp_table(t1, t2):
	#if t1.consensus_sorted < t2.consensus_sorted:
	#	return -1
	#elif t1.consensus_sorted > t2.consensus_sorted:
	#	return 1
	#if t1.isSub(t2):
	#	return -1
	#elif t2.isSub(t1):
	#	return 1
	#return t1.columnCnt - t2.columnCnt
	return t1.rowCnt - t2.rowCnt


for i in range(len(tables)):
	tables[i].id = i
	tables[i].subTables = []
	tables[i].removed = False
	if not hasMinimunRequirements(tables[i]): tables[i].removed = True

tables = sorted(tables, key=lambda t: t.consensus)

for i in range(len(tables)):
	if tables[i].removed: continue
	print >> sys.stderr, '  tab ', i
	good = True
	for j in range(len(tables)):
		if j != i and not tables[j].removed:
			iss = tables[i].isSub(tables[j])
			sub, jr = iss[0], iss[1]
			if sub: 
				if tables[i].columnCnt <= tables[j].columnCnt or (jr == 0 and i < j):
					good = False
					break
				tables[j].subTables.append(tables[i].id)
	if good:
		print >>sys.stderr, 'removed ', i
		tables[i].removed = not good

for i in range(len(tables)):
#	if i == len(tables)-1 or not tables[i].isSub(tables[i+1]):
	#if not tables[i].removed:
	if tables[i].removed: continue
	if i==0 or tables[i].consensus != tables[i-1].consensus:
		printTable(tables[i])

