CC=g++
STD=-std=c++11
FLAG=-lpthread
OBJ1=lab8
SRC1=lab8.cpp
OBJ2=apple_orange_pro
SRC2=apple_orange_pro.cpp 
OBJ3=lab8_v2 
SRC3=lab8_v2.cpp
OBJ4=lab8_change 
SRC4=lab8_change.cpp
$(OBJ1):$(SRC1)
	$(CC) -o $@ $^ $(STD) $(FLAG)

$(OBJ2):$(SRC2)
	$(CC) -o $@ $^ $(STD) $(FLAG)

$(OBJ3):$(SRC3)
	$(CC) -o $@ $^ $(STD) $(FLAG)

$(OBJ4):$(SRC4)
	$(CC) -o $@ $^ $(STD) $(FLAG)

.PHONY:clean
clean:
	rm $(OBJ1) $(OBJ2) $(OBJ3) $(OBJ4)
