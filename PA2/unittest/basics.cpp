#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <ctime>
#include <iostream>
#include <doctest.h>
#include <string>
#include <unordered_map>
#include <variant>
#include <list>
#include <iterator>
#include "sp.hpp"


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
  // check positive_sequence
  for (auto& sq : sptest.positive_sequence) {
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
  for (auto& sq : sptest.negative_sequence) {
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


// verify move1
TEST_CASE("verify_move1" * doctest_timeout(600)) {
  SPTest sptest;
  sptest.initialize_sequence();

 
  SUBCASE("SUB : positive sequence only") {
  
  std::vector<Block*> old_positive_sequence = sptest.positive_sequence;
  std::vector<Block*> old_negative_sequence = sptest.negative_sequence;

  sptest.move1(0);

  size_t changes = 0;
  for (size_t i = 0; i < sptest.positive_sequence.size(); ++i) {
    if (sptest.positive_sequence[i] == old_positive_sequence[i]) {
      continue;
    }
  }

}













/*



// verify the initial gain
TEST_CASE("verify_initial_gain" * doctest::timeout(600)) {
  std::srand(std::time(nullptr));

  HypergraphTest hypergraph;
 
  //Hypergraph hypergraph(input_file, output_file);
  //hypergraph.traverse();

  std::unordered_map<std::string, Cell>::iterator it;
  for (it = hypergraph.map_cells.begin(); 
       it != hypergraph.map_cells.end(); ++it) {
    if (it->second.name == "c1") {
      it->second.partition = 0;
    }
    else if(it->second.name == "c2") {
      it->second.partition = 0;
    }
    else if (it->second.name == "c3") {
      it->second.partition = 1;
    }
    else if (it->second.name == "c4") {
      it->second.partition = 1;
    }
    else if (it->second.name == "c5") {
      it->second.partition = 1;
    }
  }
 
  std::unordered_map<std::string, Net>::iterator it1;
  for (it1 = hypergraph.map_nets.begin(); 
       it1 != hypergraph.map_nets.end(); ++it1) {
    //it1->second.cut = false;
    it1->second.cnt_cells_p0 = 0;
    it1->second.cnt_cells_p1 = 0;
  }

  hypergraph.max_gain = -1000000;
  hypergraph.min_gain = 1000000;

  hypergraph.num_cells_p0 = 2;
  hypergraph.initialize_count_cells();
  hypergraph.initialize_gain(); 
  
  //hypergraph.display_partition();

  //hypergraph.display_cut();
  //
  //hypergraph.display_gain();

  //for (it1 = hypergraph.map_nets.begin();
  //     it1 != hypergraph.map_nets.end(); ++it1) {
  //  if (it1->second.name == "n2" || 
  //      it1->second.name == "n3" ||
  //      it1->second.name == "n4") {
  //    REQUIRE(it1->second.cut == true);
  //  }
  //  else {
  //    REQUIRE(it1->second.cut == false);
  //  }
  //}
   
  for (it = hypergraph.map_cells.begin(); 
       it != hypergraph.map_cells.end(); ++it) {
    if (it->second.name == "c1") {
      REQUIRE(it->second.gain == 1);
    }
    else if(it->second.name == "c2") {
      REQUIRE(it->second.gain == -1);
    }
    else if (it->second.name == "c3") {
      REQUIRE(it->second.gain == 0);
    }
    else if (it->second.name == "c4") {
      REQUIRE(it->second.gain == 0);
    }
    else if (it->second.name == "c5") {
      REQUIRE(it->second.gain == 1);
    }
  }
  REQUIRE(hypergraph.max_gain == 1);  
  REQUIRE(hypergraph.min_gain == -1);  
}


// verify the initial count of cells in each partition
TEST_CASE("verify_initial_count_cells_each_partition" * doctest::timeout(600)) {
  
  //Hypergraph hypergraph(input_file, output_file);
  HypergraphTest hypergraph;

  std::unordered_map<std::string, Cell>::iterator it;
  for (it = hypergraph.map_cells.begin(); 
       it != hypergraph.map_cells.end(); ++it) {
    if (it->second.name == "c1") {
      it->second.partition = 0;
    }
    else if(it->second.name == "c2") {
      it->second.partition = 0;
    }
    else if (it->second.name == "c3") {
      it->second.partition = 1;
    }
    else if (it->second.name == "c4") {
      it->second.partition = 1;
    }
    else if (it->second.name == "c5") {
      it->second.partition = 1;
    }
  }
  
  std::unordered_map<std::string, Net>::iterator it1;
  for (it1 = hypergraph.map_nets.begin(); 
       it1 != hypergraph.map_nets.end(); ++it1) {
    //it1->second.cut = false;
    it1->second.cnt_cells_p0 = 0;
    it1->second.cnt_cells_p1 = 0;
  }

  hypergraph.max_gain = -1000000;
  hypergraph.min_gain = 1000000;

  hypergraph.num_cells_p0 = 2;
  hypergraph.initialize_count_cells();
  hypergraph.initialize_gain(); 
  
  //hypergraph.traverse();
  //hypergraph.display_partition();
   
  for (it1 = hypergraph.map_nets.begin(); 
       it1 != hypergraph.map_nets.end(); ++it1) {
    if (it1->second.name == "n1") {
      REQUIRE(it1->second.cnt_cells_p0 == 2);
      REQUIRE(it1->second.cnt_cells_p1 == 0);
    }
    else if (it1->second.name == "n2") {
      REQUIRE(it1->second.cnt_cells_p0 == 2);
      REQUIRE(it1->second.cnt_cells_p1 == 1);
    }
    else if (it1->second.name == "n3") {
      REQUIRE(it1->second.cnt_cells_p0 == 1);
      REQUIRE(it1->second.cnt_cells_p1 == 1);
    }
    else if (it1->second.name == "n4") {
      REQUIRE(it1->second.cnt_cells_p0 == 1);
      REQUIRE(it1->second.cnt_cells_p1 == 1);
    }
    else if (it1->second.name == "n5") {
      REQUIRE(it1->second.cnt_cells_p0 == 0);
      REQUIRE(it1->second.cnt_cells_p1 == 2);
    }
  }
}


// verify the initial connected cells 
TEST_CASE("verify_initial_connected_cells" * doctest::timeout(600)) {
  
  //Hypergraph hypergraph(input_file, output_file);
  HypergraphTest hypergraph;

  std::unordered_map<std::string, Cell>::iterator it;
  for (it = hypergraph.map_cells.begin(); 
       it != hypergraph.map_cells.end(); ++it) {
    if (it->second.name == "c1") {
      it->second.partition = 0;
    }
    else if(it->second.name == "c2") {
      it->second.partition = 0;
    }
    else if (it->second.name == "c3") {
      it->second.partition = 1;
    }
    else if (it->second.name == "c4") {
      it->second.partition = 1;
    }
    else if (it->second.name == "c5") {
      it->second.partition = 1;
    }
  }
  
  std::unordered_map<std::string, Net>::iterator it1;
  for (it1 = hypergraph.map_nets.begin(); 
       it1 != hypergraph.map_nets.end(); ++it1) {
    //it1->second.cut = false;
    it1->second.cnt_cells_p0 = 0;
    it1->second.cnt_cells_p1 = 0;
  }

  hypergraph.max_gain = -1000000;
  hypergraph.min_gain = 1000000;

  hypergraph.num_cells_p0 = 2;
  hypergraph.initialize_count_cells();
  hypergraph.initialize_gain(); 
  
  for (it = hypergraph.map_cells.begin(); 
       it != hypergraph.map_cells.end(); ++it) {
    if (it->second.name == "c1") {
      REQUIRE(it->second.connected_cells.size() == 4);
    }
    
    else if (it->second.name == "c2") {
      REQUIRE(it->second.connected_cells.size() == 2);
      for (auto& cptr : it->second.connected_cells) {
        REQUIRE(cptr->name != "c5");
        REQUIRE(cptr->name != "c4");
      }
    }
    
    else if (it->second.name == "c3") {
      REQUIRE(it->second.connected_cells.size() == 3);
      for (auto& cptr : it->second.connected_cells) {
        REQUIRE(cptr->name != "c5");
      }
    }
    
    else if (it->second.name == "c4") {
      REQUIRE(it->second.connected_cells.size() == 2);
      for (auto& cptr : it->second.connected_cells) {
        REQUIRE(cptr->name != "c2");
        REQUIRE(cptr->name != "c5");
      }
    }
    
    else if (it->second.name == "c5") {
      REQUIRE(it->second.connected_cells.size() == 1);
      for (auto& cptr : it->second.connected_cells) {
        REQUIRE(cptr->name != "c2");
        REQUIRE(cptr->name != "c3");
        REQUIRE(cptr->name != "c4");
      }
    }
  }
}


// verify the initial bucket
TEST_CASE("verify_initial_bucket" * doctest::timeout(600)) {
  
  //Hypergraph hypergraph(input_file, output_file);
  HypergraphTest hypergraph;
  
  std::unordered_map<std::string, Cell>::iterator it;
  for (it = hypergraph.map_cells.begin(); 
       it != hypergraph.map_cells.end(); ++it) {
    if (it->second.name == "c1") {
      it->second.partition = 0;
    }
    else if(it->second.name == "c2") {
      it->second.partition = 0;
    }
    else if (it->second.name == "c3") {
      it->second.partition = 1;
    }
    else if (it->second.name == "c4") {
      it->second.partition = 1;
    }
    else if (it->second.name == "c5") {
      it->second.partition = 1;
    }
    it->second.prev = nullptr;
    it->second.next = nullptr;
  }
  
  std::unordered_map<std::string, Net>::iterator it1;
  for (it1 = hypergraph.map_nets.begin(); 
       it1 != hypergraph.map_nets.end(); ++it1) {
    //it1->second.cut = false;
    it1->second.cnt_cells_p0 = 0;
    it1->second.cnt_cells_p1 = 0;
  }

  hypergraph.max_gain = -1000000;
  hypergraph.min_gain = 1000000;

  hypergraph.num_cells_p0 = 2;
  hypergraph.max_edge = 4;
  hypergraph.initialize_count_cells();
  hypergraph.initialize_gain(); 

  hypergraph.bucket.clear();
   
  hypergraph.construct_bucket();
  
  REQUIRE(hypergraph.bucket.size() == hypergraph.max_edge*2+1);

  for (size_t i = 0; i < hypergraph.bucket.size(); ++i) {
    if (i < 3) {
      REQUIRE(hypergraph.bucket[i] == nullptr);
    }
    else if (i == 3) {
      Cell* head = hypergraph.bucket[i];
      REQUIRE(head->name == "c2");
      REQUIRE(head->prev == nullptr);
      REQUIRE(head->next == nullptr);
    }
    else if (i == 4) {
      Cell* head = hypergraph.bucket[i];
      size_t cnt = 1;
      while(head->next) {
        REQUIRE(head->name != "c1");
        REQUIRE(head->name != "c2");
        REQUIRE(head->name != "c5");
        head = head->next;  
        ++cnt;
      }
      REQUIRE(cnt == 2);
      REQUIRE(head->name != "c1");
      REQUIRE(head->name != "c2");
      REQUIRE(head->name != "c5");
      REQUIRE(head->prev->name != "c1"); 
      REQUIRE(head->prev->name != "c2"); 
      REQUIRE(head->prev->name != "c5"); 
    }
    else if (i == 5) {
      Cell* head = hypergraph.bucket[i];
      size_t cnt = 1;
      while(head->next) {
        REQUIRE(head->name != "c2");
        REQUIRE(head->name != "c3");
        REQUIRE(head->name != "c4");
        head = head->next;  
        ++cnt;
      }
      REQUIRE(cnt == 2);
      REQUIRE(head->name != "c2");
      REQUIRE(head->name != "c3");
      REQUIRE(head->name != "c4");
      REQUIRE(head->prev->name != "c2"); 
      REQUIRE(head->prev->name != "c3"); 
      REQUIRE(head->prev->name != "c4"); 
    }
    else if (i > 5) {
      REQUIRE(hypergraph.bucket[i] == nullptr);
    }
  }
}  


// verify the balance_criterion
TEST_CASE("verify_balance_criterion" * doctest::timeout(600)) {
  
  //Hypergraph hypergraph(input_file, output_file);
  HypergraphTest hypergraph;
  
  std::unordered_map<std::string, Cell>::iterator it;
  for (it = hypergraph.map_cells.begin(); 
       it != hypergraph.map_cells.end(); ++it) {
    if (it->second.name == "c1") {
      it->second.partition = 0;
    }
    else if(it->second.name == "c2") {
      it->second.partition = 0;
    }
    else if (it->second.name == "c3") {
      it->second.partition = 1;
    }
    else if (it->second.name == "c4") {
      it->second.partition = 1;
    }
    else if (it->second.name == "c5") {
      it->second.partition = 1;
    }
    it->second.prev = nullptr;
    it->second.next = nullptr;
  }
  
  std::unordered_map<std::string, Net>::iterator it1;
  for (it1 = hypergraph.map_nets.begin(); 
       it1 != hypergraph.map_nets.end(); ++it1) {
    //it1->second.cut = false;
    it1->second.cnt_cells_p0 = 0;
    it1->second.cnt_cells_p1 = 0;
  }

  hypergraph.max_gain = -1000000;
  hypergraph.min_gain = 1000000;
  hypergraph.num_cells_p0 = 2;
  hypergraph.max_edge = 4;
  hypergraph.area_lower_bound = 1.25;
  hypergraph.area_upper_bound = 3.75;
  hypergraph.initialize_count_cells();
  hypergraph.initialize_gain(); 
   
  for (it = hypergraph.map_cells.begin(); 
       it != hypergraph.map_cells.end(); 
       ++it) {
    
    if (it->second.name == "c1") {
      REQUIRE(hypergraph.meet_balance_criterion(&(it->second)) == false);  
    }
    else if (it->second.name == "c2") {
      REQUIRE(hypergraph.meet_balance_criterion(&(it->second)) == false);  
    }
    else if (it->second.name == "c3") {
      REQUIRE(hypergraph.meet_balance_criterion(&(it->second)) == true);  
    }
    else if (it->second.name == "c4") {
      REQUIRE(hypergraph.meet_balance_criterion(&(it->second)) == true);  
    }
    else if (it->second.name == "c5") {
      REQUIRE(hypergraph.meet_balance_criterion(&(it->second)) == true);  
    }
  } 
}


// verify the update_gain of c1 moved
TEST_CASE("verify_update_gain" * doctest::timeout(600)) {
  
  //Hypergraph hypergraph(input_file, output_file);
  HypergraphTest hypergraph;
  
  std::unordered_map<std::string, Cell>::iterator it;
  for (it = hypergraph.map_cells.begin(); 
       it != hypergraph.map_cells.end(); ++it) {
    if (it->second.name == "c1") {
      it->second.partition = 0;
    }
    else if(it->second.name == "c2") {
      it->second.partition = 0;
    }
    else if (it->second.name == "c3") {
      it->second.partition = 1;
    }
    else if (it->second.name == "c4") {
      it->second.partition = 1;
    }
    else if (it->second.name == "c5") {
      it->second.partition = 1;
    }
    it->second.prev = nullptr;
    it->second.next = nullptr;
  }
  
  std::unordered_map<std::string, Net>::iterator it1;
  for (it1 = hypergraph.map_nets.begin(); 
       it1 != hypergraph.map_nets.end(); ++it1) {
    //it1->second.cut = false;
    it1->second.cnt_cells_p0 = 0;
    it1->second.cnt_cells_p1 = 0;
  }

  hypergraph.max_gain = -1000000;
  hypergraph.min_gain = 1000000;
  hypergraph.num_cells_p0 = 2;
  hypergraph.max_edge = 4;
  hypergraph.initialize_count_cells();
  hypergraph.initialize_gain(); 
  hypergraph.bucket.clear();
  hypergraph.construct_bucket();
 
 
  SUBCASE("SUB : move c1") {
    Cell* target = &(hypergraph.map_cells["c1"]);
    for (size_t i = 0; i < target->nets.size(); ++i) { 
      hypergraph.update_gain(target->nets[i], target);
    }
    //hypergraph.traverse();
    //hypergraph.display_cut();
    //hypergraph.display_gain();

    //for (it1 = hypergraph.map_nets.begin();
    //     it1 != hypergraph.map_nets.end(); ++it1) {
    //  if (it1->second.name == "n1") {
    //    REQUIRE(it1->second.cut == true);
    //  }
    //  else if (it1->second.name == "n2") {
    //    REQUIRE(it1->second.cut == true);
    //  }
    //  else if (it1->second.name == "n3") {
    //    REQUIRE(it1->second.cut == false);
    //  }
    //  else if (it1->second.name == "n4") {
    //    REQUIRE(it1->second.cut == false);
    //  }
    //  else if (it1->second.name == "n5") {
    //    REQUIRE(it1->second.cut == false);
    //  }
    //}
    
    for (it = hypergraph.map_cells.begin(); 
         it != hypergraph.map_cells.end(); ++it) {
      
      if (it->second.name == "c1") {
        continue;
        //REQUIRE(it->second.gain == -1);
      }
      else if (it->second.name == "c2") {
        REQUIRE(it->second.gain == 2);
      }
      else if (it->second.name == "c3") {
        REQUIRE(it->second.gain == -1);
      }
      else if (it->second.name == "c4") {
        REQUIRE(it->second.gain == -2);
      }
      else if (it->second.name == "c5") {
        REQUIRE(it->second.gain == -1);
      }
    }

    REQUIRE(hypergraph.map_nets["n1"].cnt_cells_p0 == 1);
    REQUIRE(hypergraph.map_nets["n1"].cnt_cells_p1 == 1);
    REQUIRE(hypergraph.map_nets["n2"].cnt_cells_p0 == 1);
    REQUIRE(hypergraph.map_nets["n2"].cnt_cells_p1 == 2);
    REQUIRE(hypergraph.map_nets["n3"].cnt_cells_p0 == 0);
    REQUIRE(hypergraph.map_nets["n3"].cnt_cells_p1 == 2);
    REQUIRE(hypergraph.map_nets["n4"].cnt_cells_p0 == 0);
    REQUIRE(hypergraph.map_nets["n4"].cnt_cells_p1 == 2);
    REQUIRE(hypergraph.map_nets["n5"].cnt_cells_p0 == 0);
    REQUIRE(hypergraph.map_nets["n5"].cnt_cells_p1 == 2);

  }
 
  SUBCASE("SUB : move c2") {
    Cell* target = &(hypergraph.map_cells["c2"]);
    for (size_t i = 0; i < target->nets.size(); ++i) { 
      hypergraph.update_gain(target->nets[i], target);
    }

    //hypergraph.traverse();
    //hypergraph.display_cut();
    //hypergraph.display_gain();
    
    //for (it1 = hypergraph.map_nets.begin();
    //     it1 != hypergraph.map_nets.end(); ++it1) {
    //  if (it1->second.name == "n1") {
    //    REQUIRE(it1->second.cut == true);
    //  }
    //  else if (it1->second.name == "n2") {
    //    REQUIRE(it1->second.cut == true);
    //  }
    //  else if (it1->second.name == "n3") {
    //    REQUIRE(it1->second.cut == true);
    //  }
    //  else if (it1->second.name == "n4") {
    //    REQUIRE(it1->second.cut == true);
    //  }
    //  else if (it1->second.name == "n5") {
    //    REQUIRE(it1->second.cut == false);
    //  }
    //}
    
    for (it = hypergraph.map_cells.begin(); 
         it != hypergraph.map_cells.end(); ++it) {
      if (it->second.name == "c1") {
        REQUIRE(it->second.gain == 4);
      }
      else if (it->second.name == "c2") {
        continue;
        //REQUIRE(it->second.gain == 1);
      }
      else if (it->second.name == "c3") {
        REQUIRE(it->second.gain == -1);
      }
      else if (it->second.name == "c4") {
        REQUIRE(it->second.gain == 0);
      }
      else if (it->second.name == "c5") {
        REQUIRE(it->second.gain == 1);
      }
    }
    REQUIRE(hypergraph.map_nets["n1"].cnt_cells_p0 == 1);
    REQUIRE(hypergraph.map_nets["n1"].cnt_cells_p1 == 1);
    REQUIRE(hypergraph.map_nets["n2"].cnt_cells_p0 == 1);
    REQUIRE(hypergraph.map_nets["n2"].cnt_cells_p1 == 2);
    REQUIRE(hypergraph.map_nets["n3"].cnt_cells_p0 == 1);
    REQUIRE(hypergraph.map_nets["n3"].cnt_cells_p1 == 1);
    REQUIRE(hypergraph.map_nets["n4"].cnt_cells_p0 == 1);
    REQUIRE(hypergraph.map_nets["n4"].cnt_cells_p1 == 1);
    REQUIRE(hypergraph.map_nets["n5"].cnt_cells_p0 == 0);
    REQUIRE(hypergraph.map_nets["n5"].cnt_cells_p1 == 2);
  }

  SUBCASE("SUB : move c3") { 
    Cell* target = &(hypergraph.map_cells["c3"]);
    for (size_t i = 0; i < target->nets.size(); ++i) { 
      hypergraph.update_gain(target->nets[i], target);
    }

    //for (it1 = hypergraph.map_nets.begin();
    //     it1 != hypergraph.map_nets.end(); ++it1) {
    //  if (it1->second.name == "n1") {
    //    REQUIRE(it1->second.cut == false);
    //  }
    //  else if (it1->second.name == "n2") {
    //    REQUIRE(it1->second.cut == false);
    //  }
    //  else if (it1->second.name == "n3") {
    //    REQUIRE(it1->second.cut == true);
    //  }
    //  else if (it1->second.name == "n4") {
    //    REQUIRE(it1->second.cut == true);
    //  }
    //  else if (it1->second.name == "n5") {
    //    REQUIRE(it1->second.cut == true);
    //  }
    //}
    
    for (it = hypergraph.map_cells.begin(); 
         it != hypergraph.map_cells.end(); ++it) {
      if (it->second.name == "c1") {
        REQUIRE(it->second.gain == 0);
      }
      else if (it->second.name == "c2") {
        REQUIRE(it->second.gain == -2);
      }
      else if (it->second.name == "c3") {
        continue;
        //REQUIRE(it->second.gain == 0);
      }
      else if (it->second.name == "c4") {
        REQUIRE(it->second.gain == 2);
      }
      else if (it->second.name == "c5") {
        REQUIRE(it->second.gain == 1);
      }
    }
    REQUIRE(hypergraph.map_nets["n1"].cnt_cells_p0 == 2);
    REQUIRE(hypergraph.map_nets["n1"].cnt_cells_p1 == 0);
    REQUIRE(hypergraph.map_nets["n2"].cnt_cells_p0 == 3);
    REQUIRE(hypergraph.map_nets["n2"].cnt_cells_p1 == 0);
    REQUIRE(hypergraph.map_nets["n3"].cnt_cells_p0 == 1);
    REQUIRE(hypergraph.map_nets["n3"].cnt_cells_p1 == 1);
    REQUIRE(hypergraph.map_nets["n4"].cnt_cells_p0 == 1);
    REQUIRE(hypergraph.map_nets["n4"].cnt_cells_p1 == 1);
    REQUIRE(hypergraph.map_nets["n5"].cnt_cells_p0 == 1);
    REQUIRE(hypergraph.map_nets["n5"].cnt_cells_p1 == 1);
  }

  SUBCASE("SUB : move c4") {
    Cell* target = &(hypergraph.map_cells["c4"]);
    for (size_t i = 0; i < target->nets.size(); ++i) { 
      hypergraph.update_gain(target->nets[i], target);
    }

    //for (it1 = hypergraph.map_nets.begin();
    //     it1 != hypergraph.map_nets.end(); ++it1) {
    //  if (it1->second.name == "n1") {
    //    REQUIRE(it1->second.cut == false);
    //  }
    //  else if (it1->second.name == "n2") {
    //    REQUIRE(it1->second.cut == true);
    //  }
    //  else if (it1->second.name == "n3") {
    //    REQUIRE(it1->second.cut == false);
    //  }
    //  else if (it1->second.name == "n4") {
    //    REQUIRE(it1->second.cut == true);
    //  }
    //  else if (it1->second.name == "n5") {
    //    REQUIRE(it1->second.cut == true);
    //  }
    //}
    
    for (it = hypergraph.map_cells.begin(); 
         it != hypergraph.map_cells.end(); ++it) {
      if (it->second.name == "c1") {
        REQUIRE(it->second.gain == -1);
      }
      else if (it->second.name == "c2") {
        REQUIRE(it->second.gain == -1);
      }
      else if (it->second.name == "c3") {
        REQUIRE(it->second.gain == 2);
      }
      else if (it->second.name == "c4") {
        continue;
        //REQUIRE(it->second.gain == 0);
      }
      else if (it->second.name == "c5") {
        REQUIRE(it->second.gain == 1);
      }
    }
    REQUIRE(hypergraph.map_nets["n1"].cnt_cells_p0 == 2);
    REQUIRE(hypergraph.map_nets["n1"].cnt_cells_p1 == 0);
    REQUIRE(hypergraph.map_nets["n2"].cnt_cells_p0 == 2);
    REQUIRE(hypergraph.map_nets["n2"].cnt_cells_p1 == 1);
    REQUIRE(hypergraph.map_nets["n3"].cnt_cells_p0 == 2);
    REQUIRE(hypergraph.map_nets["n3"].cnt_cells_p1 == 0);
    REQUIRE(hypergraph.map_nets["n4"].cnt_cells_p0 == 1);
    REQUIRE(hypergraph.map_nets["n4"].cnt_cells_p1 == 1);
    REQUIRE(hypergraph.map_nets["n5"].cnt_cells_p0 == 1);
    REQUIRE(hypergraph.map_nets["n5"].cnt_cells_p1 == 1);
  }

  SUBCASE("SUB : move c5") {
    Cell* target = &(hypergraph.map_cells["c5"]);
    for (size_t i = 0; i < target->nets.size(); ++i) { 
      hypergraph.update_gain(target->nets[i], target);
    }

    //for (it1 = hypergraph.map_nets.begin();
    //     it1 != hypergraph.map_nets.end(); ++it1) {
    //  if (it1->second.name == "n1") {
    //    REQUIRE(it1->second.cut == false);
    //  }
    //  else if (it1->second.name == "n2") {
    //    REQUIRE(it1->second.cut == true);
    //  }
    //  else if (it1->second.name == "n3") {
    //    REQUIRE(it1->second.cut == true);
    //  }
    //  else if (it1->second.name == "n4") {
    //    REQUIRE(it1->second.cut == false);
    //  }
    //  else if (it1->second.name == "n5") {
    //    REQUIRE(it1->second.cut == false);
    //  }
    //}
    
    for (it = hypergraph.map_cells.begin(); 
         it != hypergraph.map_cells.end(); ++it) {
      if (it->second.name == "c1") {
        REQUIRE(it->second.gain == -1);
      }
      else if (it->second.name == "c2") {
        REQUIRE(it->second.gain == -1);
      }
      else if (it->second.name == "c3") {
        REQUIRE(it->second.gain == 0);
      }
      else if (it->second.name == "c4") {
        REQUIRE(it->second.gain == 0);
      }
      else if (it->second.name == "c5") {
        continue;
        //REQUIRE(it->second.gain == -1);
      }
    }
    REQUIRE(hypergraph.map_nets["n1"].cnt_cells_p0 == 2);
    REQUIRE(hypergraph.map_nets["n1"].cnt_cells_p1 == 0);
    REQUIRE(hypergraph.map_nets["n2"].cnt_cells_p0 == 2);
    REQUIRE(hypergraph.map_nets["n2"].cnt_cells_p1 == 1);
    REQUIRE(hypergraph.map_nets["n3"].cnt_cells_p0 == 1);
    REQUIRE(hypergraph.map_nets["n3"].cnt_cells_p1 == 1);
    REQUIRE(hypergraph.map_nets["n4"].cnt_cells_p0 == 2);
    REQUIRE(hypergraph.map_nets["n4"].cnt_cells_p1 == 0);
    REQUIRE(hypergraph.map_nets["n5"].cnt_cells_p0 == 0);
    REQUIRE(hypergraph.map_nets["n5"].cnt_cells_p1 == 2);
  }
}



// verify the update_bucket
TEST_CASE("verify_update_bucket" * doctest::timeout(600)) {
  
  //Hypergraph hypergraph(input_file, output_file);
  HypergraphTest hypergraph;
  
  std::unordered_map<std::string, Cell>::iterator it;
  for (it = hypergraph.map_cells.begin(); 
       it != hypergraph.map_cells.end(); ++it) {
    if (it->second.name == "c1") {
      it->second.partition = 0;
    }
    else if(it->second.name == "c2") {
      it->second.partition = 0;
    }
    else if (it->second.name == "c3") {
      it->second.partition = 1;
    }
    else if (it->second.name == "c4") {
      it->second.partition = 1;
    }
    else if (it->second.name == "c5") {
      it->second.partition = 1;
    }
    it->second.prev = nullptr;
    it->second.next = nullptr;
  }
  
  std::unordered_map<std::string, Net>::iterator it1;
  for (it1 = hypergraph.map_nets.begin(); 
       it1 != hypergraph.map_nets.end(); ++it1) {
    //it1->second.cut = false;
    it1->second.cnt_cells_p0 = 0;
    it1->second.cnt_cells_p1 = 0;
  }

  hypergraph.max_gain = -1000000;
  hypergraph.min_gain = 1000000;
  hypergraph.num_cells_p0 = 2;
  hypergraph.max_edge = 4;
  hypergraph.initialize_count_cells();
  hypergraph.initialize_gain(); 
  hypergraph.bucket.clear();
  hypergraph.construct_bucket();

  SUBCASE("SUB : Move c1") { 
    int old_gain = hypergraph.map_cells["c1"].gain;
    //hypergraph.map_cells["c1"].partition = 1;
    //hypergraph.map_nets["n1"].cnt_cells_p0 -=1;
    //hypergraph.map_nets["n1"].cnt_cells_p1 +=1;
    //hypergraph.map_nets["n2"].cnt_cells_p0 -=1;
    //hypergraph.map_nets["n2"].cnt_cells_p1 +=1;
    //hypergraph.map_nets["n3"].cnt_cells_p0 -=1;
    //hypergraph.map_nets["n3"].cnt_cells_p1 +=1;
    //hypergraph.map_nets["n4"].cnt_cells_p0 -=1;
    //hypergraph.map_nets["n4"].cnt_cells_p1 +=1;
 
    hypergraph.map_cells["c1"].gain *= (-1); 
    Cell* target = &(hypergraph.map_cells["c1"]);

    for (size_t i = 0; i < target->nets.size(); ++i) { 
      hypergraph.update_gain(target->nets[i], target);
    }
    hypergraph.delete_from_bucket(target);

    for (size_t i = 0; i < hypergraph.bucket.size(); ++i) {
      if (i < 2 || i == 4 || i == 5 || i > 6) {
        REQUIRE(hypergraph.bucket[i] == nullptr);
      }
      else if (i == 2) {
        REQUIRE(hypergraph.bucket[i]->name == "c4");
      }
      else if (i == 3) {
        REQUIRE(hypergraph.bucket[i]->name != "c2");
        REQUIRE(hypergraph.bucket[i]->name != "c4");
        REQUIRE(hypergraph.bucket[i]->name != "c1");
      }
      else if (i == 6) {
        REQUIRE(hypergraph.bucket[i]->name == "c2");
      }
    }
  }
  
  SUBCASE("SUB : Move c2") { 
    //int old_gain = hypergraph.map_cells["c2"].gain;
    //hypergraph.map_cells["c2"].partition = 1;
    //hypergraph.map_nets["n1"].cnt_cells_p0 -=1;
    //hypergraph.map_nets["n1"].cnt_cells_p1 +=1;
    //hypergraph.map_nets["n2"].cnt_cells_p0 -=1;
    //hypergraph.map_nets["n2"].cnt_cells_p1 +=1;
    //hypergraph.update_cut_net(&(hypergraph.map_cells["c2"]));
    //hypergraph.update_gain(&(hypergraph.map_cells["c2"]));
    //hypergraph.update_bucket(
    //  old_gain+hypergraph.num_nets(), &(hypergraph.map_cells["c2"]));

    hypergraph.map_cells["c2"].gain *= (-1); 
    Cell* target = &(hypergraph.map_cells["c2"]);

    for (size_t i = 0; i < target->nets.size(); ++i) { 
      hypergraph.update_gain(target->nets[i], target);
    }
    hypergraph.delete_from_bucket(target);

    for (size_t i = 0; i < hypergraph.bucket.size(); ++i) {
      if (i < 3 || i == 6 || i == 7 || i > 8) {
        REQUIRE(hypergraph.bucket[i] == nullptr);
      }
      else if (i == 3) {
        REQUIRE(hypergraph.bucket[i]->name == "c3");
      }
      else if (i == 4) {
        REQUIRE(hypergraph.bucket[i]->name == "c4");
      }
      else if (i == 5) {
        REQUIRE(hypergraph.bucket[i]->name == "c5");
      }
      else if (i == 8) {
        REQUIRE(hypergraph.bucket[i]->name == "c1");
      }
    }
  }
  
  SUBCASE("SUB : Move c3") { 
    //int old_gain = hypergraph.map_cells["c3"].gain;
    //hypergraph.map_cells["c3"].partition = 0;
    //hypergraph.map_nets["n2"].cnt_cells_p0 +=1;
    //hypergraph.map_nets["n2"].cnt_cells_p1 -=1;
    //hypergraph.map_nets["n5"].cnt_cells_p0 +=1;
    //hypergraph.map_nets["n5"].cnt_cells_p1 -=1;
    //hypergraph.update_cut_net(&(hypergraph.map_cells["c3"]));
    //hypergraph.update_gain(&(hypergraph.map_cells["c3"]));
    //hypergraph.update_bucket(
    //  old_gain+hypergraph.num_nets(), &(hypergraph.map_cells["c3"]));

    hypergraph.map_cells["c3"].gain *= (-1); 
    Cell* target = &(hypergraph.map_cells["c3"]);

    for (size_t i = 0; i < target->nets.size(); ++i) { 
      hypergraph.update_gain(target->nets[i], target);
    }
    hypergraph.delete_from_bucket(target);

    for (size_t i = 0; i < hypergraph.bucket.size(); ++i) {
      if (i < 2 || i == 3 || i > 6) {
        REQUIRE(hypergraph.bucket[i] == nullptr);
      }
      else if (i == 2) {
        REQUIRE(hypergraph.bucket[i]->name == "c2");
      }
      else if (i == 4) {
        REQUIRE(hypergraph.bucket[i]->name == "c1");
      }
      else if (i == 5) {
        REQUIRE(hypergraph.bucket[i]->name == "c5");
      }
      else if (i == 6) {
        REQUIRE(hypergraph.bucket[i]->name == "c4");
      }
    }
  }
  
  SUBCASE("SUB : Move c4") { 
    //int old_gain = hypergraph.map_cells["c4"].gain;
    //hypergraph.map_cells["c4"].partition = 0;
    //hypergraph.map_nets["n3"].cnt_cells_p0 +=1;
    //hypergraph.map_nets["n3"].cnt_cells_p1 -=1;
    //hypergraph.map_nets["n5"].cnt_cells_p0 +=1;
    //hypergraph.map_nets["n5"].cnt_cells_p1 -=1;
    //hypergraph.update_cut_net(&(hypergraph.map_cells["c4"]));
    //hypergraph.update_gain(&(hypergraph.map_cells["c4"]));
    //hypergraph.update_bucket(
    //  old_gain+hypergraph.num_nets(), &(hypergraph.map_cells["c4"]));

    hypergraph.map_cells["c4"].gain *= (-1); 
    Cell* target = &(hypergraph.map_cells["c4"]);

    for (size_t i = 0; i < target->nets.size(); ++i) { 
      hypergraph.update_gain(target->nets[i], target);
    }
    hypergraph.delete_from_bucket(target);
    
    for (size_t i = 0; i < hypergraph.bucket.size(); ++i) {
      if (i < 3 || i == 4 || i > 7) {
        REQUIRE(hypergraph.bucket[i] == nullptr);
      }
      else if (i == 3) {
        REQUIRE(hypergraph.bucket[i]->name != "c3");
        REQUIRE(hypergraph.bucket[i]->name != "c4");
        REQUIRE(hypergraph.bucket[i]->name != "c5");
      }
      else if (i == 5) {
        REQUIRE(hypergraph.bucket[i]->name == "c5");
      }
      else if (i == 6) {
        REQUIRE(hypergraph.bucket[i]->name == "c3");
      }
    }
  }
  
  SUBCASE("SUB : Move c5") { 
    //int old_gain = hypergraph.map_cells["c5"].gain;
    //hypergraph.map_cells["c5"].partition = 0;
    //hypergraph.map_nets["n4"].cnt_cells_p0 +=1;
    //hypergraph.map_nets["n4"].cnt_cells_p1 -=1;
    //hypergraph.update_cut_net(&(hypergraph.map_cells["c5"]));
    //hypergraph.update_gain(&(hypergraph.map_cells["c5"]));
    //hypergraph.update_bucket(
    //  old_gain+hypergraph.num_nets(), &(hypergraph.map_cells["c5"]));

    hypergraph.map_cells["c5"].gain *= (-1); 
    Cell* target = &(hypergraph.map_cells["c5"]);

    for (size_t i = 0; i < target->nets.size(); ++i) { 
      hypergraph.update_gain(target->nets[i], target);
    }
    hypergraph.delete_from_bucket(target);
    
    for (size_t i = 0; i < hypergraph.bucket.size(); ++i) {
      if (i < 3 || i > 4) {
        REQUIRE(hypergraph.bucket[i] == nullptr);
      }
      else if (i == 3) {
        REQUIRE(hypergraph.bucket[i]->name != "c3");
        REQUIRE(hypergraph.bucket[i]->name != "c4");
        REQUIRE(hypergraph.bucket[i]->name != "c5");
      }
      else if (i == 4) {
        REQUIRE(hypergraph.bucket[i]->name != "c1");
        REQUIRE(hypergraph.bucket[i]->name != "c2");
        REQUIRE(hypergraph.bucket[i]->name != "c5");
      }
    }
  }
}

// verify the find_max_cumulative_gain
TEST_CASE("verify_find_max_cumulative_gain" * doctest::timeout(600)) {
  
  //Hypergraph hypergraph(input_file, output_file);
  HypergraphTest hypergraph;
  
  std::unordered_map<std::string, Cell>::iterator it;
  for (it = hypergraph.map_cells.begin(); 
       it != hypergraph.map_cells.end(); ++it) {
    if (it->second.name == "c1") {
      it->second.partition = 0;
    }
    else if(it->second.name == "c2") {
      it->second.partition = 0;
    }
    else if (it->second.name == "c3") {
      it->second.partition = 1;
    }
    else if (it->second.name == "c4") {
      it->second.partition = 1;
    }
    else if (it->second.name == "c5") {
      it->second.partition = 1;
    }
    it->second.prev = nullptr;
    it->second.next = nullptr;
  }
  
  std::unordered_map<std::string, Net>::iterator it1;
  for (it1 = hypergraph.map_nets.begin(); 
       it1 != hypergraph.map_nets.end(); ++it1) {
    //it1->second.cut = false;
    it1->second.cnt_cells_p0 = 0;
    it1->second.cnt_cells_p1 = 0;
  }

  hypergraph.max_gain = -1000000;
  hypergraph.min_gain = 1000000;
  hypergraph.num_cells_p0 = 2;
  hypergraph.max_edge = 4;
  hypergraph.initialize_count_cells();
  hypergraph.initialize_gain(); 
  hypergraph.bucket.clear();
  hypergraph.construct_bucket();

  hypergraph.locked_cells_gain.clear();
  
  SUBCASE("SUB : 1"){
    hypergraph.locked_cells_gain = std::vector<int>{10,0,0,0,0,0};
    REQUIRE(hypergraph.find_max_cumulative_gain() == 0);
  }
  
  SUBCASE("SUB : 2"){
    hypergraph.locked_cells_gain = std::vector<int>{10,-11,0,0,0,0};
    REQUIRE(hypergraph.find_max_cumulative_gain() == 0);
  }
  
  SUBCASE("SUB : 3"){
    hypergraph.locked_cells_gain = std::vector<int>{0,0,0,0,0,0};
    REQUIRE(hypergraph.find_max_cumulative_gain() == 0);
  }
  
  SUBCASE("SUB : 4"){
    hypergraph.locked_cells_gain = std::vector<int>{10,-11, 11,0,0,0};
    REQUIRE(hypergraph.find_max_cumulative_gain() == 2);
  }
  
  SUBCASE("SUB : 5"){
    hypergraph.locked_cells_gain = std::vector<int>{0,0,10,0,-10,0};
    REQUIRE(hypergraph.find_max_cumulative_gain() == 2);
  }
  
  SUBCASE("SUB : 6"){
    hypergraph.locked_cells_gain = std::vector<int>{0,11, 11,0,6,0};
    REQUIRE(hypergraph.find_max_cumulative_gain() == 4);
  }
}


// verify the recover
TEST_CASE("verify_recover" * doctest::timeout(600)) {
  Hypergraph hypergraph(input_file, output_file);
  
  std::unordered_map<std::string, Cell>::iterator it;
  for (it = hypergraph.map_cells.begin(); 
       it != hypergraph.map_cells.end(); ++it) {
    if (it->second.name == "c1") {
      it->second.partition = 0;
    }
    else if(it->second.name == "c2") {
      it->second.partition = 0;
    }
    else if (it->second.name == "c3") {
      it->second.partition = 1;
    }
    else if (it->second.name == "c4") {
      it->second.partition = 1;
    }
    else if (it->second.name == "c5") {
      it->second.partition = 1;
    }
    it->second.prev = nullptr;
    it->second.next = nullptr;
  }
  
  std::unordered_map<std::string, Net>::iterator it1;
  for (it1 = hypergraph.map_nets.begin(); 
       it1 != hypergraph.map_nets.end(); ++it1) {
    it1->second.cut = false;
    it1->second.cnt_cells_p0 = 0;
    it1->second.cnt_cells_p1 = 0;
  }

  hypergraph.max_gain = -1000000;
  hypergraph.min_gain = 1000000;
  hypergraph.num_cells_p0 = 2;
  hypergraph.initialize_count_cells();
  hypergraph.initialize_gain(); 
  hypergraph.bucket.clear();
  hypergraph.construct_bucket();

  // recover c1 = move c1 to another partition
  SUBCASE("SUB : recover c1") {
    
    hypergraph.recover(&(hypergraph.map_cells["c1"]));
    
    //for (it1 = hypergraph.map_nets.begin();
    //     it1 != hypergraph.map_nets.end(); ++it1) {
    //  if (it1->second.name == "n1") {
    //    REQUIRE(it1->second.cut == true);
    //  }
    //  else if (it1->second.name == "n2") {
    //    REQUIRE(it1->second.cut == true);
    //  }
    //  else if (it1->second.name == "n3") {
    //    REQUIRE(it1->second.cut == false);
    //  }
    //  else if (it1->second.name == "n4") {
    //    REQUIRE(it1->second.cut == false);
    //  }
    //  else if (it1->second.name == "n5") {
    //    REQUIRE(it1->second.cut == false);
    //  }
    //}
  
    //for (it = hypergraph.map_cells.begin(); 
    //     it != hypergraph.map_cells.end(); ++it) {
    //  
    //  if (it->second.name == "c1") {
    //    REQUIRE(it->second.gain == -1);
    //  }
    //  else if (it->second.name == "c2") {
    //    REQUIRE(it->second.gain == 2);
    //  }
    //  else if (it->second.name == "c3") {
    //    REQUIRE(it->second.gain == -1);
    //  }
    //  else if (it->second.name == "c4") {
    //    REQUIRE(it->second.gain == -2);
    //  }
    //  else if (it->second.name == "c5") {
    //    REQUIRE(it->second.gain == -1);
    //  }
    //}
    //for (size_t i = 0; i < hypergraph.bucket.size(); ++i) {
    //  if (i < 3 || i == 5 || i == 6 || i > 7) {
    //    REQUIRE(hypergraph.bucket[i] == nullptr);
    //  }
    //  else if (i == 3) {
    //    REQUIRE(hypergraph.bucket[i]->name == "c4");
    //  }
    //  else if (i == 4) {
    //    REQUIRE(hypergraph.bucket[i]->name != "c2");
    //    REQUIRE(hypergraph.bucket[i]->name != "c4");
    //  }
    //  else if (i == 7) {
    //    REQUIRE(hypergraph.bucket[i]->name == "c2");
    //  }
    //}

  }
  
  // recover c2 = move c2 to another partition
  SUBCASE("SUB : recover c2") {
    hypergraph.recover(&(hypergraph.map_cells["c2"]));

    for (it1 = hypergraph.map_nets.begin();
         it1 != hypergraph.map_nets.end(); ++it1) {
      if (it1->second.name == "n1") {
        REQUIRE(it1->second.cut == true);
      }
      else if (it1->second.name == "n2") {
        REQUIRE(it1->second.cut == true);
      }
      else if (it1->second.name == "n3") {
        REQUIRE(it1->second.cut == true);
      }
      else if (it1->second.name == "n4") {
        REQUIRE(it1->second.cut == true);
      }
      else if (it1->second.name == "n5") {
        REQUIRE(it1->second.cut == false);
      }
    }
    
    for (it = hypergraph.map_cells.begin(); 
         it != hypergraph.map_cells.end(); ++it) {
      if (it->second.name == "c1") {
        REQUIRE(it->second.gain == 4);
      }
      else if (it->second.name == "c2") {
        REQUIRE(it->second.gain == 1);
      }
      else if (it->second.name == "c3") {
        REQUIRE(it->second.gain == -1);
      }
      else if (it->second.name == "c4") {
        REQUIRE(it->second.gain == 0);
      }
      else if (it->second.name == "c5") {
        REQUIRE(it->second.gain == 1);
      }
    }
    
    for (size_t i = 0; i < hypergraph.bucket.size(); ++i) {
      if (i < 4 || i == 7 || i == 8 || i > 9) {
        REQUIRE(hypergraph.bucket[i] == nullptr);
      }
      else if (i == 4) {
        REQUIRE(hypergraph.bucket[i]->name == "c3");
      }
      else if (i == 5) {
        REQUIRE(hypergraph.bucket[i]->name == "c4");
      }
      else if (i == 6) {
        REQUIRE(hypergraph.bucket[i]->name != "c1");
        REQUIRE(hypergraph.bucket[i]->name != "c3");
        REQUIRE(hypergraph.bucket[i]->name != "c4");
      }
      else if (i == 9) {
        REQUIRE(hypergraph.bucket[i]->name == "c1");
      }
    }
  }
  
  // recover c3 = move c3 to another partition
  SUBCASE("SUB : recover c3") {
    hypergraph.recover(&(hypergraph.map_cells["c3"]));

    for (it1 = hypergraph.map_nets.begin();
         it1 != hypergraph.map_nets.end(); ++it1) {
      if (it1->second.name == "n1") {
        REQUIRE(it1->second.cut == false);
      }
      else if (it1->second.name == "n2") {
        REQUIRE(it1->second.cut == false);
      }
      else if (it1->second.name == "n3") {
        REQUIRE(it1->second.cut == true);
      }
      else if (it1->second.name == "n4") {
        REQUIRE(it1->second.cut == true);
      }
      else if (it1->second.name == "n5") {
        REQUIRE(it1->second.cut == true);
      }
    }
    
    for (it = hypergraph.map_cells.begin(); 
         it != hypergraph.map_cells.end(); ++it) {
      if (it->second.name == "c1") {
        REQUIRE(it->second.gain == 0);
      }
      else if (it->second.name == "c2") {
        REQUIRE(it->second.gain == -2);
      }
      else if (it->second.name == "c3") {
        REQUIRE(it->second.gain == 0);
      }
      else if (it->second.name == "c4") {
        REQUIRE(it->second.gain == 2);
      }
      else if (it->second.name == "c5") {
        REQUIRE(it->second.gain == 1);
      }
    }
    
    for (size_t i = 0; i < hypergraph.bucket.size(); ++i) {
      if (i < 3 || i == 4 || i > 7) {
        REQUIRE(hypergraph.bucket[i] == nullptr);
      }
      else if (i == 3) {
        REQUIRE(hypergraph.bucket[i]->name == "c2");
      }
      else if (i == 5) {
        REQUIRE(hypergraph.bucket[i]->name != "c2");
        REQUIRE(hypergraph.bucket[i]->name != "c4");
        REQUIRE(hypergraph.bucket[i]->name != "c5");
      }
      else if (i == 6) {
        REQUIRE(hypergraph.bucket[i]->name == "c5");
      }
      else if (i == 7) {
        REQUIRE(hypergraph.bucket[i]->name == "c4");
      }
    }
  }
  
  // recover c4 = move c4 to another partition
  SUBCASE("SUB : recover c4") {
    hypergraph.recover(&(hypergraph.map_cells["c4"]));

    for (it1 = hypergraph.map_nets.begin();
         it1 != hypergraph.map_nets.end(); ++it1) {
      if (it1->second.name == "n1") {
        REQUIRE(it1->second.cut == false);
      }
      else if (it1->second.name == "n2") {
        REQUIRE(it1->second.cut == true);
      }
      else if (it1->second.name == "n3") {
        REQUIRE(it1->second.cut == false);
      }
      else if (it1->second.name == "n4") {
        REQUIRE(it1->second.cut == true);
      }
      else if (it1->second.name == "n5") {
        REQUIRE(it1->second.cut == true);
      }
    }
    
    for (it = hypergraph.map_cells.begin(); 
         it != hypergraph.map_cells.end(); ++it) {
      if (it->second.name == "c1") {
        REQUIRE(it->second.gain == -1);
      }
      else if (it->second.name == "c2") {
        REQUIRE(it->second.gain == -1);
      }
      else if (it->second.name == "c3") {
        REQUIRE(it->second.gain == 2);
      }
      else if (it->second.name == "c4") {
        REQUIRE(it->second.gain == 0);
      }
      else if (it->second.name == "c5") {
        REQUIRE(it->second.gain == 1);
      }
    }
    
    for (size_t i = 0; i < hypergraph.bucket.size(); ++i) {
      if (i < 4 || i > 7) {
        REQUIRE(hypergraph.bucket[i] == nullptr);
      }
      else if (i == 4) {
        REQUIRE(hypergraph.bucket[i]->name != "c3");
        REQUIRE(hypergraph.bucket[i]->name != "c4");
        REQUIRE(hypergraph.bucket[i]->name != "c5");
      }
      else if (i == 5) {
        REQUIRE(hypergraph.bucket[i]->name == "c4");
      }
      else if (i == 6) {
        REQUIRE(hypergraph.bucket[i]->name == "c5");
      }
      else if (i == 7) {
        REQUIRE(hypergraph.bucket[i]->name == "c3");
      }
    }
  }
  
  // recover c5 = move c5 to another partition
  SUBCASE("SUB : recover c5") {
    hypergraph.recover(&(hypergraph.map_cells["c5"]));
    
    for (it1 = hypergraph.map_nets.begin();
         it1 != hypergraph.map_nets.end(); ++it1) {
      if (it1->second.name == "n1") {
        REQUIRE(it1->second.cut == false);
      }
      else if (it1->second.name == "n2") {
        REQUIRE(it1->second.cut == true);
      }
      else if (it1->second.name == "n3") {
        REQUIRE(it1->second.cut == true);
      }
      else if (it1->second.name == "n4") {
        REQUIRE(it1->second.cut == false);
      }
      else if (it1->second.name == "n5") {
        REQUIRE(it1->second.cut == false);
      }
    }
    
    for (it = hypergraph.map_cells.begin(); 
         it != hypergraph.map_cells.end(); ++it) {
      if (it->second.name == "c1") {
        REQUIRE(it->second.gain == -1);
      }
      else if (it->second.name == "c2") {
        REQUIRE(it->second.gain == -1);
      }
      else if (it->second.name == "c3") {
        REQUIRE(it->second.gain == 0);
      }
      else if (it->second.name == "c4") {
        REQUIRE(it->second.gain == 0);
      }
      else if (it->second.name == "c5") {
        REQUIRE(it->second.gain == -1);
      }
    }
    
    for (size_t i = 0; i < hypergraph.bucket.size(); ++i) {
      if (i < 4 || i > 5) {
        REQUIRE(hypergraph.bucket[i] == nullptr);
      }
      else if (i == 4) {
        REQUIRE(hypergraph.bucket[i]->name != "c3");
        REQUIRE(hypergraph.bucket[i]->name != "c4");
      }
      else if (i == 5) {
        REQUIRE(hypergraph.bucket[i]->name != "c1");
        REQUIRE(hypergraph.bucket[i]->name != "c2");
        REQUIRE(hypergraph.bucket[i]->name != "c5");
      }
    }
  }
}
*/








} // end of namespace fp
