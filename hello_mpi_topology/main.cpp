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
#include "mpi.h"

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
    
    for (int i = 0; i<n; ++i) {
        
        island isle;
        
        isle.id = i;
        
        auto start_it = std::next(population.cbegin(), i*island_size);
        auto end_it = std::next(population.cbegin(), i*island_size + island_size);

        isle.population.resize(island_size);
        
        if (i*n + n > population.size()) {
            end_it = population.cend();
            isle.population.resize(population.size() - i*island_size);
        }

        std::copy(start_it, end_it, isle.population.begin());
        
        islands.push_back(isle);
        
    }

    return islands;
    
}

// main wraps the evolution process ...

int main(int argc, char * argv[]) {
    
    MPI_Init(NULL, NULL);
    
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    
    // random number seeding ...
    
    timeval time;
    gettimeofday(&time, NULL);
    srand((unsigned int)(time.tv_sec * 1000) + time.tv_usec);
    
    for(int run = 1; run <= RUNS; run++) {
        
        // initialize the population with random individuals ...
        
        std::vector<individual> population = initial_population(MU);
        
        // separate the full population into island subpopulations ...
        
        printf("world size %d\r\n", world_size);
        
        std::vector<island> islands = subpopulate(population, world_size);
        
        printf("islands = %lu\r\n", islands.size());
        
        // we want to evolve the population on each island separately, then perform
        // a population migration of n individuals at some interval
        // for each island population, perform the local search evolution using
        // parent selection, crossover, mutation, and survival selection ...
        
        if(world_rank == 0) {
            printf("RUN %d\r\n", run);
        }
        
        create_topology(islands);
        
        for(int eval = 1; eval <= EVALS; eval++) {
        
            std::vector<island>::iterator island;
            
            for(island = islands.begin(); island != islands.end(); ++island) {
             
                if(island->id == world_rank) {
                    
                    island->calc_cpd();
                    
                    std::vector<individual> children = crossover(*island);
                    
                    select_survivors(*island, children);
                    
                    island->send_migrant();
                    
                    island->receive_migrant();
        
                }
                
            }
        
        }
        
        if(world_rank == 0) {
            
            for(std::vector<island>::iterator it = islands.begin(); it != islands.end(); ++it) {

                printf("%d [%f,%f] %f\r\n", it->id, it->population[0].x[0], it->population[0].x[1], it->population[0].fitness);

            }
            
        }
        
    }
    
    MPI_Finalize();
    
}
