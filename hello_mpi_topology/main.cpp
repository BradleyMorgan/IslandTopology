//
//  main.cpp
//  hello_mpi_topology
//
//  Created by Ian Morgan on 2/4/20.
//  Copyright © 2020 Eight Bit Software Co. All rights reserved.
//

#include <iostream>
#include <mpi.h>
#include <array>
#include <cmath>
#include <sys/time.h>
#include <vector>

const static unsigned int DIM = 2;

// data structure for population individuals holding the input values
// as a DIM dimension array of double, the function result, and fitness ...

struct individual {

    std::array<double, DIM> x;
    
    double result;
    double fitness;
    double selection_distribution;
    
};

// return a random double between min and max ...

double drand(double min, double max) {
    
    double f = (double)rand() / RAND_MAX;
    
    return min + f * (max - min);
    
}

// the rastrigin function is a non-linear, multimodal function
// with a large search space and a large number of local minima ...

double rastrigin(std::array<double, DIM> x) {
    
    double sum = 10 * DIM;
    
    for (unsigned int i = 0; i < DIM; i++) {
        sum += (std::pow(x[i],2) - (10 * std::cos(2 * M_PI * x[i])));
    }
    
    return sum;
}

// populate an initial population with random inputs ...

std::vector<individual> initial_population(int size) {
    
    std::vector<individual> population;
    
    for(int i=0; i<size; i++) {
        
        individual p;
        
        for (unsigned int i = 0; i < DIM; i++) {
            p.x[i] = drand(-5.12, 5.12);
        }
        
        p.result = rastrigin(p.x);
        
        population.push_back(p);
        
    }
    
    return population;
    
}

std::vector<std::vector<individual>> subpopulate(std::vector<individual> population, int num_islands) {
    
    std::vector<std::vector<individual>> subpop;
    
    int island_size = ((int)population.size() - 1) / num_islands + 1;
    
    std::vector<individual> island[island_size];

    for (int i = 0; i<island_size; ++i) {
        
        auto start_it = std::next(population.cbegin(), i*num_islands);
        auto end_it = std::next(population.cbegin(), i*num_islands + num_islands);

        island[i].resize(num_islands);
        
        if (i*num_islands + num_islands > population.size()) {
            end_it = population.cend();
            island[i].resize(population.size() - i*num_islands);
        }

        std::copy(start_it, end_it, island[i].begin());
        
        subpop.push_back(island[i]);
        
    }

    return subpop;
    
}

// this function calculates the cumulative probability distribution to be used by
// the fitness proportional (roulette wheel) selection ...

std::vector<double> cumulative_probability_distribution(std::vector<individual> &population, int total_fitness) {

    std::vector<double> cumulative_probability_distribution;
    
    double cumulative_probability = 0.0;
    
    for(int i=0; i<population.size(); i++) {

        population[i].selection_distribution = (double)population[i].fitness / (double)total_fitness;

        cumulative_probability += population[i].selection_distribution;
        cumulative_probability_distribution.push_back(cumulative_probability);
        
    }
    
    return cumulative_probability_distribution;
    
}

// select μ parents from the population using a probability based on the
// individual fitness relative to the entire population ...

std::vector<individual> fitness_proportional_selection(const std::vector<individual> &population, std::vector<double> &cumulative_probability_distribution) {
    
    // implementation uses the single armed roulette wheel approach to select
    // μ parents from the population ...
    
    std::vector<individual> parents;
   
    int curr_member = 0;
    
    while(curr_member < 5) {
        
        int i = 1;
        
        double r = ((double)rand()/(double)RAND_MAX);
        
        while (cumulative_probability_distribution[i] < r ) { i++; }
        
        parents.push_back(population[i]);
        
        curr_member++;
        
    }
    
    return parents;
    
}


// main wraps the evolution process ...

int main(int argc, char * argv[]) {
    
    timeval time;
    gettimeofday(&time, NULL);
    srand((unsigned int)(time.tv_sec * 1000) + time.tv_usec);
    
    std::vector<individual> population = initial_population(20);
    
    for(std::vector<individual>::iterator it = population.begin(); it != population.end(); ++it) {
        
        printf("%f\r\n", it->result);
        
    }
    
    std::vector<std::vector<individual>> islands = subpopulate(population, 5);
    
}
