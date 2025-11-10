con.pdf:
	sudo pdflatex con.tex && kioclient exec con.pdf

all:
	con.pdf
clean:
	rm -f con.pdf
