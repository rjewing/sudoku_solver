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
# f_out_s = open(out_file+'_sols.txt', 'w+')

for line in f_in:
	break

count = 0
for line in f_in:
	split = line.strip().split(',')
	f_out_p.write(split[0].replace('0', '.')+'\n')
	# f_out_p.write("Grid %d\n" %(count))
	# f_out_s.write("Grid %d\n" %(count))
	# for i in range(0,81,9):
	# 	f_out_p.write(split[0][i:i+9]+'\n')
		# f_out_s.write(split[1][i:i+9]+'\n')
	count += 1
	if count >= num_puzzles:
		break


f_in.close()
f_out_p.close()
# f_out_s.close()
