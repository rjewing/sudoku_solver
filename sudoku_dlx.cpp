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

#define block(r,c) (3*((r)/3)+((c)/3))


struct Puzzle {
	std::string id;
	int grid[9][9];
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
		    	p.grid[i][j] = nums[j] - '0';
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
		    	p.grid[i][j] = line[i*9 + j] - '0';
		    }
		}
		puzzles->push_back(p);
	}
	f.close();
	return 0;
}

void printPuzzle(Puzzle puzzle) {
	#if  MODE_PRINT > 0
		std::cout << puzzle.id << '\n';
		#if MODE_PRINT > 1
			for (int x = 0; x < 9; x++) {
				if (!(x % 3) and x)
					std::cout << "------+------+------\n";
				for (int y = 0; y < 9; y++) {
					if (!(y % 3) and y)
						std::cout << '|';
					std::cout << puzzle.grid[x][y] << ' ';
				}
				std::cout << '\n';
			}
			std::cout << std::endl;
		#endif
	#endif
}

void printRawPuzzle(Puzzle puzzle) {
	#if MODE_PRINT > 0
	std::cout << puzzle.id << '\n';
	#if MODE_PRINT > 1
		for (int x = 0; x < 9; x++) {
			for (int y = 0; y < 9; y++) {
				std::cout << puzzle.grid[x][y];
			}
			std::cout << '\n';
		}
	#endif
	#endif
}

/* ---------------------------- DLX Algorithm----------------------------*/


const int MAX_ROW = 729;
const int MAX_COL = 324;
int matrix[MAX_ROW][MAX_COL] = {0};
unsigned int sol_count = 0;
clock_t total;
clock_t timer;
bool finished = false;


struct Node {
	Node* left;
	Node* right;
	Node* up;
	Node* down;
	Node* header;
	int id[2];
	int count;
};

struct Node tmp_root;
struct Node* root = &tmp_root;
Node* solutions[100];
std::vector<Node*> nodes;

std::string printID(Node* node) {
	return "(" + std::to_string(node->id[0]) + ", " + std::to_string(node->id[1]) + ") ";
}

void cover(Node* col) {
	col->left->right = col->right;
	col->right->left = col->left;
	for (Node* d = col->down; d != col; d = d->down) {
		for (Node* r = d->right; r != d; r = r->right) {
			r->down->up = r->up;
			r->up->down = r->down;
			r->header->count--;
		}
	}
}

void uncover(Node* col) {
	
	for (Node* u = col->up; u != col; u = u->up) {
		for (Node* l = u->left; l != u; l = l->left) {
			l->header->count++;
			l->down->up = l;
			l->up->down = l;
		}
	}
	col->left->right = col;
	col->right->left = col;
}


void matrixToLL() {
	Node* header = new Node;
	nodes.push_back(header);
	header->left = header;
	header->right = header;
	header->up = header;
	header->down = header;
	header->header = header;
	header->id[0] = -1;
	header->id[1] = -1;
	header->count = -1;

	Node* temp = header;

	for (int c = 0; c < MAX_COL; c++) {
		Node* newNode = new Node;
		nodes.push_back(newNode);
		newNode->count = 0;
		newNode->up = newNode;
		newNode->down = newNode;
		newNode->header = newNode;
		newNode->right = header;
		newNode->left = temp;
		temp->right = newNode;

		newNode->id[0] = -1;
		newNode->id[1] = c;

		temp = newNode;
	}
	header->left = temp;

	for (int r = 0; r < MAX_ROW; r++) {
		Node* top = header;
		Node* prev = NULL;

		for (int c = 0; c < MAX_COL; c++) {
			top = top->right;
			if (matrix[r][c]) {
				Node* newNode = new Node;
				nodes.push_back(newNode);
				newNode->id[0] = r;
				newNode->id[1] = c;

				newNode->header = top;
				newNode->up = top->up;
				newNode->down = top;
				newNode->up->down = newNode;
				top->up = newNode;

				if (top->down == top) {
					top->down = newNode;
				}

				if (prev == NULL) {
					prev = newNode;
					prev->right = newNode;
				}

				newNode->left = prev;
				newNode->right = prev->right;
				newNode->right->left = newNode;
				prev->right = newNode;


				top->count++;
				prev = newNode;

			}
		}
	}
	root = header;

}


void printSolution(int k) {
	Puzzle puzzle = { .id="Solution " + std::to_string(sol_count) };
	sol_count++;
	for (int i = 0; i < k; i++) {
		int id0 = solutions[i]->id[0];
		puzzle.grid[id0/81][id0%81/9] = id0%81%9+1;
	}
	// printRawPuzzle(puzzle);
	printPuzzle(puzzle);

	#if MODE_PRINT > 0
	clock_t end = std::clock();
	double elapsed_secs = double(end - timer) / CLOCKS_PER_SEC;
	std::cout << "Took " << elapsed_secs*1000 << " ms." << std::endl;
	timer = std::clock();
	#endif
}

void search(int k) {
	// if (finished)
	// 	return;
	if (root->right == root) {
		finished = true;
		printSolution(k);
		return;
	}


	Node* col = root->right;
	for (Node* node = col->right; node != root; node = node->right) {
		if (node->count < col->count && node->count){
			col = node;
		}
	}
	cover(col);
	for (Node* r = col->down; r != col; r = r->down) {
		solutions[k] = r;
		for (Node* j = r->right; j != r; j = j->right) {
			cover(j->header);
		}
		search(k+1);
		for (Node* j = r->left; j != r; j = j->left) {
			uncover(j->header);
		}
	}
	uncover(col);
	return;
}

int k_count = 0;
void applyHints(Puzzle puzzle) {

	for (Node* n1 = root->right; n1 != root; n1 = n1->right) {
		for (Node* n2 = n1->down; n2 != n1; n2 = n2->down) {
			int id0 = n2->id[0];
			if (puzzle.grid[id0/81][id0%81/9] == id0%81%9+1) {
				cover(n1);
				solutions[k_count] = n2;
				k_count++;
				for (Node* j = n2->right; j != n2; j = j->right) {
					cover(j->header);
				}
				break;
			}
		}
	}
}

void removeHints() {
	while (k_count > 0) {
		k_count--;
		Node* node = solutions[k_count];
		for (Node* j = node->left; j != node; j = j->left) {
			uncover(j->header);
		}
		uncover(node->header);
	}
}

void createMatrix() {
	int row,r,c,i;
	for (row=0,r=0; r<9; r++){
		for (c=0; c<9; c++) {
			for (i=0; i<9; i++,row++)
			{
				//cell constraint
				matrix[row][c+9*r]=1;
				//row constraint
				matrix[row][81+i+9*r]=1;
				//column constraint
				matrix[row][162+i+9*c]=1;
				//block constraint
				matrix[row][243+i+9*block(r,c)]=1;
			}
		}
	}
}

void deleteNodes() {
	while(nodes[0] == NULL) {
		delete nodes.back();
		nodes.pop_back();
	}
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
	createMatrix();
	matrixToLL();
	total = std::clock();
	for (int i = 0; i < puzzles.size(); i++) {
		printPuzzle(puzzles[i]);
		timer = std::clock();
		finished = false;
		applyHints(puzzles[i]);
		search(k_count);
		#if MODE_PRINT > 0
		if (!finished)
			std::cout << "No solution!\n";
		#endif
		removeHints();
	}
	deleteNodes();
	clock_t end = std::clock();
	double elapsed_secs = double(end - total) / CLOCKS_PER_SEC;
	std::cout << "Total time: " << elapsed_secs << " seconds" << std::endl;
	std::cout << "Speed: " << puzzles.size()/elapsed_secs << " puzzles/sec" << std::endl;

	
 
	return 0;
}