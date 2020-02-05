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

struct individual {

    std::array<double, DIM> x;
    
    double result;
    double fitness;
    
};

double drand(double min, double max) {
    
    double f = (double)rand() / RAND_MAX;
    
    return min + f * (max - min);
    
}

double rastrigin(std::array<double, DIM> x) {
    
    double sum = 10 * DIM;
    
    for (unsigned int i = 0; i < DIM; i++) {
        sum += (std::pow(x[i],2) - (10 * std::cos(2 * M_PI * x[i])));
    }
    
    return sum;
}

std::vector<individual> initial_population() {
    
    std::vector<individual> population;
    
    for(int i=0; i<=20; i++) {
        
        individual p;
        
        for (unsigned int i = 0; i < DIM; i++) {
            p.x[i] = drand(-5.12, 5.12);
        }
        
        p.result = rastrigin(p.x);
        
        population.push_back(p);
        
    }
    
    return population;
    
}


int main(int argc, char * argv[]) {
    
    timeval time;
    gettimeofday(&time, NULL);
    srand((unsigned int)(time.tv_sec * 1000) + time.tv_usec);
    
    std::vector<individual> population = initial_population();
    
    for(std::vector<individual>::iterator it = population.begin(); it != population.end(); ++it) {
        
        printf("%f\r\n", it->result);
        
    }
    
}
