#! /usr/bin/env python

import sys

in_file = ""
num_puzzles = 0;

puzzles = []
solutions = []

if len(sys.argv) > 2:
	in_file = sys.argv[1]
	num_puzzles = int(sys.argv[2])
elif len(sys.argv) > 1:
	in_file = sys.argv[1]
	num_puzzles = sys.maxint
else:
	in_file = raw_input("Enter in file name: ")

out_file = in_file.split('.')[0]

f_in = open(in_file, 'r')
f_out_p = open(out_file+'_puzzles.txt', 'w+')

count = 0
for line in f_in:
	f_out_p.write("Grid %d\n" %(count))
	line = line.replace('.','0')
	puzzle = [l+'\n' for l in [line[i:i+9] for i in range(0, len(line.strip()), 9)]]

	for p in puzzle:
		f_out_p.write(p)

	count += 1
	if count >= num_puzzles:
		break


f_in.close()
f_out_p.close()
