CMP: main.o Setting.o R_type.o I_type.o Decode.o Memory.o
	gcc -O3 -o CMP main.o Setting.o R_type.o I_type.o Decode.o Memory.o
%.o: %.c
	gcc -O3 -c $^

clean:
	rm -f *.o
test: a b c d
a:
	./golden
b:
	./CMP
c:
	diff b.rpt snapshot.rpt
d:
	diff a.rpt report.rpt
