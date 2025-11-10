synopsis.pdf:
	sudo pdflatex con.tex && mv con.pdf synopsis.pdf && kioclient exec synopsis.pdf

all:
	synopsis.pdf
clean:
	rm -f synopsis.pdf
