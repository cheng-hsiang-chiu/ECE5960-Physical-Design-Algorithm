#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <set>
#include <cassert>
#include <chrono>
#include <climits>

class Net;

class Cell {
public:
  bool locked = false;
  bool partition = 0;
  Cell* prev = nullptr;
  Cell* next = nullptr;
  int gain = 0;
  
  std::string name;
  std::set<Cell*> connected_cells;
  std::vector<Net*> nets;
};

class Net {
public:
  bool cut = false;
  std::string name;
  std::vector<Cell*> cells;
};


class Hypergraph {
public:
  Hypergraph(std::string&);

  std::unordered_map<std::string, Net> map_nets;

  std::unordered_map<std::string, Cell> map_cells;

  std::vector<Cell*> bucket;
  
  std::vector<Cell*> tail_bucket;

  std::vector<Cell*> locked_cells;

  double r_factor;

  double area_lower_bound;

  double area_upper_bound;

  double max_area = 1.0;

  int max_gain = INT_MIN;

  size_t num_cells_p0 = 0;

  size_t num_nets() const;

  size_t num_cells() const;

  void traverse() const;

  void initialize_gain();

  void display_partition() const;

  void display_cut() const;
  
  void display_gain() const;

  void display_connected_cells() const;

  void construct_bucket();

  void display_bucket() const;
  
  void run_fm();

  bool meet_balance_criterion(Cell*) const;

  void update_cut_net(Cell*);
  
  void update_gain(Cell*);
};


Hypergraph::Hypergraph(std::string& input_file) {
  std::ifstream inClientFile(input_file, std::ios::in);

  if (!inClientFile) {
    std::cerr << "File could not be opened or does not exist\n";
    exit(1);
  }

  std::string line;
  std::getline(inClientFile, line);
  r_factor = std::stof(line);
  
  std::string net_name;
  std::string cell_name;
  std::vector<Cell*> temp;

  while(std::getline(inClientFile, line)) {
    std::stringstream ss(line);
    temp.clear();
    while (std::getline(ss, line, ' ')) {
      if (line != ";" && line != "NET" && line.length() != 0) {
        // net string
        if (line[0] == 'n') {
          net_name = line;

          Net n;
          n.name = net_name;
          map_nets[net_name] = n;
        }
        // cell string
        else {
          cell_name = line;
          //map_nets[net_name].cells.emplace_back(cell_name);

          // new cell
          if (map_cells.find(cell_name) == map_cells.end()) {
            Cell c;
            c.name = cell_name;
            c.nets.emplace_back(&(map_nets[net_name])); 
            c.partition = std::rand()%2;
            if (c.partition == 0) {
              ++num_cells_p0;
            }
            map_cells[cell_name] = c;
          }
          else {
            map_cells[cell_name].nets.emplace_back(&(map_nets[net_name]));
          }
          map_nets[net_name].cells.emplace_back(&(map_cells[cell_name]));

          // the following is used to initialize the connected cells 
          for (size_t i = 0; i < temp.size(); ++i) {
            map_cells[cell_name].connected_cells.insert(temp[i]);
            temp[i]->connected_cells.insert(&(map_cells[cell_name]));
          }
          temp.emplace_back(&(map_cells[cell_name]));
        }
        //std::cout << line << '\n';
      }
    }
  }
  
  // all cells have unit area
  area_lower_bound = r_factor * map_cells.size() - max_area;
  area_upper_bound = r_factor * map_cells.size() + max_area;

  // initialize the gain for each cell
  initialize_gain();

  // construct bucket data structure
  construct_bucket();
}

inline size_t Hypergraph::num_nets() const {
  return map_nets.size();  
}

inline size_t Hypergraph::num_cells() const { 
  return map_cells.size();
}

inline void Hypergraph::traverse() const {
  // traverse map_nets
  std::unordered_map<std::string, Net>::const_iterator itr0;
  for (itr0 = map_nets.begin(); itr0 != map_nets.end(); ++itr0) {
    assert(itr0->first == itr0->second.name);

    std::cout << "NET " << itr0->first << " : ";
    for (size_t i = 0; i < itr0->second.cells.size(); ++i) {
      if (i == itr0->second.cells.size()-1) {
        std::cout << itr0->second.cells[i]->name << '\n';
      }
      else {
        std::cout << itr0->second.cells[i]->name << ", ";;
      } 
    }
  }
  
  // traverse map_cells
  std::unordered_map<std::string, Cell>::const_iterator itr1;
  for (itr1 = map_cells.begin(); itr1 != map_cells.end(); ++itr1) {
    assert(itr1->first == itr1->second.name);

    std::cout << "Cell " << itr1->first << " belongs to ";
    for (size_t i = 0; i < itr1->second.nets.size(); ++i) {
      if (i == itr1->second.nets.size()-1) {
        std::cout << itr1->second.nets[i]->name << '\n';
      }
      else {
        std::cout << itr1->second.nets[i]->name << ", ";
      }
    }
  }  
}

inline void Hypergraph::initialize_gain() {
  std::unordered_map<std::string, Cell>::iterator itr0;
  for (itr0 = map_cells.begin(); itr0 != map_cells.end(); ++itr0) {
    update_cut_net(&(itr0->second));  
  } 
  
  ////auto beg = std::chrono::high_resolution_clock::now();
  //// identify cut and set the cut boolean in the corresponding net object 
  //std::unordered_map<std::string, Net>::iterator itr0;
  //for (itr0 = map_nets.begin(); itr0 != map_nets.end(); ++itr0) {
  //  for (size_t i = 0; i < itr0->second.cells.size()-1; ++i) {
  //    for (size_t j = i+1; j < itr0->second.cells.size(); ++j) {
  //      if (itr0->second.cells[i]->partition != 
  //          itr0->second.cells[j]->partition) {  
  //        // cell[i] and cell[j] located in different partitions
  //        itr0->second.cut = true;
  //        break;
  //      }
  //    }
  //    if (itr0->second.cut) {
  //      break;
  //    }
  //  }
  //}
  
  std::unordered_map<std::string, Cell>::iterator itr1;
  for (itr1 = map_cells.begin(); itr1 != map_cells.end(); ++itr1) {
    int fs = 0;
    int te = 0;
    for (size_t i = 0; i < itr1->second.nets.size(); ++i) {
      // a cut net
      if (itr1->second.nets[i]->cut) {
        int same = 0;
        for (size_t j = 0; j < itr1->second.nets[i]->cells.size(); ++j) {
          if (itr1->second.nets[i]->cells[j]->partition ==
              itr1->second.partition) {
            ++same; 
          }
          if (same >= 2) {
            break;
          }
        }
        if (same == 1) {
          ++fs;
        }
      }
      // not a cut net
      else {
       ++te; 
      }
    }

    itr1->second.gain = fs-te;
    max_gain = itr1->second.gain > max_gain ? itr1->second.gain : max_gain;
  }
  //auto end = std::chrono::high_resolution_clock::now();
  //std::cout << "initialize_gain() costs "
  //          << std::chrono::duration_cast<std::chrono::microseconds>(end - beg).count()
  //          << " micro seconds. \n";
}

inline void Hypergraph::display_partition() const {
  std::unordered_map<std::string, Cell>::const_iterator itr;
  for (itr = map_cells.begin(); itr != map_cells.end(); ++itr) {
    std::cout << itr->second.name 
              << " at partition " 
              << itr->second.partition << '\n'; 
  }  
}

inline void Hypergraph::display_cut() const {
  std::unordered_map<std::string, Net>::const_iterator itr;
  for (itr = map_nets.begin(); itr != map_nets.end(); ++itr) {
    if (itr->second.cut) {
      std::cout << itr->second.name << " is a cut\n";
    }
  }
}

inline void Hypergraph::display_gain() const {
  std::unordered_map<std::string, Cell>::const_iterator itr;
  for (itr = map_cells.begin(); itr != map_cells.end(); ++itr) {
    std::cout << itr->second.name 
              << " has gain " 
              << itr->second.gain << '\n'; 
  }  
}

inline void Hypergraph::display_connected_cells() const {
  std::unordered_map<std::string, Cell>::const_iterator itr;
  for (itr = map_cells.begin(); itr != map_cells.end(); ++itr) {
    std::cout << itr->second.name 
              << " connects with ";
    for (auto& cptr : itr->second.connected_cells) {
      std::cout << cptr->name << " ";    
    }
    std::cout << '\n';
  }  
}

inline void Hypergraph::construct_bucket() {
  bucket.resize(2*map_nets.size()+1);
  tail_bucket.resize(2*map_nets.size()+1);
  int offset = static_cast<int>(num_nets());

  std::unordered_map<std::string, Cell>::iterator itr;
  for (itr = map_cells.begin(); itr != map_cells.end(); ++itr) {
    //std::cout << itr->second.name << " has gain = " << itr->second.gain << '\n';
    if (bucket[itr->second.gain+offset] == nullptr) {
      //std::cout << "bucket[" << itr->second.gain + offset << "] is nullptr\n";
      bucket[itr->second.gain+offset] = &(itr->second);
      tail_bucket[itr->second.gain+offset] = &(itr->second);
    }
    else {
      tail_bucket[itr->second.gain+offset]->next = &(itr->second);
      itr->second.prev = tail_bucket[itr->second.gain+offset];
      tail_bucket[itr->second.gain+offset] = 
      tail_bucket[itr->second.gain+offset]->next;

      ////std::cout << "bucket[" << itr->second.gain+offset << "] is not nullptr\n";
      //Cell* head = bucket[itr->second.gain+offset];
      //
      //while (head->next) {
      //  head = head->next;
      //}
      //head->next = &(itr->second);
      //itr->second.prev = head;
    }
  }
}

inline void Hypergraph::display_bucket() const {
  for (size_t i = 0; i < bucket.size(); ++i) {
    if (bucket[i] != nullptr) {
      Cell* head = bucket[i];
      std::cout << "bucket[" << static_cast<int>(i - num_nets()) << "] has cells: ";
      while (head) {
        std::cout << head->name << " ";
        head = head->next;
      }
      std::cout << '\n';
    }
  }
}

inline bool Hypergraph::meet_balance_criterion(Cell* candidate) const {
  // if moving candiate form p0 to p1 a valid move  
  if (candidate->partition == 0) {
    if (area_lower_bound <= (num_cells_p0-1) &&
        area_upper_bound >= (num_cells_p0-1)) {
      return true;
    }
    return false;
  }
  // if moving candidate from p1 to p0 a valid move
  else {
    if (area_lower_bound <= (num_cells_p0+1) &&
        area_upper_bound >= (num_cells_p0+1)) {
      return true;
    }
    return false;
  }
}

inline void Hypergraph::run_fm() {
  locked_cells.resize(num_cells());
  Cell* head = bucket[max_gain + num_nets()];
  while (head) {
    std::cout << "Test cell " << head->name << '\n';
    if (meet_balance_criterion(head)) {
      std::cout << "Move cell " << head->name 
                << " from " << head->partition
                << " to " << !(head->partition) << '\n';
      head->partition = !(head->partition);
      break;
    }
    else {
      head = head->next;
    }
  }
  //update_cut_net();
  //update_gain();
}

inline void Hypergraph::update_cut_net(Cell* target) {
  for (size_t i = 0; i < target->nets.size(); ++i) {
    target->nets[i]->cut = false;
    for (size_t j = 0; j < target->nets[i]->cells.size(); ++j) {
      if (target->partition != target->nets[i]->cells[j]->partition) {
        target->nets[i]->cut = true;
        break;   
      }
    }
  }
}

inline void Hypergraph::update_gain(Cell* target) {
  int fs = 0;
  int te = 0;
  for (size_t i = 0; i < target->nets.size(); ++i) {
    // a cut net
    if (target->nets[i]->cut) {
      int same = 0;
      for (size_t j = 0; j < target->nets[i]->cells.size(); ++j) {
        if (target->nets[i]->cells[j]->partition ==
            target->partition) {
          ++same; 
        }
        if (same >= 2) {
          break;
        }
      }
      if (same == 1) {
        ++fs;
      }
    }
    // not a cut net
    else {
     ++te; 
    }
  }
  itr1->second.gain = fs-te;
}
