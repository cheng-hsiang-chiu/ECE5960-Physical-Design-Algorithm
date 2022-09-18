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
#include <cmath>

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
  int cnt_cells_p0 = 0;
  int cnt_cells_p1 = 0;
  std::string name;
  std::vector<Cell*> cells;
};


class Hypergraph {
public:
  Hypergraph(std::string&, std::string&);

  std::string output_path;

  std::unordered_map<std::string, Net> map_nets;

  std::unordered_map<std::string, Cell> map_cells;

  std::vector<Cell*> bucket;
  
  std::vector<Cell*> tail_bucket;

  std::vector<Cell*> locked_cells;
  
  std::vector<int> locked_cells_gain;

  double r_factor;

  double area_lower_bound;

  double area_upper_bound;

  double max_area = 1.0;

  int max_gain = INT_MIN;
  
  int min_gain = INT_MAX;

  int max_edge = INT_MIN;

  bool next_pass = true;

  size_t num_cells_p0 = 0;

  size_t num_nets() const;

  size_t num_cells() const;

  void traverse() const;

  void initialize_gain();
  
  void initialize_partition();

  void initialize_count_cells();

  void display_partition() const;

  void display_gain() const;

  void display_connected_cells() const;

  void construct_bucket();

  void display_bucket() const;
  
  void run_fm();

  bool meet_balance_criterion(Cell*) const;

  void update_gain(Net*, Cell*);

  void update_bucket(int, Cell*);

  void recover(Cell*);

  size_t find_max_cumulative_gain();

  void display_locked_cells() const;
  
  void display_count_cells() const;

  void output_answer();

  void one_pass();

  void delete_from_bucket(Cell*);
};


Hypergraph::Hypergraph(std::string& input_file, std::string& output_file) {
  output_path = output_file;

  std::ifstream inClientFile(input_file, std::ios::in);

  if (!inClientFile) {
    std::cerr << "File could not be opened or does not exist\n";
    exit(1);
  }

  std::string line;
  inClientFile >> r_factor;
  
  std::string net_name;
  std::string cell_name;
  std::vector<Cell*> temp;

  while(std::getline(inClientFile, line)) {
    std::stringstream ss(line);
    temp.clear();
    while (std::getline(ss, line, ' ')) {
      if (line != ";" && line != "NET" && line.length() != 0) {
        // net string
        if (line[0] == 'n' || line[0] == 'T') {
          net_name = line;

          Net n;
          n.name = net_name;
          map_nets[net_name] = n;
        }
        // cell string
        else {
          cell_name = line;

          // new cell
          if (map_cells.find(cell_name) == map_cells.end()) {
            Cell c;
            c.name = cell_name;
            c.nets.emplace_back(&(map_nets[net_name])); 
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
      }
    }
  }
  
  // all cells have unit area
  area_lower_bound = static_cast<double>(map_cells.size()*(1-r_factor)/2.0);
  area_upper_bound = static_cast<double>(map_cells.size()*(1+r_factor)/2.0);

  // initialize partition
  initialize_partition();

  // initialize the gain for each cell
  initialize_gain();

  std::unordered_map<std::string, Cell>::iterator itr;
  for (itr = map_cells.begin(); itr != map_cells.end(); ++itr) {
    max_edge = max_edge > static_cast<int>(itr->second.nets.size()) 
             ? max_edge : static_cast<int>(itr->second.nets.size());
  }
  
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
  std::unordered_map<std::string, Cell>::iterator itr;
  for (itr = map_cells.begin(); itr != map_cells.end(); ++itr) {
    int gain = 0;
    int FromBlock = 0;
    int ToBlock = 0;

    for (size_t i = 0; i < itr->second.nets.size(); ++i) {
      if (itr->second.partition == 0) {
        FromBlock = itr->second.nets[i]->cnt_cells_p0;
        ToBlock = itr->second.nets[i]->cnt_cells_p1;
      }
      else {
        FromBlock = itr->second.nets[i]->cnt_cells_p1;
        ToBlock = itr->second.nets[i]->cnt_cells_p0;
      }

      if (FromBlock == 1) {
        ++gain;
      }
      if (ToBlock == 0) {
        --gain;
      }

      itr->second.gain = gain;
    }
    max_gain = itr->second.gain > max_gain ? itr->second.gain : max_gain;
    min_gain = itr->second.gain < min_gain ? itr->second.gain : min_gain;
  } 
}

inline void Hypergraph::display_partition() const {
  std::unordered_map<std::string, Cell>::const_iterator itr;
  for (itr = map_cells.begin(); itr != map_cells.end(); ++itr) {
    std::cout << itr->second.name 
              << " at partition " 
              << itr->second.partition << '\n'; 
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
  bucket.resize(2*max_edge+1);
  tail_bucket.resize(2*max_edge+1);
  int offset = max_edge;

  std::unordered_map<std::string, Cell>::iterator itr;
  for (itr = map_cells.begin(); itr != map_cells.end(); ++itr) {
    if (bucket[itr->second.gain+offset] == nullptr) {
      bucket[itr->second.gain+offset] = &(itr->second);
      tail_bucket[itr->second.gain+offset] = &(itr->second);
    }
    else {
      tail_bucket[itr->second.gain+offset]->next = &(itr->second);
      itr->second.prev = tail_bucket[itr->second.gain+offset];
      tail_bucket[itr->second.gain+offset] = 
      tail_bucket[itr->second.gain+offset]->next;
    }
  }
}

inline void Hypergraph::display_bucket() const {
  for (size_t i = 0; i < bucket.size(); ++i) {
    if (bucket[i] != nullptr) {
      Cell* head = bucket[i];
      std::cout << "bucket[" << static_cast<int>(i - max_edge) << "] has cells: ";
      while (head) {
        std::cout << head->name << " ";
        head = head->next;
      }
      std::cout << '\n';
    }
  }
}

inline bool Hypergraph::meet_balance_criterion(Cell* candidate) const {
  if (candidate->partition == 0) {
    if (area_lower_bound < (num_cells_p0-1) &&
        area_upper_bound > (num_cells_p0-1)) {
      return true;
    }
    return false;
  }
  else {
    if (area_lower_bound < (num_cells_p0+1) &&
        area_upper_bound > (num_cells_p0+1)) {
      return true;
    }
    return false;
  }
}

inline void Hypergraph::run_fm() {
  size_t pass = 1;
  while(1) {
    std::cout << "  Running pass " << pass++;
    one_pass();

    // prepare for the next pass
    if (next_pass) {
      // reset the cell state except the partition
      std::unordered_map<std::string, Cell>::iterator itr;
      for (itr = map_cells.begin(); itr != map_cells.end(); ++itr) {
        itr->second.locked = false;
        itr->second.prev = nullptr;
        itr->second.next = nullptr;
        itr->second.gain = 0;
      }

      max_gain = INT_MIN;
      min_gain = INT_MAX;
      bucket.clear();
      tail_bucket.clear();
      locked_cells.clear();
      locked_cells_gain.clear();
      initialize_gain();
      construct_bucket();
    }
    else {
      break;
    }
  }
}

inline void Hypergraph::recover(Cell* target) {
  int old_gain = target->gain;

  if (target->partition == 0) {
    --num_cells_p0;
    target->partition = 1;
  }
  else {
    ++num_cells_p0;
    target->partition = 0;
  }
  
  for (size_t i = 0; i < target->nets.size(); ++i) {
    if (target->partition == 1) {
      --target->nets[i]->cnt_cells_p0;
      ++target->nets[i]->cnt_cells_p1;
    }
    else {
      ++target->nets[i]->cnt_cells_p0;
      --target->nets[i]->cnt_cells_p1;
    }
  }
}

// update the gain of the target cell 
inline void Hypergraph::update_gain(Net* net, Cell* base) {
  int FromBlock = 0;
  int ToBlock = 0;
  int temp = 0;

  if (base->partition == 0) {
    FromBlock = net->cnt_cells_p0;
    ToBlock = net->cnt_cells_p1;
    
    --(net->cnt_cells_p0); 
    ++(net->cnt_cells_p1); 
  }
  else {
    FromBlock = net->cnt_cells_p1;
    ToBlock = net->cnt_cells_p0;
     
    ++(net->cnt_cells_p0); 
    --(net->cnt_cells_p1); 
  }
  if (ToBlock == 0) {
    for (size_t i = 0; i < net->cells.size(); ++i) {
      if (net->cells[i] == base) {
        continue;
      }
      if (!net->cells[i]->locked) {
        temp = net->cells[i]->gain;
        ++(net->cells[i]->gain);
        update_bucket(temp, net->cells[i]);
      }
    } 
  }
  else if (ToBlock == 1) {
    for (size_t i = 0; i < net->cells.size(); ++i) {
      if (net->cells[i] == base) {
        continue;
      }
      if (!net->cells[i]->locked && 
          net->cells[i]->partition == !(base->partition)) {
        temp = net->cells[i]->gain;
        --(net->cells[i]->gain);
        update_bucket(temp, net->cells[i]);
      }
    }
  }
  
  --FromBlock;
  ++ToBlock;
  if (FromBlock == 0) {
    for (size_t i = 0; i < net->cells.size(); ++i) {
      if (net->cells[i] == base) {
        continue;
      }
      if (!net->cells[i]->locked) {
        temp = net->cells[i]->gain;
        --(net->cells[i]->gain);
        update_bucket(temp, net->cells[i]);
      }
    }
  }
  else if (FromBlock == 1) {
    for (size_t i = 0; i < net->cells.size(); ++i) {
      if (net->cells[i] == base) {
        continue;
      }
      if (!net->cells[i]->locked &&
          net->cells[i]->partition == base->partition) {
        temp = net->cells[i]->gain;
        ++(net->cells[i]->gain);
        update_bucket(temp, net->cells[i]);
      }
    }
  }
}

inline void Hypergraph::delete_from_bucket(Cell* target) {
  int old_index = -1*target->gain + max_edge;

  if (target->prev != nullptr) {
    if (target->next != nullptr) {
      target->prev->next = target->next;
      target->next->prev = target->prev;
    }
    // target is the last
    else {
      target->prev->next = nullptr;
      tail_bucket[old_index] = target->prev;
    }
  }
  // target is the first
  else {
    if (target->next != nullptr) {
      bucket[old_index] = target->next;
      target->next->prev = nullptr;     
    }
    // target is the only element
    else {
      bucket[old_index] = nullptr;
      tail_bucket[old_index] = nullptr;
    }
  }
  
  target->next = nullptr;
  target->prev = nullptr;
}

inline void Hypergraph::initialize_count_cells() {
  std::unordered_map<std::string, Cell>::iterator itr;
  for (itr = map_cells.begin(); itr != map_cells.end(); ++itr) {
    for (size_t i = 0; i < itr->second.nets.size(); ++i) {
      if (itr->second.partition == 0) {
        itr->second.nets[i]->cnt_cells_p0 += 1;
      }
      else {
        itr->second.nets[i]->cnt_cells_p1 += 1;
      }
    }
  }
}

inline void Hypergraph::initialize_partition() {
  size_t p0 = 0;
  size_t p1 = 0;
  size_t half = num_cells()/2;

  std::unordered_map<std::string, Cell>::iterator itr;
  for (itr = map_cells.begin(); itr != map_cells.end(); ++itr) {
    // the number of cells in p0 is enough
    if (p0 == half) {
      itr->second.partition = 1;
    }
    else if (p1 == half) {
      itr->second.partition = 0;
      ++num_cells_p0;
    }
    else {
      bool p = rand()%2;
      if (p == 1) {
        itr->second.partition = 1;
        ++p1;  
      }
      else {
        itr->second.partition = 0;
        ++p0;  
        ++num_cells_p0;
      }
    }
  }
  initialize_count_cells();
}

// update the target in the bucket
inline void Hypergraph::update_bucket(int old_gain, Cell* target) {
  int old_index = old_gain + max_edge;
  // update the linked list at old_index
  // target is not the first
  if (target->prev != nullptr) {
    if (target->next != nullptr) {
      target->prev->next = target->next;
      target->next->prev = target->prev;
    }
    // target is the last
    else {
      target->prev->next = nullptr;
      tail_bucket[old_index] = target->prev;
    }
  }
  // target is the first
  else {
    if (target->next != nullptr) {
      bucket[old_index] = target->next;
      target->next->prev = nullptr;     
    }
    // target is the only element
    else {
      bucket[old_index] = nullptr;
      tail_bucket[old_index] = nullptr;
    }
  }

  int new_index = target->gain + max_edge;
  // target will be the first pointer in the new_index position
  if (bucket[new_index] == nullptr) {
    target->next = nullptr;
    target->prev = nullptr;
    bucket[new_index] = target;
    tail_bucket[new_index] = target;
  }
  else {
    tail_bucket[new_index]->next = target;
    target->next = nullptr;
    target->prev = tail_bucket[new_index];
    tail_bucket[new_index] = target;
  }
}

inline size_t Hypergraph::find_max_cumulative_gain() {
  size_t idx = 0;
  int prefix_gain = locked_cells_gain[0];
  int max_gain = prefix_gain;
  for (size_t i = 1; i < locked_cells_gain.size(); ++i) {
    prefix_gain += locked_cells_gain[i];
    if (prefix_gain >= max_gain && locked_cells_gain[i] != 0) {
      max_gain = prefix_gain;
      idx = i;
    }
  }
  std::cout << " gets " << max_gain << " gains improvement\n";
  if (max_gain <= 0) {
    next_pass = false;
  }

  return idx;
} 

inline void Hypergraph::display_locked_cells() const {
  for (size_t i = 0; i < locked_cells.size(); ++i) {
    if (locked_cells[i] == nullptr) {
      return;
    }
    std::cout << "Cell " << locked_cells[i]->name 
              << " has gain = " << locked_cells_gain[i] << '\n';
  }
}

inline void Hypergraph::output_answer() {
  std::ofstream outClientFile(output_path, std::ios::out);

  if (!outClientFile) {
    std::cerr << "File could not be opened.\n";
    exit(1);
  }
 
  size_t cutsize = 0;
  std::unordered_map<std::string, Net>::iterator itr;
  for (itr = map_nets.begin(); itr != map_nets.end(); ++itr) {
    if (itr->second.cnt_cells_p0 != 0 && itr->second.cnt_cells_p1 != 0) {
      ++cutsize;
    }
  }

  outClientFile << "Cutsize = " << cutsize << '\n';
  outClientFile << "G1 " << num_cells_p0 << '\n';
  
  std::unordered_map<std::string, Cell>::iterator itr1;
  for (itr1 = map_cells.begin(); itr1 != map_cells.end(); ++itr1) {
    if (itr1->second.partition == 0) {
      outClientFile << itr1->second.name << ' ';
    }
  }
  outClientFile << ";\n";
  
  outClientFile << "G2 " << num_cells()-num_cells_p0 << '\n';
  for (itr1 = map_cells.begin(); itr1 != map_cells.end(); ++itr1) {
    if (itr1->second.partition == 1) {
      outClientFile << itr1->second.name << ' ';
    }
  }
  outClientFile << ";\n";
}

inline void Hypergraph::display_count_cells() const {
  std::unordered_map<std::string, Net>::const_iterator itr0;
  for (itr0 = map_nets.begin(); itr0 != map_nets.end(); ++itr0) {
    std::cout << "NET " << itr0->first << " has \n";
    std::cout << "cnt_0 = " << itr0->second.cnt_cells_p0
              << ", cnt_1 = " << itr0->second.cnt_cells_p1<< '\n';
  }
}

inline void Hypergraph::one_pass() {
  locked_cells.resize(num_cells());
  locked_cells_gain.resize(num_cells());
  size_t cnt = 0;
  int index = bucket.size()-1;

  while (cnt < num_cells()) {
    Cell* head = bucket[index];

    while (head == nullptr) {
      if (index > 0) {
        --index;
      }
      else {
        break;
      }
      head = bucket[index];
    }
   
    if (index == 0 && head == nullptr) {
      break; 
    }
    while (head) {
      if (!head->locked && meet_balance_criterion(head)) {
        if (head->partition == 0) {
          --num_cells_p0;
        }
        else {
          ++num_cells_p0;
        }
        
        for (size_t i = 0; i < head->nets.size(); ++i) {
          update_gain(head->nets[i], head);
        }
        head->partition = !(head->partition);
        
        head->gain = -1 * head->gain;
        delete_from_bucket(head);

        head->locked = true;
        locked_cells[cnt] = head; 
        locked_cells_gain[cnt] = -1*head->gain; 
        
        ++cnt;

        break;
      }
      else {
        head = head->next;
      }
    }

    if (head == nullptr && index > 0) {
      --index;
    }
  }
  
  size_t idx = find_max_cumulative_gain();
  if (idx != num_cells() - 1) {
    for (size_t i = locked_cells.size()-1; i > idx; --i) {
      if (locked_cells[i] == nullptr) {
        continue;
      }
      recover(locked_cells[i]);
    }
  }
}
