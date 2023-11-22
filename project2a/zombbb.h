//9504853406CBAC39EE89AA3AD238AA12CA198043
using namespace std;
#include <getopt.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <deque>
#include <queue>

struct Zombie{
    string name;
    uint32_t distance;
    uint32_t speed;
    uint32_t health;
    uint32_t rounds_survive;
};

//functor that helps organize the priority queue
    class Prioritykill{
        public:
        bool operator()(const Zombie* zom1, const Zombie* zom2) const {
            uint32_t eta1 = (zom1->distance)/ (zom1->speed); //first sort by eta
            uint32_t eta2 = (zom2->distance)/ (zom2->speed);
            if(eta1 != eta2){
                return eta1 > eta2;
            }
            else if (zom1->health != zom2->health){ // if eta is the same sort by health
                return zom1->health > zom2->health;
            }
            else{ // if they are the same health as well, compare by names
                return (zom1->name > zom2->name);
            }
        }
};


//functors to organize zombies for zombies rounds survived
    class greater_{
        public:
        bool operator()(const uint32_t &a, const uint32_t &b){
           //if a is greater than b, it is higher prioirty   
            return a > b;
        }
    };

    class lower_{
        public:
        bool operator()(const uint32_t &a, const uint32_t &b){
            //if a is a smaller number than b, it is higher prioirty 
            return a < b;
        }
    };
    //functor for the priority queue that holds the
    class longest_survivers{
        public: 
        bool operator()(const Zombie* zom1, const Zombie* zom2) const{
            if(zom1->rounds_survive != zom2->rounds_survive){
                return zom1->rounds_survive < zom2->rounds_survive;
            }
            else{
                return zom1->name > zom2->name;
            }
        }
    };

    class shortest_sur{
        public:
        bool operator()(const Zombie* zom1, const Zombie* zom2) const{
            if(zom1->rounds_survive != zom2->rounds_survive){
                return zom1->rounds_survive > zom2->rounds_survive;
            }
            else{
                return zom1->name > zom2->name;
            }
        }
    };


class Game{
    private:
    
    //command line input bools and option arguments 
        bool verbose = false;
        bool median_on = false; 
        bool stats = false;
        uint32_t num_stat;

    //attributes of the game given by input file
        uint32_t quiver_capacity;
        uint32_t random_seed;
        uint32_t max_rand_distance;
        uint32_t max_rand_speed;
        uint32_t max_rand_health;
        
    //keeps track of the all the zombies and who to kill next
        deque<Zombie*>  masterlist;
        
        priority_queue <Zombie*, vector<Zombie*>, Prioritykill> kill_list;
    
    //keep track of what round we are in and what round is next round we have upcoming zombies again
        uint32_t current_round;
        uint32_t next_round_in;
    
    //player's atrributes
        uint32_t quiver;
        bool alive = true;

    //priority queues for running median
    //upper holds elements where the smallest number is on top
    priority_queue <uint32_t, vector<uint32_t>, greater_> upper;

    //lower holds elements where the largest number is on top
    priority_queue <uint32_t, vector<uint32_t>, lower_> lower;


    //keeps track of all the dead zombies
    deque<Zombie*> killedZombies;

    //dead zombies that gets is ordered by rounds survived
    priority_queue<Zombie*, vector<Zombie*>, longest_survivers> longest_survived;
    
    //the first element will have the longest survivor while the the last will be the shortest survivor
    priority_queue<Zombie*, vector<Zombie*>, shortest_sur> shortest_survivors;

    string zombie_killer = "";

    public:
        void help();
        

        //decide what behavior our program will follow
        void get_options(int argc, char **argv);

        //reading in all and the input provided in the text file
        void read_input();

        //play the game
        void play_game();

        ~Game(){
           for(Zombie* zomb: masterlist){
            delete zomb;
           }

        }
        
        
};