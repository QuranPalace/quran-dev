i=$1

python draw-table.py < res-bt1/o$i > res-bt1/table$i.tex
cd res-bt1
rm table.tex
ln -s table$i.tex table.tex 
pdflatex a
mv a.pdf a$i.pdf
cd ..

