rm(list=ls(all=TRUE))

library(plyr)
library(tidyr)
library(ggplot2)
library(sqldf)
library(readr)
library(xtable)

source("multiplot.R")
library(dplyr)

###########################
# READ DATA ###############
###########################


# Read iga results
iga <- read_csv("~/heuristic-optimization-ulb/assignment_2/results/results_iga.csv", 
                col_types = cols(machines = col_skip(), lambda = col_skip(), 
                                 max_time = col_skip(), seed = col_skip()))


# Read iga results
iga_l <- read_csv("~/heuristic-optimization-ulb/assignment_2/results/results_iga_long.csv", 
                  col_types = cols(machines = col_skip(), lambda = col_skip(), 
                                   max_time = col_skip(), seed = col_skip()))

# Read genetic results
gen <- read_csv("~/heuristic-optimization-ulb/assignment_2/results/results_gen.csv", 
                col_types = cols(machines = col_skip(), max_time = col_skip(), 
                                 mprob = col_skip(), mtype = col_skip(), 
                                 seed = col_skip(), wtype = col_skip(), 
                                 xprob = col_skip()))
# Read genetic results
gen_l <- read_csv("~/heuristic-optimization-ulb/assignment_2/results/results_gen_long.csv", 
                col_types = cols(machines = col_skip(), max_time = col_skip(), 
                                 mprob = col_skip(), mtype = col_skip(), 
                                 seed = col_skip(), wtype = col_skip(), 
                                 xprob = col_skip()))

# Clean the instance names.
iga$filename <- gsub("./instances/", "", iga$filename)
gen$filename <- gsub("./instances/", "", gen$filename)
iga_l$filename <- gsub("./instances/", "", iga_l$filename)
gen_l$filename <- gsub("./instances/", "", gen_l$filename)


# Read ideal solution values
bestSolutions <- read_csv("~/heuristic-optimization-ulb/assignment_2/results/bestSolutions.txt")



# Join and clean the tables
iga <- sqldf("select * from iga, bestSolutions where iga.filename = bestSolutions.filename")
gen <- sqldf("select * from gen, bestSolutions where gen.filename = bestSolutions.filename")
iga_l <- sqldf("select * from iga_l, bestSolutions where iga_l.filename = bestSolutions.filename")
gen_l <- sqldf("select * from gen_l, bestSolutions where gen_l.filename = bestSolutions.filename")

# Remove duplicates
iga[, 7:9] <- NULL
iga_l[, 7:9] <- NULL
gen[, 7:9] <- NULL
gen_l[, 7:9] <- NULL

# Add an identifying column, for the algorithm type
iga$params <- paste(iga$algo, iga$dsize, sep=", ")
iga_l$params <- paste(iga_l$algo, iga_l$dsize, sep=", ")
gen$params <- paste(gen$algo, gen$pop_size, sep=", ")
gen_l$params <- paste(gen_l$algo, gen_l$pop_size, sep=", ")

# Also add the instance name, so we can aggregate the results.
iga$id <- paste(iga$filename, iga$params, sep=" - ")
iga_l$id <- paste(iga_l$filename, iga_l$params, sep=" - ")
gen$id <- paste(gen$filename, gen$params, sep=" - ")
gen_l$id <- paste(gen_l$filename, gen_l$params, sep=" - ")

# Aggregate the values
iga_g <- iga %>% dplyr::group_by(filename, jobs, algo, dsize, best_result, params, id) %>% dplyr::summarize(exec_time=median(exec_time), result=median(result))
iga_gl <- iga_l %>% dplyr::group_by(filename, jobs, algo, dsize, best_result, params, id) %>% dplyr::summarize(exec_time=median(exec_time), result=median(result))
gen_g <- gen %>% dplyr::group_by(filename, jobs, algo, pop_size, best_result, params, id) %>% dplyr::summarize(exec_time=median(exec_time), result=median(result))
gen_gl <- gen_l %>% dplyr::group_by(filename, jobs, algo, pop_size, best_result, params, id) %>% dplyr::summarize(exec_time=median(exec_time), result=median(result))

# Compute arpd
iga_g$arpd <- 100 * (iga_g$result - iga_g$best_result) / iga_g$best_result
iga_gl$arpd <- 100 * (iga_gl$result - iga_gl$best_result) / iga_gl$best_result
gen_g$arpd <- 100 * (gen_g$result - gen_g$best_result) / gen_g$best_result
gen_gl$arpd <- 100 * (gen_gl$result - gen_gl$best_result) / gen_gl$best_result



# Add an identifying column
iga_g$params <- paste(iga_g$algo, iga_g$dsize, sep=", ")
iga_gl$params <- paste(iga_gl$algo, iga_gl$dsize, sep=", ")
gen_g$params <- paste(gen_g$algo, gen_g$pop_size, sep=", ")
gen_gl$params <- paste(gen_gl$algo, gen_gl$pop_size, sep=", ")

wilcox.test(subset(iga_g, params=="iga, 8" & jobs==50)$arpd, iga_gl$arpd)
wilcox.test(subset(gen_g, params=="gen, 20" & jobs==50)$arpd, gen_gl$arpd)
