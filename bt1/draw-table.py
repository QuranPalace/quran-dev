import sys
import cStringIO

colors = ['red', 'blue', 'brown', 'cyan', 'green', 'yellow', 'orange', 'black']

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

def nameToNode(prevColumnName, prevName, name, cnt, style):
	if prevName is not None: positioning = 'left of=' + prevName
	else:
		if prevColumnName is None:
			positioning = ''
		else:
			positioning = 'below of=' + prevColumnName +', node distance=7cm'
	#cnt = cnt.translate(None, '^{}$&_#')
	if style is not None: style = ', ' + style
	else: style = ''
	return '\t\\node ['+positioning+style+'] (' + name + ') {' + cnt + '};'

def wordAddress(colIdx, sure, aye, word):
	return str(colIdx) + '_' + str(sure) + '_' + str(aye) + '_' + str(word)

def drawTable(columns, rowCnt):
	print '\\begin{tikzpicture}[node distance = 0.1cm, auto]'
	#rows = [ [] for r in range(rowCnt) ]
	#wordStyles = {}
	#colIdx = 0
	#for col in columns:
	#	for w in col:
	#		rows[int(w[1])].append((colIdx, w[0]))
	#		x = addressToIndex(w[0])
	#		wordStyles[wordAddress(colIdx, x[0], x[1], x[2])] = 'box_' + w[1]
	#		#if len(rows[int(w[1])]) >= 2:
	#		#	y = addressToIndex(rows[int(w[1])][-2][1])
	#		#	wordStyles[wordAddress(colIdx, x[0], x[1], x[2])] += ', below of=' + wordAddress(rows[int(w[1])][-2][0], y[0], y[1], y[2])
	#	colIdx += 1

	for i in range(rowCnt+1):
		print '\t\\tikzstyle{box_' + str(i) + '} = [draw, color=' + colors[i%len(colors)] + ', thick]'

	#prevColumnName = None
	print '\\matrix (M) {'
	colIdx = 0
	for colx in columns:
		col = colx
		first, last = col[0], col[-1]
		firstAye = addressToIndex(first[0])[1]
		lastAye = addressToIndex(last[0])[1]
		sure = addressToIndex(first[0])[0]
		#prevName = str(colIdx)
		#print nameToNode(prevColumnName, None, str(colIdx), '', None)
		#for aye in range(firstAye, lastAye+1):
		#	for word in range(aye_len[str(sure) + ':' + str(aye)]):
		#		#addr = str(sure) + '_' + str(aye) + '_' + str(word+1)
		#		addr = wordAddress(colIdx, sure, aye, word+1)
		#		#print nameToNode(prevColumnName, prevName, addr, corpus_words[str(sure) + ':' + str(aye) + ':' + str(word+1)])
		#		sty = None
		#		if addr in wordStyles: sty = wordStyles[addr]
		#		print nameToNode(prevColumnName, prevName, addr, '\\includegraphics{images/'+str(corpus_word_index[str(sure) + ':' + str(aye) + ':' + str(word+1)])+'.png}', sty)
		#		prevName = addr
		#	addr = str(colIdx) + '_' + str(sure) + '_' + str(aye) + '_' + 'L'
		#	print nameToNode(prevColumnName, prevName, addr, '('+str(aye) + ')', None)
		#	prevName = addr
		#print
		#
		#prevColumnName = str(colIdx)
		
		output = cStringIO.StringIO()
		print >>output, '\t\\node [] (h_' + str(colIdx) + '_-1) {};', '&'
		lAye = firstAye
		lWord = 0
		for f in range(rowCnt+1):
			cfIdx = 0
			while cfIdx < len(col) and int(col[cfIdx][1]) < f: cfIdx += 1
			if cfIdx < len(col):
				fIndex = addressToIndex(col[cfIdx][0])
			else:
				fIndex = [sure, lastAye+1, 1]
			print >>output, '\t% col=', col, 'f=' + str(f), 'cfIdx=', cfIdx, 'fIndex=', fIndex
			#print '\t\\node[matrix, left=of h_' + str(colIdx) + '_' + str(f-1) + '.west] (m_' + str(colIdx) + '_' + str(f) + ') {';
			print >>output, '\t\t\\node [] (n_' + str(colIdx) + '_' + str(f) + '_-1) {};'
			i = 0
			while lAye != fIndex[1] or lWord != fIndex[2]-1:
				if (lAye == 124):
					print >>sys.stderr, col, colx
					print >>sys.stderr, 'BUG', lAye, lWord, fIndex
				print >>output, '\t\t\\node [left=of n_' + str(colIdx) + '_' + str(f) + '_' + str(i-1) + '] (n_' + str(colIdx) + '_' + str(f) + '_' + str(i) + ') {\\includegraphics{images/'+str(corpus_word_index[str(sure) + ':' + str(lAye) + ':' + str(lWord+1)])+'.png}};'
				i += 1
				lWord += 1
				if lWord == aye_len[str(sure) + ':' + str(lAye)]:
					lWord = 0
					lAye += 1
			#print '\t\\\\};'
			print >>output, '\t&'
			print >>output, '\t\\node[box_'+str(f)+'] (h_' + str(colIdx) + '_' + str(f) +') {',
				# left=of m_' + str(colIdx) + '_' + str(f)+ ',box_'+str(f)+'
			if cfIdx < len(col) and int(col[cfIdx][1]) == f:
				print >>output, '\\includegraphics{images/'+str(corpus_word_index[str(sure) + ':' + str(lAye) + ':' + str(lWord+1)])+'.png}',
				lWord += 1
				if lWord == aye_len[str(sure) + ':' + str(lAye)]:
					lWord = 0
					lAye += 1
			print >>output, '};&'
		r = output.getvalue().split('&')
		r.reverse()
		print '&'.join(r)
		print '\t\\\\'
				
		colIdx += 1
		#print

	#for r in rows:
	#	for i in range(1, len(r)):
	#		print '\t\\path [line] (' + str(r[i-1][0]) + '_' + addressToWordAddress(r[i-1][1]).replace(':', '_') + ') -- (' + str(r[i][0]) + '_' + addressToWordAddress(r[i][1]).replace(':', '_') + ');'
	print '};'
	print '\\end{tikzpicture}'
	print

loadCorpus()
isNew = True
for l in sys.stdin:
	if isNew:
		cnts = [x for x in l.split()]
		columnCnt, rowCnt = int(cnts[0]), int(cnts[1])
		columns = []
		isNew = False
		i = 0
	else:
		columns.append([])
		for s in l.split():
			columns[-1].append(s.split('|'))
		i += 1
		if i == columnCnt:
			isNew = True
			drawTable(columns, rowCnt)



