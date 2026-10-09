FLAGS = -g -Wall -c -std=c++17
BIN_DIR = bin

all: $(BIN_DIR)/.dirstamp chess
	
chess: $(BIN_DIR)/board.o $(BIN_DIR)/pieces.o $(BIN_DIR)/player.o $(BIN_DIR)/chess.o
	g++ -g -Wall $^ -o $@
# g++ -g -Wall -c -std=c++17 bin/chess.o bin/board.o bin/pieces.o bin/player.o -o chess

$(BIN_DIR)/board.o: board.cpp board.h
	g++ $(FLAGS) $< -o $@

$(BIN_DIR)/pieces.o: pieces.cpp pieces.h
	g++ $(FLAGS) $< -o $@

$(BIN_DIR)/player.o: player.cpp player.h
	g++ $(FLAGS) $< -o $@

$(BIN_DIR)/chess.o: chess.cpp
	g++ $(FLAGS) $< -o $@

$(BIN_DIR)/.dirstamp: 
	mkdir -p $(BIN_DIR)
	touch $@

clean: 
	rm -r $(BIN_DIR)