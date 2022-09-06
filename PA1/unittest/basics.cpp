#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <ctime>
#include <iostream>
#include <doctest.h>
#include <string>
#include <unordered_map>
#include "graph.hpp"

std::string input_file("/home/chchiu/Documents/courses/ece5960/ECE5960-Physical-Design-Algorithm/PA1/unittest/test.dat");

std::string output_file("/home/chchiu/Documents/courses/ece5960/ECE5960-Physical-Design-Algorithm/PA1/build/out.dat");

// verify the initial gain
TEST_CASE("verify_initial_gain" * doctest::timeout(600)) {
  std::srand(std::time(nullptr));
 
  Hypergraph hypergraph(input_file, output_file);
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
    it1->second.cut = false;
  }

  hypergraph.max_gain = -1000000;

  hypergraph.initialize_gain(); 
  
  //hypergraph.display_partition();

  //hypergraph.display_cut();
  
  //hypergraph.display_gain();

  for (it1 = hypergraph.map_nets.begin(); 
       it1 != hypergraph.map_nets.end(); ++it1) {
    if (it1->second.name == "n2" || 
        it1->second.name == "n3" ||
        it1->second.name == "n4") {
      REQUIRE(it1->second.cut == true);
    }
    else {
      REQUIRE(it1->second.cut == false);
    }
  }
   
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

  
  // set c1 and c2 partition 1
  //for (it = hypergraph.map_cells.begin(); 
  //     it != hypergraph.map_cells.end(); ++it) {
  //  if (it->second.name == "c1") {
  //    it->second.partition = 1;
  //  }
  //  else if(it->second.name == "c2") {
  //    it->second.partition = 1;
  //  }
  //  else if (it->second.name == "c3") {
  //    it->second.partition = 0;
  //  }
  //  else if (it->second.name == "c4") {
  //    it->second.partition = 0;
  //  }
  //  else if (it->second.name == "c5") {
  //    it->second.partition = 0;
  //  }
  //}
  //
  //for (it1 = hypergraph.map_nets.begin(); 
  //     it1 != hypergraph.map_nets.end(); ++it1) {
  //  it1->second.cut = false;
  //}

  //hypergraph.initialize_gain(); 
  //
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
  // 
  //for (it = hypergraph.map_cells.begin(); 
  //     it != hypergraph.map_cells.end(); ++it) {
  //  if (it->second.name == "c1") {
  //    REQUIRE(it->second.gain == 1);
  //  }
  //  else if(it->second.name == "c2") {
  //    REQUIRE(it->second.gain == -1);
  //  }
  //  else if (it->second.name == "c3") {
  //    REQUIRE(it->second.gain == 0);
  //  }
  //  else if (it->second.name == "c4") {
  //    REQUIRE(it->second.gain == 0);
  //  }
  //  else if (it->second.name == "c5") {
  //    REQUIRE(it->second.gain == 1);
  //  }
  //}
  REQUIRE(hypergraph.max_gain == 1);  
}


// verify the initial connected cells 
TEST_CASE("verify_initial_connected_cells" * doctest::timeout(600)) {
  
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
  }
  
  std::unordered_map<std::string, Net>::iterator it1;
  for (it1 = hypergraph.map_nets.begin(); 
       it1 != hypergraph.map_nets.end(); ++it1) {
    it1->second.cut = false;
  }

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
  REQUIRE(true == true);
  
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
  }

  hypergraph.initialize_gain(); 

  hypergraph.bucket.clear();
  
  hypergraph.construct_bucket();
  
  REQUIRE(hypergraph.bucket.size() == hypergraph.num_nets()*2+1);

  for (size_t i = 0; i < hypergraph.bucket.size(); ++i) {
    if (i <= 3) {
      REQUIRE(hypergraph.bucket[i] == nullptr);
    }
    else if (i == 4) {
      Cell* head = hypergraph.bucket[i];
      REQUIRE(head->name == "c2");
      REQUIRE(head->prev == nullptr);
      REQUIRE(head->next == nullptr);
    }
    else if (i == 5) {
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
    else if (i == 6) {
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
    else if (i >= 7) {
      REQUIRE(hypergraph.bucket[i] == nullptr);
    }
  }
}  
