//
//  island.h
//  hello_mpi_topology
//
//  Created by Bradley Morgan on 2/24/20.
//  Copyright © 2020 Eight Bit Software Co. All rights reserved.
//

#ifndef island_h
#define island_h

const static unsigned int DIM = 2;

// data structure for population individuals holding the input values
// as a DIM dimension array of double, the function result, and fitness ...

struct individual {

    std::array<double, DIM> x;
    
    double result = 0.0;
    double fitness = 0.0;
    double selection_distribution = 0.0;
    
};

struct island {
    
    unsigned int id = 0;
    double total_fitness = 0;
    std::vector<individual> population;
    std::vector<double> cpd;
    std::vector<island> neighbors;
    
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
        
        printf("Receiving migrant from island %d: [%f,%f]\r\n", this->neighbors[0].id, this->neighbors[0].population[0].x[0], this->neighbors[0].population[0].x[1]);
        
        this->population[0] = this->neighbors[0].population[0];
        
    }
    
};

void create_topology(std::vector<island> &islands) {
    
    std::vector<island>::iterator it;
    
    for(it = islands.begin(); it != islands.end(); ++it) {
        
        int next = it->id+1;
        
        if(next < islands.size()) {
            it->neighbors.push_back(islands[next]);
        } else {
            next = 0;
            it->neighbors.push_back(islands[next]);
        }
        
        printf("%d -> %d\r\n", it->id, next);
        
    }
    
}

#endif /* island_h */
