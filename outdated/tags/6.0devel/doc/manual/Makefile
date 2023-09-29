PDF_FILES = syntax.pdf syntaxman.pdf tabc.pdf

all: $(PDF_FILES)

clean:
	rm -f $(PDF_FILES) *.{aux,lof,log,lot,toc,dvi,ps}

syntaxman.pdf: syntaxman-title.tex ../man/*.1 ../man/*.3
	rm -rf tmp
	mkdir tmp
	cat `ls ../man/*.1` `ls ../man/*.3` | groff -man -Tps > tmp/manpages.ps
	cp -p syntaxman-title.tex tmp/; cd tmp; latex syntaxman-title.tex; latex syntaxman-title.tex; \
	        dvips syntaxman-title.dvi -o syntaxman-title.ps; rm -f syntaxman-title.{aux,lof,log,lot,toc,dvi}; cd ..
	cd tmp; gs -q -dNOPAUSE -dBATCH -sDEVICE=pdfwrite -sOutputFile=../syntaxman.pdf syntaxman-title.ps manpages.ps; cd ..
	rm -rf tmp

%.pdf: %.tex
	pdflatex $<
	pdflatex $<
	pdflatex $<
	rm -f $*.{aux,lof,log,lot,toc,dvi,ps}

