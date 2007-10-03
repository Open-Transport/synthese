all: clean refman.pdf

refman.pdf: refman.tex
	pdflatex refman.tex
	makeindex refman.idx
	pdflatex refman.tex
	pdflatex refman.tex
	pdflatex refman.tex
	pdflatex refman.tex
	pdflatex refman.tex
	pdflatex refman.tex


clean:
	rm -f *.ps *.dvi *.aux *.toc *.idx *.ind *.ilg *.log *.out refman.pdf
