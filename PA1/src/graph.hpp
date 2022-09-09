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
  bool cut = false;
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

  size_t num_cells_p0 = 0;

  size_t num_nets() const;

  size_t num_cells() const;

  void traverse() const;

  void initialize_gain();
  
  void initialize_partition();

  void initialize_count_cells();

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

  void update_bucket(int, Cell*);

  void recover(Cell*);

  size_t find_max_cumulative_gain() const;

  void display_locked_cells() const;

  void output_answer();
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
  //std::cout << "r factor = " << r_factor << "\n\n\n\n";
  //std::getline(inClientFile, line);
  //r_factor = std::stof(line);
  
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
          //map_nets[net_name].cells.emplace_back(cell_name);

          // new cell
          if (map_cells.find(cell_name) == map_cells.end()) {
            Cell c;
            c.name = cell_name;
            c.nets.emplace_back(&(map_nets[net_name])); 
            //c.partition = std::rand()%2;
            //if (c.partition == 0) {
            //  ++num_cells_p0;
            //}
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
  area_lower_bound = static_cast<double>(map_cells.size()*(1-r_factor)/2.0);
  area_upper_bound = static_cast<double>(map_cells.size()*(1+r_factor)/2.0);

  // initialize partition
  initialize_partition();

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
    min_gain = itr1->second.gain < min_gain ? itr1->second.gain : min_gain;
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
  //if (candidate->locked == true) {
  //  return false;
  //}
  //size_t num_cells_p1 = num_cells() - num_cells_p0;
  // if moving candiate form p0 to p1 a valid move  
  if (candidate->partition == 0) {
    if (area_lower_bound < (num_cells_p0-1) &&
        area_upper_bound > (num_cells_p0-1)) {
      //std::cout << "true\n";
      return true;
    }
    return false;
  }
  else {
    if (area_lower_bound < (num_cells_p0+1) &&
        area_upper_bound > (num_cells_p0+1)) {
      //std::cout << "true\n";
      return true;
    }
    return false;
  }



  ////std::cout << "p0:" << num_cells_p0 << '\n';
  //size_t num_cells_p1 = num_cells() - num_cells_p0;
  //// if moving candiate form p0 to p1 a valid move  
  //if (candidate->partition == 0) {
  //  if (area_lower_bound < (num_cells_p0-1) &&
  //      area_upper_bound > (num_cells_p0-1)) {
  //    //std::cout << "true\n";
  //    return true;
  //  }
  //  else if (area_lower_bound < (num_cells_p1+1) &&
  //      area_upper_bound > (num_cells_p1+1)) {
  //    //std::cout << "true\n";
  //    return true;
  //  }
  //  //std::cout << "false\n";
  //  return false;
  //}
  //// if moving candidate from p1 to p0 a valid move
  //else {
  //  if (area_lower_bound < (num_cells_p0+1) &&
  //      area_upper_bound > (num_cells_p0+1)) {
  //    //std::cout << "true\n";
  //    return true;
  //  }
  //  else if (area_lower_bound < (num_cells_p1-1) &&
  //      area_upper_bound > (num_cells_p1-1)) {
  //    //std::cout << "true\n";
  //    return true;
  //  }
  //  //std::cout << "false\n";
  //  return false;
  //}
}

inline void Hypergraph::run_fm() {
  locked_cells.resize(num_cells());
  locked_cells_gain.resize(num_cells());
  size_t cnt = 0;
  int temp_gain = max_gain;
  
  while (cnt < num_cells()) {
    //std::cout << "cnt = " << cnt << "/" << num_cells() << '\n';
    Cell* head = bucket[temp_gain + num_nets()];
    //std::cout << "-------------------------\n";
    //std::cout << "Max gain = " << max_gain << '\n';
    //std::cout << "Testing cell " << head->name << " of gain " << head->gain << '\n';
    while (head) {
      if (!head->locked && meet_balance_criterion(head)) {
        //std::cout << "Move cell " << head->name 
        //          << " from " << head->partition
        //          << " to " << !(head->partition) << '\n';
        if (head->partition == 0) {
          --num_cells_p0;
        }
        else {
          ++num_cells_p0;
        }
        head->partition = !(head->partition);

        for (size_t i = 0; i < head->nets.size(); ++i) {
          if (head->partition == 1) {
            head->nets[i]->cnt_cells_p0 -= 1;
            head->nets[i]->cnt_cells_p1 += 1;
          }
          else {
            head->nets[i]->cnt_cells_p0 += 1;
            head->nets[i]->cnt_cells_p1 -= 1;
          }
        }

        update_cut_net(head);
      
        int old_gain = head->gain;
        head->gain = -1 * old_gain;
        if (old_gain != head->gain) {
          update_bucket(old_gain+num_nets(), head); 
        }
        
        max_gain = max_gain > head->gain
                 ? max_gain : head->gain;
        min_gain = min_gain < head->gain
                 ? min_gain : head->gain;
       
        std::set<Cell*>::iterator itr;
        for (itr = head->connected_cells.begin(); 
             itr != head->connected_cells.end(); ++itr) {
          
          old_gain = (*itr)->gain;
          update_gain(*itr);
          max_gain = max_gain > (*itr)->gain
                   ? max_gain : (*itr)->gain;
          min_gain = min_gain < (*itr)->gain
                   ? min_gain : (*itr)->gain;
          if (old_gain != (*itr)->gain) {
            update_bucket(old_gain+num_nets(), *itr);
          }
        }

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
    
    // a cell pointed by head is moved smoothly.
    if (head) {
      temp_gain = max_gain;
      continue;
    }
    // a cell pointed by head is not a successful move.
    if (head == nullptr) {
      --temp_gain;
      if (temp_gain < min_gain) {
        break;
      }
      continue;
    }

    //if (head == nullptr) {
    //  --temp_gain;  
    //}
    //if (temp_gain < min_gain) {
    //  break;
    //}
    //// search for the new max_gain if old max_gain does not exist
    //while (temp_gain >= min_gain && bucket[temp_gain+num_nets()] == nullptr) {
    //  --temp_gain;
    //}
    //if (temp_gain < min_gain) {
    //  break;
    //}
  }
  //display_locked_cells();
  
  size_t idx = find_max_cumulative_gain();
  //std::cout << "idx = " << idx << '\n';
  if (idx != num_cells() - 1) {
    for (size_t i = locked_cells.size()-1; i > idx; --i) {
      if (locked_cells[i] == nullptr) {
        continue;
      }
      //std::cout << "recover i = " << i << '\n';
      recover(locked_cells[i]);
    }
  }
}

inline void Hypergraph::update_cut_net(Cell* target) {
  for (size_t i = 0; i < target->nets.size(); ++i) {
    if (target->nets[i]->cnt_cells_p0 != 0 && 
        target->nets[i]->cnt_cells_p1 != 0) {
      target->nets[i]->cut = true;
    }
    else {
      target->nets[i]->cut = false;
    }
    //target->nets[i]->cut = false;
    //for (size_t j = 0; j < target->nets[i]->cells.size(); ++j) {
    //  if (target->partition != target->nets[i]->cells[j]->partition) {
    //    target->nets[i]->cut = true;
    //    break;   
    //  }
    //}
  }
}

// update the gain of the target cell 
inline void Hypergraph::update_gain(Cell* target) {
  int fs = 0;
  int te = 0;
  for (size_t i = 0; i < target->nets.size(); ++i) {
    // a cut net
    if (target->nets[i]->cut) {
      if (target->partition == 0 && target->nets[i]->cnt_cells_p0 == 1) {
        ++fs;
        continue;
      }
      if (target->partition == 1 && target->nets[i]->cnt_cells_p1 == 1) {
        ++fs;
        continue;
      }
    }
    // not a cut net
    else {
      ++te;
    }
  }
  //for (size_t i = 0; i < target->nets.size(); ++i) {
  //  // a cut net
  //  if (target->nets[i]->cut) {
  //    int same = 0;
  //    for (size_t j = 0; j < target->nets[i]->cells.size(); ++j) {
  //      if (target->nets[i]->cells[j]->partition ==
  //          target->partition) {
  //        ++same; 
  //      }
  //      if (same >= 2) {
  //        break;
  //      }
  //    }
  //    if (same == 1) {
  //      ++fs;
  //    }
  //  }
  //  // not a cut net
  //  else {
  //   ++te; 
  //  }
  //}
  target->gain = fs-te;
}

inline void Hypergraph::initialize_count_cells() {
  std::unordered_map<std::string, Cell>::iterator itr;
  for (itr = map_cells.begin(); itr != map_cells.end(); ++itr) {
    //std::cout << "cell " << itr->second.name << '\n';
    for (size_t i = 0; i < itr->second.nets.size(); ++i) {
      if (itr->second.partition == 0) {
        //std::cout << "partition = 0, add 1 to " << itr->second.nets[i]->name << '\n';
        itr->second.nets[i]->cnt_cells_p0 += 1;
      }
      else {
        //std::cout << "partition = 1, add 1 to " << itr->second.nets[i]->name << '\n';
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

    //for (size_t i = 0; i < itr->second.nets.size(); ++i) {
    //  if (itr->second.partition == 0) {
    //    itr->second.nets[i]->cnt_cells_p0 += 1;
    //  }
    //  else {
    //    itr->second.nets[i]->cnt_cells_p1 += 1;
    //  }
    //}
  }

  initialize_count_cells();
  //num_cells_p0 = num_cells()-ceil(area_lower_bound) > ceil(area_lower_bound) 
  //           ? ceil(area_lower_bound) : num_cells()-ceil(area_lower_bound);
  //size_t num_cells_p1 = num_cells() - num_cells_p0; 
  //size_t cnt = num_cells_p0 > num_cells_p1
  //           ? num_cells_p1 : num_cells_p0;

  //std::unordered_map<std::string, Cell>::iterator itr;
  //for (itr = map_cells.begin(); itr != map_cells.end(); ++itr) {
  //  if (cnt == 0) {
  //    itr->second.partition = 1;
  //  }
  //  else {
  //    //itr->second.partition = rand()%2;
  //    //if (itr->second.partition == 0) {
  //    //  --cnt;
  //    //}
  //    itr->second.partition = 0;
  //    --cnt;
  //  }
  //}
}

// update the target in the bucket
inline void Hypergraph::update_bucket(int old_index, Cell* target) {
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

  // update target at its new index
  int new_index = target->gain + num_nets();
  // target will be the first pointer in the new_index position
  if (bucket[new_index] == nullptr) {
    target->next = nullptr;
    target->prev = nullptr;
    bucket[new_index] = target;
    tail_bucket[new_index] = target;
  }
  else {
    //Cell* head = bucket[new_index];
    //while (head->next) {
    //  head = head->next;
    //}
    //head->next = target;
    //target->next = nullptr;
    //target->prev = head;
    tail_bucket[new_index]->next = target;
    target->next = nullptr;
    target->prev = tail_bucket[new_index];
    tail_bucket[new_index] = target;
  }
}

inline void Hypergraph::recover(Cell* target) {
  //if (target == nullptr) {
  //  return;
  //}
  //
  int old_gain = target->gain;

  if (target->partition == 0) {
    --num_cells_p0;
  }
  else {
    ++num_cells_p0;
  }
  
  for (size_t i = 0; i < target->nets.size(); ++i) {
    if (target->partition == 0) {
      target->nets[i]->cnt_cells_p0 -= 1;
      target->nets[i]->cnt_cells_p1 += 1;
    }
    else {
      target->nets[i]->cnt_cells_p0 += 1;
      target->nets[i]->cnt_cells_p1 -= 1;
    }
  }

  target->partition = !(target->partition);
  target->gain = -1 * old_gain;
  update_cut_net(target);
  if (old_gain != target->gain) {
    update_bucket(old_gain+num_nets(), target);
  }

  std::set<Cell*>::iterator itr;
  for (itr = target->connected_cells.begin(); 
       itr != target->connected_cells.end(); ++itr) {
    
    old_gain = (*itr)->gain;
    update_gain(*itr);
    if (old_gain != (*itr)->gain) {
      update_bucket(old_gain+num_nets(), *itr);
    }
  }
  target->locked = false;
}

inline size_t Hypergraph::find_max_cumulative_gain() const {
  size_t idx = 0;
  int cum_gain = locked_cells_gain[0];
  int max_gain = cum_gain;
  for (size_t i = 1; i < locked_cells_gain.size(); ++i) {
    cum_gain += locked_cells_gain[i];
    //std::cout << "prefix sum = " << cum_gain << '\n';
    if (cum_gain >= max_gain && locked_cells_gain[i] != 0) {
      max_gain = cum_gain;
      idx = i;
    }
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
    if (itr->second.cut) {
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
