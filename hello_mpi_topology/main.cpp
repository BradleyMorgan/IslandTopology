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
#include "island.h"
#include "evolution.h"

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
        
        isle.id = i;
        
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

// main wraps the evolution process ...

int main(int argc, char * argv[]) {
    
    // random number seeding ...
    
    timeval time;
    gettimeofday(&time, NULL);
    srand((unsigned int)(time.tv_sec * 1000) + time.tv_usec);
    
    for(int run = 1; run <= RUNS; run++) {
    
        // initialize the population with random individuals ...
        
        std::vector<individual> population = initial_population(MU);
        
        for(std::vector<individual>::iterator it = population.begin(); it != population.end(); ++it) {

            printf("[%f,%f] %f -> %f\r\n", it->x[0], it->x[1], it->result, it->fitness);

        }
        
        // separate the full population into island subpopulations ...
        
        std::vector<island> islands = subpopulate(population, ISLANDS);
        
        // we want to evolve the population on each island separately, then perform
        // a population migration of n individuals at some interval ...
        
        // for each island population, perform the local search evolution using
        // parent selection, crossover, mutation, and survival selection ...
        
        for(int eval = 1; eval <= EVALS; eval++) {
        
            std::vector<island>::iterator island;
            
            for(island = islands.begin(); island != islands.end(); ++island) {
             
                printf("\r\nIsland %d\r\n", island->id);
                
                island->calc_cpd();
                
                // create lambda offspring, parent selection ...
                
                std::vector<individual> children = crossover(*island);
                
                island->calc_cpd();
                
                select_survivors(*island, children);
                
                for(std::vector<individual>::iterator it = island->population.begin(); it != island->population.end(); ++it) {

                    printf("[%f,%f] %f -> %f\r\n", it->x[0], it->x[1], it->result, it->fitness);

                }
                
                
                
            }
            
        }
        
    }
    
}
