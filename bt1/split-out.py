import sys
import cStringIO

file_index = 0
handle = None
for l in sys.stdin:
	if l.find('(') == -1:
		if handle is not None: handle.close()
		file_index += 1
		handle = open('o' + str(file_index), 'w')
	print >>handle, l[:-1]
if handle is not None: handle.close()
