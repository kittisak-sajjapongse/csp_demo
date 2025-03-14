.PHONY: clean

-include *.d

all: encoder
	@echo Built

clean:
	rm -f *.d
	rm -f encoder
	@echo Clean all artifacts

encoder: schifra
	g++ -MD -I schifra encoder.cpp -o encoder
	@echo Built encoder

schifra:
	git clone https://github.com/ArashPartow/schifra.git

