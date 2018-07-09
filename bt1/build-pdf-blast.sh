i=$1

python draw-table.py < res-blast/o$i > res-blast/table$i.tex
cd res-blast
rm table.tex
ln -s table$i.tex table.tex 
pdflatex a
mv a.pdf a$i.pdf
cd ..

