# $Id: Makefile,v 1.14 2016-03-24 14:40:54-07 - - $

GPP   = g++ -g -O0 -Wall -Wextra -std=gnu++14
GRIND = valgrind --leak-check=full --show-reachable=yes

all : oc

oc : main.o stringset.o auxlib.o
	${GPP} main.o stringset.o auxlib.o -o oc

%.o : %.cpp
	${GPP} -c $<

ci :
	cid + Makefile auxlib.h auxlib.cpp stringset.h
			 stringset.cpp main.cpp

spotless : clean
	- rm oc test?.out test?.err

clean :
	- rm auxlib.o stringset.o main.o

test : oc
	${GRIND} oc * * >test1.out 2>test1.err
	${GRIND} oc foo foo foo foo bar bar bar foo qux baz \
	         >test2.out 2>test2.err

# Depencencies.
main.o: main.cpp stringset.h auxlib.h
stringset.o: stringset.cpp stringset.h
auxlib.o: auxlib.cpp auxlib.h
