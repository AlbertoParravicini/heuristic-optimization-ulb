import os
from random import randint

from os import listdir
from os.path import isfile, join

instances_path = "./instances"
instances = [f for f in listdir(instances_path) if isfile(join(instances_path, f))]

instances = instances[:30]

print(instances)



crossover_rate = ["0.99"]
population_size = ["10"]
weights = ["uni"]


for instance in instances:
	rand_seed = randint(0, 1000000)
	for c in crossover_rate:	
		for p in population_size:
			for w in weights:
				command_input = "./build/flowshop_gen -f ./instances/" + instance + " -r " + str(rand_seed) + " -c " + c + " -p " + p + " -w " + w + " - e 0"
				print("\n\nEXECUTING:", command_input)
				os.system(command_input)

