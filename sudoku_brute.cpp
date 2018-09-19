#include <iostream>
#include <fstream>
#include <vector>
#include <stdio.h>
#include <unistd.h>
#include <ctime>
#include <cstring>
#include <string>


#ifndef MODE_PRINT
	#define MODE_PRINT 1
#endif
#ifndef MODE_FILE
	#define MODE_FILE 1
#endif


struct Cell {
	int allowed_vals[9];
	int num_allowed;
	int value;
};

struct Puzzle {
	std::string id;
	Cell grid[9][9];
};

int fparse(std::string file, std::vector<Puzzle> *puzzles) {
	std::ifstream f;
	f.open(file);

	if (!f) {
		std::cerr << "Can't open input file " << file << std::endl;
		exit(1);
	}

	std::string line;
	std::string nums;
	while(getline(f, line)) {
		Puzzle p = {line};
		for (int i = 0; i < 9 && std::getline(f, nums); i++)
		{
		    for (int j = 0; j < 9; j++) {
		    	p.grid[i][j] = { .allowed_vals = { 0 }, .num_allowed = 0, .value = nums[j] - '0' };
		    }
		}
		puzzles->push_back(p);
	}
	f.close();
	return 0;
}

int fparse_line(std::string file, std::vector<Puzzle> *puzzles) {
	std::ifstream f;
	f.open(file);

	if (!f) {
		std::cerr << "Can't open input file " << file << std::endl;
		exit(1);
	}

	std::string line;
	int count = 0;
	while(getline(f, line)) {
		Puzzle p = { "Grid " + std::to_string(count++) };
		for (int i = 0; i < 9; i++) {
		    for (int j = 0; j < 9; j++) {
		    	if (line[i*9+j] == '.')
		    		line[i*9+j] = '0';
		    	p.grid[i][j] = { .allowed_vals = { 0 }, .num_allowed = 0, .value = line[i*9 + j] - '0' };
		    }
		}
		puzzles->push_back(p);
	}
	f.close();
	return 0;
}

void printPuzzle(Puzzle puzzle) {
	#if MODE_PRINT > 0
	std::cout << puzzle.id << '\n';
	#if MODE_PRINT > 1
	for (int x = 0; x < 9; x++) {
		if (!(x % 3) and x)
			std::cout << "------+------+------\n";

		for (int y = 0; y < 9; y++) {
			if (!(y % 3) and y)
				std::cout << '|';
			std::cout << puzzle.grid[x][y].value << ' ';
		}
		std::cout << '\n';
	}
	std::cout << std::endl;
	#endif
	#endif
}

bool checkValid(Puzzle *puzzle, int row, int col, int cur_val) {
	// int cur_val = puzzle->grid[row][col];
	for (int i = 0; i < 9; i++) {
		if (i != col && puzzle->grid[row][i].value == cur_val || i != row && puzzle->grid[i][col].value == cur_val) {
			// std::cout << "i=" << i << " values = " << cur_val << " " << puzzle->grid[row][i] << '\n';
			return false;
		}
	}
	for (int i = row/3*3; i < row/3*3+3; i++) {
		for (int j = col/3*3; j < col/3*3+3; j++) {
			if (i != row || j != col) {
				if (puzzle->grid[i][j].value == cur_val) {
					// std::cout << "i=" << i << "j=" << j << " values = " << cur_val << " " << puzzle->grid[row/3+i][col/3+j] << '\n';
					return false;
				}
			}
		}
	}

	return true;
}

void getValids(Puzzle* puzzle) {
	for (int r = 0; r < 9; r++) {
		for (int c = 0; c < 9; c++) {
			if (!puzzle->grid[r][c].value) {
				for (int i = 1; i < 10; i++) {
					if (checkValid(puzzle, r, c, i)) {
						puzzle->grid[r][c].allowed_vals[puzzle->grid[r][c].num_allowed] = i;
						puzzle->grid[r][c].num_allowed++;
					}
				}
				if (puzzle->grid[r][c].num_allowed == 1) {
					puzzle->grid[r][c].value = puzzle->grid[r][c].allowed_vals[0];
				}
			}
		}
	}
}

bool solvePuzzleBrute(Puzzle *puzzle, int row, int col) {
	// std::cout << "Checking " << row << ", " << col%9 << '\n';
	// std::cout << "Initial value = " << puzzle->grid[row][col%9] << '\n';
	if (col%9 == 0 && col != 0) {
		col = 0;
		row += 1;
	}
	if (row == 9) {
		return true;
	}
	if (puzzle->grid[row][col].value == 0) {
		for (int i = 0; i < puzzle->grid[row][col].num_allowed; i++) {
			int val = puzzle->grid[row][col].allowed_vals[i];
			if (checkValid(puzzle, row, col, val)) {
				puzzle->grid[row][col].value = val;
				if (solvePuzzleBrute(puzzle, row, col+1)) {
					return true;
				}
			}
		}
		puzzle->grid[row][col].value = 0;
	}
	else {
		return solvePuzzleBrute(puzzle, row, col+1);
	}
	return false;
}

int main(int argc, char *argv[]) {
	std::vector<std::string> files;
	std::string filename;

	// Get file names from user input or command line args
	if (argc < 2) {
		std::cout << "Enter file paths: " << std::endl;
		while (getline(std::cin, filename))
		{
		    if (filename.empty())
		        break;
		    files.push_back(filename);
		}
	} else {
		for (int i = 1; i < argc; i++) {
			files.push_back(argv[i]);
		}
	}

	std::vector<Puzzle> puzzles;
	#if MODE_FILE
	fparse(files[0], &puzzles);
	#else
	fparse_line(files[0], &puzzles);
	#endif

	clock_t total = std::clock();

	for (int i = 0; i < puzzles.size(); i++) {
		printPuzzle(puzzles[i]);
		clock_t begin = std::clock();
		getValids(&puzzles[i]);
		if (solvePuzzleBrute(&puzzles[i], 0, 0)) {
			std::cout << "Solved " << puzzles[i].id << "!\n";
			printPuzzle(puzzles[i]);
		}
		#if MODE_PRINT > 0
		clock_t end = std::clock();
  		double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
  		std::cout << "Took " << elapsed_secs*1000 << " ms." << std::endl;
  		#endif
	}

	clock_t end = std::clock();
	double elapsed_secs = double(end - total) / CLOCKS_PER_SEC;
	std::cout << "Total time: " << elapsed_secs << " seconds" << std::endl;
	std::cout << "Speed: " << puzzles.size()/elapsed_secs << " puzzles/sec" << std::endl;

	
 
	return 0;
}