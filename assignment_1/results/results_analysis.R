library(dplyr)
library(plyr)
library(tidyr)
library(ggplot2)
library(sqldf)
library(readr)

source("multiplot.R")






###########################
# READ DATA ###############
###########################

# Read iterative improvement results
results_ii <- read_csv(
      "~/heuristic-optimization-ulb/assignment_1/results/results_ii.csv",
      col_types = cols(
        algorithm = col_skip(),
        initial_state = col_factor(levels = c("random", "rz")),
        neighbour_function = col_factor(levels = c("t", "e", "i")),
        neighbour_vector = col_skip(),
        use_best_improvement = col_factor(levels = c("0", "1"))
      )
    )

# Read vnd results
results_vnd <- read_csv("~/heuristic-optimization-ulb/assignment_1/results/results_vnd.csv", 
                col_types = cols(algorithm = col_skip(), 
                initial_state = col_skip(), neighbour_function = col_skip(), 
                neighbour_vector = col_factor(levels = c("tie", "tei")),
                use_best_improvement = col_skip()))

# Read ideal solution values
bestSolutions <- read_csv("~/heuristic-optimization-ulb/assignment_1/results/bestSolutions.txt")

# Join and clean the tables
results_ii <- sqldf("select * from results_ii, bestSolutions where results_ii.filename = bestSolutions.filename")
results_vnd <- sqldf("select * from results_vnd, bestSolutions where results_vnd.filename = bestSolutions.filename")
# Remove duplicates
results_ii[, 10:12] <- NULL
results_vnd[, 8:10] <- NULL


best_100 <- data.frame(neighbour_vector="best", result=subset(bestSolutions, number_of_jobs==100, select=best_result))
best_100 <- plyr::rename(best_100, replace=c("best_result"="result"))
best_50 <- data.frame(neighbour_vector="best", result=subset(bestSolutions, number_of_jobs==50, select=best_result))
best_50 <- plyr::rename(best_50, replace=c("best_result"="result"))

# Get useful subsets of data for VND
vnd_100_time <- subset(results_vnd, number_of_jobs==100, select=c(neighbour_vector, execution_time.ms))
vnd_50_time <- subset(results_vnd, number_of_jobs==50, select=c(neighbour_vector, execution_time.ms))

vnd_100_res <- subset(results_vnd, number_of_jobs==100, select=c(neighbour_vector, result, best_result))
vnd_50_res <- subset(results_vnd, number_of_jobs==50, select=c(neighbour_vector, result, best_result))

vnd_100_res_best <- rbind(vnd_100_res[, 0:2], best_100)
vnd_50_res_best <- rbind(vnd_50_res[, 0:2], best_50)

# Get useful subsets of data for II
ii_100_time_algo <- subset(results_ii, number_of_jobs==100, select=c(use_best_improvement, execution_time.ms))
ii_50_time_algo <- subset(results_ii, number_of_jobs==50, select=c(use_best_improvement, execution_time.ms))
ii_100_res_algo <- subset(results_ii, number_of_jobs==100, select=c(use_best_improvement, result, best_result))
ii_50_res_algo <- subset(results_ii, number_of_jobs==50, select=c(use_best_improvement, result, best_result))


vnd_100_res_best <- rbind(vnd_100_res[, 0:2], best_100)
vnd_50_res_best <- rbind(vnd_50_res[, 0:2], best_50)

# Merge the combinations of parameters of II into one column
results_ii$initial_params <- paste(results_ii$use_best_improvement, results_ii$initial_state, results_ii$neighbour_function)

ii_100 <- subset(results_ii, number_of_jobs==100)
ii_50 <- subset(results_ii, number_of_jobs==50)


ii_100_res_best <- data.frame(result=ii_100$result, initial_params=ii_100$initial_params)
ii_100_res_best <- rbind(ii_100_res_best, data.frame(result=best_100$result, initial_params="best"))

ii_50_res_best <- data.frame(result=ii_50$result, initial_params=ii_50$initial_params)
ii_50_res_best <- rbind(ii_50_res_best, data.frame(result=best_50$result, initial_params="best"))


###########################
# PLOTS ###################
###########################

# Plot a quick boxplot for execution time and results of vnd
p1 <- ggplot(vnd_100_time, aes(neighbour_vector, execution_time.ms, col=neighbour_vector, fill=neighbour_vector)) +
        geom_boxplot(alpha=0.5) + 
        labs(title ="Execution time, VND, 100 jobs", x = "Neighbourhood vector ", y = "Execution time [ms]") + 
        theme_minimal() +
        scale_color_hue(labels = c("tie", "tei")) +
        scale_x_discrete(labels=c("TIE", "TEI")) +
        theme(legend.position="none")
p3 <- ggplot(vnd_50_time, aes(neighbour_vector, execution_time.ms, col=neighbour_vector, fill=neighbour_vector)) +
        geom_boxplot(alpha=0.5) + 
        labs(title ="Execution time, VND, 50 jobs", x = "Neighbourhood vector ", y = "Execution time [ms]") +
        theme_minimal() + 
        scale_color_hue(labels = c("tie", "tei")) +
        scale_x_discrete(labels=c("TIE", "TEI")) +
        theme(legend.position="none")
p2 <- ggplot(vnd_100_res_best, aes(neighbour_vector, result, col=neighbour_vector, fill=neighbour_vector)) +
        geom_boxplot(alpha=0.5) + 
        labs(title ="Results, VND, 100 jobs", x = "Neighbourhood vector ", y = "Results (lower is better)") +
        theme_minimal() +
        scale_color_hue(labels = c("tie", "tei", "best")) +
        scale_x_discrete(labels=c("TIE", "TEI", "Ideal result")) +
        theme(legend.position="none")
p4 <- ggplot(vnd_50_res_best, aes(neighbour_vector, result, col=neighbour_vector, fill=neighbour_vector)) +
        geom_boxplot(alpha=0.5) + 
        labs(title ="Results, VND, 50 jobs", x = "Neighbourhood vector ", y = "Results (lower is better)") +
        theme_minimal() +
        scale_color_hue(labels = c("tie", "tei", "best")) +
        scale_x_discrete(labels=c("TIE", "TEI", "Ideal result")) +
        theme(legend.position="none")

multiplot(p1, p2, p3, p4, cols=2)

p5 <- ggplot(ii_100, aes(initial_params, execution_time.ms, col=initial_params, fill=initial_params)) +
        geom_boxplot(alpha=0.5) + 
        labs(title ="Execution time, II, 100 jobs", x = "Initial parameters", y = "Execution time [ms]") +
        theme_minimal() +
        scale_x_discrete(labels=c("FI, Random, E", "FI, Random, I", "FI, Random, T", "FI, RZ, E", "FI, RZ, I", "FI, RZ, T", "BI, Random, E", "BI, Random, I", "BI, Random, T", "BI, RZ, E", "BI, RZ, I", "BI, RZ, T")) +
        theme(legend.position="none")
p6 <-ggplot(ii_50, aes(initial_params, execution_time.ms, col=initial_params, fill=initial_params)) +
        geom_boxplot(alpha=0.5) + 
        labs(title ="Execution time, II, 50 jobs", x = "Initial parameters", y = "Execution time [ms]") +
        theme_minimal() +
        scale_x_discrete(labels=c("FI, Random, E", "FI, Random, I", "FI, Random, T", "FI, RZ, E", "FI, RZ, I", "FI, RZ, T", "BI, Random, E", "BI, Random, I", "BI, Random, T", "BI, RZ, E", "BI, RZ, I", "BI, RZ, T")) +
        theme(legend.position="none")

multiplot(p5, p6, cols=1)


p7 <- ggplot(ii_100_res_best, aes(initial_params, result, col=initial_params, fill=initial_params)) +
        geom_boxplot(alpha=0.5) + 
        labs(title ="Results, II, 100 jobs", x = "Initial parameters", y = "Results (lower is better)") +
        theme_minimal() +
        scale_x_discrete(labels=c("FI, Random, E", "FI, Random, I", "FI, Random, T", "FI, RZ, E", "FI, RZ, I", "FI, RZ, T", "BI, Random, E", "BI, Random, I", "BI, Random, T", "BI, RZ, E", "BI, RZ, I", "BI, RZ, T", "Ideal result")) +
        theme(legend.position="none")
p8 <-ggplot(ii_50_res_best, aes(initial_params, result, col=initial_params, fill=initial_params)) +
        geom_boxplot(alpha=0.5) + 
        labs(title ="Results, II, 50 jobs", x = "Initial parameters", y = "Results (lower is better)") +
        theme_minimal() +
        scale_x_discrete(labels=c("FI, Random, E", "FI, Random, I", "FI, Random, T", "FI, RZ, E", "FI, RZ, I", "FI, RZ, T", "BI, Random, E", "BI, Random, I", "BI, Random, T", "BI, RZ, E", "BI, RZ, I", "BI, RZ, T", "Ideal result")) +
        theme(legend.position="none")

multiplot(p7, p8, cols=1)


###########################
# STATISTICAL TESTS #######
###########################

# Is there a statistical difference between tie and tei?
t1 <- t.test(subset(results_vnd, neighbour_vector=="tie" & number_of_jobs==100, select=execution_time.ms),
       subset(results_vnd, neighbour_vector=="tei" & number_of_jobs==100, select=execution_time.ms))
t2 <- t.test(subset(results_vnd, neighbour_vector=="tie" & number_of_jobs==50, select=execution_time.ms),
             subset(results_vnd, neighbour_vector=="tei" & number_of_jobs==50, select=execution_time.ms))
t3 <- summary(lm(result ~ neighbour_vector, data=vnd_100_res_best))
t4 <- summary(lm(result ~ neighbour_vector, data=vnd_50_res_best))

t1
t2
t3
t4


# ANOVA on all the II execution times
t5 <- summary(lm(execution_time.ms ~ initial_params, data=ii_100))
t6 <- summary(lm(execution_time.ms ~ initial_params, data=ii_50))
t5
t6

# ANOVA on all the II results (including the ideal one)
t7 <- summary(lm(result ~ initial_params, data=ii_100))
t8 <- summary(lm(result ~ initial_params, data=ii_50))
t7
t8

# Both show different mean, let's do paired t-test with bonferroni correction.

# 1.1: RZ vs RANDOM EXEC TIME
d1 <- subset(ii_50, initial_params=="1 rz i", select=execution_time.ms)
d2 <- subset(ii_50, initial_params=="1 random i", select=execution_time.ms)
n1 <- shapiro.test(d1$execution_time.ms)
n2 <- shapiro.test(d2$execution_time.ms)
t <- t.test(d1, d2)
w <- wilcox.test(d1$execution_time.ms,d2$execution_time.ms)
n1
n2
t
w

# 1.1: RZ vs RANDOM RESULT
d1 <- subset(ii_50, initial_params=="1 rz i", select=execution_time.ms)
d2 <- subset(ii_50, initial_params=="1 random i", select=execution_time.ms)
n1 <- shapiro.test(d1$execution_time.ms)
n2 <- shapiro.test(d2$execution_time.ms)
t <- t.test(d1, d2)
w <- wilcox.test(d1$execution_time.ms,d2$execution_time.ms)
n1
n2
t
w

# 
# # 1.2: RZ vs RANDOM VS BEST RES
# d1 <- subset(ii_100, initial_params=="0 rz i", select=result)
# d2 <- subset(ii_100, initial_params=="0 random i", select=result)
# d3 <- best_100$result
# n1 <- shapiro.test(d1$result)
# n2 <- shapiro.test(d2$result)
# n3 <- shapiro.test(d3)
# data1 <- data.frame(type="0 rz i", result=d1$result)
# data2 <- data.frame(type="0 random i", result=d2$result)
# data3 <- data.frame(type="best", result=d3)
# data <- rbind(data1, data2, data3)
# a <- summary(lm(result ~ type, data))
# n1
# n2
# n3
# a


# 2.1 FI VS BI RESULTS AND EXEC TIME
tests_1 = c("0 rz t", "0 rz e", "0 rz i", "0 random t", "0 random e", "0 random i")
tests_2 = c("1 rz t", "1 rz e", "1 rz i", "1 random t", "1 random e", "1 random i")
for (i in 1:6) {
  d1 <- subset(ii_100, initial_params==tests_1[i], select=result)
  d2 <- subset(ii_100, initial_params==tests_2[i], select=result)
  n1 <- shapiro.test(d1$result)
  n2 <- shapiro.test(d2$result)
  t <- t.test(d1, d2)
  w <- wilcox.test(d1$result, d2$result)
  print("----------------")
  print(n1)
  print(n2)
  print(t)
  print(w)
  print("----------------")
}



# 3.1: T VStests_1 = c("0 rz t", "1 rz e", "0 random t", "1 random t")
tests_1 = c("0 rz t", "1 rz e", "0 random t", "1 random t") 
tests_2 = c("0 rz e", "1 rz e", "0 random e", "1 random e")
tests_3 = c("0 rz i", "1 rz e", "0 random i", "1 random t")
for (i in 1:4) {
  d1 <- subset(ii_50, initial_params==tests_1[i], select=result)
  d2 <- subset(ii_50, initial_params==tests_2[i], select=result)
  d3 <- subset(ii_50, initial_params==tests_3[i], select=result)
  n1 <- shapiro.test(d1$result)
  n2 <- shapiro.test(d2$result)
  n3 <- shapiro.test(d3$result)
  kruskal.test(d1$result, d2$result, d3$result)
  print("----------------")
  print(n1)
  print(n2)
  print(n3)
  print(t)
  #print(w)
  print("----------------")
}


# 3.2 E VS I
tests_1 = c("0 rz e", "1 rz e", "0 random e", "1 random e")
tests_2 = c("0 rz i", "1 rz i", "0 random i", "1 random i")
for (i in 1:4) {
  d1 <- subset(ii_50, initial_params==tests_1[i], select=result)
  d2 <- subset(ii_50, initial_params==tests_2[i], select=result)
  n1 <- shapiro.test(d1$result)
  n2 <- shapiro.test(d2$result)
  t <- t.test(d1, d2)
  w <- wilcox.test(d1$result, d2$result)
  print("----------------")
  print(n1)
  print(n2)
  print(t)
  print(w)
  print("----------------")
}


