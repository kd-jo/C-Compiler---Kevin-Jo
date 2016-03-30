#****************************************************************************
#File name:	 	Makefile
#Author:	 	Kevin Jo
#Date:		 	4/5/15
#Class: 	 	CS480
#Assignment: 	Semantic Actions 3
#Purpose: 	 	This is the makefile for the SA3 assignment.
#***************************************************************************

CC = gcc
CFLAGS = -g -Wall
OBJECTS_LEX = bin/st.o bin/lex.o bin/er.o bin/list.o \
bin/hashtable.o  bin/ct.o bin/rts.o bin/lex_driver.o

OBJECTS_ST = bin/st.o bin/lex.o bin/er.o bin/list.o \
bin/hashtable.o  bin/ct.o bin/rts.o bin/st_driver.o

OBJECTS_TD = bin/st.o bin/lex.o bin/er.o bin/list.o \
bin/hashtable.o bin/ct.o bin/rts.o bin/td.o bin/td_driver.o bin/quad.o \
bin/bu.o

OBJECTS_SA = bin/st.o bin/lex.o bin/er.o bin/list.o \
bin/hashtable.o  bin/ct.o bin/rts.o bin/td.o bin/pcc.o bin/quad.o bin/bu.o

OBJECTS_BU = bin/bu_driver.o bin/lex.o bin/er.o bin/list.o bin/st.o \
bin/hashtable.o bin/ct.o bin/rts.o bin/bu.o bin/quad.o

.PHONY: all clean valgrind tarball

all: bin/lex_driver bin/st_driver bin/td_driver bin/bu_driver bin/pcc

bin/pcc: bin/pcc.o bin/lex.o bin/st.o bin/er.o bin/list.o \
bin/hashtable.o bin/ct.o bin/rts.o bin/td.o bin/quad.o bin/bu.o
	${CC} ${CFLAGS} -o bin/pcc ${OBJECTS_SA}

bin/lex_driver: bin/lex_driver.o bin/lex.o bin/st.o bin/er.o bin/list.o \
bin/hashtable.o bin/ct.o bin/rts.o bin/td.o
	${CC} ${CFLAGS} -o bin/lex_driver ${OBJECTS_LEX}
	
bin/st_driver: bin/st_driver.o bin/st.o bin/er.o bin/list.o bin/hashtable.o \
bin/ct.o bin/rts.o
	${CC} ${CFLAGS} -o bin/st_driver ${OBJECTS_ST}
	
bin/td_driver: bin/td_driver.o bin/lex.o bin/st.o bin/er.o bin/list.o \
bin/hashtable.o bin/ct.o bin/rts.o bin/td.o bin/quad.o bin/bu.o
	${CC} ${CFLAGS} -o bin/td_driver ${OBJECTS_TD}
		
bin/bu_driver: bin/bu_driver.o bin/bu.o bin/lex.o bin/hashtable.o bin/ct.o \
bin/rts.o bin/er.o bin/quad.o
	${CC} ${CFLAGS} -o bin/bu_driver ${OBJECTS_BU}
	
bin/bu.o: src/bu/bu.c src/include/bu.h src/include/lex.h \
src/include/pcc13defines.h src/include/quad.h src/include/td.h
	${CC} ${CFLAGS} -o bin/bu.o -c src/bu/bu.c
	
bin/quad.o: src/quad/quad.c src/include/quad.h src/include/pcc13defines.h
	${CC} ${CFLAGS} -o bin/quad.o -c src/quad/quad.c
	
bin/td.o: src/td/td.c src/include/td.h src/include/pcc13defines.h \
src/include/lex.h src/include/quad.h src/include/bu.h
	${CC} ${CFLAGS} -o bin/td.o -c src/td/td.c
	
bin/lex.o: src/lex/lex.c src/include/lex.h src/include/pcc13defines.h
	${CC} ${CFLAGS} -o bin/lex.o -c src/lex/lex.c
	
bin/st.o: src/st/st.c src/include/st.h src/include/pcc13defines.h \
src/include/hashtable.h src/include/list.h
	${CC} ${CFLAGS} -o bin/st.o -c src/st/st.c
	
bin/ct.o: src/ct/ct.c src/include/ct.h src/include/hashtable.h \
src/include/list.h
	${CC} ${CFLAGS} -o bin/ct.o -c src/ct/ct.c
		
bin/rts.o: src/rts/rts.c src/include/rts.h src/include/hashtable.h \
src/include/list.h
	${CC} ${CFLAGS} -o bin/rts.o -c src/rts/rts.c
	
bin/er.o: src/er/er.c src/include/er.h src/include/pcc13defines.h \
src/include/list.h
	${CC} ${CFLAGS} -o bin/er.o -c src/er/er.c
	
bin/lex_driver.o: src/drivers/lex_driver.c src/include/er.h src/include/st.h \
src/include/pcc13defines.h src/include/hashtable.h src/include/list.h \
src/include/lex.h
	${CC} ${CFLAGS} -o bin/lex_driver.o -c src/drivers/lex_driver.c
	
bin/st_driver.o: src/drivers/lex_driver.c src/include/er.h src/include/st.h \
src/include/pcc13defines.h src/include/hashtable.h src/include/list.h
	${CC} ${CFLAGS} -o bin/st_driver.o -c src/drivers/st_driver.c
	
bin/td_driver.o: src/drivers/td_driver.c src/include/er.h src/include/st.h \
src/include/pcc13defines.h src/include/hashtable.h src/include/list.h \
src/include/lex.h src/include/td.h
	${CC} ${CFLAGS} -o bin/td_driver.o -c src/drivers/td_driver.c
	
bin/pcc.o: src/drivers/pcc.c src/include/er.h src/include/st.h \
src/include/pcc13defines.h src/include/hashtable.h src/include/list.h \
src/include/lex.h src/include/td.h src/include/quad.h src/include/bu.h
	${CC} ${CFLAGS} -o bin/pcc.o -c src/drivers/pcc.c
	
bin/bu_driver.o: src/drivers/bu_driver.c src/include/er.h src/include/st.h \
src/include/pcc13defines.h src/include/hashtable.h src/include/list.h \
src/include/lex.h
	${CC} ${CFLAGS} -o bin/bu_driver.o -c src/drivers/bu_driver.c
		
bin/hashtable.o: src/util/hashtable.c src/include/hashtable.h \
src/include/list.h
	${CC} ${CFLAGS} -o bin/hashtable.o -c src/util/hashtable.c
	
bin/list.o: src/util/list.c src/include/list.h
	${CC} ${CFLAGS} -o bin/list.o -c src/util/list.c
	
run_bu_valgrind_test1: bin/bu_driver
	valgrind -v --leak-check=yes --track-origins=yes bin/bu_driver \
	bu_test/test1.bu
	
run_td_valgrind_test1: bin/td_driver
	valgrind -v --leak-check=yes --track-origins=yes bin/td_driver \
	td_test/test1.td

run_valgrind_lex: bin/lex_driver
	valgrind -v --leak-check=yes --track-origins=yes bin/lex_driver \
	testcases/test.txt
	
run_SA1_valgrind: bin/pcc
	valgrind -v --leak-check=yes --track-origins=yes bin/pcc \
	sa1_test/test1.sa
	
run_SA2_valgrind: bin/pcc
	valgrind -v --leak-check=yes --track-origins=yes bin/pcc \
	sa1_test/test2.sa
	
diff_lex_test1: bin/lex_driver
	diff -Bw testcases/test.out testcases/expected.out
	
run_bu_test1: bin/bu_driver
	bin/bu_driver bu_test/test1.bu
	
run_td_test1: bin/td_driver
	bin/td_driver td_test/test1.td
	
run_st_test1: bin/st_driver
	bin/st_driver testcases/test1.st
	
run_lex_test1: bin/lex_driver
	bin/lex_driver testcases/test.txt > testcases/test.out
	
run_SA1_test1: bin/pcc
	bin/pcc sa1_test/test1.sa
	
run_SA2_test1: bin/pcc
	bin/pcc sa1_test/test2.sa
	
run_test_SA3_1: bin/pcc
	script run_test_SA3_1.full.out -c './test_SA3_full.sh 1'
	
run_test_SA3_2: bin/pcc
	script run_test_SA3_2.full.out -c './test_SA3_full.sh 2'
	
run_test_SA3_3: bin/pcc
	script run_test_SA3_3.full.out -c './test_SA3_full.sh 3'
	
run_test_SA3_4: bin/pcc
	script run_test_SA3_4.full.out -c './test_SA3_full.sh 4'
	
run_test_SA3_5: bin/pcc
	script run_test_SA3_5.full.out -c './test_SA3_full.sh 5'
	
run_test_SA3_6: bin/pcc
	script run_test_SA3_6.full.out -c './test_SA3_full.sh 6'
	
run_test_SA3_7: bin/pcc
	script run_test_SA3_7.full.out -c 'echo $(USER); bin/pcc test_SA3_7.c \
	--sa3typecheck; echo $(USER)'
	
run_test_SA3_8: bin/pcc
	script run_test_SA3_8.full.out -c './test_SA3_full.sh 8'
	
run_test_FI_1: bin/pcc
	script run_test_FI_1.full.out -c './test_FI_full.sh 1'
	
run_test_FI_2: bin/pcc
	script run_test_FI_2.full.out -c './test_FI_full.sh 2'
	
run_test_FI_3: bin/pcc
	script run_test_FI_3.full.out -c './test_FI_full.sh 3'
	
run_test_FI_4: bin/pcc
	script run_test_FI_4.full.out -c './test_FI_full.sh 4'
	
run_test_FI_5: bin/pcc
	script run_test_FI_5.full.out -c './test_FI_full.sh 5'

run_test_FI_6: bin/pcc
	script run_test_FI_6.full.out -c './test_FI_full.sh 6'
	
run_test_FI_7: bin/pcc
	script run_test_FI_7.full.out -c './test_FI_full.sh 7'
	
run_test_FI_8: bin/pcc
	script run_test_FI_8.full.out -c './test_FI_full.sh 8'

clean:
	rm  bin/st_driver bin/lex_driver bin/st.o bin/er.o bin/list.o \
	bin/st_driver.o bin/hashtable.o bin/lex.o bin/ct.o bin/rts.o bin/td.o \
	bin/td_driver bin/td_driver.o bin/bu_driver bin/bu_driver.o \
	bin/lex_driver.o bin/bu.o bin/pcc bin/pcc.o bin/quad.o