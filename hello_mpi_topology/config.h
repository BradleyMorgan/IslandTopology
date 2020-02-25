//
//  config.h
//  hello_mpi_topology
//
//  Created by Bradley Morgan on 2/24/20.
//  Copyright © 2020 Eight Bit Software Co. All rights reserved.
//

#ifndef config_h
#define config_h

const static unsigned int EVALS = 100;
const static unsigned int RUNS = 10;
const static unsigned int LAMBDA = 10;
const static unsigned int MU = 60;
const static unsigned int ISLANDS = 3;
const static unsigned int IMU = MU / ISLANDS;
const static double MUTATION_RATE = 0.03;

#endif /* config_h */
