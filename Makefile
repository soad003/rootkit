obj-m		:= rootkit.o

KBUILD_DIR	:= /lib/modules/$(shell uname -r)/build

FILE=documentation

default:
	$(MAKE) -C $(KBUILD_DIR) M=$(shell pwd)

tex: $(FILE).pdf


clean:
	$(MAKE) -C $(KBUILD_DIR) M=$(shell pwd) clean
	\rm *.aux *.blg *.out *.bbl *.log *.toc *.pdf *~


$(FILE).pdf: $(FILE).tex
	pdflatex $(FILE)
	pdflatex $(FILE)
	#bibtex $(FILE)
	#pdflatex $(FILE)
	#pdflatex $(FILE)
