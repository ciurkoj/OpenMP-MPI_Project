#include "Particle.h"

#include <stdio.h>
#include <stdlib.h>

#include <chrono>
#include <iostream>
#include <random>
Particle::Particle(int x, int y, int z) : initialPosition(Position(x, y, z)), currentPosition(Position(x, y, z)) {}

void Particle::moveParticle(int pos_alter_var)
{
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::normal_distribution<double> distribution(5.0, 1.0);
    std::vector<char> pos_table = {'x', 'y', 'z'};

    int index = rand() % pos_table.size();
    char coordinate = pos_table[index];

    // then remove selected number from the vector
    pos_table.erase(pos_table.begin() + index);

    // randomly decide whether to increment or decrement the value
    if ((int)distribution(generator) < 5)
    {
        switch (coordinate)
        {
        case 'x':
            this->currentPosition.x += pos_alter_var;
            break;
        case 'y':
            this->currentPosition.y += pos_alter_var;
            break;
        case 'z':
            this->currentPosition.z += pos_alter_var;
            break;
        }

        char coordinate1;  // = table[*select_randomly(table.begin(), table.end())];
                           // //if chosen to add

        // randomly decide whether 2nd index should be selected
        if ((int)distribution(generator) < 5)
        {
            coordinate1 = pos_table[rand() % pos_table.size()];
            switch (coordinate1)
            {
            case 'x':
                this->currentPosition.x -= pos_alter_var;
                break;
            case 'y':
                this->currentPosition.y -= pos_alter_var;
                break;
            case 'z':
                this->currentPosition.z -= pos_alter_var;
                break;
            }
        }
    }
    else
    {
        // change vector posision in the main vector array
        switch (coordinate)
        {
        case 'x':
            this->currentPosition.x -= pos_alter_var;
            break;
        case 'y':
            this->currentPosition.y -= pos_alter_var;
            break;
        case 'z':
            this->currentPosition.z -= pos_alter_var;
            break;
        };
        char coordinate1;  // = table[*select_randomly(table.begin(), table.end())];
                           // //if chosen to add
        if ((int)distribution(generator) < 5)
        {
            coordinate1 = pos_table[rand() % pos_table.size()];
            switch (coordinate1)
            {
            case 'x':
                this->currentPosition.x += pos_alter_var;
                break;
            case 'y':
                this->currentPosition.y += pos_alter_var;
                break;
            case 'z':
                this->currentPosition.z += pos_alter_var;
                break;
            }
        }
    }
}

// Decides in which direction move particles to
int decide(int current_postion, int center_point, int pos_alter_var)
{
    if (current_postion > center_point)return current_postion - pos_alter_var;
    else if (current_postion < center_point) return current_postion + pos_alter_var;
    return current_postion;
}

// Randomly moves particles closer to the center point
void Particle::moveToTheCenter(Particle *center_point, int pos_alter_var)
{
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::normal_distribution<double> distribution(5.0, 1.0);
    if ((int)distribution(generator) < 5){
        this->currentPosition.x = decide(this->currentPosition.x, center_point->currentPosition.x, pos_alter_var);
         if ((int)distribution(generator) < 5){
             this->currentPosition.y = decide(this->currentPosition.y, center_point->currentPosition.y, pos_alter_var);
        } else if((int)distribution(generator) > 5){
            this->currentPosition.z = decide(this->currentPosition.z, center_point->currentPosition.z, pos_alter_var);
        }else{}
    }else{
        this->currentPosition.y = decide(this->currentPosition.y, center_point->currentPosition.y, pos_alter_var);
            if ((int)distribution(generator) < 5){
                this->currentPosition.z = decide(this->currentPosition.z, center_point->currentPosition.z, pos_alter_var);
            }else if ((int)distribution(generator) > 5){
                this->currentPosition.x = decide(this->currentPosition.x, center_point->currentPosition.x, pos_alter_var);
            }else{}
    }
}
