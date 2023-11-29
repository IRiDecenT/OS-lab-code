CC=g++
FLAG=-std=c++17

main:main.o job.o FCFS.o HRRF.o RR.o MLQF.o SJF.o SRTF.o
	$(CC) $^ -o $@ $(FLAG)

job.o:job.cc
	$(CC) -c $^ -o $@ $(FLAG)

SJF.o:SJF.cc
	$(CC) -c $^ -o $@ $(FLAG)

SRTF.o:SRTF.cc
	$(CC) -c $^ -o $@ $(FLAG)

MLQF.o:MLQF.cc
	$(CC) -c $^ -o $@ $(FLAG)

RR.o:RR.cc
	$(CC) -c $^ -o $@ $(FLAG)

HRRF.o: HRRF.cc
	$(CC) -c $^ -o $@ $(FLAG)

FCFS.o:FCFS.cc
	$(CC) -c $^ -o $@ $(FLAG)

main.o:main.cc
	$(CC) -c $^ -o $@ $(FLAG)

.PHNOY:
clean:
	rm -f main *.o