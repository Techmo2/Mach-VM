SDIR=src
IDIR=include
ODIR=obj

CC=gcc
CFLAGS=-I$(IDIR) -O2
LIBS=

_DEPS = mconfig.h mobject.h mstack.h mtable.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = mstack.o mtable.o mvm.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

mach: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ mach $(INCDIR)/*~ 