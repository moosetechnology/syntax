

PDF_FILES = Boullier-Deschamp-91.pdf Boullier-Deschamp-88.pdf Bouchon-88.pdf

all: $(PDF_FILES)

clean:
	rm -f *.{aux,lof,log,lot,toc,dvi,ps,pdf}

Boullier-Deschamp-88.pdf: syntaxman-title.tex ../man/*.1 ../man/*.3
	rm -rf tmp
	mkdir tmp
	cat `ls ../man/*.1` `ls ../man/*.3` | groff -man -Tps > tmp/manpages.ps
	cp -p syntaxman-title.tex tmp/; cd tmp; latex syntaxman-title.tex; latex syntaxman-title.tex; \
	        dvips syntaxman-title.dvi -o syntaxman-title.ps; rm -f syntaxman-title.{aux,lof,log,lot,toc,dvi}; cd ..
	cd tmp; gs -q -dNOPAUSE -dBATCH -sDEVICE=pdfwrite -sOutputFile=../$@ syntaxman-title.ps manpages.ps; cd ..
	rm -rf tmp

Boullier-Deschamp-91.pdf: syntax.tex
	pdflatex $<
	pdflatex $<
	pdflatex $<
	mv syntax.pdf $@
	rm -f $*.{aux,lof,log,lot,toc,dvi,ps}

Bouchon-88.pdf: semc.tex
	pdflatex $<
	pdflatex $<
	pdflatex $<
	mv semc.pdf $@
	rm -f $*.{aux,lof,log,lot,toc,dvi,ps}



