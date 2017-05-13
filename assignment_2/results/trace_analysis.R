rm(list=ls(all=TRUE))

library(plyr)
library(tidyr)
library(ggplot2)
library(sqldf)
library(readr)
library(xtable)

source("multiplot.R")
library(dplyr)

iga <- read_csv("~/heuristic-optimization-ulb/assignment_2/results/results_details_iga.csv", 
                                col_types = cols(dsize = col_skip(), 
                                                 lambda = col_skip(), machines = col_skip(), 
                                                 max_time = col_skip(), plateau = col_skip(), id=col_character()))

gen <- read_csv("~/heuristic-optimization-ulb/assignment_2/results/results_details_gen.csv", 
                                col_types = cols(machines = col_skip(), 
                                                 max_time = col_skip(), mprob = col_skip(), 
                                                 plateau = col_skip(), pop_size = col_skip(), 
                                                 xprob = col_skip(), id=col_character()))

# Read ideal solution values
bestSolutions <- read_csv("~/heuristic-optimization-ulb/assignment_2/results/bestSolutions.txt")
bestSolutions <- subset(bestSolutions, filename %in% c("50_20_01", "50_20_02", "50_20_03", "50_20_04", "50_20_05"))
# Add instance id
bestSolutions$instance <- as.numeric(substr(bestSolutions$filename , nchar(bestSolutions$filename)-1, nchar(bestSolutions$filename)))

# Turn the ids to numerical labels
iga$numid <- factor(iga$id, labels = 1:125)
gen$numid <- factor(gen$id, labels = 1:125)

# Extract the run number (from 1 to 25)
iga$run <- (as.integer(iga$numid) %% 25) + 1 
# Extract the instance number (from 1 to 5)
iga$instance <- ceiling((as.integer(iga$numid) / 25))
# Extract the run number (from 1 to 25)
gen$run <- (as.integer(gen$numid) %% 25) + 1 
# Extract the instance number (from 1 to 5)
gen$instance <- ceiling((as.integer(gen$numid) / 25))

# Aggregate runs based on step size
iga_g <- iga %>% dplyr::group_by(algo, instance, step) %>% dplyr::summarize(res=median(res), time=median(time))
gen_g <- gen %>% dplyr::group_by(algo, instance, step) %>% dplyr::summarize(res=median(res), time=median(time))

# Join and clean the tables
iga_g <- sqldf("select * from iga_g, bestSolutions where iga_g.instance = bestSolutions.instance")
gen_g <- sqldf("select * from gen_g, bestSolutions where gen_g.instance = bestSolutions.instance")

# Remove duplicates
iga_g[, 10] <- NULL
gen_g[, 10] <- NULL


# Compute arpd
iga_g$arpd <- 100 * (iga_g$res - iga_g$best_result) / iga_g$best_result
gen_g$arpd <- 100 * (gen_g$res - gen_g$best_result) / gen_g$best_result



penalty_factor = 2
iga_g$limit <- iga_g$best_result + iga_g$best_result * penalty_factor
iga_g$better <- iga_g$arpd < penalty_factor

iga_prob <- iga_g %>% dplyr::group_by(step) %>% dplyr::summarize(time=median(time), count=sum(better))  


iga_prob$count <- cumsum(iga_prob$count)

gen_g$limit <- gen_g$best_result + gen_g$best_result * penalty_factor
gen_g$better <- gen_g$arpd < penalty_factor 

gen_prob <- gen_g %>% dplyr::group_by(step) %>% dplyr::summarize(time=median(time), count=sum(better))  


gen_prob$count <- cumsum(gen_prob$count)


# Remove noisy data
while(T)
{
  iga_prob <- iga_prob[order(iga_prob$time), ]
  rem = c()
  for(i in 2:nrow(iga_prob))
  {
    if (iga_prob[i, ]$count < iga_prob[i-1, ]$count)
    {
      rem = c(rem, i-1)
    }
    
  }
  if(length(rem)==0)
  {
    break
  }
  iga_prob <- iga_prob[-rem, ]
}


# Remove noisy data
while(T)
{
  gen_prob <- gen_prob[order(gen_prob$time), ]
  rem = c()
  for(i in 2:nrow(gen_prob))
  {
    if (gen_prob[i, ]$count < gen_prob[i-1, ]$count)
    {
      rem = c(rem, i-1)
    }
  }
  if(length(rem)==0)
  {
    break
  }
  gen_prob <- gen_prob[-rem, ]
}


max_count = max(max(gen_prob$count), max(iga_prob$count))

ggplot(data=iga_prob) + geom_line(aes(time, count / max_count), col="#5A9ACC", alpha=0.5, size=1) + geom_line(data=gen_prob, aes(time, count / max_count), color="#A73532", alpha=0.5, size=1)  + 
  labs(title=paste("Run-time distribution, 50 jobs, ARPD threshold =", penalty_factor), x = "Time [ms]", y = "Probability") +
  theme_minimal() +
  theme(legend.position="none") 
