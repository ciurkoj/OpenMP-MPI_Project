#include "particleManipulation.h"

#include <omp.h>
#include <stdlib.h>

#include <chrono>
#include <iostream>
#include <random>
#include <thread>
#include <vector>

#include "Particle.h"

const int MAX_ITERATIONS = 50;

std::vector<Particle *> particles = {new Particle(5, 14, 10), new Particle(7, -8, -14), new Particle(-2, 9, 8),
                                 new Particle(15, -6, 3), new Particle(12, 4, -5), new Particle(4, 20, 17),
                                 new Particle(-16, 5, -1), new Particle(-11, 3, 16), new Particle(3, 10, -19),
                                 new Particle(-16, 7, 4)};



void displayCurrentPosition(){
    int i = 0;
    for (Particle *particle : particles)
    {std::cout << particle->currentPosition.convertToString(i += 1) << std::endl;}
    i = 0;
    std::cout << std::endl;
}

void moveParticlesInRandomDirection(int type, int pos_alter_var){
    switch (type)
    {
        case 1:
        {
#pragma omp parallel for shared(particles) shared(pos_alter_var) schedule(static)
            for (Particle *particle : particles)
            {particle->moveParticle(pos_alter_var);}
            break;
        }

        case 2:
        {
#pragma omp parallel for shared(particles) shared(pos_alter_var) schedule(dynamic)
            for (Particle *particle : particles)
            {particle->moveParticle(pos_alter_var);}
            break;
        }
        case 3:
        {
#pragma omp parallel for shared(particles) shared(pos_alter_var) schedule(guided)
            for (Particle *particle : particles)
            {particle->moveParticle(pos_alter_var);}
            break;
        }
        case 4:
        {
#pragma omp parallel for shared(particles) shared(pos_alter_var) schedule(auto)
            for (Particle *particle : particles)
            {particle->moveParticle(pos_alter_var);}
            break;
        }
        break;
    }
}

std::vector<int> calculateCenterPoint(){
    int x=0;
    int y=0;
    int z=0;
    for(Particle *particle: particles){

        x += particle->currentPosition.x;
        y += particle->currentPosition.y;
        z += particle->currentPosition.z;
    }
    int vectorSize=  particles.size();
    x = x /vectorSize;
    y = y / vectorSize;
    z = z /vectorSize;
    std::vector<int> centerPoint = {x,y,z};
    return centerPoint;
}

void particleManipulation(){
    omp_sched_t kind;
    int chunk;
    int pos_alter_var = 2;
    omp_get_schedule(&kind, &chunk);
    std::cout << "Initial particles position:" << std::endl;
    displayCurrentPosition();
    // auto start = std::chrono::high_resolution_clock::now();

    //for loop to iterate n-number steps
#pragma omp parallel for
    for (int i = 0; i < MAX_ITERATIONS; i++)
    {
        moveParticlesInRandomDirection(kind, pos_alter_var);
        pos_alter_var += rand() % 6;
        if (i == 24)
        {
            std::cout << "particles at 25th step:" << std::endl;
            displayCurrentPosition();
        }
    }
    // auto stop = std::chrono::high_resolution_clock::now();
    // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    // std::cout << "Time taken by iterations: " << duration.count() << " milliseconds" << std::endl;
    std::cout << "particles at 50th step:" << std::endl;
    displayCurrentPosition();
    std::vector<int> centerPoint =calculateCenterPoint();
      std::cout<< "The center Point is: [" <<
      std::to_string(centerPoint.at(0)) << ", " <<
      std::to_string(centerPoint.at(1)) << ", " <<
      std::to_string(centerPoint.at(2)) << "]"<<std::endl;
}
