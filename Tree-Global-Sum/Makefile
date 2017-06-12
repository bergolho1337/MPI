MPICC = mpicc
MPICC_FLAGS = -Wall -O2
LINKER = -lm
PROGRAM_NAME = parallel

all: $(PROGRAM_NAME)

$(PROGRAM_NAME): $(PROGRAM_NAME).c
	$(MPICC) $(MPICC_FLAGS) -o $@ $< $(LINKER)

clear:
	rm $(PROGRAM_NAME)

rebuild:
	$(MAKE) clear
	rm -f count.dat
	$(MAKE)
