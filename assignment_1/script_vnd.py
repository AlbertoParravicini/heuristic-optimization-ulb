import os
from random import randint

from os import listdir
from os.path import isfile, join

instances_path = "./instances"
instances = [f for f in listdir(instances_path) if isfile(join(instances_path, f))]
print(instances)



vec_neigh_list = ["tei", "tie"]
for instance in instances:
	for i in range(5):
		rand_seed = randint(0, 1000000)	
		print("\n\n INSTANCE:", instance, " -- RUN: ", i, "\n\n")
		for v in vec_neigh_list:	
			command_input = "./build/flowshop -f ./instances/" + instance + " -a vnd -r " + str(rand_seed) + " -v " + v
			print("\n\nEXECUTING:", command_input)
			os.system(command_input)