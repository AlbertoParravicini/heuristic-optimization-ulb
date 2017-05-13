import os
from random import randint

from os import listdir
from os.path import isfile, join

instances_path = "./instances"
instances = [f for f in listdir(instances_path) if isfile(join(instances_path, f))]

instances = instances[30:35]
print(instances)


distr_vec_size = ["8"]
lambda_temp = "2"


for instance in instances:
	for i in range(25):
		rand_seed = randint(0, 1000000)	
		print("\n\n INSTANCE:", instance, " -- RUN: ", i, "\n\n")
		for d in distr_vec_size:	
			command_input = "./build/flowshop_iga -f ./instances/" + instance + " -r " + str(rand_seed) + " -d " + d + " -l " + lambda_temp + " -e 1 -t 1000000"
			print("\n\nEXECUTING:", command_input)
			os.system(command_input)

