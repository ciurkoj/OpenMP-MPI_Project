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

std::vector<Particle *> particles = {new Particle(5, 14, 10),  new Particle(7, -8, -14), new Particle(-2, 9, 8),
                                     new Particle(15, -6, 3),  new Particle(12, 4, -5),  new Particle(4, 20, 17),
                                     new Particle(-16, 5, -1), new Particle(-11, 3, 16), new Particle(3, 10, -19),
                                     new Particle(-16, 7, 4)};

void displayCurrentPosition()
{
    int i = 0;
    for (Particle *particle : particles)
    {
        std::cout << particle->currentPosition.convertToString(i += 1) << std::endl;
    }
    i = 0;
    std::cout << std::endl;
}

void moveParticlesInRandomDirection(int type, int pos_alter_var)
{
    switch (type)
    {
    case 1:
    {
#pragma omp parallel for shared(particles) shared(pos_alter_var) schedule(static)
        for (Particle *particle : particles)
        {
            particle->moveParticle(pos_alter_var);
        }
        break;
    }

    case 2:
    {
#pragma omp parallel for shared(particles) shared(pos_alter_var) schedule(dynamic)
        for (Particle *particle : particles)
        {
            particle->moveParticle(pos_alter_var);
        }
        break;
    }
    case 3:
    {
#pragma omp parallel for shared(particles) shared(pos_alter_var) schedule(guided)
        for (Particle *particle : particles)
        {
            particle->moveParticle(pos_alter_var);
        }
        break;
    }
    case 4:
    {
#pragma omp parallel for shared(particles) shared(pos_alter_var) schedule(auto)
        for (Particle *particle : particles)
        {
            particle->moveParticle(pos_alter_var);
        }
        break;
    }
    break;
    }
}

Particle calculateCenterPoint()
{
    int x = 0;
    int y = 0;
    int z = 0;
    for (Particle *particle : particles)
    {
        x += particle->currentPosition.x;
        y += particle->currentPosition.y;
        z += particle->currentPosition.z;
    }
    int vectorSize = particles.size();
    x = x / vectorSize;
    y = y / vectorSize;
    z = z / vectorSize;
    Particle centerPoint(x, y, z);
    // std::vector<int> centerPoint = {x,y,z};
    return centerPoint;
}

double calculateDistance(Particle *particle, Particle *centerPoint){
    int x1 = particle->currentPosition.x;
    int x2 = centerPoint->currentPosition.x;
    int y1 = particle->currentPosition.y;
    int y2 = centerPoint->currentPosition.y;    
    int z1 = particle->currentPosition.z;
    int z2 = centerPoint->currentPosition.z;
    double x_squared = pow(x1-x2, 2.0);
    double y_squared = pow(y1-y2, 2.0);
    double z_squared = pow(z1-z2, 2.0);
    return sqrt(x_squared + y_squared + z_squared);
}

void particleManipulation()
{
    omp_sched_t kind;
    int chunk;
    int pos_alter_var = 2;
    omp_get_schedule(&kind, &chunk);
    std::cout << "Initial particles position:" << std::endl;
    displayCurrentPosition();
    // auto start = std::chrono::high_resolution_clock::now();

    // for loop to iterate n-number steps and dissipate particles
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
    // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop
    // - start); std::cout << "Time taken by iterations: " << duration.count() <<
    // " milliseconds" << std::endl;
    std::cout << "particles at 50th step:" << std::endl;
    displayCurrentPosition();

    // moving particles to the center point
    Particle centerPoint = calculateCenterPoint();
    std::cout << centerPoint.currentPosition.convertToString("The center point is: ") << std::endl;
    pos_alter_var = 2;
    #pragma omp parallel for
    for (int i =0; i<MAX_ITERATIONS ;i++){
        for (Particle *particle : particles)
        {
            particle->moveToTheCenter(&centerPoint,pos_alter_var);
        }
        pos_alter_var += rand() % 6;
        if(i ==24) {
            std::cout << "Particles going to the center point at 25th step:" << std::endl;
            displayCurrentPosition();
        }
    }
    std::cout << "Particles around the center point at 50th step:" << std::endl;
    displayCurrentPosition();
    int n = 1;
    std::cout << "Distance betweeen a particle and the center point:" << std::endl;
    for( Particle *particle : particles){
        
        double distance_to_center = calculateDistance(particle, &centerPoint);
        std::cout<<particle->currentPosition.convertToString(n += 1)<<"   \t distance = "<< (int)distance_to_center <<std::endl;
        n++;
    }

    std::cout << std::endl;
}
