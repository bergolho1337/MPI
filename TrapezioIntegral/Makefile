MPICC = mpicc
MPICC_FLAGS = -std=c99 -DDEBUG
PNAME = trapezio

all: $(PNAME)

$(PNAME): $(PNAME).c
	$(MPICC) $(MPICC_FLAGS) -o $@ $<

clear:
	rm $(PNAME)
     