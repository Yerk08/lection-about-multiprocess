synopsis.pdf: con.tex
	sudo pdflatex con.tex && sudo mv con.pdf synopsis.pdf && kioclient exec synopsis.pdf

all:
	synopsis.pdf
clean:
	rm -f synopsis.pdf
