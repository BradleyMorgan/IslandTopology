//
//  island.h
//  hello_mpi_topology
//
//  Created by Bradley Morgan on 2/24/20.
//  Copyright © 2020 Eight Bit Software Co. All rights reserved.
//

#ifndef island_h
#define island_h

#include "mpi.h"

const static unsigned int DIM = 2;

// data structure for population individuals holding the input values
// as a DIM dimension array of double, the function result, and fitness ...

struct individual {

    std::array<double, DIM> x;
    
    double result = 0.0;
    double fitness = 0.0;
    double selection_distribution = 0.0;
    
};

// island data structure to hold subpopulations and corresponding
// properties ...

struct island {
    
    int id = 0;
    double total_fitness = 0;
    
    std::vector<double> cpd;
    std::vector<individual> population;
    std::vector<island> senders;
    std::vector<island> receivers;
    
    // calculate the island's total fitness for distribution ...
    
    void calc_total_fitness() {

        this->total_fitness = 0;
        
        std::vector<individual>::iterator it;
        
        for(it = this->population.begin(); it != this->population.end(); ++it) {
            this->total_fitness += it->fitness;
        }
        
    }
    
    // this function calculates the cumulative probability distribution to be used by
    // the fitness proportional (roulette wheel) selection ...
    
    void calc_cpd() {
        
        this->calc_total_fitness();
        this->cpd.clear();
        
        double cumulative_probability = 0.0;
        
        for(int i=0; i<this->population.size(); i++) {

            this->population[i].selection_distribution = (double)this->population[i].fitness / this->total_fitness;

            cumulative_probability += this->population[i].selection_distribution;
            this->cpd.push_back(cumulative_probability);
            
        }
        
    }
    
    void receive_migrant() {
        
        std::array<double, DIM> x;
        
        MPI_Status migrant_status;
        
        MPI_Recv(&x, 2, MPI_DOUBLE, this->senders[0].id, 0, MPI_COMM_WORLD, &migrant_status);
        
        //printf("island %d received migrant from island %d: [%f,%f] with status %d\r\n", this->id, migrant_status.MPI_SOURCE, x[0], x[1], migrant_status.MPI_ERROR);
        
        this->population[0].x = x;
        
    }
    
    void send_migrant() {
        
        MPI_Send(&this->population[0].x, 2, MPI_DOUBLE, this->receivers[0].id, 0, MPI_COMM_WORLD);
        
        //printf("island %d sending migrant to island %d: [%f,%f]\r\n", this->id, this->receivers[0].id, this->population[0].x[0], this->population[0].x[1]);
        
    }
    
    
};

void create_topology(std::vector<island> &islands) {
    
    std::vector<island>::iterator it;
    
    for(it = islands.begin(); it != islands.end(); ++it) {
        
        int next = it->id+1 < islands.size() ? it->id+1 : 0;
        int prev = it->id-1 < 0 ? (int)islands.size()-1 : it->id-1;
        
        it->receivers.push_back(islands[next]);
        it->senders.push_back(islands[prev]);
        
    }
    
}

#endif /* island_h */
