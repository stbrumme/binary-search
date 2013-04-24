CC = @g++
CFLAGS = -std=c++0x -O3 -I. -Wall -Wextra -pedantic -march=native -mtune=native -fno-exceptions
CFLAGS_SMALL_EXE = -s -fno-rtti
CFLAGS_PROFILE_RUN1 = -fprofile-arcs -fvpt
CFLAGS_PROFILE_RUN2 = -fbranch-probabilities -freorder-functions -fprofile-use
GEN_ASM = -S -fverbose-asm
CPP = BinarySearch.cpp

BinarySearch.exe: $(CPP)
	$(CC) $(CPP) $(CFLAGS) $(GEN_ASM)
	$(CC) -o $@ $(CPP) $(CFLAGS) $(CFLAGS_SMALL_EXE)
#	$(CC) -o $@ $(CPP) $(CFLAGS) $(CFLAGS_PROFILE_RUN1)
#	@BinarySearch.exe
#	$(CC) -o $@ $(CPP) $(CFLAGS) $(CFLAGS_PROFILE_RUN2)

clean:
	del BinarySearch.exe
