LATEST:=$(shell year=`ls *.tex | grep -o '..\.tex' | cut -c -2 | sort -n --reverse | head -n 1`; ls *.tex | grep $$year | rev | cut -c 5- | rev)

.PHONY: clean distclean

latest: $(LATEST).tex
	pdflatex $(LATEST).tex
	bibtex $(LATEST)
	pdflatex $(LATEST).tex

distclean: clean
	rm -f $(LATEST).pdf

clean:
	rm -f $(LATEST).aux
	rm -f $(LATEST).log
	rm -f $(LATEST).bbl
	rm -f $(LATEST).blg
