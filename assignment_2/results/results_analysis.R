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

# Read vnd results
vnd <- read_csv("~/heuristic-optimization-ulb/assignment_2/results/results_vnd.csv", 
                        col_types = cols(machines = col_skip(), 
                                         initial_state = col_skip(), neighbour_function = col_skip(), random_seed = col_skip(),
                                         neighbour_vector = col_factor(levels = c("tie", "tei")),
                                         use_best_improvement = col_skip()))
# Clean the instance names.
iga$filename <- gsub("./instances/", "", iga$filename)
gen$filename <- gsub("./instances/", "", gen$filename)
vnd$filename <- gsub("./instances/", "", vnd$filename)
gen_l$filename <- gsub("./instances/", "", gen$filename)


# Read ideal solution values
bestSolutions <- read_csv("~/heuristic-optimization-ulb/assignment_2/results/bestSolutions.txt")

# Keep only the tie data for VND
vnd <- vnd[vnd$neighbour_vector == "tie", ]

# Join and clean the tables
iga <- sqldf("select * from iga, bestSolutions where iga.filename = bestSolutions.filename")
gen <- sqldf("select * from gen, bestSolutions where gen.filename = bestSolutions.filename")
vnd <- sqldf("select * from vnd, bestSolutions where vnd.filename = bestSolutions.filename")

# Remove duplicates
iga[, 7:9] <- NULL
gen[, 7:9] <- NULL
vnd[, 7:9] <- NULL

# Add an identifying column, for the algorithm type
iga$params <- paste(iga$algo, iga$dsize, sep=", ")
gen$params <- paste(gen$algo, gen$pop_size, sep=", ")
vnd$params <- paste(vnd$algo, vnd$neighbour_vector, sep=", ")

# Also add the instance name, so we can aggregate the results.
iga$id <- paste(iga$filename, iga$params, sep=" - ")
gen$id <- paste(gen$filename, gen$params, sep=" - ")
vnd$id <- paste(vnd$filename, vnd$params, sep=" - ")

# Aggregate the values
iga_g <- iga %>% dplyr::group_by(filename, jobs, algo, dsize, best_result, params, id) %>% dplyr::summarize(exec_time=median(exec_time), result=median(result))
gen_g <- gen %>% dplyr::group_by(filename, jobs, algo, pop_size, best_result, params, id) %>% dplyr::summarize(exec_time=median(exec_time), result=median(result))
vnd_g <- vnd %>% dplyr::group_by(filename, jobs, algo, neighbour_vector, best_result, params, id) %>% dplyr::summarize(exec_time=median(exec_time), result=median(result))

# Compute arpd
iga_g$arpd <- 100 * (iga_g$result - iga_g$best_result) / iga_g$best_result
gen_g$arpd <- 100 * (gen_g$result - gen_g$best_result) / gen_g$best_result
vnd_g$arpd <- 100 * (vnd_g$result - vnd_g$best_result) / vnd_g$best_result



# Add an identifying column
iga_g$params <- paste(iga_g$algo, iga_g$dsize, sep=", ")
gen_g$params <- paste(gen_g$algo, gen_g$pop_size, sep=", ")
vnd_g$params <- paste(vnd_g$algo, vnd_g$neighbour_vector, sep=", ")

# Build 2 dataframes for plotting
tot <- rbind(iga_g[, c(1:3, 5:10)], gen_g[, c(1:3, 5:10)], vnd_g[, c(1:3, 5:10)])

tot_50 <- subset(tot, jobs==50)
tot_100 <- subset(tot, jobs==100)


###########################
# PLOTS ###################
###########################

# Execution times for vnd_g, iga_g, gen_g
p1 <- ggplot(tot_50, aes(x = reorder(params, -exec_time, FUN=median), exec_time, col=params, fill=params)) +
  geom_boxplot(alpha=0.5) + 
  labs(title ="Execution time, 50 jobs", x = "Algorithm", y = "Execution time [ms]") +
  theme_minimal() +
  theme(legend.position="none")
p2 <- ggplot(tot_50, aes(x = reorder(params, -exec_time, FUN=median), log(exec_time), col=params, fill=params)) +
  geom_boxplot(alpha=0.5) + 
  labs(title ="Execution time, 50 jobs. log y-axis", x = "Algorithm", y = "Execution time, log [ms]") +
  theme_minimal() +
  theme(legend.position="none")
p11 <- ggplot(tot_100, aes(x = reorder(params, -exec_time, FUN=median), exec_time, col=params, fill=params)) +
  geom_boxplot(alpha=0.5) + 
  labs(title ="Execution time, 100 jobs", x = "Algorithm", y = "Execution time [ms]") +
  theme_minimal() +
  theme(legend.position="none")
p22 <- ggplot(tot_100, aes(x = reorder(params, -exec_time, FUN=median), log(exec_time), col=params, fill=params)) +
  geom_boxplot(alpha=0.5) + 
  labs(title ="Execution time, 100 jobs. log y-axis", x = "Algorithm", y = "Execution time, log [ms]") +
  theme_minimal() +
  theme(legend.position="none")

multiplot(p1, p11, p2, p22, cols=2)

# ARPD for vnd_g, iga_g, gen_g
p3 <- ggplot(tot_50, aes(x=reorder(params, -arpd, FUN=median), arpd, col=params, fill=params)) +
  geom_boxplot(alpha=0.5) + 
  labs(title ="ARPD (%), 50 jobs", x = "Algorithm", y = "ARPD, %") +
  theme_minimal() +
  theme(legend.position="none")
p4 <- ggplot(tot_100, aes(x=reorder(params, -arpd, FUN=median), arpd, col=params, fill=params)) +
  geom_boxplot(alpha=0.5) + 
  labs(title ="ARPD (%), 100 jobs", x = "Algorithm", y = "ARPD, %") +
  theme_minimal() +
  theme(legend.position="none")
multiplot(p3, p4, cols=2)

p5 <- ggplot(tot_50[tot_50$algo == "iga", ], aes(x=reorder(params, -arpd, FUN=median), exec_time, col=params, fill=params)) +
  geom_boxplot(alpha=0.5) + 
  labs(title ="IGA: Execution time, 50 jobs", x = "Elements modified in D/C", y = "Execution time [ms]") +
  theme_minimal() +
  theme(legend.position="none")
p6 <- ggplot(tot_50[tot_50$algo == "iga", ], aes(x=reorder(params, -arpd, FUN=median), arpd, col=params, fill=params)) +
  geom_boxplot(alpha=0.5) + 
  labs(title ="IGA: ARPD (%), 50 jobs", x = "Elements modified in D/C", y = "ARPD, %") +
  theme_minimal() +
  theme(legend.position="none")
multiplot(p5, p6, cols=2)

# Wilcoxon and kruskal tests on exec time of iga
d1 <- subset(tot_50, params=="iga, 6")$exec_time
d2 <- subset(tot_50, params=="iga, 8")$exec_time
d3 <- subset(tot_50, params=="iga, 10")$exec_time
d4 <- subset(tot_50, params=="iga, 6")$arpd
d5 <- subset(tot_50, params=="iga, 8")$arpd
d6 <- subset(tot_50, params=="iga, 10")$arpd

k1 <- kruskal.test(list(d1, d2, d3))
w <- wilcox.test(d2, d3)
k
w
k2 <- kruskal.test(list(d4, d5, d6))
w <- wilcox.test(d2, d3)
k
w


res <- data.frame(Type = c("Execution time", "ARPD"), "p-value"=c(k1$p.value, k2$p.value), 
                  "median, 6"=c(median(tot_50[tot_50$params == "iga, 6", ]$exec_time), median(tot_50[tot_50$params == "iga, 6", ]$arpd)),
                  "median, 8"=c(median(tot_50[tot_50$params == "iga, 8", ]$exec_time), median(tot_50[tot_50$params == "iga, 8", ]$arpd)),
                  "median, 10"=c(median(tot_50[tot_50$params == "iga, 10", ]$exec_time), median(tot_50[tot_50$params == "iga, 10", ]$arpd)))
xtable(res,display = c("s", "f", "f", "f", "f", "f"))


# Wilcoxon on genetic
d1 <- subset(tot_50, params=="gen, 10")$exec_time
d2 <- subset(tot_50, params=="gen, 20")$exec_time
d3 <- subset(tot_50, params=="gen, 10")$arpd
d4 <- subset(tot_50, params=="gen, 20")$arpd

w1 <- wilcox.test(d1, d2)
w2 <- wilcox.test(d3, d4)
w1
w2
res <- data.frame(Type = c("Execution time", "ARPD"), "p-value"=c(w1$p.value, w2$p.value), 
                  "median, 10"=c(median(d1), median(d3)),
                  "median, 20"=c(median(d2), median(d4)))
xtable(res,display = c("s", "f", "f", "f", "f"))

##################
# Keep only IGA, 8 (and GEN, 20 in tot_50)
##################
iga_g <- subset(iga_g, dsize == 8)
gen_g <- subset(gen_g, (jobs==100) | (jobs==50 & params=="gen, 20"))

# Build 2 dataframes for plotting
tot <- rbind(iga_g[, c(1:3, 5:10)], gen_g[, c(1:3, 5:10)], vnd_g[, c(1:3, 5:10)])
tot$params <- as.factor(tot$params)
tot_50 <- subset(tot, jobs==50)
tot_100 <- subset(tot, jobs==100)


#################
# SUMMARY STATS #
#################
res <- tot_50 %>% dplyr::group_by(algo) %>% dplyr::summarize(min=min(exec_time), mean=mean(exec_time), median=median(exec_time), max=max(exec_time), sd=sd(exec_time))
xtable(res,display = c("s", "f", "f", "f", "f", "f", "f"))
res <- tot_100 %>% dplyr::group_by(algo) %>% dplyr::summarize(min=min(exec_time), mean=mean(exec_time), median=median(exec_time), max=max(exec_time), sd=sd(exec_time))
xtable(res,display = c("s", "f", "f", "f", "f", "f", "f"))
res <- tot_50 %>% dplyr::group_by(algo) %>% dplyr::summarize(min=min(arpd), mean=mean(arpd), median=median(arpd), max=max(arpd), sd=sd(arpd))
xtable(res,display = c("s", "f", "f", "f", "f", "f", "f"))
res <- tot_100 %>% dplyr::group_by(algo) %>% dplyr::summarize(min=min(arpd), mean=mean(arpd), median=median(arpd), max=max(arpd), sd=sd(arpd))
xtable(res,display = c("s", "f", "f", "f", "f", "f", "f"))

#################
# TESTS #########
#################

# Wilcoxon test on exec time and results
w1 <- wilcox.test(tot_50[tot_50$algo == "iga", ]$exec_time, tot_50[tot_50$algo == "gen", ]$exec_time)
w2 <- wilcox.test(tot_50[tot_50$algo == "iga", ]$arpd, tot_50[tot_50$algo == "gen", ]$arpd)
w1
w2
res <- data.frame(Type = c("Execution time", "ARPD"), "p-value"=c(w1$p.value, w2$p.value), 
                  "Mean, IGA"=c(mean(tot_50[tot_50$algo == "iga", ]$exec_time), mean(tot_50[tot_50$algo == "iga", ]$arpd)),
                  "Mean, GEN"=c(mean(tot_50[tot_50$algo == "gen", ]$exec_time), mean(tot_50[tot_50$algo == "gen", ]$arpd)))
xtable(res,display = c("s", "f", "f", "f", "f"))


w1 <- wilcox.test(tot_100[tot_100$algo == "iga", ]$exec_time, tot_100[tot_100$algo == "gen", ]$exec_time)
w2 <- wilcox.test(tot_100[tot_100$algo == "iga", ]$arpd, tot_100[tot_100$algo == "gen", ]$arpd)
w1
w2
res <- data.frame(Type = c("Execution time", "ARPD"), "p-value"=c(w1$p.value, w2$p.value), 
                  "Mean, IGA"=c(mean(tot_100[tot_100$algo == "iga", ]$exec_time), mean(tot_100[tot_100$algo == "iga", ]$arpd)),
                  "Mean, GEN"=c(mean(tot_100[tot_100$algo == "gen", ]$exec_time), mean(tot_100[tot_100$algo == "gen", ]$arpd)))
xtable(res,display = c("s", "f", "f", "f", "f"))


#################
# CORRELATION PLOTS
#################

# 50, exec
d1 <- subset(tot_50, params=="iga, 8" & log(exec_time) < 10)$exec_time
d2 <- subset(tot_50, params=="gen, 20" & log(exec_time) < 10)$exec_time
p1 <- ggplot(data.frame(iga_exec = d1, gen_exec=d2), aes(iga_exec, gen_exec)) + geom_point(size=3, alpha=0.5, aes(colour = iga_exec^2 + gen_exec^2)) + 
  labs(title ="Correlation plot, Execution time, 50 jobs", x = "IGA, 8 - Exec. time [ms]", y = "GEN, 20 - Exec. time [ms]") +
  theme_minimal() +
  theme(legend.position="none")
p1 <- p1 + geom_smooth(method = lm, formula = y ~ x, alpha=0.2)
p1

t1 <- cor.test(d2, d1)
t1

# 50, arpd
d3 <- subset(tot_50, params=="iga, 8")$arpd
d4 <- subset(tot_50, params=="gen, 20")$arpd
p2 <- ggplot(data.frame(iga_arpd=d3, gen_arpd=d4), aes(iga_arpd, gen_arpd)) + geom_point(size=3, alpha=0.5, aes(colour = iga_arpd^2 + gen_arpd^2)) + 
  labs(title ="Correlation plot, ARPD, 50 jobs", x = "IGA, 8 - ARPD", y = "GEN, 20 - ARPD") +
  theme_minimal() +
  theme(legend.position="none")
p2 <- p2 + geom_smooth(method = lm, formula = y ~ x, alpha=0.2)
p2

t2 <- cor.test(d3, d4)
t2

# 100, exec
d5 <- subset(tot_100, params=="iga, 8")$exec_time
d6 <- subset(tot_100, params=="gen, 10")$exec_time
p3 <- ggplot(data.frame(iga_exec = d5, gen_exec=d6), aes(iga_exec, gen_exec)) + geom_point(size=3, alpha=0.5, aes(colour = iga_exec^2 + gen_exec^2)) + 
  labs(title ="Correlation plot, Execution time, 100 jobs", x = "IGA, 8 - Exec. time [ms]", y = "GEN, 10 - Exec. time [ms]") +
  theme_minimal() +
  theme(legend.position="none")
p3 <- p3 + geom_smooth(method = lm, formula = y ~ x, alpha=0.2)
p3

t3 <- cor.test(d5, d6)
t3

# 100, arpd
d7 <- subset(tot_100, params=="iga, 8")$arpd
d8 <- subset(tot_100, params=="gen, 10")$arpd
p4 <- ggplot(data.frame(iga_arpd=d7, gen_arpd=d8), aes(iga_arpd, gen_arpd)) + geom_point(size=3, alpha=0.5, aes(colour = iga_arpd^2 + gen_arpd^2)) + 
  labs(title ="Correlation plot, ARPD, 100 jobs", x = "IGA, 8 - ARPD", y = "GEN, 10 - ARPD") +
  theme_minimal() +
  theme(legend.position="none")
p4 <- p4 + geom_smooth(method = lm, formula = y ~ x, alpha=0.2)
p4

t4 <- cor.test(d7, d8)
t4


multiplot(p1, p2, p3, p4, cols = 2)

res <- data.frame(Type = c("Execution time, 50", "ARPD, 50", "Execution time, 100", "ARPD, 100"), "Correlation"=c(t1$estimate, t2$estimate, t3$estimate, t4$estimate), "p-value"=c(t1$p.value, t2$p.value, t3$p.value, t4$p.value), 
                  "Mean, IGA"=c(mean(d1), mean(d3), mean(d5), mean(d7)),
                  "Mean, GEN"=c(mean(d2), mean(d4), mean(d6), mean(d8)))
xtable(res,display = c("s", "f", "f", "f", "f", "f"))