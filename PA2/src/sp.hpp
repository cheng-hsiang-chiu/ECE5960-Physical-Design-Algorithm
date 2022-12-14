#pragma once

#include <vector>
#include <variant>
#include <fstream>
#include <regex>
#include <unordered_map>
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <deque>
#include <climits>
#include <random>
#include <set>


namespace fp {



// random generator
static std::minstd_rand engine{std::random_device{}()};
static std::uniform_int_distribution<size_t> distribution;
static size_t max_dist = distribution.max();
static size_t random_value() {
  return distribution(engine);
}


class Block {
public:
  size_t lower_left_x = 0;
  size_t lower_left_y = 0;
  size_t backup_lower_left_x = 0;
  size_t backup_lower_left_y = 0;
  size_t width = 0;
  size_t height = 0;
  size_t backup_width = 0;
  size_t backup_height = 0;
  std::string name;

  std::vector<Block*> rightof;
  std::vector<Block*> aboveof;
  std::vector<Block*> backup_rightof;
  std::vector<Block*> backup_aboveof;

  int idx_positive_sequence = -1;
  int idx_negative_sequence = -1;
  int backup_idx_positive_sequence = -1;
  int backup_idx_negative_sequence = -1;
};

class Terminal {
public:
  size_t pos_x = 0;
  size_t pos_y = 0;
  std::string name;
};

class Net {
public:
  std::vector<std::variant<Block*, Terminal*>> net; 
};


enum Sequence {
  POS = 0,
  NEG,
  BOTH
};

enum Orientation {
  Horizontal = 0,
  Vertical
};


enum MoveType {
  nomove = 0,
  move1,
  move2,
  move3,
  move4
};


class SP {
public:

  std::unordered_map<std::string, Block> map_blocks;
  std::unordered_map<std::string, Terminal> map_terminals;
  std::vector<Net> vec_nets;

  std::vector<Block*> positive_sequence;
  std::vector<Block*> negative_sequence;
  std::vector<Block*> backup_positive_sequence;
  std::vector<Block*> backup_negative_sequence;

  size_t outline_width;
  size_t outline_height;
  size_t num_blocks;
  size_t num_terminals;
  size_t num_nets;
  
  double alpha = 0;

  // parameters for Simulated Annealing
  const double initial_temperature = 1000.0;

  const double frozen_temperature = 0.1;

  const double decay = 0.95; 
    
  const size_t iterations_per_temperature = 3000;
 
  // source and terminus blocks for horizontal/vertical constraint graph
  Block source;
  Block terminus;
 
  // bounding box width and height and its backups
  int bb_width = 0;
  int bb_height = 0;
  int hpwl = 0;
  int backup_bb_width = 0;
  int backup_bb_height = 0; 
  int backup_hpwl = 0;

  double average_area = 0.0;
  double average_length = 0.0;
  size_t num_iterations = 0;

  int overshoot_length = 0;

  int penalty_ratio = 2;
 
  std::string output_path;
  
  SP() = default;

  SP(double, const std::string&, const std::string&, const std::string&);

  void dump(std::ostream&) const;

  void dump_backup(std::ostream&) const;

  void visualize() const;

  void initialize_sequence();

  std::pair<size_t, size_t> move1();
  
  std::pair<size_t, size_t> move2();

  std::pair<size_t, size_t> move3();
  
  std::pair<size_t, size_t> move4();

  void run();

  double pack();

  void construct_relative_locations(const size_t, const size_t, const MoveType);
  
  std::vector<int> spfa(const Orientation);
  
  void compute_block_locations(std::vector<int>&, const Orientation);

  void compute_area(const std::vector<int>&, int&, const Orientation);

  int compute_hpwl() const;

  void initialize_backup_data();

  void update_backup_data(const size_t, const size_t, const MoveType);

  void resume_backup_data(const size_t, const size_t, const MoveType);

  void dump_solution() const; 
};


inline SP::SP(double a, const std::string& input_block_path, 
              const std::string& input_net_path,
              const std::string& out_path) {

  output_path = out_path;
  source.name = "source";
  terminus.name = "terminus";

  alpha = a;
   
  std::ifstream inBlkFile(input_block_path, std::ios::in);
  if (!inBlkFile) {
    std::cerr << "File could not be opened or does not exist\n";
    exit(1);
  }

  std::ifstream inNetFile(input_net_path, std::ios::in);
  if (!inNetFile) {
    std::cerr << "File could not be opened or does not exist\n";
    exit(1);
  }

  std::string line;
  std::regex reg("\\s+");
 
  /***************  
   * parsing
   **************/ 
  // outline of width and height
  std::getline(inBlkFile, line);
  std::sregex_token_iterator it(line.begin(), line.end(), reg, -1);
  outline_width  = std::stoi(*(++it)); 
  outline_height = std::stoi(*(++it)); 
  //std::cout << outline_width << " " << outline_height << '\n';

  // number of blocks
  std::getline(inBlkFile, line);
  it = std::sregex_token_iterator(line.begin(), line.end(), reg, -1);
  num_blocks = std::stoi(*(++it)); 
  //std::cout << "num_blocks = " << num_blocks << '\n';
  
  // number of terminals
  std::getline(inBlkFile, line);
  it = std::sregex_token_iterator(line.begin(), line.end(), reg, -1);
  num_terminals = std::stoi(*(++it)); 
  //std::cout << "num_terminals = " << num_terminals << '\n';

  size_t num_lines = 0;
  while (std::getline(inBlkFile, line)) {
    
    if (line[0] < 'A' || (line[0] > 'Z' && line[0] < 'a') || line[0] > 'z') {
      continue; 
    }
    
    it = std::sregex_token_iterator(line.begin(), line.end(), reg, -1);
    
    // dimensions of blocks
    if (num_lines < num_blocks) {
      Block blk;
      blk.name = *(it);
      blk.width = std::stoi(*(++it));
      blk.height = std::stoi(*(++it));
      map_blocks[blk.name] = blk;
      ++num_lines;
    }

    // settings of terminals
    else {
      Terminal t;
      t.name = *(it);
      ++it;
      t.pos_x = std::stoi(*(++it));
      t.pos_y = std::stoi(*(++it));
      map_terminals[t.name] = t;
    }
  }

  
  // number of nets 
  std::getline(inNetFile, line);
  it = std::sregex_token_iterator(line.begin(), line.end(), reg, -1);
  num_nets  = std::stoi(*(++it)); 

  // configurations of nets
  while (std::getline(inNetFile, line)) {
    if (line.size() == 0) {
      continue;
    }
    
    size_t netdegree = 0;
    it = std::sregex_token_iterator(line.begin(), line.end(), reg, -1);
    netdegree = std::stoi(*(++it));
    
    Net n;
    for (size_t i = 0; i < netdegree; ++i) {
      std::variant<Block*, Terminal*> var;
      std::getline(inNetFile, line);
      it = std::sregex_token_iterator(line.begin(), line.end(), reg, -1);
      
      if (map_blocks.find(*it) != map_blocks.end()) {
        var = &(map_blocks[*it]);
      }
      else {
        var = &(map_terminals[*it]);
      }
      n.net.emplace_back(var);
    }
    vec_nets.emplace_back(n);
  }
  // end of parsing
  
  // initialize positive and negative sequences
  initialize_sequence();
}


// initialize positive and negative sequences
inline void SP::initialize_sequence() {
  positive_sequence.resize(num_blocks); 
  negative_sequence.resize(num_blocks);
  
  size_t idx = random_value()%num_blocks;

  for (auto& [key, value] : map_blocks) {
    while (positive_sequence[idx] != nullptr) {
      idx = random_value()%num_blocks;
    }
    positive_sequence[idx] = &(map_blocks[key]);
    map_blocks[key].idx_positive_sequence = idx;

    idx = random_value()%num_blocks;
    while (negative_sequence[idx] != nullptr) {
      idx = random_value()%num_blocks;
    }
    negative_sequence[idx] = &(map_blocks[key]);
    map_blocks[key].idx_negative_sequence = idx;
  }
}


// construct relative locations of blocks between [id1, id2]
inline void SP::construct_relative_locations(
  const size_t id1, const size_t id2, const MoveType move_type) {
  
  std::set<Block*> affected;

  switch(move_type) {
    // nomove or move1
    case 0:
    case 1:
      for (size_t i = id1; i <= id2; ++i) {
        affected.insert(positive_sequence[i]);
      }
    break;

    case 2:
      for (size_t i = id1; i <= id2; ++i) {
        affected.insert(negative_sequence[i]);
      }
    break;

    case 3:
      {
        size_t nid1, nid2;
        nid1 = positive_sequence[id1]->idx_negative_sequence;
        nid2 = positive_sequence[id2]->idx_negative_sequence;

        for (size_t i = id1; i <= id2; ++i) {
          affected.insert(positive_sequence[i]);
        }

        if (nid1 > nid2) {
          std::swap(nid1, nid2);
        }

        for (size_t i = nid1; i <= nid2; ++i) {
          affected.insert(negative_sequence[i]);
        }
      }
    break;

    case 4:
      
    break;
  }

  int pidx, nidx;
  // construct rightof vector
  for (auto& blk : affected) {
    blk->rightof.clear();
    
    pidx = blk->idx_positive_sequence+1;
    nidx = blk->idx_negative_sequence;
    
    while (pidx < num_blocks) {
      Block* target = positive_sequence[pidx];
      if (target->idx_negative_sequence > nidx) {
        blk->rightof.push_back(target);
      }
      ++pidx;
    }
    if (move_type != MoveType::nomove) {
      blk->rightof.push_back(&terminus);
    }
  }

  // construct aboveof vector
  for (auto& blk : affected) {
    blk->aboveof.clear();
    
    pidx = static_cast<int>(blk->idx_positive_sequence)-1; 
    nidx = blk->idx_negative_sequence;

    while (pidx >= 0) {
      Block* target = positive_sequence[pidx];
      if (target->idx_negative_sequence > nidx) {
        blk->aboveof.push_back(target);
      }
      --pidx;
    }
    if (move_type != MoveType::nomove) {
      blk->aboveof.push_back(&terminus);
    }
  }
  
  // rightof and aboveof regarding source and 
  // terminus blocks have been inserted
  if (move_type != MoveType::nomove) {
    return; 
  }

  // construct relative locations for source & terminus blocks
  for (auto& [key, value] : map_blocks) {
    source.rightof.push_back(&value);
    source.aboveof.push_back(&value);
    value.rightof.push_back(&terminus);
    value.aboveof.push_back(&terminus);
  }
}


// calculate shortest path fast algorithm
// with minus weight
inline std::vector<int> SP::spfa(const Orientation orientation) {
  std::vector<int> distance(num_blocks+2);
  std::fill_n(distance.begin(), num_blocks+2, INT_MAX);
  std::vector<bool> in_queue(num_blocks+2);
  std::fill_n(in_queue.begin(), num_blocks+2, false);

  distance[0] = 0;
  
  std::deque<Block*> Q;

  Q.push_back(&source);
  in_queue[0] = true;

  while (!Q.empty()) {
    Block* u = Q.front();
    Q.pop_front();

    size_t indexu = 0, indexv = 0;
    if (u->idx_positive_sequence == -1) {
      if (u == &source) {
        indexu = 0;
      }
      else {
        indexu = num_blocks+1;
      }
    }
    else {
      indexu = u->idx_positive_sequence+1;
    }
    in_queue[indexu] = false;
  
    std::vector<Block*> vec; 
    if(orientation == Orientation::Horizontal) {
      vec = u->rightof;
    }
    else {
      vec = u->aboveof;
    }

    for (auto& blk : vec) {

      if (blk->idx_positive_sequence == -1) {
        if (blk == &source) {
          indexv = 0;
        }
        else {
          indexv = num_blocks+1;
        }
      }
      else {
        indexv = blk->idx_positive_sequence+1;
      }
     
      int weight = 0;
      if (orientation == Orientation::Horizontal) {
        weight = -1 * static_cast<int>(blk->width);
      }
      else {
        weight = -1 * static_cast<int>(blk->height);
      }
      
      if (distance[indexu] + weight < distance[indexv]) {
        distance[indexv] = distance[indexu] + weight;

        if (in_queue[indexv] == false) {
          Q.push_back(blk);
          in_queue[indexv] = true;
        }
      }
    }
  }
  return distance;
}


// compute hpwl
inline int SP::compute_hpwl() const {
  int tmp_hpwl = 0;
  int max_x = 0, min_x = INT_MAX, max_y = 0, min_y = INT_MAX;
  int llx = 0, lly = 0;
  
  for (auto& net : vec_nets) {

    max_x = 0;
    min_x = INT_MAX;
    max_y = 0;
    min_y = INT_MAX;
    llx = 0;
    lly = 0;
    
    for (auto& n : net.net) {
      // n is a terminal
      if (std::holds_alternative<Terminal*>(n)) {
        llx = 2*std::get<Terminal*>(n)->pos_x;
        lly = 2*std::get<Terminal*>(n)->pos_y;
      }
      // n is a block
      else {
        int x = std::get<Block*>(n)->lower_left_x;
        int y = std::get<Block*>(n)->lower_left_y;
        int w = std::get<Block*>(n)->width;
        int h = std::get<Block*>(n)->height;
        
        llx = 2*x + ((x+w)-x); 
        lly = 2*y + ((y+h)-y); 
      }

      if (llx >= max_x) {
        max_x = llx;    
      }
      if (llx <= min_x) {
        min_x = llx;
      }
      if (lly >= max_y) {
        max_y = lly;
      }
      if (lly <= min_y) {
        min_y = lly;
      }
    }

    tmp_hpwl = tmp_hpwl + (max_x-min_x) + (max_y-min_y);
  }
    
  return tmp_hpwl;
}


// compute the lower left x and y of blocks
inline void SP::compute_block_locations(std::vector<int>& distance,
  const Orientation orientation) {
  
  switch (orientation) {
    // horizontal
    case 0:
      for (auto& [key, value] : map_blocks) {
        value.lower_left_x = 
          -1*distance[value.idx_positive_sequence+1] - value.width;
      }
    break;

    // vertical
    case 1:
      for (auto& [key, value] : map_blocks) {
        value.lower_left_y = 
          -1*distance[value.idx_positive_sequence+1] - value.height;
      }
    break;
  }
}


// compute the bounding box area 
inline void SP::compute_area(const std::vector<int>& distance, int& bb,
  const Orientation orientation) {

  size_t limit = 0;
  if (orientation == Orientation::Horizontal) {
    limit = outline_width;
  }
  else {
    limit = outline_height;
  }

  bb = INT_MIN;
  for (auto& [key, value] : map_blocks) {
    bb = bb > -1*distance[value.idx_positive_sequence+1]
            ? bb : -1*distance[value.idx_positive_sequence+1];  
  
    int len = -1*distance[value.idx_positive_sequence+1]-static_cast<int>(limit);
    
    if (len > 0) {
      overshoot_length = overshoot_length + penalty_ratio*len;
    }
  }
}


// move 1 : swp a random pair of blocks in the positive sequence
inline std::pair<size_t, size_t> SP::move1() {
  size_t id1 = random_value()%num_blocks;
  size_t id2 = random_value()%num_blocks;

  while (id1 == id2) {
    id2 = random_value()%num_blocks;
  }

  if (id1 > id2) { 
    std::swap(id1, id2);
  }
  
  std::swap(positive_sequence[id1], positive_sequence[id2]);

  positive_sequence[id1]->idx_positive_sequence = id1;
  positive_sequence[id2]->idx_positive_sequence = id2;
 
  return std::make_pair(id1, id2);
}


// move 2 : swap a random pair of blocks in the negative sequence
inline std::pair<size_t, size_t> SP::move2() {
  size_t id1 = random_value()%num_blocks;
  size_t id2 = random_value()%num_blocks;

  while (id1 == id2) {
    id2 = random_value()%num_blocks;
  }

  if (id1 > id2) {
    std::swap(id1, id2);
  }
  
  std::swap(negative_sequence[id1], negative_sequence[id2]);
  
  negative_sequence[id1]->idx_negative_sequence = id1;
  negative_sequence[id2]->idx_negative_sequence = id2;
 
  return std::make_pair(id1, id2);
}


// move 3 : swap a random pair of blocks in both sequences 
inline std::pair<size_t, size_t> SP::move3() {
  size_t id1 = random_value()%num_blocks;
  size_t id2 = random_value()%num_blocks;

  while (id1 == id2) {
    id2 = random_value()%num_blocks;
  }

  if (id1 > id2) {
    std::swap(id1, id2);
  }

  std::swap(positive_sequence[id1], positive_sequence[id2]);
  
  positive_sequence[id1]->idx_positive_sequence = id1;
  positive_sequence[id2]->idx_positive_sequence = id2;
 
  size_t nid1 = positive_sequence[id1]->idx_negative_sequence;
  size_t nid2 = positive_sequence[id2]->idx_negative_sequence;
  
  std::swap(negative_sequence[nid1], negative_sequence[nid2]);
  
  negative_sequence[nid1]->idx_negative_sequence = nid1;
  negative_sequence[nid2]->idx_negative_sequence = nid2;

  return std::make_pair(id1, id2);
}


// move 4 : rotate a randomly selected block
inline std::pair<size_t, size_t> SP::move4() {
  size_t id = random_value()%num_blocks;

  while (positive_sequence[id]->width == positive_sequence[id]->height) {
    id = random_value()%num_blocks;
  }
  
  std::swap(positive_sequence[id]->width,
            positive_sequence[id]->height);

  return std::make_pair(id, id);
}


// backup the data 
inline void SP::initialize_backup_data() {
  for (auto& [key, value] : map_blocks) {
    value.backup_width = value.width;
    value.backup_height = value.height;
    value.backup_lower_left_x = value.lower_left_x;
    value.backup_lower_left_y = value.lower_left_y;
    value.backup_rightof = value.rightof;
    value.backup_aboveof = value.aboveof;
    value.backup_idx_positive_sequence = value.idx_positive_sequence;
    value.backup_idx_negative_sequence = value.idx_negative_sequence;
  }

  backup_bb_width = bb_width;
  backup_bb_height = bb_height;
  backup_hpwl = hpwl;
  backup_positive_sequence = positive_sequence;
  backup_negative_sequence = negative_sequence;
}


// update backup data
// copy modified data to backup data
inline void SP::update_backup_data(
  const size_t id1, const size_t id2, const MoveType move_type) {

  size_t nid1, nid2;
  switch(move_type) {
    case 1:
      std::swap(backup_positive_sequence[id1], backup_positive_sequence[id2]);
      positive_sequence[id1]->backup_idx_positive_sequence = id1;
      positive_sequence[id2]->backup_idx_positive_sequence = id2;
     
      for (size_t i = id1; i <= id2; ++i) {
        positive_sequence[i]->backup_rightof = 
          positive_sequence[i]->rightof;
        positive_sequence[i]->backup_aboveof = 
          positive_sequence[i]->aboveof;
      }

      for (auto& [key, value] : map_blocks) {
        value.backup_lower_left_x = value.lower_left_x;
        value.backup_lower_left_y = value.lower_left_y;
      }

      assert(backup_positive_sequence[id1] == positive_sequence[id1]);
      assert(backup_positive_sequence[id2] == positive_sequence[id2]);
    break;
    
    case 2:
      std::swap(backup_negative_sequence[id1], backup_negative_sequence[id2]);
      negative_sequence[id1]->backup_idx_negative_sequence = id1;
      negative_sequence[id2]->backup_idx_negative_sequence = id2;
      
      for (size_t i = id1; i <= id2; ++i) {
        negative_sequence[i]->backup_rightof = 
          negative_sequence[i]->rightof;
        negative_sequence[i]->backup_aboveof = 
          negative_sequence[i]->aboveof;
      }
      
      for (auto& [key, value] : map_blocks) {
        value.backup_lower_left_x = value.lower_left_x;
        value.backup_lower_left_y = value.lower_left_y;
      }
      assert(backup_negative_sequence[id1] == negative_sequence[id1]);
      assert(backup_negative_sequence[id2] == negative_sequence[id2]);
    break;

    case 3:
      std::swap(backup_positive_sequence[id1], backup_positive_sequence[id2]);
      positive_sequence[id1]->backup_idx_positive_sequence = id1;
      positive_sequence[id2]->backup_idx_positive_sequence = id2;
      
      for (size_t i = id1; i <= id2; ++i) {
        positive_sequence[i]->backup_rightof = 
          positive_sequence[i]->rightof;
        positive_sequence[i]->backup_aboveof =
          positive_sequence[i]->aboveof;
      }

      
      nid1 = positive_sequence[id1]->idx_negative_sequence;
      nid2 = positive_sequence[id2]->idx_negative_sequence;
      std::swap(backup_negative_sequence[nid1], backup_negative_sequence[nid2]);
      negative_sequence[nid1]->backup_idx_negative_sequence = nid1;
      negative_sequence[nid2]->backup_idx_negative_sequence = nid2;
    
      if (nid1 > nid2) {
        std::swap(nid1, nid2);
      }

      for (size_t i = nid1; i <= nid2; ++i) {
        negative_sequence[i]->backup_rightof = 
          negative_sequence[i]->rightof;
        negative_sequence[i]->backup_aboveof = 
          negative_sequence[i]->aboveof;
      }
      
      for (auto& [key, value] : map_blocks) {
        value.backup_lower_left_x = value.lower_left_x;
        value.backup_lower_left_y = value.lower_left_y;
      }
      assert(backup_positive_sequence[id1] == positive_sequence[id1]);
      assert(backup_negative_sequence[id1] == negative_sequence[id1]);
      assert(backup_positive_sequence[id2] == positive_sequence[id2]);
      assert(backup_negative_sequence[id2] == negative_sequence[id2]);
    break;

    case 4:
      positive_sequence[id1]->backup_width = 
        positive_sequence[id1]->width; 
      positive_sequence[id1]->backup_height = 
        positive_sequence[id1]->height; 
      
      for (auto& [key, value] : map_blocks) {
        value.backup_lower_left_x = value.lower_left_x;
        value.backup_lower_left_y = value.lower_left_y;
      }
    break;

    case 0:
      std::cerr << "Nomove does not update backup data\n";
      exit(-1);
    break;
  }

  backup_bb_width  = bb_width;
  backup_bb_height = bb_height;
  backup_hpwl = hpwl;
}


// resume backup data
// resume data from backup data
inline void SP::resume_backup_data(
  const size_t id1, const size_t id2, const MoveType move_type) {

  size_t nid1, nid2;
  switch(move_type) {
    case 1:
      std::swap(positive_sequence[id1], positive_sequence[id2]);

      positive_sequence[id1]->idx_positive_sequence  = 
      positive_sequence[id1]->backup_idx_positive_sequence;

      positive_sequence[id2]->idx_positive_sequence = 
      positive_sequence[id2]->backup_idx_positive_sequence;
      
      for (size_t i = id1; i <= id2; ++i) {
        positive_sequence[i]->rightof = 
        positive_sequence[i]->backup_rightof;
       
        positive_sequence[i]->aboveof = 
        positive_sequence[i]->backup_aboveof;
      }

      for (auto& [key, value] : map_blocks) {
        value.lower_left_x = value.backup_lower_left_x;
        value.lower_left_y = value.backup_lower_left_y;
      }
    break;
    
    case 2:
      std::swap(negative_sequence[id1], negative_sequence[id2]);
     
      negative_sequence[id1]->idx_negative_sequence = 
      negative_sequence[id1]->backup_idx_negative_sequence; 
     
      negative_sequence[id2]->idx_negative_sequence = 
      negative_sequence[id2]->backup_idx_negative_sequence; 
      
      for (size_t i = id1; i <= id2; ++i) {
        negative_sequence[i]->rightof = 
        negative_sequence[i]->backup_rightof;

        negative_sequence[i]->aboveof = 
        negative_sequence[i]->backup_aboveof;
      }
      
      for (auto& [key, value] : map_blocks) {
        value.lower_left_x = value.backup_lower_left_x;
        value.lower_left_y = value.backup_lower_left_y;
      }
    break;

    case 3:
      std::swap(positive_sequence[id1], positive_sequence[id2]);

      positive_sequence[id1]->idx_positive_sequence = 
      positive_sequence[id1]->backup_idx_positive_sequence;
      
      positive_sequence[id2]->idx_positive_sequence = 
      positive_sequence[id2]->backup_idx_positive_sequence;
      
      for (size_t i = id1; i <= id2; ++i) {
        positive_sequence[i]->rightof = 
        positive_sequence[i]->backup_rightof;

        positive_sequence[i]->aboveof = 
        positive_sequence[i]->backup_aboveof;
      }

      nid1 = positive_sequence[id1]->idx_negative_sequence;
      nid2 = positive_sequence[id2]->idx_negative_sequence;
      std::swap(negative_sequence[nid1], negative_sequence[nid2]);
     
      negative_sequence[nid1]->idx_negative_sequence =  
      negative_sequence[nid1]->backup_idx_negative_sequence;

      negative_sequence[nid2]->idx_negative_sequence = 
      negative_sequence[nid2]->backup_idx_negative_sequence;
    
      if (nid1 > nid2) {
        std::swap(nid1, nid2);
      }

      for (size_t i = nid1; i <= nid2; ++i) {
        negative_sequence[i]->rightof = 
        negative_sequence[i]->backup_rightof;

        negative_sequence[i]->aboveof = 
        negative_sequence[i]->backup_aboveof;
      }
      
      for (auto& [key, value] : map_blocks) {
        value.lower_left_x = value.backup_lower_left_x;
        value.lower_left_y = value.backup_lower_left_y;
      }
    break;

    case 4:
      positive_sequence[id1]->width =
      positive_sequence[id1]->backup_width;

      positive_sequence[id1]->height =
      positive_sequence[id1]->backup_height;
      
      for (auto& [key, value] : map_blocks) {
        value.lower_left_x = value.backup_lower_left_x;
        value.lower_left_y = value.backup_lower_left_y;
      }
    break;

    case 0:
      std::cerr << "Nomove does not update backup data\n";
      exit(-1);
    break;
  }

  bb_width  = backup_bb_width;
  bb_height = backup_bb_height;
  hpwl = backup_hpwl;
}


// pack SP
inline double SP::pack() {
  overshoot_length = 0;
  std::vector<int> distance = spfa(Orientation::Horizontal);
  
  compute_block_locations(distance, Orientation::Horizontal);
  compute_area(distance, bb_width, Orientation::Horizontal);
  
  
  distance = spfa(Orientation::Vertical);
  
  compute_block_locations(distance, Orientation::Vertical);
  compute_area(distance, bb_height, Orientation::Vertical); 
  
  hpwl = compute_hpwl()/2;
  
  average_area = 
    (average_area*(num_iterations-1) + bb_width*bb_height)/static_cast<double>(num_iterations);
  
  average_length = 
    (average_length*(num_iterations-1) + hpwl)/static_cast<double>(num_iterations); 
  

  return (alpha*bb_width*bb_height/average_area)+(1-alpha)*(hpwl+overshoot_length)/average_length;
}


// run SP with SA
inline void SP::run() {
  bool pass = false;
  double current_temperature = initial_temperature;
  
  // first run
  ++num_iterations;
  construct_relative_locations(0, num_blocks-1, MoveType::nomove);
  double backup_cost = pack();
  double cost = backup_cost;
 
  // backup data  
  initialize_backup_data(); 
  
  // SA 
  double delta = 0.0;
  size_t rv;
  std::pair<size_t, size_t> pair_idx;
  MoveType move_type;
 
  do { 
    while (current_temperature > frozen_temperature) {
      for (size_t ite = 0; ite < iterations_per_temperature; ++ite) {

        rv = random_value()%4;
        // neighborhood from four moves   
        switch (rv) {
          // a new neighbor from move1
          case 0:
            //std::cout << "iteration " << ite << ", temperature : " << current_temperature << ", move 1 chosen" << '\n';
            pair_idx = move1();
            move_type = MoveType::move1;
          break;

          // a new neighbor from move2 
          case 1:
            //std::cout << "iteration " << ite << ", temperature : " << current_temperature << ", move 2 chosen" << '\n';
            pair_idx = move2();
            move_type = MoveType::move2;
          break;
      
          // a new neighbor from move3
          case 2:
            //std::cout << "iteration " << ite << ", temperature : " << current_temperature << ", move 3 chosen" << '\n';
            pair_idx = move3();
            move_type = MoveType::move3;
          break;

          // a new neighbor from move4
          case 3:
            //std::cout << "iteration " << ite << ", temperature : " << current_temperature << ", move 4 chosen" << '\n';
            pair_idx = move4();
            move_type = MoveType::move4;
          break;
        }

        construct_relative_locations(pair_idx.first, pair_idx.second, move_type);
        ++num_iterations;
        cost = pack(); 
        delta = (cost - backup_cost) * 1000;
        
        // better neighbor, always accept it
        if (delta < 0) {
          update_backup_data(pair_idx.first, pair_idx.second, move_type);
          backup_cost = cost;
        }

        // worse neighbor
        else {
          auto prob = std::exp(-delta/current_temperature);

          // accept the worse neighbor
          if (prob > static_cast<double>(random_value())/max_dist) { 
            update_backup_data(pair_idx.first, pair_idx.second, move_type);  
            backup_cost = cost;
          }

          // decline the worse neighbor
          else {
            average_area = (average_area*num_iterations-bb_width*bb_height)/(num_iterations-1);
            average_length = (average_length*num_iterations-hpwl)/(num_iterations-1);
            --num_iterations;

            resume_backup_data(pair_idx.first, pair_idx.second, move_type);
          }
        }
      }

      current_temperature *= decay;
    }

    if (outline_width >= outline_height) {
      if (bb_width >= bb_height) {
        if (outline_width >= bb_width && outline_height >= bb_height) {
          pass = true; 
        }
      }
      else {
        if (outline_width >= bb_height && outline_height >= bb_width) {
          pass = true;
        }
      }
    }

    else {
      if (bb_width >= bb_height) {
        if (outline_width >= bb_height && outline_height >= bb_width) {
          pass = true;
        }
      }
      else {
        if (outline_width >= bb_width && outline_height >= bb_height) {
          pass = true;
        }
      }
    }
    if (pass == false) {
      current_temperature = initial_temperature;
      penalty_ratio *= 2;
    }
  }
  while(pass == false);

  visualize();
}


// dump SP data structure
inline void SP::dump(std::ostream& os) const {
  os << "outline width = " << outline_width 
     << ", outline height = " << outline_height
     << ", num_blocks = " << num_blocks
     << ", num_terminals = " << num_terminals << '\n';

  for (auto& [key, value] : map_blocks) {
    assert(key == value.name);
    os << "Block " << value.name
       << " has width : " << value.width
       << ", height : "   << value.height
       << ", lf.x : "     << value.lower_left_x
       << ", lf.y : "     << value.lower_left_y
       << ", ur.x : "     << (value.lower_left_x + value.width)
       << ", ur.y : "     << (value.lower_left_y + value.height)
       << '\n';
  }
  
  for (auto& [key, value] : map_terminals) {
    assert(key == value.name);
    os << "Terminal " << value.name
       << " at x : "  << value.pos_x
       << ", y : "    << value.pos_y
       << '\n';
  }

  for (auto& vec : vec_nets) {
    os << "Net : ";
    for (auto& v : vec.net) {
      if (std::holds_alternative<Block*>(v)) {
        os << (std::get<Block*>(v))->name    << "(b) ";
      }
      else {
        os << (std::get<Terminal*>(v))->name << "(t) ";
      }
    }
    os << '\n';
  }

  os << "Positive sequence : ";
  for (auto& ps : positive_sequence) {
    os << ps->name << ' '; 
  }
  os << '\n';
  
  os << "Negative sequence : ";
  for (auto& ns : negative_sequence) {
    os << ns->name << ' '; 
  }
  os << '\n';

  os << "Relative Locations : \n";
  for (auto& [key, value] : map_blocks) {
    os << key << ".rightof = [";
    for (auto& v : value.rightof) {
      os << v->name << ", ";
    }
    os << "]\n";
  }
  
  for (auto& [key, value] : map_blocks) {
    os << key << ".aboveof = [";
    for (auto& v : value.aboveof) {
      os << v->name << ", ";
    }
    os << "]\n";
  }

  os << "Area = " << bb_width 
     << " * "     << bb_height
     << " = "     << bb_width * bb_height << '\n'; 

  visualize();
}


inline void SP::visualize() const {
  std::cout << "{\"block_number\":" << num_blocks << ",";
  std::cout << "\"llx\":0,\"lly\":0,"
            << "\"urx\":" << bb_width  << ","
            << "\"ury\":" << bb_height << ","
            << "\"area\":" << bb_width*bb_height << ","
            << "\"coordinates\":[";
  
  size_t blks = 0;
  for (auto& [key, value] : map_blocks) {
    std::cout << "{\"idx\":\"" << key << "\","
              << "\"llx\":" << value.lower_left_x << ","
              << "\"lly\":" << value.lower_left_y << ","
              << "\"width\":" << value.width << ","
              << "\"height\":" << value.height << "}";
    if (blks++ < num_blocks-1) {
      std::cout << ",";
    }
  }
  std::cout << "]}" << '\n';
}


// dump backup SP data structure
inline void SP::dump_backup(std::ostream& os) const {
  os << "outline width = " << outline_width 
     << ", outline height = " << outline_height
     << ", num_blocks = " << num_blocks
     << ", num_terminals = " << num_terminals << '\n';

  for (auto& [key, value] : map_blocks) {
    assert(key == value.name);
    os << "Block " << value.name
       << " has width : " << value.backup_width
       << ", height : "   << value.backup_height
       << ", lf.x : "     << value.backup_lower_left_x
       << ", lf.y : "     << value.backup_lower_left_y
       << ", ur.x : "     << (value.backup_lower_left_x + value.backup_width)
       << ", ur.y : "     << (value.backup_lower_left_y + value.backup_height)
       << '\n';
    assert(value.backup_width == value.width);
    assert(value.backup_height == value.height);
    assert(value.backup_lower_left_x == value.lower_left_x);
    assert(value.backup_lower_left_y == value.lower_left_y);
    assert(value.backup_idx_positive_sequence == value.idx_positive_sequence);
    assert(value.backup_idx_negative_sequence == value.idx_negative_sequence);
  }

  assert(positive_sequence == backup_positive_sequence);
  os << "Backup positive sequence : ";
  for (auto& ps : backup_positive_sequence) {
    os << ps->name << ' '; 
  }
  os << '\n';
  
  assert(negative_sequence == backup_negative_sequence);
  os << "Backup negative sequence : ";
  for (auto& ns : backup_negative_sequence) {
    os << ns->name << ' '; 
  }
  os << '\n';

  os << "Backup relative Locations : \n";
  for (auto& [key, value] : map_blocks) {
    os << key << ".backup_rightof = [";
    assert(value.backup_rightof == value.rightof);
    for (auto& v : value.backup_rightof) {
      os << v->name << ", ";
    }
    os << "]\n";
  }
  
  for (auto& [key, value] : map_blocks) {
    os << key << ".backup_aboveof = [";
    assert(value.backup_aboveof == value.aboveof);
    for (auto& v : value.backup_aboveof) {
      os << v->name << ", ";
    }
    os << "]\n";
  }

  os << "Area = " << backup_bb_width 
     << " * "     << backup_bb_height
     << " = "     << backup_bb_width * backup_bb_height << '\n'; 
  assert(backup_bb_width == bb_width);
  assert(backup_bb_height == bb_height);
  //visualize();
}


inline void SP::dump_solution() const {
  std::ofstream outFile(output_path, std::ios::out);

  if (!outFile) {
    std::cerr << "Output file could not be opened or does not exist\n";
    exit(1);
  }

  outFile << (hpwl + bb_width*bb_height) << '\n';
  outFile << hpwl << '\n';
  outFile << bb_width * bb_height << '\n';
  outFile << bb_width << ' ' << bb_height << '\n';
  outFile << 10 << '\n';
  
  if ( ((outline_width >= outline_height) && (bb_width >= bb_height)) ||
       ((outline_width <= outline_height) && (bb_width <= bb_height)) ) {
    for (auto& [key, value] : map_blocks) {
      outFile << key << ' '
              << value.lower_left_x << ' '
              << value.lower_left_y << ' '
              << value.lower_left_x + value.width  << ' '
              << value.lower_left_y + value.height << '\n';   
    }
  }
 
  else { 
    for (auto& [key, value] : map_blocks) {
      outFile << key << ' '
              << value.lower_left_y << ' '
              << value.lower_left_x << ' '
              << value.lower_left_y + value.height << ' '
              << value.lower_left_x + value.width  << '\n';   
    }
  }
}










} // end of namespace fp
