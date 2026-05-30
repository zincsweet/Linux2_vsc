BIN=main
CC=g++
RSC=$(wildcard *.cpp)
OBJ=$(RSC:.cpp=.o)

$(BIN):$(OBJ)
	@$(CC) $^ -o $@
	@echo "linking $^ to $@..."

%.o:%.cpp
	@$(CC) -c $<
	@echo "compling $< to $@..."

.PHONY:clean
clean:
	@rm -rf $(BIN) $(OBJ)
	@echo "删除文件$(BIN);$(OBJ)"
