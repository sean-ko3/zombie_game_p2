//9504853406CBAC39EE89AA3AD238AA12CA198043
#include "P2random.h"
#include "zombbb.h"

void Game::help(){
    cout << "This game is a game zombies, it will read for an input file and simulate a game\n"; //!!! NEEDS WORK
    return;
} 
void Game::get_options(int argc, char **argv){
        int choice;
        int option_index = 0;

        option long_options[] = {
            {"verbose",    no_argument,       nullptr, 'v'},
            {"median",     no_argument,       nullptr, 'm'},
            {"help",       no_argument,       nullptr, 'h'},
            {"statistics", required_argument, nullptr, 's'},
         };
            //long options

        while((choice = getopt_long(argc, argv, "vmhs:", long_options, &option_index)) != -1){
            switch(choice){
                case 'h':
                    help();
                    exit(0);
                    
                case 'v':
                    verbose = true;
                    break;
                    
                case 'm':
                    median_on = true;
                    break;
                    
                case 's':
                    stats = true;
                    num_stat = static_cast<uint32_t>(stoul(optarg));
                    break;
                    
                default:
                    cerr << "Invalid input! \n";
                    exit(1);

     
            }
        }
    }

void Game::read_input(){
            //GET LINE we can ignore this first line of commnent  
        string comment;
        getline(cin, comment);

        cin >> comment >> quiver_capacity;
        cin >> comment >> random_seed;
        cin >> comment >> max_rand_distance;
        cin >> comment >> max_rand_speed;
        cin >> comment >> max_rand_health;

        
        }

void Game::play_game(){
     string extras; //
     string extras1;
     current_round = 1; // we as a player always start at round 1
     cin >> extras >>  extras1 >> next_round_in; //we read in next_round because not every file starts with round 1 having zombies
     
     
     P2random::initialize(random_seed, max_rand_distance, max_rand_speed, max_rand_health);

    //seeding the upper and lower priority queues for running median
    upper.push(UINT32_MAX);
    lower.push(0);

     while(!cin.fail() || alive){
        //1. if verbose then print round
        if(verbose){
            cout << "Round: " << current_round << '\n';
        }

        //2. refill quiver
        quiver = quiver_capacity; 
        
        //3. updating zombies
        if(masterlist.size() != 0){
            for(Zombie* zombie: masterlist){
                //i. moving zombies closer and update the amount of rounds survived
                if(zombie->health > 0){
                    zombie->distance -= min(zombie->distance, zombie->speed);
                
                    //updating rounds
                    zombie->rounds_survive++;

                    //ii. if verbose then you print the information of the zombie updated
                    if(verbose){
                        cout << "Moved: " << zombie->name << " (distance: " << zombie->distance
                            << ", speed: " << zombie->speed << ", health: " << zombie->health << ")\n";
                    }
                
                    //iii. when you die --> zombie is 0 distance away from you
                    if(zombie->distance == 0 && alive == true){
                        //3 & 4. print demise message but you still update every other zombie
                        zombie_killer = zombie->name;
                        alive = false;
                    }
                }
            }
        }
        //Exit and go straight to statistics if defeated
        if(alive == false){
            cout << "DEFEAT IN ROUND " << current_round << "! " << zombie_killer << " ate your brains!\n";
           break;
        }

        //5. New zombies appear
        //i. Randoms
        //only create zombies when we approached the next round specified on the text
        if(current_round == next_round_in){
            uint32_t num_randoms; 
            cin >> extras >> num_randoms;
            for(uint32_t i = 0; i < num_randoms; i++){
                Zombie *zombie = new Zombie;
                zombie->rounds_survive = 1;
                zombie->name = P2random::getNextZombieName();
                zombie->distance = P2random::getNextZombieDistance();
                zombie->speed = P2random::getNextZombieSpeed();
                zombie->health = P2random::getNextZombieHealth();

                kill_list.push(zombie);
                masterlist.push_back(zombie);
                

                if(verbose){
                    cout << "Created: " << zombie->name << " (distance: " << zombie->distance <<
                            ", speed: " << zombie->speed << ", health: " << zombie->health << ")\n"; 
                }

            }
            
            //ii. Named Zombies
            string named_zombie;
            string labels;
            cin >> extras >> extras1 >> named_zombie;
            while(named_zombie != "---" && !cin.fail()){
                Zombie *named_zombie_ptr = new Zombie;
                named_zombie_ptr->rounds_survive = 1;
                named_zombie_ptr->name = named_zombie;
                cin >> labels >> named_zombie_ptr->distance;
                cin >> labels >> named_zombie_ptr->speed;
                cin >> labels >> named_zombie_ptr->health;
                
                kill_list.push(named_zombie_ptr);
                masterlist.push_back(named_zombie_ptr);       
                
                if(verbose){
                    cout << "Created: " << named_zombie_ptr->name << " (distance: " << named_zombie_ptr->distance <<
                            ", speed: " << named_zombie_ptr->speed << ", health: " << named_zombie_ptr->health << ")\n"; 
                }
                cin >> named_zombie;

            }
        }
        //iii. verbose is implemented to print useful messages within the loop
        //6. Player shoot 
        while(quiver != 0 && !kill_list.empty()){
            
            Zombie* priority_zomb = kill_list.top();
            if(priority_zomb->health <= quiver){
                quiver = quiver - priority_zomb->health;
                priority_zomb->health = 0;

                if(kill_list.top()->rounds_survive >= upper.top()){
                    upper.push(kill_list.top()->rounds_survive);
                }
                else{
                    lower.push(kill_list.top()->rounds_survive);
                }

                if(upper.size() - lower.size() == 2){
                    lower.push(upper.top());
                    upper.pop();
                }
                else if(lower.size() - upper.size() == 2){
                    upper.push(lower.top());
                    lower.pop();
                }
                
                if(verbose){
                    cout << "Destroyed: " << kill_list.top()->name << " (distance: " << kill_list.top()->distance
                         << ", speed: " << kill_list.top()->speed << ", health: " << kill_list.top()->health << ")";
                    cout << "\n";
                }

                kill_list.pop();
                killedZombies.push_back(priority_zomb);
            }
            else{
                priority_zomb->health -= quiver;
                quiver = 0;
            }
            
            
        }
  
        //7. median flag        
        if(median_on && !killedZombies.empty()){
            //running median
            uint32_t running_med;
            if(upper.size() == lower.size()){
                running_med = static_cast<uint32_t>((upper.top() + lower.top()) / 2.0);

            }
            else if(upper.size() > lower.size()){
                running_med = upper.top();
            }
            else{
                running_med = lower.top();
            }

            cout << "At the end of round " << current_round << ", the median zombie lifetime is " << running_med << "\n";
        }

        
        if(current_round == next_round_in){
        cin >> extras >> next_round_in;
        }
        if(kill_list.empty() && current_round >= next_round_in){
            break;
        }
        current_round++;

     }//while
    if(alive){
        cout << "VICTORY IN ROUND " <<  current_round << "! " << killedZombies.back()->name << " was the last zombie.\n";
    }
     //outside of the reading loop now
     //cout statistics if flag is in command
    if(stats){
        //cout number of alive zombies
        cout << "Zombies still active: " << kill_list.size() << "\n";
        uint32_t stats_output = min(num_stat, static_cast<uint32_t> (masterlist.size()));
        
        uint32_t deadzombscount = min(stats_output, static_cast<uint32_t>(killedZombies.size()));
        //first stats_ouput names that were killed
        cout << "First zombies killed:\n";
       for(uint32_t i = 0; i < deadzombscount; i++){
            cout << killedZombies[i]->name << " " << i + 1 << "\n";
        }
       
       //last stats_output names that were killed 
       cout << "Last zombies killed:\n";
        for(uint32_t i = 0; i < deadzombscount; i++){
            cout << killedZombies[killedZombies.size() - i - 1]->name << " " << deadzombscount - i << "\n";
        }
        //put all the zombies in priority queue that sorts by rounds survived
        for(Zombie* zom: masterlist){
            longest_survived.push(zom);
            shortest_survivors.push(zom);
        }
        

       //the stats_output zombies that survived the longest 
        cout << "Most active zombies:\n";
        for (uint32_t i = 0; i < stats_output; i++){
            cout << longest_survived.top()->name << " " << longest_survived.top()->rounds_survive << "\n";
            //update shortest survivors along the way because you need it for the last stats output 
            longest_survived.pop();
        }
     
       cout << "Least active zombies:\n";
       for(uint32_t i = 0; i < stats_output; i++){  
            cout << shortest_survivors.top()->name << " " << shortest_survivors.top()->rounds_survive << "\n";
            shortest_survivors.pop();
       }
    }
     

}
        
int main(int argc, char **argv){
    Game zombie_game;
    
    zombie_game.get_options(argc, argv);
    zombie_game.read_input();
    zombie_game.play_game();

    return 0;
}