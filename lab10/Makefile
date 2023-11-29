main:main.o FirstFit.o MemoryManage.o Log.o NextFit.o BestFit.o WorstFit.o
	gcc -o $@ $^ -g

main.o:main.c
	gcc -c -o $@ $^ -g

FirstFit.o:FirstFit.c
	gcc -c -o $@ $^ -g

MemoryManage.o:MemoryManage.c
	gcc -c -o $@ $^ -g

Log.o:Log.c
	gcc -c -o $@ $^ -g

NextFit.o:NextFit.c
	gcc -c -o $@ $^ -g

BestFit.o:BestFit.c
	gcc -c -o $@ $^ -g

WorstFit.o:WorstFit.c
	gcc -c -o $@ $^ -g

.PHONY:clean
clean:
	rm *.o main
