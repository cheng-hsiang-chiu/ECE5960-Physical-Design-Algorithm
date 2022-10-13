#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <ctime>
#include <iostream>
#include <doctest.h>
#include <string>
#include <unordered_map>
#include <variant>
#include <list>
#include <iterator>
#include <set>
#include "sp.hpp"
#include <algorithm>
#include <climits>


/*
 *
 * Blocks of benchmarks/1.block 
 *   Outline: 600 1200 
 *   NumBlocks: 5                                                                   
 *   NumTerminals: 3
 *   bk1 200  200
 *   bk2 600  600
 *   bk3 400  200
 *   bk4 400  400
 *   bk5 200  400
 *   VSS terminal 1281 1463 
 *   VDD terminal 1687 0    
 *   P9 terminal 266 0       
 *
 * Nets of benchmarks/1.net
 *   NumNets: 1
 *   NetDegree: 4
 *   bk1
 *   bk2
 *   bk3
 *   bk4
 *
 * alpha = 0.6
 */

namespace fp {



class SPTest : public SP {
public:
  SPTest() {
    alpha = 0.6;

    Block b1;
    Block b2;
    Block b3;
    Block b4;
    Block b5;
    
    Terminal t1;
    Terminal t2;
    Terminal t3;
    
    Net n;
    
    b1.name = "bk1";
    b2.name = "bk2";
    b3.name = "bk3";
    b4.name = "bk4";
    b5.name = "bk5";
    b1.width  = 200;
    b1.height = 200;    
    b2.width  = 600;
    b2.height = 600;    
    b3.width  = 400;
    b3.height = 200;    
    b4.width  = 400;
    b4.height = 400;
    b5.width  = 200;
    b5.height = 400;
   
    t1.name = "VSS";
    t2.name = "VDD";
    t3.name = "P9";
    t1.pos_x = 1281;
    t1.pos_y = 1463;
    t2.pos_x = 1687;
    t2.pos_y = 0;
    t3.pos_x = 266;
    t3.pos_y = 0;
   
    num_blocks = 5;
    num_terminals = 3;
    num_nets = 1;

    map_blocks[b1.name] = b1;
    map_blocks[b2.name] = b2;
    map_blocks[b3.name] = b3;
    map_blocks[b4.name] = b4;
    map_blocks[b5.name] = b5;

    map_terminals[t1.name] = t1;
    map_terminals[t2.name] = t2;
    map_terminals[t3.name] = t3;
    
    std::variant<Block*, Terminal*> v1 = &(map_blocks["bk1"]);
    std::variant<Block*, Terminal*> v2 = &(map_blocks["bk2"]);
    std::variant<Block*, Terminal*> v3 = &(map_blocks["bk3"]);
    std::variant<Block*, Terminal*> v4 = &(map_blocks["bk4"]);
    n.net.emplace_back(v1);
    n.net.emplace_back(v2);
    n.net.emplace_back(v3);
    n.net.emplace_back(v4);
    
    vec_nets.emplace_back(n);

    outline_width  = 600;
    outline_height = 1200;
  
    source.name = "source";
    terminus.name = "terminus";  
  }
};


// verify the initialization of positive and negative sequences
TEST_CASE("verify_initialize_sequence" * doctest::timeout(600)) {
  SPTest sptest;

  sptest.initialize_sequence();

  //sptest.dump(std::cout);

  REQUIRE(sptest.positive_sequence.size() == sptest.num_blocks);
  REQUIRE(sptest.negative_sequence.size() == sptest.num_blocks);

  std::vector<std::string> blocks;
  size_t index = 0;
  // check positive_sequence
  for (auto& sq : sptest.positive_sequence) {
    REQUIRE(sq->idx_positive_sequence == index++);
    blocks.push_back(sq->name);
  }

  std::vector<std::string>::iterator it;
  for (it = blocks.begin(); it != blocks.end();) {
    if (sptest.map_blocks.find(*it) != sptest.map_blocks.end()) {
      it =  blocks.erase(it);
    }
    else {
      ++it;
    }
  }
  REQUIRE(blocks.size() == 0);
  
  // check negative_sequence
  index = 0;
  for (auto& sq : sptest.negative_sequence) {
    REQUIRE(sq->idx_negative_sequence == index++);
    blocks.push_back(sq->name);
  }

  for (it = blocks.begin(); it != blocks.end();) {
    if (sptest.map_blocks.find(*it) != sptest.map_blocks.end()) {
      it =  blocks.erase(it);
    }
    else {
      ++it;
    }
  }
  REQUIRE(blocks.size() == 0);
}


// verify relative locations
TEST_CASE("verify_relative_locations" * doctest::timeout(600)) {
  SPTest sptest;
  sptest.initialize_sequence();

  sptest.source.rightof.clear();
  sptest.terminus.rightof.clear();
  sptest.map_blocks["bk1"].rightof.clear();
  sptest.map_blocks["bk2"].rightof.clear();
  sptest.map_blocks["bk3"].rightof.clear();
  sptest.map_blocks["bk4"].rightof.clear();
  sptest.map_blocks["bk5"].rightof.clear();

  sptest.source.aboveof.clear();
  sptest.terminus.aboveof.clear();
  sptest.map_blocks["bk1"].aboveof.clear();
  sptest.map_blocks["bk2"].aboveof.clear();
  sptest.map_blocks["bk3"].aboveof.clear();
  sptest.map_blocks["bk4"].aboveof.clear();
  sptest.map_blocks["bk5"].aboveof.clear();
  
  // positive sequence = [bk1, bk2, bk3, bk4, bk5]
  // negative sequence = [bk1, bk2, bk3, bk4, bk5]
  SUBCASE("SUB : case 1 with nomove") { 
    sptest.positive_sequence[0] = &(sptest.map_blocks["bk1"]);
    sptest.map_blocks["bk1"].idx_positive_sequence = 0;
    sptest.positive_sequence[1] = &(sptest.map_blocks["bk2"]);
    sptest.map_blocks["bk2"].idx_positive_sequence = 1;
    sptest.positive_sequence[2] = &(sptest.map_blocks["bk3"]);
    sptest.map_blocks["bk3"].idx_positive_sequence = 2;
    sptest.positive_sequence[3] = &(sptest.map_blocks["bk4"]);
    sptest.map_blocks["bk4"].idx_positive_sequence = 3;
    sptest.positive_sequence[4] = &(sptest.map_blocks["bk5"]);
    sptest.map_blocks["bk5"].idx_positive_sequence = 4;
    
    sptest.negative_sequence[0] = &(sptest.map_blocks["bk1"]);
    sptest.map_blocks["bk1"].idx_negative_sequence = 0;
    sptest.negative_sequence[1] = &(sptest.map_blocks["bk2"]);
    sptest.map_blocks["bk2"].idx_negative_sequence = 1;
    sptest.negative_sequence[2] = &(sptest.map_blocks["bk3"]);
    sptest.map_blocks["bk3"].idx_negative_sequence = 2;
    sptest.negative_sequence[3] = &(sptest.map_blocks["bk4"]);
    sptest.map_blocks["bk4"].idx_negative_sequence = 3;
    sptest.negative_sequence[4] = &(sptest.map_blocks["bk5"]);
    sptest.map_blocks["bk5"].idx_negative_sequence = 4;

    sptest.construct_relative_locations(0, 4, MoveType::nomove);
  
    for (auto& [key1, value1] : sptest.map_blocks) {
      for (auto& [key2, value2] : sptest.map_blocks) {
        if (key1 != key2) {
          REQUIRE(value1.aboveof.size() == 1);
          REQUIRE(value1.aboveof[0] == &sptest.terminus);
          
          auto found = std::find(
            value1.rightof.begin(),
            value1.rightof.end(),
            &value2);
          
          if (key1 == "bk1") {
            REQUIRE(found != value1.rightof.end());
          }

          else if (key1 == "bk2") {
            if (key2 == "bk1") {
              REQUIRE(found == value1.rightof.end());
            }
            else {
              REQUIRE(found != value1.rightof.end());
            }
          }

          else if (key1 == "bk3") {
            if (key2 == "bk1" || key2 == "bk2") {
              REQUIRE(found == value1.rightof.end());
            }
            else {
              REQUIRE(found != value1.rightof.end());
            }
          }

          else if (key1 == "bk4") {
            if (key2 == "bk5") {
              REQUIRE(found != value1.rightof.end());
            }
            else {
              REQUIRE(found == value1.rightof.end());
            }
          }

          else {
            REQUIRE(found == value1.rightof.end());
          }
        }
      }
    }

    // check rightof and aboveof of source and teminus blocks
    for (auto& [key, value] : sptest.map_blocks) {
      auto found1 = std::find(
        sptest.source.rightof.begin(),
        sptest.source.rightof.end(),
        &value
      );

      auto found2 = std::find(
        sptest.source.aboveof.begin(),
        sptest.source.aboveof.end(),
        &value
      );

      auto found3 = std::find(
        value.rightof.begin(),
        value.rightof.end(),
        &sptest.terminus
      );
      
      auto found4 = std::find(
        value.aboveof.begin(),
        value.aboveof.end(),
        &sptest.terminus
      );

      REQUIRE(found1 != sptest.source.rightof.end());
      REQUIRE(found2 != sptest.source.aboveof.end());
      REQUIRE(found3 != value.rightof.end());
      REQUIRE(found4 != value.aboveof.end());
    }
    REQUIRE(sptest.terminus.rightof.size() == 0);
    REQUIRE(sptest.terminus.aboveof.size() == 0);
  } 

  SUBCASE("SUB : case 1 with move1") { 
    sptest.positive_sequence[0] = &(sptest.map_blocks["bk1"]);
    sptest.map_blocks["bk1"].idx_positive_sequence = 0;
    sptest.positive_sequence[1] = &(sptest.map_blocks["bk2"]);
    sptest.map_blocks["bk2"].idx_positive_sequence = 1;
    sptest.positive_sequence[2] = &(sptest.map_blocks["bk3"]);
    sptest.map_blocks["bk3"].idx_positive_sequence = 2;
    sptest.positive_sequence[3] = &(sptest.map_blocks["bk4"]);
    sptest.map_blocks["bk4"].idx_positive_sequence = 3;
    sptest.positive_sequence[4] = &(sptest.map_blocks["bk5"]);
    sptest.map_blocks["bk5"].idx_positive_sequence = 4;
    
    sptest.negative_sequence[0] = &(sptest.map_blocks["bk1"]);
    sptest.map_blocks["bk1"].idx_negative_sequence = 0;
    sptest.negative_sequence[1] = &(sptest.map_blocks["bk2"]);
    sptest.map_blocks["bk2"].idx_negative_sequence = 1;
    sptest.negative_sequence[2] = &(sptest.map_blocks["bk3"]);
    sptest.map_blocks["bk3"].idx_negative_sequence = 2;
    sptest.negative_sequence[3] = &(sptest.map_blocks["bk4"]);
    sptest.map_blocks["bk4"].idx_negative_sequence = 3;
    sptest.negative_sequence[4] = &(sptest.map_blocks["bk5"]);
    sptest.map_blocks["bk5"].idx_negative_sequence = 4;

    sptest.construct_relative_locations(0, 4, MoveType::nomove);
  
    auto pair_idx = sptest.move1();
    std::cout << "pair_idx = " << pair_idx.first << "," << pair_idx.second << '\n';

    sptest.construct_relative_locations(
      pair_idx.first,
      pair_idx.second,
      MoveType::move1
    );
   
    REQUIRE(sptest.source.rightof.size() == 5);
    REQUIRE(sptest.source.aboveof.size() == 5);
    
    if (pair_idx.first == 0 && pair_idx.second == 1) {
      for (auto& [key, value] : sptest.map_blocks) {
        if (key == "bk1") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");  
            REQUIRE(blk->name != "bk2");  
          }
          REQUIRE(value.rightof.size() == 4);

          REQUIRE(value.aboveof.size() == 2);
          REQUIRE(value.aboveof[0]->name == "bk2");
        }
        else if (key == "bk2") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");  
            REQUIRE(blk->name != "bk2");  
          }
          REQUIRE(value.rightof.size() == 4);
          
          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }
        else if (key == "bk3") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1"); 
            REQUIRE(blk->name != "bk2"); 
            REQUIRE(blk->name != "bk3"); 
          }
          REQUIRE(value.rightof.size() == 3);

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }
        else if (key == "bk4") {
          REQUIRE(value.rightof[0]->name == "bk5");
          REQUIRE(value.rightof.size() == 2);
          
          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }
        else {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }
      }
    }
    else if (pair_idx.first == 0 && pair_idx.second == 2) {
      for (auto& [key, value] : sptest.map_blocks) {
        if (key == "bk1") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");
            REQUIRE(blk->name != "bk2");
            REQUIRE(blk->name != "bk3");
          }
          REQUIRE(value.rightof.size() == 3);
          
          for (auto& blk : value.aboveof) {
            REQUIRE(blk->name != "bk1");
            REQUIRE(blk->name != "bk4");
            REQUIRE(blk->name != "bk5");
          }
          REQUIRE(value.aboveof.size() == 3);
        }

        else if (key == "bk2") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");
            REQUIRE(blk->name != "bk2");
            REQUIRE(blk->name != "bk3");
          }
          REQUIRE(value.rightof.size() == 3);
        
          REQUIRE(value.aboveof.size() == 2);
          REQUIRE(value.aboveof[0]->name == "bk3");
        }

        else if (key == "bk3") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");
            REQUIRE(blk->name != "bk2");
            REQUIRE(blk->name != "bk3");
          }
          REQUIRE(value.rightof.size() == 3);
          
          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk4") {
          REQUIRE(value.rightof[0]->name == "bk5");
          REQUIRE(value.rightof.size() == 2);
          
          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");
          
          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }
      }
    }

    else if (pair_idx.first == 0 && pair_idx.second == 3) {
      for (auto& [key, value] : sptest.map_blocks) {
        if (key == "bk1") {
          REQUIRE(value.rightof.size() == 2);
          REQUIRE(value.rightof[0]->name == "bk5");

          for (auto& blk : value.aboveof) {
            REQUIRE(blk->name != "bk1");
            REQUIRE(blk->name != "bk5");
          }
          REQUIRE(value.aboveof.size() == 4);
        }
        
        else if (key == "bk2") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");
            REQUIRE(blk->name != "bk2");
            REQUIRE(blk->name != "bk4");
          }
          REQUIRE(value.rightof.size() == 3);
        
          REQUIRE(value.aboveof.size() == 2);
          REQUIRE(value.aboveof[0]->name == "bk4");
        }

        else if (key == "bk3") {
          REQUIRE(value.rightof.size() == 2);
          REQUIRE(value.rightof[0]->name == "bk5");

          REQUIRE(value.aboveof.size() == 2);
          REQUIRE(value.aboveof[0]->name == "bk4");
        }

        else if (key == "bk4") {
          REQUIRE(value.rightof.size() == 2);
          REQUIRE(value.rightof[0]->name == "bk5");

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }
      }
    }
    
    else if (pair_idx.first == 0 && pair_idx.second == 4) {
      for (auto& [key, value] : sptest.map_blocks) {
        if (key == "bk1") {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");

          for (auto& blk : value.aboveof) {
            REQUIRE(blk->name != "bk1");
          }
          REQUIRE(value.aboveof.size() == 5);
        }

        else if (key == "bk2") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");
            REQUIRE(blk->name != "bk2");
            REQUIRE(blk->name != "bk5");
          }
          REQUIRE(value.rightof.size() == 3);
        
          REQUIRE(value.aboveof[0]->name == "bk5");
          REQUIRE(value.aboveof.size() == 2);
        }

        else if (key == "bk3") {
          REQUIRE(value.rightof[0]->name == "bk4");
          REQUIRE(value.rightof.size() == 2);

          REQUIRE(value.aboveof[0]->name == "bk5");
          REQUIRE(value.aboveof.size() == 2);
        }

        else if (key == "bk4") {
          REQUIRE(value.rightof[0]->name == "terminus");
          REQUIRE(value.rightof.size() == 1);

          REQUIRE(value.aboveof[0]->name == "bk5");
          REQUIRE(value.aboveof.size() == 2);
        }

        else {
          REQUIRE(value.rightof[0]->name == "terminus");
          REQUIRE(value.rightof.size() == 1);
          
          REQUIRE(value.aboveof[0]->name == "terminus");
          REQUIRE(value.aboveof.size() == 1);
        }
      }
    }

    else if (pair_idx.first == 1 && pair_idx.second == 2) {
      for (auto& [key, value] : sptest.map_blocks) {
        if (key == "bk1") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");
          }
          REQUIRE(value.rightof.size() == 5);
        
          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk2") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");
            REQUIRE(blk->name != "bk2");
            REQUIRE(blk->name != "bk3");
          }
          REQUIRE(value.rightof.size() == 3);
        
          REQUIRE(value.aboveof.size() == 2);
          REQUIRE(value.aboveof[0]->name == "bk3");
        }

        else if (key == "bk3") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");
            REQUIRE(blk->name != "bk2");
            REQUIRE(blk->name != "bk3");
          }
          REQUIRE(value.rightof.size() == 3);
         
          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus"); 
        }

        else if (key == "bk4") {
          REQUIRE(value.rightof[0]->name == "bk5");
          REQUIRE(value.rightof.size() == 2);

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus"); 
        }

        else {
          REQUIRE(value.rightof[0]->name == "terminus");
          REQUIRE(value.rightof.size() == 1);

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus"); 
        }
      }
    }

    else if (pair_idx.first == 1 && pair_idx.second == 3) {
      for (auto& [key, value] : sptest.map_blocks) {
        if (key == "bk1") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");
          }
          REQUIRE(value.rightof.size() == 5);

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk2") {
          REQUIRE(value.rightof.size() == 2);
          REQUIRE(value.rightof[0]->name == "bk5");
        
          for (auto& blk : value.aboveof) {
            REQUIRE(blk->name != "bk1"); 
            REQUIRE(blk->name != "bk2"); 
            REQUIRE(blk->name != "bk5"); 
          }
          REQUIRE(value.aboveof.size() == 3);
        }

        else if (key == "bk3") {
          REQUIRE(value.rightof.size() == 2);
          REQUIRE(value.rightof[0]->name == "bk5");
          
          REQUIRE(value.aboveof.size() == 2);
          REQUIRE(value.aboveof[0]->name == "bk4");
        }

        else if (key == "bk4") {
          REQUIRE(value.rightof.size() == 2);
          REQUIRE(value.rightof[0]->name == "bk5");

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }
      }
    }

    else if (pair_idx.first == 1 && pair_idx.second == 4) {
      for (auto& [key, value] : sptest.map_blocks) {
        if (key == "bk1")  {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");
          }
          REQUIRE(value.rightof.size() == 5);
        
          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk2") {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");
         
          for (auto& blk : value.aboveof) {
            REQUIRE(blk->name != "bk1");
            REQUIRE(blk->name != "bk2");
          }
          REQUIRE(value.aboveof.size() == 4);
        }

        else if (key == "bk3") {
          REQUIRE(value.rightof.size() == 2);
          REQUIRE(value.rightof[0]->name == "bk4");
        
          REQUIRE(value.aboveof.size() == 2);
          REQUIRE(value.aboveof[0]->name == "bk5");
        }

        else if (key == "bk4") {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");

          REQUIRE(value.aboveof.size() == 2);
          REQUIRE(value.aboveof[0]->name == "bk5");
        }

        else {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }
      }
    }

    else if (pair_idx.first == 2 && pair_idx.second == 3) {
      for (auto& [key, value] : sptest.map_blocks) {
        if (key == "bk1") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");
          }
          REQUIRE(value.rightof.size() == 5);
        
          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk2") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");
            REQUIRE(blk->name != "bk2");
          }
          REQUIRE(value.rightof.size() == 4);
         
          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk3") {
          REQUIRE(value.rightof.size() == 2);
          REQUIRE(value.rightof[0]->name == "bk5");
        
          REQUIRE(value.aboveof.size() == 2);
          REQUIRE(value.aboveof[0]->name == "bk4");
        }

        else if (key == "bk4") {
          REQUIRE(value.rightof.size() == 2);
          REQUIRE(value.rightof[0]->name == "bk5");
         
          REQUIRE(value.aboveof.size() == 1); 
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else {
          REQUIRE(value.rightof.size() == 1); 
          REQUIRE(value.rightof[0]->name == "terminus");
          
          REQUIRE(value.aboveof.size() == 1); 
          REQUIRE(value.aboveof[0]->name == "terminus");
        }
      }
    }

    else if (pair_idx.first == 2 && pair_idx.second == 4) {
      for (auto& [key, value] : sptest.map_blocks) {
        if (key == "bk1") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");
          }
          REQUIRE(value.rightof.size() == 5);
        
          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk2") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");
            REQUIRE(blk->name != "bk2");
          }
          REQUIRE(value.rightof.size() == 4);
        
          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk3") {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");
          
          for (auto& blk : value.aboveof) {
            REQUIRE(blk->name != "bk1");
            REQUIRE(blk->name != "bk2");
            REQUIRE(blk->name != "bk3");
          }
          REQUIRE(value.aboveof.size() == 3);
        }

        else if (key == "bk4") {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");
    
          REQUIRE(value.aboveof.size() == 2);
          REQUIRE(value.aboveof[0]->name == "bk5");
        }

        else {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");
    
          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }
      }
    }
  
    else if (pair_idx.first == 3 && pair_idx.second == 4) {
      for (auto& [key, value] : sptest.map_blocks) {
        if (key == "bk1") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");
          }
          REQUIRE(value.rightof.size() == 5);
        
          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk2") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");
            REQUIRE(blk->name != "bk2");
          }
          REQUIRE(value.rightof.size() == 4);
        
          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk3") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");
            REQUIRE(blk->name != "bk2");
            REQUIRE(blk->name != "bk3");
          }
          REQUIRE(value.rightof.size() == 3);

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }
    
        else if (key == "bk4") {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");
          
          REQUIRE(value.aboveof.size() == 2);
          REQUIRE(value.aboveof[0]->name == "bk5");
        }

        else {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");
          
          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }
      }
    }
  }
  
  // positive sequence = [bk1, bk3, bk2, bk5, bk4]
  // negative sequence = [bk4, bk1, bk3, bk5, bk2]
  SUBCASE("SUB : case 2 with nomove") { 
    sptest.positive_sequence[0] = &(sptest.map_blocks["bk1"]);
    sptest.map_blocks["bk1"].idx_positive_sequence = 0;
    sptest.positive_sequence[1] = &(sptest.map_blocks["bk3"]);
    sptest.map_blocks["bk3"].idx_positive_sequence = 1;
    sptest.positive_sequence[2] = &(sptest.map_blocks["bk2"]);
    sptest.map_blocks["bk2"].idx_positive_sequence = 2;
    sptest.positive_sequence[3] = &(sptest.map_blocks["bk5"]);
    sptest.map_blocks["bk5"].idx_positive_sequence = 3;
    sptest.positive_sequence[4] = &(sptest.map_blocks["bk4"]);
    sptest.map_blocks["bk4"].idx_positive_sequence = 4;
    
    sptest.negative_sequence[0] = &(sptest.map_blocks["bk4"]);
    sptest.map_blocks["bk4"].idx_negative_sequence = 0;
    sptest.negative_sequence[1] = &(sptest.map_blocks["bk1"]);
    sptest.map_blocks["bk1"].idx_negative_sequence = 1;
    sptest.negative_sequence[2] = &(sptest.map_blocks["bk3"]);
    sptest.map_blocks["bk3"].idx_negative_sequence = 2;
    sptest.negative_sequence[3] = &(sptest.map_blocks["bk5"]);
    sptest.map_blocks["bk5"].idx_negative_sequence = 3;
    sptest.negative_sequence[4] = &(sptest.map_blocks["bk2"]);
    sptest.map_blocks["bk2"].idx_negative_sequence = 4;

    sptest.construct_relative_locations(0, 4, MoveType::nomove);
  
    for (auto& [key1, value1] : sptest.map_blocks) {
      for (auto& [key2, value2] : sptest.map_blocks) {
        if (key1 != key2) {
          auto found1 = std::find(
            value1.rightof.begin(),
            value1.rightof.end(),
            &value2);
          
          auto found2 = std::find(
            value1.aboveof.begin(),
            value1.aboveof.end(),
            &value2);
          
          if (key1 == "bk1") {
            REQUIRE(found2 == value1.aboveof.end());
            if (key2 == "bk4") {
              REQUIRE(found1 == value1.rightof.end());
            }
            else {
              REQUIRE(found1 != value1.rightof.end());
            }
          }

          else if (key1 == "bk2") {
            REQUIRE(found2 == value1.aboveof.end());
            REQUIRE(found1 == value1.rightof.end());
          }

          else if (key1 == "bk3") {
            REQUIRE(found2 == value1.aboveof.end());
            if (key2 == "bk2" || key2 == "bk5") {
              REQUIRE(found1 != value1.rightof.end());
            }
            else {
              REQUIRE(found1 == value1.rightof.end());
            }
          }

          else if (key1 == "bk4") {
            REQUIRE(found1 == value1.rightof.end());
            REQUIRE(found2 != value1.aboveof.end());
          }

          else {
            if (key2 == "bk2") {
              REQUIRE(found2 != value1.aboveof.end());
            }
            else {
              REQUIRE(found2 == value1.aboveof.end());
            }
            REQUIRE(found1 == value1.rightof.end());
          }
        }
      }
    }
    // check rightof and aboveof of source and teminus blocks
    for (auto& [key, value] : sptest.map_blocks) {
      auto found1 = std::find(
        sptest.source.rightof.begin(),
        sptest.source.rightof.end(),
        &value
      );

      auto found2 = std::find(
        sptest.source.aboveof.begin(),
        sptest.source.aboveof.end(),
        &value
      );

      auto found3 = std::find(
        value.rightof.begin(),
        value.rightof.end(),
        &sptest.terminus
      );
      
      auto found4 = std::find(
        value.aboveof.begin(),
        value.aboveof.end(),
        &sptest.terminus
      );

      REQUIRE(found1 != sptest.source.rightof.end());
      REQUIRE(found2 != sptest.source.aboveof.end());
      REQUIRE(found3 != value.rightof.end());
      REQUIRE(found4 != value.aboveof.end());
    }
    REQUIRE(sptest.terminus.rightof.size() == 0);
    REQUIRE(sptest.terminus.aboveof.size() == 0);
  }
  
  SUBCASE("SUB : case 2 with move2") { 
    sptest.positive_sequence[0] = &(sptest.map_blocks["bk1"]);
    sptest.map_blocks["bk1"].idx_positive_sequence = 0;
    sptest.positive_sequence[1] = &(sptest.map_blocks["bk3"]);
    sptest.map_blocks["bk3"].idx_positive_sequence = 1;
    sptest.positive_sequence[2] = &(sptest.map_blocks["bk2"]);
    sptest.map_blocks["bk2"].idx_positive_sequence = 2;
    sptest.positive_sequence[3] = &(sptest.map_blocks["bk5"]);
    sptest.map_blocks["bk5"].idx_positive_sequence = 3;
    sptest.positive_sequence[4] = &(sptest.map_blocks["bk4"]);
    sptest.map_blocks["bk4"].idx_positive_sequence = 4;
    
    sptest.negative_sequence[0] = &(sptest.map_blocks["bk4"]);
    sptest.map_blocks["bk4"].idx_negative_sequence = 0;
    sptest.negative_sequence[1] = &(sptest.map_blocks["bk1"]);
    sptest.map_blocks["bk1"].idx_negative_sequence = 1;
    sptest.negative_sequence[2] = &(sptest.map_blocks["bk3"]);
    sptest.map_blocks["bk3"].idx_negative_sequence = 2;
    sptest.negative_sequence[3] = &(sptest.map_blocks["bk5"]);
    sptest.map_blocks["bk5"].idx_negative_sequence = 3;
    sptest.negative_sequence[4] = &(sptest.map_blocks["bk2"]);
    sptest.map_blocks["bk2"].idx_negative_sequence = 4;

    sptest.construct_relative_locations(0, 4, MoveType::nomove);
    auto pair_idx = sptest.move2();
    std::cout << "pair_idx = " << pair_idx.first << "," << pair_idx.second << '\n';

    sptest.construct_relative_locations(
      pair_idx.first,
      pair_idx.second,
      MoveType::move2
    );
   
    REQUIRE(sptest.source.rightof.size() == 5);
    REQUIRE(sptest.source.aboveof.size() == 5);
    
    if (pair_idx.first == 0 && pair_idx.second == 1) {
      for (auto& [key, value] : sptest.map_blocks) {
        if (key == "bk1") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");
          }
          REQUIRE(value.rightof.size() == 5);
        
          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk2") {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");
          
          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk3") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");
            REQUIRE(blk->name != "bk3");
            REQUIRE(blk->name != "bk4");
          }
          REQUIRE(value.rightof.size() == 3);

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk4") {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");
          
          for (auto& blk : value.aboveof) {
            REQUIRE(blk->name != "bk1");
            REQUIRE(blk->name != "bk4");
          }
          REQUIRE(value.aboveof.size() == 4);
        }

        else {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");
          
          REQUIRE(value.aboveof[0]->name == "bk2");
          REQUIRE(value.aboveof.size() == 2);
        }
      }
    }
  
    else if (pair_idx.first == 0 && pair_idx.second == 2) {
      for (auto& [key, value] : sptest.map_blocks) {
        if (key == "bk1") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");
            REQUIRE(blk->name != "bk3");
          }
          REQUIRE(value.rightof.size() == 4);

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk2") {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");
          
          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk3") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");
            REQUIRE(blk->name != "bk3");
          }
          REQUIRE(value.rightof.size() == 4);
         
          REQUIRE(value.aboveof.size() == 2);
          REQUIRE(value.aboveof[0]->name == "bk1"); 
        }

        else if (key == "bk4") {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");
         
          for (auto& blk : value.aboveof) {
            REQUIRE(blk->name != "bk1");
            REQUIRE(blk->name != "bk3");
            REQUIRE(blk->name != "bk4");
          }
          REQUIRE(value.aboveof.size() == 3);
        }

        else {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");

          REQUIRE(value.aboveof.size() == 2);
          REQUIRE(value.aboveof[0]->name == "bk2");
        }
      }
    }
 
    else if (pair_idx.first == 0 && pair_idx.second == 3) {
      for (auto& [key, value] : sptest.map_blocks) {
        if (key == "bk1") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");
            REQUIRE(blk->name != "bk5");
          }
          REQUIRE(value.rightof.size() == 4);

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk2") {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk3") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");
            REQUIRE(blk->name != "bk3");
            REQUIRE(blk->name != "bk5");
          }
          REQUIRE(value.rightof.size() == 3);

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }
        
        else if (key == "bk4") {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");

          REQUIRE(value.aboveof[0]->name == "bk2");
          REQUIRE(value.aboveof.size() == 2);
        }

        else {
          REQUIRE(value.rightof[0]->name == "bk4");
          REQUIRE(value.rightof.size() == 2);

          for (auto& blk : value.aboveof) {
            REQUIRE(blk->name != "bk4");
            REQUIRE(blk->name != "bk5");
          }
          REQUIRE(value.aboveof.size() == 4);
        }
      }
    }

    else if (pair_idx.first == 0 && pair_idx.second == 4) {
      for (auto& [key, value] : sptest.map_blocks) {
        if (key == "bk1") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");
            REQUIRE(blk->name != "bk2");
          }
          REQUIRE(value.rightof.size() == 4);
        
          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk2") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");
            REQUIRE(blk->name != "bk2");
            REQUIRE(blk->name != "bk3");
          }
          REQUIRE(value.rightof.size() == 3);
          
          for (auto& blk : value.aboveof) {
            REQUIRE(blk->name != "bk2");
            REQUIRE(blk->name != "bk4");
            REQUIRE(blk->name != "bk5");
          }
          REQUIRE(value.rightof.size() == 3);
        }

        else if (key == "bk3") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");
            REQUIRE(blk->name != "bk2");
            REQUIRE(blk->name != "bk3");
          }
          REQUIRE(value.rightof.size() == 3);
         
          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus"); 
        }

        else if (key == "bk4") {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else {
          REQUIRE(value.rightof.size() == 2);
          REQUIRE(value.rightof[0]->name == "bk4");

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }
      }
    }

    else if (pair_idx.first == 1 && pair_idx.second == 2) {
      for (auto& [key, value] : sptest.map_blocks) {
        if (key == "bk1") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");
            REQUIRE(blk->name != "bk3");
            REQUIRE(blk->name != "bk4");
          } 
          REQUIRE(value.rightof.size() == 3);
        
          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk2") {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");
        
          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk3") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");
            REQUIRE(blk->name != "bk3");
            REQUIRE(blk->name != "bk4");
          } 
          REQUIRE(value.rightof.size() == 3);
         
          REQUIRE(value.aboveof.size() == 2);
          REQUIRE(value.aboveof[0]->name == "bk1"); 
        }

        else if (key == "bk4") {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");
 
          for (auto& blk : value.aboveof) {
            REQUIRE(blk->name != "bk4");
          }
          REQUIRE(value.aboveof.size() == 5);
        }

        else {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");

          REQUIRE(value.aboveof.size() == 2);
          REQUIRE(value.aboveof[0]->name == "bk2");
        }
      }
    }

    else if (pair_idx.first == 1 && pair_idx.second == 3) {
      for (auto& [key, value] : sptest.map_blocks) {
        if (key == "bk1") {
          REQUIRE(value.rightof.size() == 2);
          REQUIRE(value.rightof[0]->name == "bk2");
          
          REQUIRE(value.aboveof.size() == 1); 
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk2") {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk3") {
          REQUIRE(value.rightof.size() == 2);
          REQUIRE(value.rightof[0]->name == "bk2");

          REQUIRE(value.aboveof.size() == 2);
          REQUIRE(value.aboveof[0]->name == "bk1");
        }

        else if (key == "bk4") {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");

          for (auto& blk : value.aboveof) {
            REQUIRE(blk->name != "bk4");
          }
          REQUIRE(value.aboveof.size() == 5);
        }

        else {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");

          for (auto& blk : value.aboveof) {
            REQUIRE(blk->name != "bk4");
            REQUIRE(blk->name != "bk5");
          }
          REQUIRE(value.aboveof.size() == 4);
        }
      }
    }

    else if (pair_idx.first == 1 && pair_idx.second == 4) {
      for (auto& [key, value] : sptest.map_blocks) {
        if (key == "bk1") {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk2") {
          REQUIRE(value.rightof.size() == 2);
          REQUIRE(value.rightof[0]->name == "bk5");

          for (auto& blk : value.aboveof) {
            REQUIRE(blk->name != "bk2");
            REQUIRE(blk->name != "bk4");
            REQUIRE(blk->name != "bk5");
          }
          REQUIRE(value.aboveof.size() == 3);
        }

        else if (key == "bk3") {
          REQUIRE(value.rightof[0]->name == "bk5");
          REQUIRE(value.rightof.size() == 2);

          REQUIRE(value.aboveof[0]->name == "bk1");
          REQUIRE(value.aboveof.size() == 2);
        }

        else if (key == "bk4") {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");

          for (auto& blk : value.aboveof) {
            REQUIRE(blk->name != "bk4");
          }
          REQUIRE(value.aboveof.size() == 5);
        }

        else {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");

          REQUIRE(value.aboveof.size() == 2);
          REQUIRE(value.aboveof[0]->name == "bk1");
        }
      }
    }

    else if (pair_idx.first == 2 && pair_idx.second == 3) {
      for (auto& [key, value] : sptest.map_blocks) {
        if (key == "bk1") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");
            REQUIRE(blk->name != "bk4");
          }
          REQUIRE(value.rightof.size() == 4);

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk2") {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");
          
          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk3") {
          REQUIRE(value.rightof[0]->name == "bk2");
          REQUIRE(value.rightof.size() == 2);
          
          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk4") {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");

          for (auto& blk : value.aboveof) {
            REQUIRE(blk->name != "bk4");
          }
          REQUIRE(value.aboveof.size() == 5);
        }

        else {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");
        
          for (auto& blk : value.aboveof) {
            REQUIRE(blk->name != "bk1");
            REQUIRE(blk->name != "bk4");
            REQUIRE(blk->name != "bk5");
          }
          REQUIRE(value.aboveof.size() == 3);
        }
      }
    }

    else if (pair_idx.first == 2 && pair_idx.second == 4) {
      for (auto& [key, value] : sptest.map_blocks) {
        if (key == "bk1") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");
            REQUIRE(blk->name != "bk4");
          }
          REQUIRE(value.rightof.size() == 4);

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk2") {
          REQUIRE(value.rightof[0]->name == "bk5");
          REQUIRE(value.rightof.size() == 2);

          REQUIRE(value.aboveof.size() == 2);
          REQUIRE(value.aboveof[0]->name == "bk3");
        }

        else if (key == "bk3") {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");
          
          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk4") {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");

          for (auto& blk : value.aboveof) {
            REQUIRE(blk->name != " bk4");
          }
          REQUIRE(value.aboveof.size() == 5);
        }

        else {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");

          REQUIRE(value.aboveof.size() == 2);
          REQUIRE(value.aboveof[0]->name == "bk3");
        }
      }
    }
    
    else if (pair_idx.first == 3 && pair_idx.second) {
      for (auto& [key, value] : sptest.map_blocks) {
        if (key == "bk1") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");
            REQUIRE(blk->name != "bk4");
          }
          REQUIRE(value.rightof.size() == 4);

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk2") {
          REQUIRE(value.rightof.size() == 2);
          REQUIRE(value.rightof[0]->name == "bk5");

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk3") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");
            REQUIRE(blk->name != "bk3");
            REQUIRE(blk->name != "bk4");
          }
          REQUIRE(value.rightof.size() == 3);

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk4") {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");

          for (auto& blk : value.aboveof) {
            REQUIRE(blk->name != "bk4");
          }
          REQUIRE(value.aboveof.size() == 5);
        }

        else {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }
      }
    }
  }
  
  SUBCASE("SUB : case 2 with move3") { 
    sptest.positive_sequence[0] = &(sptest.map_blocks["bk1"]);
    sptest.map_blocks["bk1"].idx_positive_sequence = 0;
    sptest.positive_sequence[1] = &(sptest.map_blocks["bk3"]);
    sptest.map_blocks["bk3"].idx_positive_sequence = 1;
    sptest.positive_sequence[2] = &(sptest.map_blocks["bk2"]);
    sptest.map_blocks["bk2"].idx_positive_sequence = 2;
    sptest.positive_sequence[3] = &(sptest.map_blocks["bk5"]);
    sptest.map_blocks["bk5"].idx_positive_sequence = 3;
    sptest.positive_sequence[4] = &(sptest.map_blocks["bk4"]);
    sptest.map_blocks["bk4"].idx_positive_sequence = 4;
    
    sptest.negative_sequence[0] = &(sptest.map_blocks["bk4"]);
    sptest.map_blocks["bk4"].idx_negative_sequence = 0;
    sptest.negative_sequence[1] = &(sptest.map_blocks["bk1"]);
    sptest.map_blocks["bk1"].idx_negative_sequence = 1;
    sptest.negative_sequence[2] = &(sptest.map_blocks["bk3"]);
    sptest.map_blocks["bk3"].idx_negative_sequence = 2;
    sptest.negative_sequence[3] = &(sptest.map_blocks["bk5"]);
    sptest.map_blocks["bk5"].idx_negative_sequence = 3;
    sptest.negative_sequence[4] = &(sptest.map_blocks["bk2"]);
    sptest.map_blocks["bk2"].idx_negative_sequence = 4;

    sptest.construct_relative_locations(0, 4, MoveType::nomove);
    auto pair_idx = sptest.move3();
    std::cout << "pair_idx = " << pair_idx.first << "," << pair_idx.second << '\n';

    sptest.construct_relative_locations(
      pair_idx.first,
      pair_idx.second,
      MoveType::move3
    );
   
    REQUIRE(sptest.source.rightof.size() == 5);
    REQUIRE(sptest.source.aboveof.size() == 5);

    if (pair_idx.first == 0 && pair_idx.second == 1) {
      for (auto& [key, value] : sptest.map_blocks) {
        if (key == "bk1") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");
            REQUIRE(blk->name != "bk3");
            REQUIRE(blk->name != "bk4");
          }
          REQUIRE(value.rightof.size() == 3);

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk2") {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");
          
          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk3") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk3");
            REQUIRE(blk->name != "bk4");
          }
          REQUIRE(value.rightof.size() == 4);

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk4") {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");

          for (auto& blk : value.aboveof) {
            REQUIRE(blk->name != "bk4");
          }
          REQUIRE(value.aboveof.size() == 5);
        }

        else {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");

          REQUIRE(value.aboveof.size() == 2);
          REQUIRE(value.aboveof[0]->name == "bk2");
        }
      }
    }

    else if (pair_idx.first == 0 && pair_idx.second == 2) {
      for (auto& [key, value] : sptest.map_blocks) {
        if (key == "bk1") {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");
          
          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk2") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk2");
            REQUIRE(blk->name != "bk4");
          }
          REQUIRE(value.rightof.size() == 4);

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk3") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk2");
            REQUIRE(blk->name != "bk3");
            REQUIRE(blk->name != "bk4");
          }
          REQUIRE(value.rightof.size() == 3);

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk4") {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");

          for (auto& blk : value.aboveof) {
            REQUIRE(blk->name != "bk4");
          }
          REQUIRE(value.aboveof.size() == 5);
        }

        else {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");

          REQUIRE(value.aboveof.size() == 2);
          REQUIRE(value.aboveof[0]->name == "bk1");
        }
      }
    }

    else if (pair_idx.first == 0 && pair_idx.second == 3) {
      for (auto& [key, value] : sptest.map_blocks) {
        if (key == "bk1") {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");

          REQUIRE(value.aboveof.size() == 2);
          REQUIRE(value.aboveof[0]->name == "bk2");
        }

        else if (key == "bk2") {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");
          
          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk3") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk3");
            REQUIRE(blk->name != "bk4");
            REQUIRE(blk->name != "bk5");
          }
          REQUIRE(value.rightof.size() == 3);

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk4") {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");

          for (auto& blk : value.aboveof) {
            REQUIRE(blk->name != " bk4");
          }
          REQUIRE(value.aboveof.size() == 5);
        }

        else {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != " bk4");
            REQUIRE(blk->name != " bk5");
          }
          REQUIRE(value.rightof.size() == 4);

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }
      }
    }

    else if (pair_idx.first == 0 && pair_idx.second == 4) {
      for (auto& [key, value] : sptest.map_blocks) {
        if (key == "bk1") {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");

          for (auto& blk : value.aboveof) {
            REQUIRE(blk->name != "bk1");
          }
          REQUIRE(value.aboveof.size() == 5);
        }

        else if (key == "bk2") {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");
          
          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk3") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");
            REQUIRE(blk->name != "bk3");
            REQUIRE(blk->name != "bk4");
          }
          REQUIRE(value.rightof.size() == 3);

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk4") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");
            REQUIRE(blk->name != "bk4");
          }
          REQUIRE(value.rightof.size() == 4);

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");

          REQUIRE(value.aboveof.size() == 2);
          REQUIRE(value.aboveof[0]->name == "bk2");
        }
      }
    }

    else if (pair_idx.first == 1 && pair_idx.second == 2) {
      for (auto& [key, value] : sptest.map_blocks) {
        if (key == "bk1") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");
            REQUIRE(blk->name != "bk4");
          }
          REQUIRE(value.rightof.size() == 4);

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk2") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");
            REQUIRE(blk->name != "bk2");
            REQUIRE(blk->name != "bk4");
          }
          REQUIRE(value.rightof.size() == 3);

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk3") {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk4") {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");
       
          for (auto& blk : value.aboveof) {
            REQUIRE(blk->name != "bk4");
          }
          REQUIRE(value.aboveof.size() == 5);
        }

        else {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");
  
          REQUIRE(value.aboveof.size() == 2);
          REQUIRE(value.aboveof[0]->name == "bk3");
        }
      }
    }

    else if (pair_idx.first == 1 && pair_idx.second == 3) {
      for (auto& [key, value] : sptest.map_blocks) {
        if (key == "bk1") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");
            REQUIRE(blk->name != "bk4");
          }
          REQUIRE(value.rightof.size() == 4);

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk2") {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");
          
          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk3") {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");
          
          REQUIRE(value.aboveof.size() == 2);
          REQUIRE(value.aboveof[0]->name == "bk2");
        }

        else if (key == "bk4") {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");
   
          for (auto& blk : value.aboveof) {
            REQUIRE(blk->name != "bk4");
          }
          REQUIRE(value.aboveof.size() == 5);
        }

        else {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != " bk1");
            REQUIRE(blk->name != " bk4");
            REQUIRE(blk->name != " bk5");
          }
          REQUIRE(value.rightof.size() == 3);

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }
      }
    }

    else if (pair_idx.first == 1 && pair_idx.second == 4) {
      for (auto& [key, value] : sptest.map_blocks) {
        if (key == "bk1") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");
            REQUIRE(blk->name != "bk3");
          }
          REQUIRE(value.rightof.size() == 4);

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk2") {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");
          
          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk3") {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");

          for (auto& blk : value.aboveof) {
            REQUIRE(blk->name != "bk3");
          }
          REQUIRE(value.aboveof.size() == 5);
        }

        else if (key == "bk4") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");
            REQUIRE(blk->name != "bk3");
            REQUIRE(blk->name != "bk4");
          }
          REQUIRE(value.rightof.size() == 3);

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");

          REQUIRE(value.aboveof.size() == 2);
          REQUIRE(value.aboveof[0]->name == "bk2");
        }
      }
    }

    else if (pair_idx.first == 2 && pair_idx.second == 3) {
      for (auto& [key, value] : sptest.map_blocks) {
        if (key == "bk1") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");
            REQUIRE(blk->name != "bk4");
          }
          REQUIRE(value.rightof.size() == 4);

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk2") {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");

          REQUIRE(value.aboveof.size() == 2);
          REQUIRE(value.aboveof[0]->name == "bk5");
        }

        else if (key == "bk3") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");
            REQUIRE(blk->name != "bk3");
            REQUIRE(blk->name != "bk4");
          }
          REQUIRE(value.rightof.size() == 3);

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk4") {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");

          for (auto& blk : value.aboveof) {
            REQUIRE(blk->name != "bk4");
          }
          REQUIRE(value.aboveof.size() == 5);
        }

        else {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }
      }
    }

    else if (pair_idx.first == 2 && pair_idx.second == 4) {
      for (auto& [key, value] : sptest.map_blocks) {
        if (key == "bk1") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");
            REQUIRE(blk->name != "bk2");
          }
          REQUIRE(value.rightof.size() == 4);

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk2") {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");

          for (auto& blk : value.aboveof) {
            REQUIRE(blk->name != "bk2");
          }
          REQUIRE(value.aboveof.size() == 5);
        }

        else if (key == "bk3") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");
            REQUIRE(blk->name != "bk2");
            REQUIRE(blk->name != "bk3");
          }
          REQUIRE(value.rightof.size());

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk4") {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");
          
          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }
        
        else {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");
          
          REQUIRE(value.aboveof.size() == 2);
          REQUIRE(value.aboveof[0]->name == "bk4");
        }
      }
    }

    else if (pair_idx.first == 3 && pair_idx.second == 4) {
      for (auto& [key, value] : sptest.map_blocks) {
        if (key == "bk1") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");
            REQUIRE(blk->name != "bk5");
          }
          REQUIRE(value.rightof.size() == 4);

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk2") {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk3") {
          for (auto& blk : value.rightof) {
            REQUIRE(blk->name != "bk1");
            REQUIRE(blk->name != "bk3");
            REQUIRE(blk->name != "bk5");
          }
          REQUIRE(value.rightof.size() == 3);

          REQUIRE(value.aboveof.size() == 1);
          REQUIRE(value.aboveof[0]->name == "terminus");
        }

        else if (key == "bk4") {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");

          REQUIRE(value.aboveof.size() == 2);
          REQUIRE(value.aboveof[0]->name == "bk2");
        }

        else {
          REQUIRE(value.rightof.size() == 1);
          REQUIRE(value.rightof[0]->name == "terminus");

          for (auto& blk : value.aboveof) {
            REQUIRE(blk->name != "bk5");
          }
          REQUIRE(value.aboveof.size() == 5);
        }
      }
    }
  }
  
  SUBCASE("SUB : case 2 with move4") { 
    sptest.positive_sequence[0] = &(sptest.map_blocks["bk1"]);
    sptest.map_blocks["bk1"].idx_positive_sequence = 0;
    sptest.positive_sequence[1] = &(sptest.map_blocks["bk3"]);
    sptest.map_blocks["bk3"].idx_positive_sequence = 1;
    sptest.positive_sequence[2] = &(sptest.map_blocks["bk2"]);
    sptest.map_blocks["bk2"].idx_positive_sequence = 2;
    sptest.positive_sequence[3] = &(sptest.map_blocks["bk5"]);
    sptest.map_blocks["bk5"].idx_positive_sequence = 3;
    sptest.positive_sequence[4] = &(sptest.map_blocks["bk4"]);
    sptest.map_blocks["bk4"].idx_positive_sequence = 4;
    
    sptest.negative_sequence[0] = &(sptest.map_blocks["bk4"]);
    sptest.map_blocks["bk4"].idx_negative_sequence = 0;
    sptest.negative_sequence[1] = &(sptest.map_blocks["bk1"]);
    sptest.map_blocks["bk1"].idx_negative_sequence = 1;
    sptest.negative_sequence[2] = &(sptest.map_blocks["bk3"]);
    sptest.map_blocks["bk3"].idx_negative_sequence = 2;
    sptest.negative_sequence[3] = &(sptest.map_blocks["bk5"]);
    sptest.map_blocks["bk5"].idx_negative_sequence = 3;
    sptest.negative_sequence[4] = &(sptest.map_blocks["bk2"]);
    sptest.map_blocks["bk2"].idx_negative_sequence = 4;

    sptest.construct_relative_locations(0, 4, MoveType::nomove);
    auto pair_idx = sptest.move4();
    std::cout << "pair_idx = " << pair_idx.first << "," << pair_idx.second << '\n';

    sptest.construct_relative_locations(
      pair_idx.first,
      pair_idx.second,
      MoveType::move4
    );
   
    REQUIRE(sptest.source.rightof.size() == 5);
    REQUIRE(sptest.source.aboveof.size() == 5);

    for (auto& [key, value] : sptest.map_blocks) {
      if (key == "bk1") {
        for (auto& blk : value.rightof) {
          REQUIRE(blk->name != "bk1");
          REQUIRE(blk->name != "bk4");
        }
        REQUIRE(value.rightof.size() == 4);

        REQUIRE(value.aboveof.size() == 1);
        REQUIRE(value.aboveof[0]->name == "terminus");
      }

      else if (key == "bk2") {
        REQUIRE(value.rightof.size() == 1);
        REQUIRE(value.rightof[0]->name == "terminus");

        REQUIRE(value.aboveof.size() == 1);
        REQUIRE(value.aboveof[0]->name == "terminus");
      }

      else if (key == "bk3") {
        for (auto& blk : value.rightof) {
          REQUIRE(blk->name != "bk1");
          REQUIRE(blk->name != "bk3");
          REQUIRE(blk->name != "bk4");
        }
        REQUIRE(value.rightof.size() == 3);

        REQUIRE(value.aboveof.size() == 1);
        REQUIRE(value.aboveof[0]->name == "terminus");
      }

      else if (key == "bk4") {
        REQUIRE(value.rightof.size() == 1);
        REQUIRE(value.rightof[0]->name == "terminus");
        
        for (auto& blk : value.aboveof) {
          REQUIRE(blk->name != "bk4"); 
        }
        REQUIRE(value.aboveof.size() == 5);
      }

      else {
        REQUIRE(value.rightof.size() == 1);
        REQUIRE(value.rightof[0]->name == "terminus");

        REQUIRE(value.aboveof.size() == 2);
        REQUIRE(value.aboveof[0]->name == "bk2");
      }
    }
  }
}


// verify compute_spfa
TEST_CASE("verify_compute_spfa" * doctest::timeout(600)) {
  SPTest sptest;
  sptest.initialize_sequence();

  sptest.source.rightof.clear();
  sptest.terminus.rightof.clear();
  sptest.map_blocks["bk1"].rightof.clear();
  sptest.map_blocks["bk2"].rightof.clear();
  sptest.map_blocks["bk3"].rightof.clear();
  sptest.map_blocks["bk4"].rightof.clear();
  sptest.map_blocks["bk5"].rightof.clear();

  sptest.source.aboveof.clear();
  sptest.terminus.aboveof.clear();
  sptest.map_blocks["bk1"].aboveof.clear();
  sptest.map_blocks["bk2"].aboveof.clear();
  sptest.map_blocks["bk3"].aboveof.clear();
  sptest.map_blocks["bk4"].aboveof.clear();
  sptest.map_blocks["bk5"].aboveof.clear();

  // positive sequence = [bk1, bk2, bk3, bk4, bk5]
  // negative sequence = [bk1, bk2, bk3, bk4, bk5]
  SUBCASE("SUB : case 1") { 
    sptest.positive_sequence[0] = &(sptest.map_blocks["bk1"]);
    sptest.map_blocks["bk1"].idx_positive_sequence = 0;
    sptest.positive_sequence[1] = &(sptest.map_blocks["bk2"]);
    sptest.map_blocks["bk2"].idx_positive_sequence = 1;
    sptest.positive_sequence[2] = &(sptest.map_blocks["bk3"]);
    sptest.map_blocks["bk3"].idx_positive_sequence = 2;
    sptest.positive_sequence[3] = &(sptest.map_blocks["bk4"]);
    sptest.map_blocks["bk4"].idx_positive_sequence = 3;
    sptest.positive_sequence[4] = &(sptest.map_blocks["bk5"]);
    sptest.map_blocks["bk5"].idx_positive_sequence = 4;
    
    sptest.negative_sequence[0] = &(sptest.map_blocks["bk1"]);
    sptest.map_blocks["bk1"].idx_negative_sequence = 0;
    sptest.negative_sequence[1] = &(sptest.map_blocks["bk2"]);
    sptest.map_blocks["bk2"].idx_negative_sequence = 1;
    sptest.negative_sequence[2] = &(sptest.map_blocks["bk3"]);
    sptest.map_blocks["bk3"].idx_negative_sequence = 2;
    sptest.negative_sequence[3] = &(sptest.map_blocks["bk4"]);
    sptest.map_blocks["bk4"].idx_negative_sequence = 3;
    sptest.negative_sequence[4] = &(sptest.map_blocks["bk5"]);
    sptest.map_blocks["bk5"].idx_negative_sequence = 4;

    sptest.construct_relative_locations(0, 4, MoveType::nomove);
  
    std::vector<int> distance = sptest.spfa(Orientation::Horizontal);

    REQUIRE(distance[0] == 0);
    REQUIRE(distance[1] == -200);
    REQUIRE(distance[2] == -800);
    REQUIRE(distance[3] == -1200);
    REQUIRE(distance[4] == -1600);
    REQUIRE(distance[5] == -1800);
    REQUIRE(distance[6] == -1800);
    
    // check vertical area
    distance = sptest.spfa(Orientation::Vertical);
    
    REQUIRE(distance[0] == 0);
    REQUIRE(distance[1] == -200);
    REQUIRE(distance[2] == -600);
    REQUIRE(distance[3] == -200);
    REQUIRE(distance[4] == -400);
    REQUIRE(distance[5] == -400);
    REQUIRE(distance[6] == -600);
  }
  
  // positive sequence = [bk1, bk3, bk2, bk5, bk4]
  // negative sequence = [bk4, bk1, bk3, bk5, bk2]
  SUBCASE("SUB : case 2") { 
    sptest.positive_sequence[0] = &(sptest.map_blocks["bk1"]);
    sptest.map_blocks["bk1"].idx_positive_sequence = 0;
    sptest.positive_sequence[1] = &(sptest.map_blocks["bk3"]);
    sptest.map_blocks["bk3"].idx_positive_sequence = 1;
    sptest.positive_sequence[2] = &(sptest.map_blocks["bk2"]);
    sptest.map_blocks["bk2"].idx_positive_sequence = 2;
    sptest.positive_sequence[3] = &(sptest.map_blocks["bk5"]);
    sptest.map_blocks["bk5"].idx_positive_sequence = 3;
    sptest.positive_sequence[4] = &(sptest.map_blocks["bk4"]);
    sptest.map_blocks["bk4"].idx_positive_sequence = 4;
    
    sptest.negative_sequence[0] = &(sptest.map_blocks["bk4"]);
    sptest.map_blocks["bk4"].idx_negative_sequence = 0;
    sptest.negative_sequence[1] = &(sptest.map_blocks["bk1"]);
    sptest.map_blocks["bk1"].idx_negative_sequence = 1;
    sptest.negative_sequence[2] = &(sptest.map_blocks["bk3"]);
    sptest.map_blocks["bk3"].idx_negative_sequence = 2;
    sptest.negative_sequence[3] = &(sptest.map_blocks["bk5"]);
    sptest.map_blocks["bk5"].idx_negative_sequence = 3;
    sptest.negative_sequence[4] = &(sptest.map_blocks["bk2"]);
    sptest.map_blocks["bk2"].idx_negative_sequence = 4;

    sptest.construct_relative_locations(0, 4, MoveType::nomove);
    
    // check horizontal area
    std::vector<int> distance = sptest.spfa(Orientation::Horizontal);
    
    REQUIRE(distance[0] == 0);
    REQUIRE(distance[1] == -200);
    REQUIRE(distance[2] == -600);
    REQUIRE(distance[3] == -1200);
    REQUIRE(distance[4] == -800);
    REQUIRE(distance[5] == -400);
    REQUIRE(distance[6] == -1200);

    // check vertical area
    distance = sptest.spfa(Orientation::Vertical);
    REQUIRE(distance[0] == 0);
    REQUIRE(distance[1] == -600);
    REQUIRE(distance[2] == -600);
    REQUIRE(distance[3] == -1400);
    REQUIRE(distance[4] == -800);
    REQUIRE(distance[5] == -400);
    REQUIRE(distance[6] == -1400);
  }
}


// verify compute_block_locations
TEST_CASE("verify_compute_block_locations" * doctest::timeout(600)) {
  SPTest sptest;
  sptest.initialize_sequence();

  sptest.source.rightof.clear();
  sptest.terminus.rightof.clear();
  sptest.map_blocks["bk1"].rightof.clear();
  sptest.map_blocks["bk2"].rightof.clear();
  sptest.map_blocks["bk3"].rightof.clear();
  sptest.map_blocks["bk4"].rightof.clear();
  sptest.map_blocks["bk5"].rightof.clear();

  sptest.source.aboveof.clear();
  sptest.terminus.aboveof.clear();
  sptest.map_blocks["bk1"].aboveof.clear();
  sptest.map_blocks["bk2"].aboveof.clear();
  sptest.map_blocks["bk3"].aboveof.clear();
  sptest.map_blocks["bk4"].aboveof.clear();
  sptest.map_blocks["bk5"].aboveof.clear();

  // positive sequence = [bk1, bk2, bk3, bk4, bk5]
  // negative sequence = [bk1, bk2, bk3, bk4, bk5]
  SUBCASE("SUB : case 1") { 
    sptest.positive_sequence[0] = &(sptest.map_blocks["bk1"]);
    sptest.map_blocks["bk1"].idx_positive_sequence = 0;
    sptest.positive_sequence[1] = &(sptest.map_blocks["bk2"]);
    sptest.map_blocks["bk2"].idx_positive_sequence = 1;
    sptest.positive_sequence[2] = &(sptest.map_blocks["bk3"]);
    sptest.map_blocks["bk3"].idx_positive_sequence = 2;
    sptest.positive_sequence[3] = &(sptest.map_blocks["bk4"]);
    sptest.map_blocks["bk4"].idx_positive_sequence = 3;
    sptest.positive_sequence[4] = &(sptest.map_blocks["bk5"]);
    sptest.map_blocks["bk5"].idx_positive_sequence = 4;
    
    sptest.negative_sequence[0] = &(sptest.map_blocks["bk1"]);
    sptest.map_blocks["bk1"].idx_negative_sequence = 0;
    sptest.negative_sequence[1] = &(sptest.map_blocks["bk2"]);
    sptest.map_blocks["bk2"].idx_negative_sequence = 1;
    sptest.negative_sequence[2] = &(sptest.map_blocks["bk3"]);
    sptest.map_blocks["bk3"].idx_negative_sequence = 2;
    sptest.negative_sequence[3] = &(sptest.map_blocks["bk4"]);
    sptest.map_blocks["bk4"].idx_negative_sequence = 3;
    sptest.negative_sequence[4] = &(sptest.map_blocks["bk5"]);
    sptest.map_blocks["bk5"].idx_negative_sequence = 4;

    sptest.construct_relative_locations(0, 4, MoveType::nomove);
  
    std::vector<int> distance = sptest.spfa(Orientation::Horizontal);
    sptest.compute_block_locations(distance, Orientation::Horizontal);

    // check x coordinate 
    REQUIRE(sptest.map_blocks["bk1"].lower_left_x == 0);
    REQUIRE(sptest.map_blocks["bk2"].lower_left_x == 200);
    REQUIRE(sptest.map_blocks["bk3"].lower_left_x == 800);
    REQUIRE(sptest.map_blocks["bk4"].lower_left_x == 1200);
    REQUIRE(sptest.map_blocks["bk5"].lower_left_x == 1600);
    
    // check y coordinate
    distance = sptest.spfa(Orientation::Vertical);
    sptest.compute_block_locations(distance, Orientation::Vertical);
    
    REQUIRE(sptest.map_blocks["bk1"].lower_left_y == 0);
    REQUIRE(sptest.map_blocks["bk2"].lower_left_y == 0);
    REQUIRE(sptest.map_blocks["bk3"].lower_left_y == 0);
    REQUIRE(sptest.map_blocks["bk4"].lower_left_y == 0);
    REQUIRE(sptest.map_blocks["bk5"].lower_left_y == 0);
  }
  
  // positive sequence = [bk1, bk3, bk2, bk5, bk4]
  // negative sequence = [bk4, bk1, bk3, bk5, bk2]
  SUBCASE("SUB : case 2") { 
    sptest.positive_sequence[0] = &(sptest.map_blocks["bk1"]);
    sptest.map_blocks["bk1"].idx_positive_sequence = 0;
    sptest.positive_sequence[1] = &(sptest.map_blocks["bk3"]);
    sptest.map_blocks["bk3"].idx_positive_sequence = 1;
    sptest.positive_sequence[2] = &(sptest.map_blocks["bk2"]);
    sptest.map_blocks["bk2"].idx_positive_sequence = 2;
    sptest.positive_sequence[3] = &(sptest.map_blocks["bk5"]);
    sptest.map_blocks["bk5"].idx_positive_sequence = 3;
    sptest.positive_sequence[4] = &(sptest.map_blocks["bk4"]);
    sptest.map_blocks["bk4"].idx_positive_sequence = 4;
    
    sptest.negative_sequence[0] = &(sptest.map_blocks["bk4"]);
    sptest.map_blocks["bk4"].idx_negative_sequence = 0;
    sptest.negative_sequence[1] = &(sptest.map_blocks["bk1"]);
    sptest.map_blocks["bk1"].idx_negative_sequence = 1;
    sptest.negative_sequence[2] = &(sptest.map_blocks["bk3"]);
    sptest.map_blocks["bk3"].idx_negative_sequence = 2;
    sptest.negative_sequence[3] = &(sptest.map_blocks["bk5"]);
    sptest.map_blocks["bk5"].idx_negative_sequence = 3;
    sptest.negative_sequence[4] = &(sptest.map_blocks["bk2"]);
    sptest.map_blocks["bk2"].idx_negative_sequence = 4;

    sptest.construct_relative_locations(0, 4, MoveType::nomove);
    
    // check x coordinate
    std::vector<int> distance = sptest.spfa(Orientation::Horizontal);
    sptest.compute_block_locations(distance, Orientation::Horizontal);
     
    REQUIRE(sptest.map_blocks["bk1"].lower_left_x == 0);
    REQUIRE(sptest.map_blocks["bk3"].lower_left_x == 200);
    REQUIRE(sptest.map_blocks["bk2"].lower_left_x == 600);
    REQUIRE(sptest.map_blocks["bk5"].lower_left_x == 600);
    REQUIRE(sptest.map_blocks["bk4"].lower_left_x == 0);

    // check y coordinate
    distance = sptest.spfa(Orientation::Vertical);
    sptest.compute_block_locations(distance, Orientation::Vertical);
    REQUIRE(sptest.map_blocks["bk1"].lower_left_y == 400);
    REQUIRE(sptest.map_blocks["bk3"].lower_left_y == 400);
    REQUIRE(sptest.map_blocks["bk2"].lower_left_y == 800);
    REQUIRE(sptest.map_blocks["bk5"].lower_left_y == 400);
    REQUIRE(sptest.map_blocks["bk4"].lower_left_y == 0);
  }
}


// verify compute_hpwl
TEST_CASE("verify_compute_hpwl" * doctest::timeout(600)) {
  SPTest sptest; 
  sptest.initialize_sequence();
  
  sptest.source.rightof.clear();
  sptest.terminus.rightof.clear();
  sptest.map_blocks["bk1"].rightof.clear();
  sptest.map_blocks["bk2"].rightof.clear();
  sptest.map_blocks["bk3"].rightof.clear();
  sptest.map_blocks["bk4"].rightof.clear();
  sptest.map_blocks["bk5"].rightof.clear();

  sptest.source.aboveof.clear();
  sptest.terminus.aboveof.clear();
  sptest.map_blocks["bk1"].aboveof.clear();
  sptest.map_blocks["bk2"].aboveof.clear();
  sptest.map_blocks["bk3"].aboveof.clear();
  sptest.map_blocks["bk4"].aboveof.clear();
  sptest.map_blocks["bk5"].aboveof.clear();

  // positive sequence = [bk1, bk2, bk3, bk4, bk5]
  // negative sequence = [bk1, bk2, bk3, bk4, bk5]
  SUBCASE("SUB : case 1") { 
    sptest.positive_sequence[0] = &(sptest.map_blocks["bk1"]);
    sptest.map_blocks["bk1"].idx_positive_sequence = 0;
    sptest.positive_sequence[1] = &(sptest.map_blocks["bk2"]);
    sptest.map_blocks["bk2"].idx_positive_sequence = 1;
    sptest.positive_sequence[2] = &(sptest.map_blocks["bk3"]);
    sptest.map_blocks["bk3"].idx_positive_sequence = 2;
    sptest.positive_sequence[3] = &(sptest.map_blocks["bk4"]);
    sptest.map_blocks["bk4"].idx_positive_sequence = 3;
    sptest.positive_sequence[4] = &(sptest.map_blocks["bk5"]);
    sptest.map_blocks["bk5"].idx_positive_sequence = 4;
    
    sptest.negative_sequence[0] = &(sptest.map_blocks["bk1"]);
    sptest.map_blocks["bk1"].idx_negative_sequence = 0;
    sptest.negative_sequence[1] = &(sptest.map_blocks["bk2"]);
    sptest.map_blocks["bk2"].idx_negative_sequence = 1;
    sptest.negative_sequence[2] = &(sptest.map_blocks["bk3"]);
    sptest.map_blocks["bk3"].idx_negative_sequence = 2;
    sptest.negative_sequence[3] = &(sptest.map_blocks["bk4"]);
    sptest.map_blocks["bk4"].idx_negative_sequence = 3;
    sptest.negative_sequence[4] = &(sptest.map_blocks["bk5"]);
    sptest.map_blocks["bk5"].idx_negative_sequence = 4;

    sptest.construct_relative_locations(0, 4, MoveType::nomove);
  
    std::vector<int> distance = sptest.spfa(Orientation::Horizontal);
    sptest.compute_block_locations(distance, Orientation::Horizontal);

    distance = sptest.spfa(Orientation::Vertical);
    sptest.compute_block_locations(distance, Orientation::Vertical);

    int hpwl = sptest.compute_hpwl();
    REQUIRE(hpwl == 1500*2);
  }

  // positive sequence = [bk1, bk3, bk2, bk5, bk4]
  // negative sequence = [bk4, bk1, bk3, bk5, bk2]
  SUBCASE("SUB : case 2") { 
    sptest.positive_sequence[0] = &(sptest.map_blocks["bk1"]);
    sptest.map_blocks["bk1"].idx_positive_sequence = 0;
    sptest.positive_sequence[1] = &(sptest.map_blocks["bk3"]);
    sptest.map_blocks["bk3"].idx_positive_sequence = 1;
    sptest.positive_sequence[2] = &(sptest.map_blocks["bk2"]);
    sptest.map_blocks["bk2"].idx_positive_sequence = 2;
    sptest.positive_sequence[3] = &(sptest.map_blocks["bk5"]);
    sptest.map_blocks["bk5"].idx_positive_sequence = 3;
    sptest.positive_sequence[4] = &(sptest.map_blocks["bk4"]);
    sptest.map_blocks["bk4"].idx_positive_sequence = 4;
    
    sptest.negative_sequence[0] = &(sptest.map_blocks["bk4"]);
    sptest.map_blocks["bk4"].idx_negative_sequence = 0;
    sptest.negative_sequence[1] = &(sptest.map_blocks["bk1"]);
    sptest.map_blocks["bk1"].idx_negative_sequence = 1;
    sptest.negative_sequence[2] = &(sptest.map_blocks["bk3"]);
    sptest.map_blocks["bk3"].idx_negative_sequence = 2;
    sptest.negative_sequence[3] = &(sptest.map_blocks["bk5"]);
    sptest.map_blocks["bk5"].idx_negative_sequence = 3;
    sptest.negative_sequence[4] = &(sptest.map_blocks["bk2"]);
    sptest.map_blocks["bk2"].idx_negative_sequence = 4;

    sptest.construct_relative_locations(0, 4, MoveType::nomove);
    
    std::vector<int> distance = sptest.spfa(Orientation::Horizontal);
    sptest.compute_block_locations(distance, Orientation::Horizontal);

    distance = sptest.spfa(Orientation::Vertical);
    sptest.compute_block_locations(distance, Orientation::Vertical);

    int hpwl = sptest.compute_hpwl();
    REQUIRE(hpwl == 1700*2);
  }
}


// verify initialize_backup_data
TEST_CASE("verify_backup_data" * doctest::timeout(600)) {
  SPTest sptest;
  sptest.initialize_sequence();

  // positive sequence = [bk1, bk3, bk2, bk5, bk4]
  // negative sequence = [bk4, bk1, bk3, bk5, bk2]
  sptest.positive_sequence[0] = &(sptest.map_blocks["bk1"]);
  sptest.map_blocks["bk1"].idx_positive_sequence = 0;
  sptest.positive_sequence[1] = &(sptest.map_blocks["bk3"]);
  sptest.map_blocks["bk3"].idx_positive_sequence = 1;
  sptest.positive_sequence[2] = &(sptest.map_blocks["bk2"]);
  sptest.map_blocks["bk2"].idx_positive_sequence = 2;
  sptest.positive_sequence[3] = &(sptest.map_blocks["bk5"]);
  sptest.map_blocks["bk5"].idx_positive_sequence = 3;
  sptest.positive_sequence[4] = &(sptest.map_blocks["bk4"]);
  sptest.map_blocks["bk4"].idx_positive_sequence = 4;
  
  sptest.negative_sequence[0] = &(sptest.map_blocks["bk4"]);
  sptest.map_blocks["bk4"].idx_negative_sequence = 0;
  sptest.negative_sequence[1] = &(sptest.map_blocks["bk1"]);
  sptest.map_blocks["bk1"].idx_negative_sequence = 1;
  sptest.negative_sequence[2] = &(sptest.map_blocks["bk3"]);
  sptest.map_blocks["bk3"].idx_negative_sequence = 2;
  sptest.negative_sequence[3] = &(sptest.map_blocks["bk5"]);
  sptest.map_blocks["bk5"].idx_negative_sequence = 3;
  sptest.negative_sequence[4] = &(sptest.map_blocks["bk2"]);
  sptest.map_blocks["bk2"].idx_negative_sequence = 4;

  sptest.construct_relative_locations(0, 4, MoveType::nomove);
  
  std::vector<int> distance = sptest.spfa(Orientation::Horizontal);
  sptest.compute_block_locations(distance, Orientation::Horizontal);

  distance = sptest.spfa(Orientation::Vertical);
  sptest.compute_block_locations(distance, Orientation::Vertical);

  sptest.initialize_backup_data();

  for (auto& [key, value] : sptest.map_blocks) {
    REQUIRE(value.width == value.backup_width);
    REQUIRE(value.height == value.backup_height);
    REQUIRE(value.lower_left_x == value.backup_lower_left_x);
    REQUIRE(value.lower_left_y == value.backup_lower_left_y);
    REQUIRE(value.idx_positive_sequence == value.backup_idx_positive_sequence);
    REQUIRE(value.idx_negative_sequence == value.backup_idx_negative_sequence);
    REQUIRE(value.rightof == value.backup_rightof);
    REQUIRE(value.aboveof == value.backup_aboveof);
  }
}


// verify move1
TEST_CASE("verify move1" * doctest::timeout(600)) {
  SPTest sptest;
  sptest.initialize_sequence();

  sptest.initialize_backup_data();

  std::pair<size_t, size_t> pair_idx = sptest.move1();

  REQUIRE(pair_idx.first < pair_idx.second);
 
  for (size_t i = 0; i < sptest.negative_sequence.size(); ++i) {
    REQUIRE(
      sptest.backup_negative_sequence[i] == sptest.negative_sequence[i]);
    REQUIRE(
      sptest.backup_negative_sequence[i]->idx_negative_sequence == i);
    REQUIRE(
      sptest.backup_negative_sequence[i]->backup_idx_negative_sequence == i);
    REQUIRE(
      sptest.negative_sequence[i]->idx_negative_sequence == i);
    REQUIRE(
      sptest.negative_sequence[i]->backup_idx_negative_sequence == i);
  }

  REQUIRE(sptest.positive_sequence[pair_idx.first] == 
          sptest.backup_positive_sequence[pair_idx.second]);
  
  REQUIRE(sptest.positive_sequence[pair_idx.second] ==
          sptest.backup_positive_sequence[pair_idx.first]);
  
  for (size_t i = 0; i < sptest.positive_sequence.size(); ++i) {
    if (i != pair_idx.first && i != pair_idx.second) {
      REQUIRE(sptest.positive_sequence[i] == 
              sptest.backup_positive_sequence[i]);
      REQUIRE(sptest.positive_sequence[i]->idx_positive_sequence ==
              sptest.positive_sequence[i]->backup_idx_positive_sequence);
      REQUIRE(sptest.backup_positive_sequence[i]->idx_positive_sequence ==
              sptest.backup_positive_sequence[i]->backup_idx_positive_sequence);
    }

    REQUIRE(sptest.positive_sequence[i]->idx_positive_sequence == i);

    REQUIRE(
      sptest.positive_sequence[pair_idx.first]->backup_idx_positive_sequence ==
      sptest.backup_positive_sequence[pair_idx.second]->backup_idx_positive_sequence);    
    
    REQUIRE(
      sptest.positive_sequence[pair_idx.second]->backup_idx_positive_sequence ==
      sptest.backup_positive_sequence[pair_idx.first]->backup_idx_positive_sequence);    

    REQUIRE(
      sptest.positive_sequence[pair_idx.first]->idx_positive_sequence !=
      sptest.positive_sequence[pair_idx.first]->backup_idx_positive_sequence);    
    
    REQUIRE(
      sptest.positive_sequence[pair_idx.second]->idx_positive_sequence !=
      sptest.positive_sequence[pair_idx.second]->backup_idx_positive_sequence);    
  }

  for (size_t i = 0; i < sptest.num_blocks; ++i) {
    REQUIRE(
      sptest.negative_sequence[i]->lower_left_x ==
      sptest.negative_sequence[i]->backup_lower_left_x);
    REQUIRE(
      sptest.negative_sequence[i]->lower_left_y ==
      sptest.negative_sequence[i]->backup_lower_left_y);
    REQUIRE(
      sptest.negative_sequence[i]->width ==
      sptest.negative_sequence[i]->backup_width);
    REQUIRE(
      sptest.negative_sequence[i]->height ==
      sptest.negative_sequence[i]->backup_height);
    REQUIRE(
      sptest.negative_sequence[i]->rightof ==
      sptest.negative_sequence[i]->backup_rightof);
    REQUIRE(
      sptest.negative_sequence[i]->aboveof ==
      sptest.negative_sequence[i]->backup_aboveof);
  }
}


// verify move2
TEST_CASE("verify move2" * doctest::timeout(600)) {
  SPTest sptest;
  sptest.initialize_sequence();

  sptest.initialize_backup_data();

  std::pair<size_t, size_t> pair_idx = sptest.move2();

  REQUIRE(pair_idx.first < pair_idx.second);

  for (size_t i = 0; i < sptest.positive_sequence.size(); ++i) {
    REQUIRE(
      sptest.backup_positive_sequence[i] == sptest.positive_sequence[i]);
    REQUIRE(
      sptest.backup_positive_sequence[i]->idx_positive_sequence == i);
    REQUIRE(
      sptest.backup_positive_sequence[i]->backup_idx_positive_sequence == i);
    REQUIRE(
      sptest.positive_sequence[i]->idx_positive_sequence == i);
    REQUIRE(
      sptest.positive_sequence[i]->backup_idx_positive_sequence == i);
  }

  REQUIRE(sptest.negative_sequence[pair_idx.first] == 
          sptest.backup_negative_sequence[pair_idx.second]);
  
  REQUIRE(sptest.negative_sequence[pair_idx.second] ==
          sptest.backup_negative_sequence[pair_idx.first]);
  
  for (size_t i = 0; i < sptest.negative_sequence.size(); ++i) {
    if (i != pair_idx.first && i != pair_idx.second) {
      REQUIRE(
        sptest.negative_sequence[i] == 
        sptest.backup_negative_sequence[i]);
      REQUIRE(
        sptest.negative_sequence[i]->idx_negative_sequence ==
        sptest.negative_sequence[i]->backup_idx_negative_sequence);
      REQUIRE(
        sptest.backup_negative_sequence[i]->idx_negative_sequence ==
        sptest.backup_negative_sequence[i]->backup_idx_negative_sequence);
    }

    REQUIRE(
      sptest.negative_sequence[i]->idx_negative_sequence == i);

    REQUIRE(
      sptest.negative_sequence[pair_idx.first]->backup_idx_negative_sequence ==
      sptest.backup_negative_sequence[pair_idx.second]->backup_idx_negative_sequence);    
    
    REQUIRE(
      sptest.negative_sequence[pair_idx.second]->backup_idx_negative_sequence ==
      sptest.backup_negative_sequence[pair_idx.first]->backup_idx_negative_sequence);    

    REQUIRE(
      sptest.negative_sequence[pair_idx.first]->idx_negative_sequence !=
      sptest.negative_sequence[pair_idx.first]->backup_idx_negative_sequence);    
    
    REQUIRE(
      sptest.negative_sequence[pair_idx.second]->idx_negative_sequence !=
      sptest.negative_sequence[pair_idx.second]->backup_idx_negative_sequence);    
  }
  
  for (size_t i = 0; i < sptest.num_blocks; ++i) {
    REQUIRE(
      sptest.positive_sequence[i]->lower_left_x ==
      sptest.positive_sequence[i]->backup_lower_left_x);
    REQUIRE(
      sptest.positive_sequence[i]->lower_left_y ==
      sptest.positive_sequence[i]->backup_lower_left_y);
    REQUIRE(
      sptest.positive_sequence[i]->width ==
      sptest.positive_sequence[i]->backup_width);
    REQUIRE(
      sptest.positive_sequence[i]->height ==
      sptest.positive_sequence[i]->backup_height);
    REQUIRE(
      sptest.positive_sequence[i]->rightof ==
      sptest.positive_sequence[i]->backup_rightof);
    REQUIRE(
      sptest.positive_sequence[i]->aboveof ==
      sptest.positive_sequence[i]->backup_aboveof);
  }
}


// verify move3
TEST_CASE("verify move3" * doctest::timeout(600)) {
  SPTest sptest;
  sptest.initialize_sequence();
  sptest.initialize_backup_data();

  std::pair<size_t, size_t> pair_idx = sptest.move3();
  REQUIRE(pair_idx.first < pair_idx.second);

  REQUIRE(sptest.positive_sequence[pair_idx.first] == 
          sptest.backup_positive_sequence[pair_idx.second]);
  
  REQUIRE(sptest.positive_sequence[pair_idx.second] ==
          sptest.backup_positive_sequence[pair_idx.first]);
  
  for (size_t i = 0; i < sptest.positive_sequence.size(); ++i) {
    if (i != pair_idx.first && i != pair_idx.second) {
      REQUIRE(sptest.positive_sequence[i] == 
              sptest.backup_positive_sequence[i]);
      REQUIRE(sptest.positive_sequence[i]->idx_positive_sequence ==
              sptest.positive_sequence[i]->backup_idx_positive_sequence);
      REQUIRE(sptest.backup_positive_sequence[i]->idx_positive_sequence ==
              sptest.backup_positive_sequence[i]->backup_idx_positive_sequence);
    }

    REQUIRE(sptest.positive_sequence[i]->idx_positive_sequence == i);

    REQUIRE(
      sptest.positive_sequence[pair_idx.first]->backup_idx_positive_sequence ==
      sptest.backup_positive_sequence[pair_idx.second]->backup_idx_positive_sequence);    
    
    REQUIRE(
      sptest.positive_sequence[pair_idx.second]->backup_idx_positive_sequence ==
      sptest.backup_positive_sequence[pair_idx.first]->backup_idx_positive_sequence);    

    REQUIRE(
      sptest.positive_sequence[pair_idx.first]->idx_positive_sequence !=
      sptest.positive_sequence[pair_idx.first]->backup_idx_positive_sequence);    
    
    REQUIRE(
      sptest.positive_sequence[pair_idx.second]->idx_positive_sequence !=
      sptest.positive_sequence[pair_idx.second]->backup_idx_positive_sequence);    
  }
  
  
  size_t nid1 = sptest.positive_sequence[pair_idx.first]->idx_negative_sequence;
  size_t nid2 = sptest.positive_sequence[pair_idx.second]->idx_negative_sequence; 
  
  if (nid1 > nid2) {
    std::swap(nid1, nid2);
  }
  REQUIRE(sptest.negative_sequence[nid1] == 
          sptest.backup_negative_sequence[nid2]);
  
  REQUIRE(sptest.negative_sequence[nid2] ==
          sptest.backup_negative_sequence[nid1]);
  
  for (size_t i = 0; i < sptest.negative_sequence.size(); ++i) {
    if (i != nid1 && i != nid2) {
      REQUIRE(
        sptest.negative_sequence[i] == 
        sptest.backup_negative_sequence[i]);
      REQUIRE(
        sptest.negative_sequence[i]->idx_negative_sequence ==
        sptest.negative_sequence[i]->backup_idx_negative_sequence);
      REQUIRE(
        sptest.backup_negative_sequence[i]->idx_negative_sequence ==
        sptest.backup_negative_sequence[i]->backup_idx_negative_sequence);
    }

    REQUIRE(
      sptest.negative_sequence[i]->idx_negative_sequence == i);

    REQUIRE(
      sptest.negative_sequence[nid1]->backup_idx_negative_sequence ==
      sptest.backup_negative_sequence[nid2]->backup_idx_negative_sequence);    
    
    REQUIRE(
      sptest.negative_sequence[nid2]->backup_idx_negative_sequence ==
      sptest.backup_negative_sequence[nid1]->backup_idx_negative_sequence);    

    REQUIRE(
      sptest.negative_sequence[nid1]->idx_negative_sequence !=
      sptest.negative_sequence[nid1]->backup_idx_negative_sequence);    
    
    REQUIRE(
      sptest.negative_sequence[nid2]->idx_negative_sequence !=
      sptest.negative_sequence[nid2]->backup_idx_negative_sequence);    
  }

  for (size_t i = 0; i < sptest.num_blocks; ++i) {
    REQUIRE(
      sptest.positive_sequence[i]->lower_left_x ==
      sptest.positive_sequence[i]->backup_lower_left_x);
    REQUIRE(
      sptest.positive_sequence[i]->lower_left_y ==
      sptest.positive_sequence[i]->backup_lower_left_y);
    REQUIRE(
      sptest.positive_sequence[i]->width ==
      sptest.positive_sequence[i]->backup_width);
    REQUIRE(
      sptest.positive_sequence[i]->height ==
      sptest.positive_sequence[i]->backup_height);
    REQUIRE(
      sptest.positive_sequence[i]->rightof ==
      sptest.positive_sequence[i]->backup_rightof);
    REQUIRE(
      sptest.positive_sequence[i]->aboveof ==
      sptest.positive_sequence[i]->backup_aboveof);
  }

  for (size_t i = 0; i < sptest.num_blocks; ++i) {
    REQUIRE(
      sptest.negative_sequence[i]->lower_left_x ==
      sptest.negative_sequence[i]->backup_lower_left_x);
    REQUIRE(
      sptest.negative_sequence[i]->lower_left_y ==
      sptest.negative_sequence[i]->backup_lower_left_y);
    REQUIRE(
      sptest.negative_sequence[i]->width ==
      sptest.negative_sequence[i]->backup_width);
    REQUIRE(
      sptest.negative_sequence[i]->height ==
      sptest.negative_sequence[i]->backup_height);
    REQUIRE(
      sptest.negative_sequence[i]->rightof ==
      sptest.negative_sequence[i]->backup_rightof);
    REQUIRE(
      sptest.negative_sequence[i]->aboveof ==
      sptest.negative_sequence[i]->backup_aboveof);
  }
}  


// verify move4
TEST_CASE("verify move4" * doctest::timeout(600)) {
  SPTest sptest;
  sptest.initialize_sequence();
  sptest.initialize_backup_data();

  std::vector<std::pair<size_t, size_t>> old_positive_sequence;
  std::vector<std::pair<size_t, size_t>> old_negative_sequence; 

  for (auto& ps : sptest.positive_sequence) {
    old_positive_sequence.push_back(std::make_pair(ps->width, ps->height));
  }

  for (auto& ns : sptest.negative_sequence) {
    old_negative_sequence.push_back(std::make_pair(ns->width, ns->height));
  }

  sptest.move4();

  size_t changes = 0;
  
  for (size_t i = 0; i < old_positive_sequence.size(); ++i) {
    if (old_positive_sequence[i].first == sptest.positive_sequence[i]->width &&
        old_positive_sequence[i].second == sptest.positive_sequence[i]->height) {
      continue;
    }
    ++changes;
  }
  REQUIRE(changes == 1);

  for (size_t i = 0; i < old_negative_sequence.size(); ++i) {
    if (old_negative_sequence[i].first == sptest.negative_sequence[i]->width &&
        old_negative_sequence[i].second == sptest.negative_sequence[i]->height) {
      continue;
    }
    ++changes;
  }
  REQUIRE(changes == 2);

  for (size_t i = 0; i < sptest.positive_sequence.size(); ++i) {
    REQUIRE(sptest.positive_sequence[i] == sptest.backup_positive_sequence[i]); 
    REQUIRE(sptest.negative_sequence[i] == sptest.backup_negative_sequence[i]); 
    REQUIRE(
      sptest.positive_sequence[i]->lower_left_x == 
      sptest.positive_sequence[i]->backup_lower_left_x);
    REQUIRE(
      sptest.positive_sequence[i]->lower_left_y == 
      sptest.positive_sequence[i]->backup_lower_left_y);
    REQUIRE(sptest.positive_sequence[i]->idx_positive_sequence == i);
    REQUIRE(sptest.positive_sequence[i]->backup_idx_positive_sequence == i);
    REQUIRE(
      sptest.positive_sequence[i]->idx_negative_sequence ==
      sptest.positive_sequence[i]->backup_idx_negative_sequence);
    REQUIRE(
      sptest.positive_sequence[i]->rightof == 
      sptest.positive_sequence[i]->backup_rightof);
    REQUIRE(
      sptest.positive_sequence[i]->aboveof == 
      sptest.positive_sequence[i]->backup_aboveof);
  }
}


// verify update_backup_data
TEST_CASE("verify update_backup_data" * doctest::timeout(600)) {
  SPTest sptest;
  sptest.initialize_sequence();
  
  // positive sequence = [bk1, bk3, bk2, bk5, bk4]
  // negative sequence = [bk4, bk1, bk3, bk5, bk2]
  sptest.positive_sequence[0] = &(sptest.map_blocks["bk1"]);
  sptest.map_blocks["bk1"].idx_positive_sequence = 0;
  sptest.positive_sequence[1] = &(sptest.map_blocks["bk3"]);
  sptest.map_blocks["bk3"].idx_positive_sequence = 1;
  sptest.positive_sequence[2] = &(sptest.map_blocks["bk2"]);
  sptest.map_blocks["bk2"].idx_positive_sequence = 2;
  sptest.positive_sequence[3] = &(sptest.map_blocks["bk5"]);
  sptest.map_blocks["bk5"].idx_positive_sequence = 3;
  sptest.positive_sequence[4] = &(sptest.map_blocks["bk4"]);
  sptest.map_blocks["bk4"].idx_positive_sequence = 4;
  
  sptest.negative_sequence[0] = &(sptest.map_blocks["bk4"]);
  sptest.map_blocks["bk4"].idx_negative_sequence = 0;
  sptest.negative_sequence[1] = &(sptest.map_blocks["bk1"]);
  sptest.map_blocks["bk1"].idx_negative_sequence = 1;
  sptest.negative_sequence[2] = &(sptest.map_blocks["bk3"]);
  sptest.map_blocks["bk3"].idx_negative_sequence = 2;
  sptest.negative_sequence[3] = &(sptest.map_blocks["bk5"]);
  sptest.map_blocks["bk5"].idx_negative_sequence = 3;
  sptest.negative_sequence[4] = &(sptest.map_blocks["bk2"]);
  sptest.map_blocks["bk2"].idx_negative_sequence = 4;

  sptest.construct_relative_locations(0, 4, MoveType::nomove);
  
  std::vector<int> distance = sptest.spfa(Orientation::Horizontal);
  sptest.compute_block_locations(distance, Orientation::Horizontal);

  distance = sptest.spfa(Orientation::Vertical);
  sptest.compute_block_locations(distance, Orientation::Vertical);

  sptest.initialize_backup_data();

  SUBCASE("SUB : update after move1") {
    std::pair<size_t, size_t> pair_idx = sptest.move1();
    sptest.update_backup_data(pair_idx.first, pair_idx.second, MoveType::move1);

    for (size_t i = 0; i < sptest.num_blocks; ++i) {
      REQUIRE(
        sptest.positive_sequence[i] ==
        sptest.backup_positive_sequence[i]);
      REQUIRE(
        sptest.negative_sequence[i] ==
        sptest.backup_negative_sequence[i]);

      REQUIRE(
        sptest.positive_sequence[i]->idx_positive_sequence == i);
      
      REQUIRE(
        sptest.positive_sequence[i]->idx_positive_sequence ==
        sptest.positive_sequence[i]->backup_idx_positive_sequence);
      REQUIRE(
        sptest.positive_sequence[i]->idx_negative_sequence ==
        sptest.positive_sequence[i]->backup_idx_negative_sequence);
      
      REQUIRE(
        sptest.positive_sequence[i]->idx_positive_sequence ==
        sptest.backup_positive_sequence[i]->idx_positive_sequence);
      REQUIRE(
        sptest.positive_sequence[i]->idx_negative_sequence ==
        sptest.backup_positive_sequence[i]->idx_negative_sequence);
      
      REQUIRE(
        sptest.backup_positive_sequence[i]->idx_positive_sequence ==
        sptest.backup_positive_sequence[i]->backup_idx_positive_sequence);
      REQUIRE(
        sptest.backup_positive_sequence[i]->idx_negative_sequence ==
        sptest.backup_positive_sequence[i]->backup_idx_negative_sequence);
      
      REQUIRE(
        sptest.positive_sequence[i]->width ==
        sptest.positive_sequence[i]->backup_width);
      REQUIRE(
        sptest.positive_sequence[i]->height ==
        sptest.positive_sequence[i]->backup_height);
    
      REQUIRE(
        sptest.positive_sequence[i]->rightof ==
        sptest.positive_sequence[i]->backup_rightof);
      REQUIRE(
        sptest.positive_sequence[i]->aboveof ==
        sptest.positive_sequence[i]->backup_aboveof);
    }
  }
  
  SUBCASE("SUB : update after move2") {
    std::pair<size_t, size_t> pair_idx = sptest.move2();
    sptest.update_backup_data(pair_idx.first, pair_idx.second, MoveType::move2);

    for (size_t i = 0; i < sptest.num_blocks; ++i) {
      REQUIRE(
        sptest.positive_sequence[i] ==
        sptest.backup_positive_sequence[i]);
      REQUIRE(
        sptest.negative_sequence[i] ==
        sptest.backup_negative_sequence[i]);

      REQUIRE(
        sptest.positive_sequence[i]->idx_positive_sequence == i);
      
      REQUIRE(
        sptest.positive_sequence[i]->idx_positive_sequence ==
        sptest.positive_sequence[i]->backup_idx_positive_sequence);
      REQUIRE(
        sptest.positive_sequence[i]->idx_negative_sequence ==
        sptest.positive_sequence[i]->backup_idx_negative_sequence);
      
      REQUIRE(
        sptest.positive_sequence[i]->idx_positive_sequence ==
        sptest.backup_positive_sequence[i]->idx_positive_sequence);
      REQUIRE(
        sptest.positive_sequence[i]->idx_negative_sequence ==
        sptest.backup_positive_sequence[i]->idx_negative_sequence);
      
      REQUIRE(
        sptest.backup_positive_sequence[i]->idx_positive_sequence ==
        sptest.backup_positive_sequence[i]->backup_idx_positive_sequence);
      REQUIRE(
        sptest.backup_positive_sequence[i]->idx_negative_sequence ==
        sptest.backup_positive_sequence[i]->backup_idx_negative_sequence);
      
      REQUIRE(
        sptest.positive_sequence[i]->width ==
        sptest.positive_sequence[i]->backup_width);
      REQUIRE(
        sptest.positive_sequence[i]->height ==
        sptest.positive_sequence[i]->backup_height);
    
      REQUIRE(
        sptest.positive_sequence[i]->rightof ==
        sptest.positive_sequence[i]->backup_rightof);
      REQUIRE(
        sptest.positive_sequence[i]->aboveof ==
        sptest.positive_sequence[i]->backup_aboveof);
    }
  }

  SUBCASE("SUB : update after move3") {
    std::pair<size_t, size_t> pair_idx = sptest.move3();
    sptest.update_backup_data(pair_idx.first, pair_idx.second, MoveType::move3);

    for (size_t i = 0; i < sptest.num_blocks; ++i) {
      REQUIRE(
        sptest.positive_sequence[i] ==
        sptest.backup_positive_sequence[i]);
      REQUIRE(
        sptest.negative_sequence[i] ==
        sptest.backup_negative_sequence[i]);

      REQUIRE(
        sptest.positive_sequence[i]->idx_positive_sequence == i);
      
      REQUIRE(
        sptest.positive_sequence[i]->idx_positive_sequence ==
        sptest.positive_sequence[i]->backup_idx_positive_sequence);
      REQUIRE(
        sptest.positive_sequence[i]->idx_negative_sequence ==
        sptest.positive_sequence[i]->backup_idx_negative_sequence);
      
      REQUIRE(
        sptest.positive_sequence[i]->idx_positive_sequence ==
        sptest.backup_positive_sequence[i]->idx_positive_sequence);
      REQUIRE(
        sptest.positive_sequence[i]->idx_negative_sequence ==
        sptest.backup_positive_sequence[i]->idx_negative_sequence);
      
      REQUIRE(
        sptest.backup_positive_sequence[i]->idx_positive_sequence ==
        sptest.backup_positive_sequence[i]->backup_idx_positive_sequence);
      REQUIRE(
        sptest.backup_positive_sequence[i]->idx_negative_sequence ==
        sptest.backup_positive_sequence[i]->backup_idx_negative_sequence);
      
      REQUIRE(
        sptest.positive_sequence[i]->width ==
        sptest.positive_sequence[i]->backup_width);
      REQUIRE(
        sptest.positive_sequence[i]->height ==
        sptest.positive_sequence[i]->backup_height);
    
      REQUIRE(
        sptest.positive_sequence[i]->rightof ==
        sptest.positive_sequence[i]->backup_rightof);
      REQUIRE(
        sptest.positive_sequence[i]->aboveof ==
        sptest.positive_sequence[i]->backup_aboveof);
    }
  }
  
  SUBCASE("SUB : update after move4") {
    std::pair<size_t, size_t> pair_idx = sptest.move4();
    sptest.update_backup_data(pair_idx.first, pair_idx.second, MoveType::move4);

    for (size_t i = 0; i < sptest.num_blocks; ++i) {
      REQUIRE(
        sptest.positive_sequence[i] ==
        sptest.backup_positive_sequence[i]);
      REQUIRE(
        sptest.negative_sequence[i] ==
        sptest.backup_negative_sequence[i]);

      REQUIRE(
        sptest.positive_sequence[i]->idx_positive_sequence == i);
      
      REQUIRE(
        sptest.positive_sequence[i]->idx_positive_sequence ==
        sptest.positive_sequence[i]->backup_idx_positive_sequence);
      REQUIRE(
        sptest.positive_sequence[i]->idx_negative_sequence ==
        sptest.positive_sequence[i]->backup_idx_negative_sequence);
      
      REQUIRE(
        sptest.positive_sequence[i]->idx_positive_sequence ==
        sptest.backup_positive_sequence[i]->idx_positive_sequence);
      REQUIRE(
        sptest.positive_sequence[i]->idx_negative_sequence ==
        sptest.backup_positive_sequence[i]->idx_negative_sequence);
      
      REQUIRE(
        sptest.backup_positive_sequence[i]->idx_positive_sequence ==
        sptest.backup_positive_sequence[i]->backup_idx_positive_sequence);
      REQUIRE(
        sptest.backup_positive_sequence[i]->idx_negative_sequence ==
        sptest.backup_positive_sequence[i]->backup_idx_negative_sequence);
      
      REQUIRE(
        sptest.positive_sequence[i]->width ==
        sptest.positive_sequence[i]->backup_width);
      REQUIRE(
        sptest.positive_sequence[i]->height ==
        sptest.positive_sequence[i]->backup_height);
    
      REQUIRE(
        sptest.positive_sequence[i]->rightof ==
        sptest.positive_sequence[i]->backup_rightof);
      REQUIRE(
        sptest.positive_sequence[i]->aboveof ==
        sptest.positive_sequence[i]->backup_aboveof);
    }
  }
}


// verify resume_backup_data
TEST_CASE("verify_resume_backup_data" * doctest::timeout(600)) {
  SPTest sptest;
  sptest.initialize_sequence();
  
  // positive sequence = [bk1, bk3, bk2, bk5, bk4]
  // negative sequence = [bk4, bk1, bk3, bk5, bk2]
  sptest.positive_sequence[0] = &(sptest.map_blocks["bk1"]);
  sptest.map_blocks["bk1"].idx_positive_sequence = 0;
  sptest.positive_sequence[1] = &(sptest.map_blocks["bk3"]);
  sptest.map_blocks["bk3"].idx_positive_sequence = 1;
  sptest.positive_sequence[2] = &(sptest.map_blocks["bk2"]);
  sptest.map_blocks["bk2"].idx_positive_sequence = 2;
  sptest.positive_sequence[3] = &(sptest.map_blocks["bk5"]);
  sptest.map_blocks["bk5"].idx_positive_sequence = 3;
  sptest.positive_sequence[4] = &(sptest.map_blocks["bk4"]);
  sptest.map_blocks["bk4"].idx_positive_sequence = 4;
  
  sptest.negative_sequence[0] = &(sptest.map_blocks["bk4"]);
  sptest.map_blocks["bk4"].idx_negative_sequence = 0;
  sptest.negative_sequence[1] = &(sptest.map_blocks["bk1"]);
  sptest.map_blocks["bk1"].idx_negative_sequence = 1;
  sptest.negative_sequence[2] = &(sptest.map_blocks["bk3"]);
  sptest.map_blocks["bk3"].idx_negative_sequence = 2;
  sptest.negative_sequence[3] = &(sptest.map_blocks["bk5"]);
  sptest.map_blocks["bk5"].idx_negative_sequence = 3;
  sptest.negative_sequence[4] = &(sptest.map_blocks["bk2"]);
  sptest.map_blocks["bk2"].idx_negative_sequence = 4;

  sptest.construct_relative_locations(0, 4, MoveType::nomove);
  
  std::vector<int> distance = sptest.spfa(Orientation::Horizontal);
  sptest.compute_block_locations(distance, Orientation::Horizontal);

  distance = sptest.spfa(Orientation::Vertical);
  sptest.compute_block_locations(distance, Orientation::Vertical);
  
  sptest.compute_hpwl();

  sptest.initialize_backup_data();

  SUBCASE("SUB : resume after move1") {
    std::pair<size_t, size_t> pair_idx = sptest.move1();
    sptest.resume_backup_data(pair_idx.first, pair_idx.second, MoveType::move1);

    for (size_t i = 0; i < sptest.num_blocks; ++i) {
      REQUIRE(
        sptest.positive_sequence[i] ==
        sptest.backup_positive_sequence[i]);
      REQUIRE(
        sptest.negative_sequence[i] ==
        sptest.backup_negative_sequence[i]);

      REQUIRE(
        sptest.positive_sequence[i]->idx_positive_sequence == i);
      
      REQUIRE(
        sptest.positive_sequence[i]->idx_positive_sequence ==
        sptest.positive_sequence[i]->backup_idx_positive_sequence);
      REQUIRE(
        sptest.positive_sequence[i]->idx_negative_sequence ==
        sptest.positive_sequence[i]->backup_idx_negative_sequence);
      
      REQUIRE(
        sptest.positive_sequence[i]->idx_positive_sequence ==
        sptest.backup_positive_sequence[i]->idx_positive_sequence);
      REQUIRE(
        sptest.positive_sequence[i]->idx_negative_sequence ==
        sptest.backup_positive_sequence[i]->idx_negative_sequence);
      
      REQUIRE(
        sptest.backup_positive_sequence[i]->idx_positive_sequence ==
        sptest.backup_positive_sequence[i]->backup_idx_positive_sequence);
      REQUIRE(
        sptest.backup_positive_sequence[i]->idx_negative_sequence ==
        sptest.backup_positive_sequence[i]->backup_idx_negative_sequence);
      
      REQUIRE(
        sptest.positive_sequence[i]->width ==
        sptest.positive_sequence[i]->backup_width);
      REQUIRE(
        sptest.positive_sequence[i]->height ==
        sptest.positive_sequence[i]->backup_height);
    
      REQUIRE(
        sptest.positive_sequence[i]->rightof ==
        sptest.positive_sequence[i]->backup_rightof);
      REQUIRE(
        sptest.positive_sequence[i]->aboveof ==
        sptest.positive_sequence[i]->backup_aboveof);
    }
  }
  
  SUBCASE("SUB : resume after move2") {
    std::pair<size_t, size_t> pair_idx = sptest.move2();
    sptest.resume_backup_data(pair_idx.first, pair_idx.second, MoveType::move2);

    for (size_t i = 0; i < sptest.num_blocks; ++i) {
      REQUIRE(
        sptest.positive_sequence[i] ==
        sptest.backup_positive_sequence[i]);
      REQUIRE(
        sptest.negative_sequence[i] ==
        sptest.backup_negative_sequence[i]);

      REQUIRE(
        sptest.positive_sequence[i]->idx_positive_sequence == i);
      
      REQUIRE(
        sptest.positive_sequence[i]->idx_positive_sequence ==
        sptest.positive_sequence[i]->backup_idx_positive_sequence);
      REQUIRE(
        sptest.positive_sequence[i]->idx_negative_sequence ==
        sptest.positive_sequence[i]->backup_idx_negative_sequence);
      
      REQUIRE(
        sptest.positive_sequence[i]->idx_positive_sequence ==
        sptest.backup_positive_sequence[i]->idx_positive_sequence);
      REQUIRE(
        sptest.positive_sequence[i]->idx_negative_sequence ==
        sptest.backup_positive_sequence[i]->idx_negative_sequence);
      
      REQUIRE(
        sptest.backup_positive_sequence[i]->idx_positive_sequence ==
        sptest.backup_positive_sequence[i]->backup_idx_positive_sequence);
      REQUIRE(
        sptest.backup_positive_sequence[i]->idx_negative_sequence ==
        sptest.backup_positive_sequence[i]->backup_idx_negative_sequence);
      
      REQUIRE(
        sptest.positive_sequence[i]->width ==
        sptest.positive_sequence[i]->backup_width);
      REQUIRE(
        sptest.positive_sequence[i]->height ==
        sptest.positive_sequence[i]->backup_height);
    
      REQUIRE(
        sptest.positive_sequence[i]->rightof ==
        sptest.positive_sequence[i]->backup_rightof);
      REQUIRE(
        sptest.positive_sequence[i]->aboveof ==
        sptest.positive_sequence[i]->backup_aboveof);
    }
  }
  
  SUBCASE("SUB : resume after move3") {
    std::pair<size_t, size_t> pair_idx = sptest.move3();
    sptest.resume_backup_data(pair_idx.first, pair_idx.second, MoveType::move3);

    for (size_t i = 0; i < sptest.num_blocks; ++i) {
      REQUIRE(
        sptest.positive_sequence[i] ==
        sptest.backup_positive_sequence[i]);
      REQUIRE(
        sptest.negative_sequence[i] ==
        sptest.backup_negative_sequence[i]);

      REQUIRE(
        sptest.positive_sequence[i]->idx_positive_sequence == i);
      
      REQUIRE(
        sptest.positive_sequence[i]->idx_positive_sequence ==
        sptest.positive_sequence[i]->backup_idx_positive_sequence);
      REQUIRE(
        sptest.positive_sequence[i]->idx_negative_sequence ==
        sptest.positive_sequence[i]->backup_idx_negative_sequence);
      
      REQUIRE(
        sptest.positive_sequence[i]->idx_positive_sequence ==
        sptest.backup_positive_sequence[i]->idx_positive_sequence);
      REQUIRE(
        sptest.positive_sequence[i]->idx_negative_sequence ==
        sptest.backup_positive_sequence[i]->idx_negative_sequence);
      
      REQUIRE(
        sptest.backup_positive_sequence[i]->idx_positive_sequence ==
        sptest.backup_positive_sequence[i]->backup_idx_positive_sequence);
      REQUIRE(
        sptest.backup_positive_sequence[i]->idx_negative_sequence ==
        sptest.backup_positive_sequence[i]->backup_idx_negative_sequence);
      
      REQUIRE(
        sptest.positive_sequence[i]->width ==
        sptest.positive_sequence[i]->backup_width);
      REQUIRE(
        sptest.positive_sequence[i]->height ==
        sptest.positive_sequence[i]->backup_height);
    
      REQUIRE(
        sptest.positive_sequence[i]->rightof ==
        sptest.positive_sequence[i]->backup_rightof);
      REQUIRE(
        sptest.positive_sequence[i]->aboveof ==
        sptest.positive_sequence[i]->backup_aboveof);
    }
  }
  
  SUBCASE("SUB : resume after move4") {
    std::pair<size_t, size_t> pair_idx = sptest.move4();
    sptest.resume_backup_data(pair_idx.first, pair_idx.second, MoveType::move4);

    for (size_t i = 0; i < sptest.num_blocks; ++i) {
      REQUIRE(
        sptest.positive_sequence[i] ==
        sptest.backup_positive_sequence[i]);
      REQUIRE(
        sptest.negative_sequence[i] ==
        sptest.backup_negative_sequence[i]);

      REQUIRE(
        sptest.positive_sequence[i]->idx_positive_sequence == i);
      
      REQUIRE(
        sptest.positive_sequence[i]->idx_positive_sequence ==
        sptest.positive_sequence[i]->backup_idx_positive_sequence);
      REQUIRE(
        sptest.positive_sequence[i]->idx_negative_sequence ==
        sptest.positive_sequence[i]->backup_idx_negative_sequence);
      
      REQUIRE(
        sptest.positive_sequence[i]->idx_positive_sequence ==
        sptest.backup_positive_sequence[i]->idx_positive_sequence);
      REQUIRE(
        sptest.positive_sequence[i]->idx_negative_sequence ==
        sptest.backup_positive_sequence[i]->idx_negative_sequence);
      
      REQUIRE(
        sptest.backup_positive_sequence[i]->idx_positive_sequence ==
        sptest.backup_positive_sequence[i]->backup_idx_positive_sequence);
      REQUIRE(
        sptest.backup_positive_sequence[i]->idx_negative_sequence ==
        sptest.backup_positive_sequence[i]->backup_idx_negative_sequence);
      
      REQUIRE(
        sptest.positive_sequence[i]->width ==
        sptest.positive_sequence[i]->backup_width);
      REQUIRE(
        sptest.positive_sequence[i]->height ==
        sptest.positive_sequence[i]->backup_height);
    
      REQUIRE(
        sptest.positive_sequence[i]->rightof ==
        sptest.positive_sequence[i]->backup_rightof);
      REQUIRE(
        sptest.positive_sequence[i]->aboveof ==
        sptest.positive_sequence[i]->backup_aboveof);
    }
  }
}

/*
// verify pack
TEST_CASE("verify_pack" * doctest::timeout(600)) {
  SPTest sptest;
  sptest.initialize_sequence();
  
  // positive sequence = [bk1, bk2, bk3, bk4, bk5]
  // negative sequence = [bk1, bk2, bk3, bk4, bk5]
  SUBCASE("SUB : case 1") { 
    sptest.positive_sequence[0] = &(sptest.map_blocks["bk1"]);
    sptest.map_blocks["bk1"].idx_positive_sequence = 0;
    sptest.positive_sequence[1] = &(sptest.map_blocks["bk2"]);
    sptest.map_blocks["bk2"].idx_positive_sequence = 1;
    sptest.positive_sequence[2] = &(sptest.map_blocks["bk3"]);
    sptest.map_blocks["bk3"].idx_positive_sequence = 2;
    sptest.positive_sequence[3] = &(sptest.map_blocks["bk4"]);
    sptest.map_blocks["bk4"].idx_positive_sequence = 3;
    sptest.positive_sequence[4] = &(sptest.map_blocks["bk5"]);
    sptest.map_blocks["bk5"].idx_positive_sequence = 4;
    
    sptest.negative_sequence[0] = &(sptest.map_blocks["bk1"]);
    sptest.map_blocks["bk1"].idx_negative_sequence = 0;
    sptest.negative_sequence[1] = &(sptest.map_blocks["bk2"]);
    sptest.map_blocks["bk2"].idx_negative_sequence = 1;
    sptest.negative_sequence[2] = &(sptest.map_blocks["bk3"]);
    sptest.map_blocks["bk3"].idx_negative_sequence = 2;
    sptest.negative_sequence[3] = &(sptest.map_blocks["bk4"]);
    sptest.map_blocks["bk4"].idx_negative_sequence = 3;
    sptest.negative_sequence[4] = &(sptest.map_blocks["bk5"]);
    sptest.map_blocks["bk5"].idx_negative_sequence = 4;

    sptest.construct_relative_locations(0, 4, MoveType::nomove);

    double cost = sptest.pack();
    REQUIRE(cost == 648600); 

    auto pair_idx = sptest.move1();

  }
  
  SUBCASE("SUB : case 2") { 
    sptest.positive_sequence[0] = &(sptest.map_blocks["bk1"]);
    sptest.map_blocks["bk1"].idx_positive_sequence = 0;
    sptest.positive_sequence[1] = &(sptest.map_blocks["bk3"]);
    sptest.map_blocks["bk3"].idx_positive_sequence = 1;
    sptest.positive_sequence[2] = &(sptest.map_blocks["bk2"]);
    sptest.map_blocks["bk2"].idx_positive_sequence = 2;
    sptest.positive_sequence[3] = &(sptest.map_blocks["bk5"]);
    sptest.map_blocks["bk5"].idx_positive_sequence = 3;
    sptest.positive_sequence[4] = &(sptest.map_blocks["bk4"]);
    sptest.map_blocks["bk4"].idx_positive_sequence = 4;
    
    sptest.negative_sequence[0] = &(sptest.map_blocks["bk4"]);
    sptest.map_blocks["bk4"].idx_negative_sequence = 0;
    sptest.negative_sequence[1] = &(sptest.map_blocks["bk1"]);
    sptest.map_blocks["bk1"].idx_negative_sequence = 1;
    sptest.negative_sequence[2] = &(sptest.map_blocks["bk3"]);
    sptest.map_blocks["bk3"].idx_negative_sequence = 2;
    sptest.negative_sequence[3] = &(sptest.map_blocks["bk5"]);
    sptest.map_blocks["bk5"].idx_negative_sequence = 3;
    sptest.negative_sequence[4] = &(sptest.map_blocks["bk2"]);
    sptest.map_blocks["bk2"].idx_negative_sequence = 4;

    sptest.construct_relative_locations(0, 4, MoveType::nomove);
    
    double cost = sptest.pack();
    REQUIRE(cost == 1008680); 
  }
}
*/






} //end namespace SP
