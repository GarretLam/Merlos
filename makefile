# Compiler
CC = g++
#CC = pgCC

# Compiling for architecture nehalem-64
ARCH_FLAG = NO

# Source directory of codes
SRC1 = ../

# Options for compiler
NE_ARCH='-tp=nehalem-64'
AMD_ARCH='-tp k8-64e'
#OPT=-Kieee -fastsse -Mipa=fast,inline -mp -w# pgCC options - Intel machine
OPT=-fopenmp -O2

#ifeq ($(ARCH_FLAG),NEH)
#  OPT+=$(NE_ARCH)
#endif
#ifeq ($(ARCH_FLAG),AMD)
#  OPT+=$(AM_ARCH)
#endif

# Linker
LNK=

OBJS = merlos.o

OBJS_F=$(OBJS)
SUF_OPTS1=$(OBJS_F)
SUF_OPTS2=

# Details of compiling
merlos.out: $(OBJS_F)
	$(CC) $(OPT) -o $@ $(SUF_OPTS1)
%.o: $(SRC1)/%.cpp
	$(CC) $(OPT) -c $< $(SUF_OPTS2)

# Clean
.PHONY: clean
clean:
	@rm -rf *.o *.oo *.log
