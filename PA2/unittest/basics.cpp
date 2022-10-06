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
 */

namespace fp {



class SPTest : public SP {
public:
  SPTest() {
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
    
    std::srand(std::time(nullptr)); 
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

    sptest.construct_relative_locations();
  
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

    sptest.construct_relative_locations();
  
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

    sptest.construct_relative_locations();
  
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

    sptest.construct_relative_locations();
    
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

    sptest.construct_relative_locations();
  
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

    sptest.construct_relative_locations();
    
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


// verify move1
TEST_CASE("verify_move1" * doctest::timeout(600)) {
  SPTest sptest;
  sptest.initialize_sequence();

 
  SUBCASE("SUB : positive sequence only") {
  
    std::vector<Block*> old_positive_sequence = sptest.positive_sequence;
    std::vector<Block*> old_negative_sequence = sptest.negative_sequence;

    sptest.move1(Sequence::POS);

    size_t changes = 0;
    for (size_t i = 0; i < sptest.positive_sequence.size(); ++i) {
      if (sptest.positive_sequence[i] == old_positive_sequence[i]) {
        continue;
      }
      else {
        auto found = std::find(
          old_positive_sequence.begin(), 
          old_positive_sequence.end(), 
          sptest.positive_sequence[i]);
        REQUIRE(found != old_positive_sequence.end());
        REQUIRE(sptest.positive_sequence[i]->name == (*found)->name);
        ++changes;
      }
    }
    REQUIRE(changes == 2);

    for (size_t i = 0; i < sptest.negative_sequence.size(); ++i) {
      REQUIRE(old_negative_sequence[i] == sptest.negative_sequence[i]);
    }
  }
  
  SUBCASE("SUB : negative sequence only") {
  
    std::vector<Block*> old_positive_sequence = sptest.positive_sequence;
    std::vector<Block*> old_negative_sequence = sptest.negative_sequence;

    sptest.move1(Sequence::NEG);

    size_t changes = 0;
    for (size_t i = 0; i < sptest.negative_sequence.size(); ++i) {
      if (sptest.negative_sequence[i] == old_negative_sequence[i]) {
        continue;
      }
      else {
        auto found = std::find(
          old_negative_sequence.begin(), 
          old_negative_sequence.end(), 
          sptest.negative_sequence[i]);
        REQUIRE(found != old_negative_sequence.end());
        REQUIRE(sptest.negative_sequence[i]->name == (*found)->name);
        ++changes;
      }
    }
    REQUIRE(changes == 2);

    for (size_t i = 0; i < sptest.positive_sequence.size(); ++i) {
      REQUIRE(old_positive_sequence[i] == sptest.positive_sequence[i]);
    }
  }

  SUBCASE("SUB : both sequences") {
    std::vector<Block*> old_positive_sequence = sptest.positive_sequence;
    std::vector<Block*> old_negative_sequence = sptest.negative_sequence;
    
    std::set<std::string> names;

    sptest.move1(Sequence::BOTH);

    size_t changes = 0;
    for (size_t i = 0; i < sptest.positive_sequence.size(); ++i) {
      if (sptest.positive_sequence[i] == old_positive_sequence[i]) {
        continue;
      }
      else {
        names.insert(sptest.positive_sequence[i]->name);
        auto found = std::find(
          old_positive_sequence.begin(), 
          old_positive_sequence.end(), 
          sptest.positive_sequence[i]);
        REQUIRE(found != old_positive_sequence.end());
        REQUIRE(sptest.positive_sequence[i]->name == (*found)->name);
        ++changes;
      }
    }
    REQUIRE(changes == 2);
    
    changes = 0;
    for (size_t i = 0; i < sptest.negative_sequence.size(); ++i) {
      if (sptest.negative_sequence[i] == old_negative_sequence[i]) {
        continue;
      }
      else {
        names.insert(sptest.negative_sequence[i]->name);
        auto found = std::find(
          old_negative_sequence.begin(), 
          old_negative_sequence.end(), 
          sptest.negative_sequence[i]);
        REQUIRE(found != old_negative_sequence.end());
        REQUIRE(sptest.negative_sequence[i]->name == (*found)->name);
        ++changes;
      }
    }
    REQUIRE(changes == 2);
    REQUIRE(names.size() == 2);
  }
}


// verify move2
TEST_CASE("verify_move2" * doctest::timeout(600)) {
  SPTest sptest;
  sptest.initialize_sequence();
  
  std::vector<Block*> old_positive_sequence = sptest.positive_sequence;
  std::vector<Block*> old_negative_sequence = sptest.negative_sequence;
  
  sptest.move2();

  size_t changes = 0;
  for (size_t i = 0; i < sptest.positive_sequence.size(); ++i) {
    if (sptest.positive_sequence[i]->width  == old_positive_sequence[i]->width &&
        sptest.positive_sequence[i]->height == old_positive_sequence[i]->height) {
      continue;
    }
    else {
      ++changes;
      REQUIRE(changes == 1);
    }
  }

  for (size_t i = 0; i < sptest.negative_sequence.size(); ++i) {
    if (sptest.negative_sequence[i]->width  == old_negative_sequence[i]->width &&
        sptest.negative_sequence[i]->height == old_negative_sequence[i]->height) {
      continue;
    }
    else {
      ++changes;
      REQUIRE(changes == 1);
    }
  }
}









} //end namespace SP
