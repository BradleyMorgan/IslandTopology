//
//  config.h
//  hello_mpi_topology
//
//  Created by Bradley Morgan on 2/24/20.
//  Copyright © 2020 Eight Bit Software Co. All rights reserved.
//

#ifndef config_h
#define config_h

const static unsigned int EVALS = 2000;
const static unsigned int RUNS = 30;
const static unsigned int LAMBDA = 5;
const static unsigned int MU = 20;
const static unsigned int ISLANDS = 4;
const static unsigned int IMU = MU / ISLANDS;
const static double MUTATION_RATE = 0.03;

#endif /* config_h */
