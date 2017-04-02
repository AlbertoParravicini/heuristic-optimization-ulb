import os
from random import randint

from os import listdir
from os.path import isfile, join

instances_path = "./instances"
instances = [f for f in listdir(instances_path) if isfile(join(instances_path, f))]

instances = instances[:30]

print(instances)


# start_list = ["random", "rz"]
# best_improvement = ["0", "1"]
# neigh_list = ["t", "e", "i"]

start_list = ["rz"]
best_improvement = ["0"]
neigh_list = ["e"]


for instance in instances:
	rand_seed = randint(0, 1000000)
	for b in best_improvement:	
		for i in start_list:
			for n in neigh_list:
				command_input = "./build/flowshop -f ./instances/" + instance + " -a ii -r " + str(rand_seed) + " -n " + n + " -i " + i + " -b " + b
				print("\n\nEXECUTING:", command_input)
				os.system(command_input)

