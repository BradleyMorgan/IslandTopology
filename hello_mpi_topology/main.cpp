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
const static unsigned int EVALS = 20;
const static unsigned int RUNS = 20;
const static unsigned int TERM = 100;

// data structure for population individuals holding the input values
// as a DIM dimension array of double, the function result, and fitness ...

struct individual {

    std::array<double, DIM> x;
    
    double result;
    double fitness;
    double selection_distribution;
    
};

struct island {
    
    std::vector<individual> population;
    
    double total_fitness() {
        
        double tmp_fitness = 0;
        
        std::vector<individual>::iterator it;
        
        for(it = this->population.begin(); it != this->population.end(); ++it) {
            
            tmp_fitness += it->fitness;
            
        }
        
        return tmp_fitness;
        
    }
    
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
        p.fitness = p.result * -1;
        
        population.push_back(p);
        
    }
    
    return population;
    
}

// create n sub-populations from provided population

std::vector<island> subpopulate(std::vector<individual> population, int n) {
    
    std::vector<island> islands;
    
    int island_size = ((int)population.size() - 1) / n + 1;
    
    for (int i = 0; i<island_size; ++i) {
        
        island isle;
        
        auto start_it = std::next(population.cbegin(), i*n);
        auto end_it = std::next(population.cbegin(), i*n + n);

        isle.population.resize(n);
        
        if (i*n + n > population.size()) {
            end_it = population.cend();
            isle.population.resize(population.size() - i*n);
        }

        std::copy(start_it, end_it, isle.population.begin());
        
        islands.push_back(isle);
        
    }

    return islands;
    
}

// this function calculates the cumulative probability distribution to be used by
// the fitness proportional (roulette wheel) selection ...

std::vector<double> cumulative_probability_distribution(island &isle) {

    std::vector<double> cumulative_probability_distribution;
    
    double cumulative_probability = 0.0;
    
    for(int i=0; i<isle.population.size(); i++) {

        isle.population[i].selection_distribution = (double)isle.population[i].fitness / (double)isle.total_fitness();

        cumulative_probability += isle.population[i].selection_distribution;
        cumulative_probability_distribution.push_back(cumulative_probability);
        
    }
    
    return cumulative_probability_distribution;
    
}

// pick a parent from the population using the provided method ...

individual select_parent(const std::vector<individual> &population, std::vector<double> &cumulative_probability_distribution) {
    
    individual p;

    // implementation uses the single armed roulette wheel approach to select
    // μ parents from the population ...
    
    int i = 1;
    
    double r = ((double)rand()/(double)RAND_MAX);
    
    while (cumulative_probability_distribution[i] < r ) { i++; }
    
    p = population[i];
    
    return p;
        
}

// comparator for parent fitness values ...

bool compare_fitness(const individual &p1, const individual &p2) {
    
    return p1.fitness < p2.fitness;
    
}


void select_survivors(island &isle, std::vector<individual> &children) {

    // truncation: add new children to the population, and then kill the weakest
    
    isle.population.insert(isle.population.end(), children.begin(), children.end());
    
    std::sort(isle.population.begin(), isle.population.end(), compare_fitness);
    std::reverse(isle.population.begin(), isle.population.end());
    
    isle.population.erase(isle.population.begin()+20, isle.population.end());
    
}


std::vector<individual> crossover(std::vector<individual> parents, std::vector<double> &cumulative_probability_distribution) {

    std::vector<individual> children;
    
    for(int i = 0; i < 20; i++) {
        
        individual p1 = select_parent(parents, cumulative_probability_distribution);
        individual p2 = select_parent(parents, cumulative_probability_distribution);
        
        individual child;
        
        child.x[0] = p1.x[1];
        child.x[1] = p2.x[0];
        
        child.result = rastrigin(child.x);
        child.fitness = child.result * -1;
        
        children.push_back(child);
        
    }
    
    return children;
    
}

// select μ parents from the population using a probability based on the
// individual fitness relative to the entire population ...

std::vector<individual> fitness_proportional_selection(const std::vector<individual> &population, std::vector<double> &cumulative_probability_distribution) {
    
    // implementation uses the single armed roulette wheel approach to select
    // μ parents from the population ...
    
    std::vector<individual> parents;
   
    int curr_member = 0;
    
    while(curr_member < 20) {
        
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
    
    std::vector<individual> population = initial_population(100);
    
    for(std::vector<individual>::iterator it = population.begin(); it != population.end(); ++it) {

        printf("%f -> %f\r\n", it->result, it->fitness);

    }
    
    for(int run = 1; run <= RUNS; run++) {
     
        std::vector<island> islands = subpopulate(population, 20);
        
        for(int eval = 1; eval <= EVALS; eval++) {
        
            std::vector<island>::iterator island;
            
            for(island = islands.begin(); island != islands.end(); ++island) {
             
                // get the probability distribution for use with the routette wheel selection ...
                
                std::vector<double> parent_probabilities = cumulative_probability_distribution(*island);
                
                std::vector<individual> parents = fitness_proportional_selection(island->population, parent_probabilities);
                
                std::vector<individual> children = crossover(parents, parent_probabilities);
                
                select_survivors(*island, children);
                
                for(std::vector<individual>::iterator it = island->population.begin(); it != island->population.end(); ++it) {

                    printf("%f -> %f\r\n", it->result, it->fitness);

                }
                
            }
            
        }
        
    }
    
}
