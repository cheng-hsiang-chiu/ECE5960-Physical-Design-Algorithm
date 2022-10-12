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

  //std::vector<double> allcost;


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
  
  SP() = default;

  SP(double, const std::string&, const std::string&, const std::string&);

  void dump(std::ostream&) const;

  void dump_backup(std::ostream&) const;

  void visualize() const;

  void initialize_sequence();

  //void move1(const Sequence);

  std::pair<size_t, size_t> move1();
  
  std::pair<size_t, size_t> move2();

  std::pair<size_t, size_t> move3();
  
  std::pair<size_t, size_t> move4();

  void run();

  double pack();

  void construct_relative_locations(const size_t, const size_t, const MoveType);
  
  std::vector<int> spfa(const Orientation);
  
  void compute_block_locations(std::vector<int>&, const Orientation);

  void compute_area(const std::vector<int>&, int&);

  int compute_hpwl() const;

  void initialize_backup_data();

  void update_backup_data(const size_t, const size_t, const MoveType);

  void resume_backup_data(const size_t, const size_t, const MoveType);
};


inline SP::SP(double a, const std::string& input_block_path, 
              const std::string& input_net_path,
              const std::string& output_path) {

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

  // construct relative locations of blocks
  construct_relative_locations(0, num_blocks-1, MoveType::nomove);

  //dump(std::cout);
}


// initialize positive and negative sequences
inline void SP::initialize_sequence() {
  positive_sequence.resize(num_blocks); 
  negative_sequence.resize(num_blocks);
  
  //size_t idx = std::rand()%num_blocks; 
  size_t idx = random_value()%num_blocks;

  for (auto& [key, value] : map_blocks) {
    while (positive_sequence[idx] != nullptr) {
      //idx = std::rand()%num_blocks;
      idx = random_value()%num_blocks;
    }
    positive_sequence[idx] = &(map_blocks[key]);
    map_blocks[key].idx_positive_sequence = idx;

    //idx = std::rand()%num_blocks;
    idx = random_value()%num_blocks;
    while (negative_sequence[idx] != nullptr) {
      //idx = std::rand()%num_blocks;
      idx = random_value()%num_blocks;
    }
    negative_sequence[idx] = &(map_blocks[key]);
    map_blocks[key].idx_negative_sequence = idx;
  }
}


// construct relative locations of blocks between [id1, id2]
// id1 and id2 are index of positive sequence
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
  //std::for_each(affected.begin(), affected.end(), [&pidx, &nidx](Block* blk) {
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
    if (move_type != 0) {
      blk->rightof.push_back(&terminus);
    }
  }

  // construct aboveof vector
  for (auto& blk : affected) {
    blk->aboveof.clear();
    pidx = static_cast<int>(blk->idx_positive_sequence) - 1; 
    nidx = blk->idx_negative_sequence;

    while (pidx >= 0) {
      Block* target = positive_sequence[pidx];
      if (target->idx_negative_sequence > nidx) {
        blk->aboveof.push_back(target);
      }
      --pidx;
    }
    if (move_type != 0) {
      blk->aboveof.push_back(&terminus);
    }
  }
  
  //int pidx, nidx;

  //// construct rightof vector
  //for (size_t i = id1; i <= id2; ++i) {
  //  positive_sequence[i]->rightof.clear();
  //  pidx = i+1;
  //  nidx = positive_sequence[i]->idx_negative_sequence;
  //  while (pidx <= id2) {
  //    Block* target = positive_sequence[pidx];
  //    if (target->idx_negative_sequence > nidx) {
  //      positive_sequence[i]->rightof.push_back(target);  
  //    }
  //    ++pidx;
  //  }
  //}

  //// construct aboveof vector
  //for (size_t i = id1; i <= id2; ++i) {
  //  positive_sequence[i]->aboveof.clear();
  //  pidx = static_cast<int>(i)-1;
  //  nidx = positive_sequence[i]->idx_negative_sequence; 

  //  while (pidx >= 0)  {
  //    Block* target = positive_sequence[pidx];
  //    if (target->idx_negative_sequence > nidx) {
  //      positive_sequence[i]->aboveof.push_back(target);
  //    }
  //    --pidx;
  //  }
  //}

  // rightof and aboveof regarding source and 
  // terminus blocks have been inserted
  if (move_type != 0) {
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


/*
// construct relative locations of blocks
inline void SP::construct_relative_locations() {
  int pidx, nidx;

  // construct rightof vector
  for (auto& ps : positive_sequence) {
    pidx = ps->idx_positive_sequence+1;
    nidx = ps->idx_negative_sequence;      
    //std::cout << "pidx = " << pidx << ", nidx = " << nidx << '\n';
    while (pidx < num_blocks) {
      Block* target = positive_sequence[pidx];
      if (target->idx_negative_sequence > nidx) {
        ps->rightof.push_back(target);
      }
      ++pidx;
    }
  }

  // construct aboveof vector
  for (auto& ps : positive_sequence) {
    pidx = ps->idx_positive_sequence-1;
    nidx = ps->idx_negative_sequence;

    while (pidx >= 0) {
      Block* target = positive_sequence[pidx];
      if (target->idx_negative_sequence > nidx) {
        ps->aboveof.push_back(target);
      }
      --pidx;
    }
  }
  // construct relative locations for source & terminus blocks
  for (auto& [key, value] : map_blocks) {
    source.rightof.push_back(&value);
    source.aboveof.push_back(&value);
    value.rightof.push_back(&terminus);
    value.aboveof.push_back(&terminus);
  }
}
*/


inline std::vector<int> SP::spfa(const Orientation orientation) {
  std::vector<int> distance(num_blocks+2);
  std::fill_n(distance.begin(), num_blocks+2, INT_MAX);
  distance[0] = 0;
  
  std::deque<Block*> Q;

  Q.push_back(&source);

  while (!Q.empty()) {
    Block* u = Q.front();
    Q.pop_front();
  
    std::vector<Block*> vec; 
    if(orientation == Orientation::Horizontal) {
      vec = u->rightof;
    }
    else {
      vec = u->aboveof;
    }

    size_t indexu = 0, indexv = 0;
    for (auto& blk : vec) {
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

        if (std::find(Q.begin(), Q.end(), blk) == Q.end()) {
          Q.push_back(blk);
        }
      }
    }
  }
  return distance;
}


// compute HPWL
inline int SP::compute_hpwl() const {
  int hpwl = 0;
  for (auto& net : vec_nets) {
    std::vector<Terminal*> terminals;

    int max_x = 0, min_x = INT_MAX, max_y = 0, min_y = INT_MAX;
    int llx = 0, lly = 0;
    for (auto& n : net.net) {
      // terminal in a net
      if (std::holds_alternative<Terminal*>(n)) {
        terminals.push_back(std::get<Terminal*>(n));
      }
      // blocks in a net
      else {
        llx = std::get<Block*>(n)->lower_left_x + std::get<Block*>(n)->width/2;
        lly = std::get<Block*>(n)->lower_left_y + std::get<Block*>(n)->height/2; 
        if (llx > max_x) {
          max_x = llx;    
        }
        if (llx < min_x) {
          min_x = llx;
        }
        if (lly > max_y) {
          max_y = lly;
        }
        if (lly < min_y) {
          min_y = lly;
        }
      }
    }

    hpwl = hpwl + (max_x-min_x) + (max_y-min_y);
    
    // length to terminals
    int center_x = (max_x + min_x)/2;
    int center_y = (max_y + min_y)/2;
    for (auto& t : terminals) {
      hpwl = hpwl + 
             std::abs(center_x-static_cast<int>(t->pos_x)) + 
             std::abs(center_y-static_cast<int>(t->pos_y));  
    }
  }
  
  return hpwl;
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
inline void SP::compute_area(const std::vector<int>& distance, int& bb) {
  bb = INT_MIN;
  for (auto& [key, value] : map_blocks) {
    bb = bb > -1*distance[value.idx_positive_sequence+1]
            ? bb : -1*distance[value.idx_positive_sequence+1];  
    
    //bb_width = bb_width > -1*distance[value.idx_positive_sequence+1]
    //           ? bb_width : -1*distance[value.idx_positive_sequence+1];  
    //bb_height = bb_height > -1*distance[value.idx_positive_sequence+1] 
    //           ? bb_height : -1*distance[value.idx_positive_sequence+1];  
  }
  //std::cout << "bb = " << bb << '\n';
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
  //size_t id = std::rand()%num_blocks;
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
     
      //assert(positive_sequence[id1]->idx_positive_sequence == positive_sequence[id1]->backup_positive_sequence);
      //assert(positive_sequence[id2]->idx_positive_sequence == positive_sequence[id2]->backup_positive_sequence);
      
      for (size_t i = id1; i <= id2; ++i) {
        backup_positive_sequence[i]->backup_rightof = 
          positive_sequence[i]->rightof;
        backup_positive_sequence[i]->backup_aboveof = 
          positive_sequence[i]->aboveof;
        //backup_positive_sequence[i]->backup_lower_left_x =
        //  positive_sequence[i]->lower_left_x;
        //backup_positive_sequence[i]->backup_lower_left_y =
        //  positive_sequence[i]->lower_left_y;
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
        backup_negative_sequence[i]->backup_rightof = 
          negative_sequence[i]->rightof;
        backup_negative_sequence[i]->backup_aboveof = 
          negative_sequence[i]->aboveof;
        //backup_negative_sequence[i]->backup_lower_left_x =
        //  negative_sequence[i]->lower_left_x;
        //backup_negative_sequence[i]->backup_lower_left_y =
        //  negative_sequence[i]->lower_left_y;
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
        backup_positive_sequence[i]->backup_rightof = 
          positive_sequence[i]->rightof;
        backup_positive_sequence[i]->backup_aboveof =
          positive_sequence[i]->aboveof;
        //backup_positive_sequence[i]->backup_lower_left_x =
        //  positive_sequence[i]->lower_left_x;
        //backup_positive_sequence[i]->backup_lower_left_y =
        //  positive_sequence[i]->lower_left_y;
      }

      
      nid1 = backup_positive_sequence[id1]->idx_negative_sequence;
      nid2 = backup_positive_sequence[id2]->idx_negative_sequence;
      std::swap(backup_negative_sequence[nid1], backup_negative_sequence[nid2]);
      backup_negative_sequence[nid1]->backup_idx_negative_sequence = nid1;
      backup_negative_sequence[nid2]->backup_idx_negative_sequence = nid2;
    
      if (nid1 > nid2) {
        std::swap(nid1, nid2);
      }

      for (size_t i = nid1; i <= nid2; ++i) {
        backup_negative_sequence[i]->backup_rightof = 
          negative_sequence[i]->rightof;
        backup_negative_sequence[i]->backup_aboveof = 
          negative_sequence[i]->aboveof;
        //backup_negative_sequence[i]->backup_lower_left_x =
        //  negative_sequence[i]->lower_left_x;
        //backup_negative_sequence[i]->backup_lower_left_y =
        //  negative_sequence[i]->lower_left_y;
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
      backup_positive_sequence[id1]->backup_width = 
        positive_sequence[id1]->width; 
      backup_positive_sequence[id1]->backup_height = 
        positive_sequence[id1]->height; 
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
         
        //positive_sequence[i]->lower_left_x = 
        //positive_sequence[i]->backup_lower_left_x;

        //positive_sequence[i]->lower_left_y = 
        //positive_sequence[i]->backup_lower_left_y;
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
     
        //negative_sequence[i]->lower_left_x = 
        //negative_sequence[i]->backup_lower_left_x; 
      
        //negative_sequence[i]->lower_left_y = 
        //negative_sequence[i]->backup_lower_left_y; 
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
        
        //positive_sequence[i]->lower_left_x = 
        //positive_sequence[i]->backup_lower_left_x;
        //
        //positive_sequence[i]->lower_left_y = 
        //positive_sequence[i]->backup_lower_left_y;
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

        //negative_sequence[i]->lower_left_x = 
        //negative_sequence[i]->backup_lower_left_x;

        //negative_sequence[i]->lower_left_y = 
        //negative_sequence[i]->backup_lower_left_y;
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
  std::vector<int> distance = spfa(Orientation::Horizontal);
  
  compute_block_locations(distance, Orientation::Horizontal);
  //std::cout << "after first compute_block_locations-------------\n";
  //dump(std::cout); 
  
  compute_area(distance, bb_width);
  
  //std::cout << "after first compute_area-------------\n";
  //dump(std::cout); 
  
  distance = spfa(Orientation::Vertical);
  //std::cout << "after second spfa-------------\n";
  //dump(std::cout); 
  compute_block_locations(distance, Orientation::Vertical);
  //std::cout << "after second compute_block_locations-------------\n";
  //dump(std::cout); 
  compute_area(distance, bb_height); 
  //std::cout << "after second compute_area-------------\n";
  //dump(std::cout); 
  
  hpwl = compute_hpwl();
  //std::cout << "after compute_hpwl-------------\n";
  //dump(std::cout); 
  
  average_area = 
    (average_area*(num_iterations-1) + bb_width*bb_height)/static_cast<double>(num_iterations);
  
  average_length = 
    (average_length*(num_iterations-1) + hpwl)/static_cast<double>(num_iterations); 
  //std::cout << "hpwl = " << hpwl << '\n';
  //std::cout << alpha*bb_width*bb_height + (1-alpha) * hpwl << '\n'; 
  

  return (alpha*bb_width*bb_height/average_area)+(1-alpha)*hpwl/average_length;
  //return alpha*bb_width*bb_height + (1-alpha) * hpwl; 
  //return alpha*bb_width*bb_height; 
}


// run SP with SA
inline void SP::run() {
  double current_temperature = initial_temperature;
  //std::cout << "before first pack------\n";
  //dump(std::cout); 
  // first run
  ++num_iterations;
  double backup_cost = pack();
  double cost = backup_cost;
 
  //allcost.push_back(cost); 
  // backup data  
  //std::cout << "before initialize_backup_data\n\n";
  //dump(std::cout); 
  initialize_backup_data(); 
  //std::cout << "after initialize_backup_data\n\n";
  //dump(std::cout); 
  //dump_backup(std::cout);
  
  //std::cout << "what the hell\n\n\n\n";
  // SA 
  double delta = 0.0;
  size_t rv;
  std::pair<size_t, size_t> pair_idx;
  MoveType move_type;
  
  while (current_temperature > frozen_temperature) {
    for (size_t ite = 0; ite < iterations_per_temperature; ++ite) {
      if ((outline_width >= bb_width && outline_height >= bb_height) ||
          (outline_width >= bb_height && outline_height >= bb_width)) {
        std::cout << "Found\n";
        dump(std::cout); 
        return;  
      }

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
      delta = (cost - backup_cost)*1000;
      //std::cout << "cost = " << cost << ", delta = " << delta << '\n';
      //allcost.push_back(cost);
      // better neighbor, always accept it
      if (delta < 0) {
        update_backup_data(pair_idx.first, pair_idx.second, move_type);
        backup_cost = cost;
        //allcost.push_back(cost);
        //std::cout << "\n\n\naccept\n\n\n";
        //dump(std::cout); 
        //dump_backup(std::cout);
        //return;
      }

      // worse neighbor
      else {
        auto prob = std::exp(-delta/current_temperature);

        // accept the worse neighbor
        if (prob > static_cast<double>(random_value())/max_dist) { 
          update_backup_data(pair_idx.first, pair_idx.second, move_type);  
          backup_cost = cost;
          //allcost.push_back(cost);
          //std::cout << "accept\n";
          //dump(std::cout); 
          //dump_backup(std::cout);
          //return;
        }

        // decline the worse neighbor
        else {
          average_area = (average_area*num_iterations-bb_width*bb_height)/(num_iterations-1);
          average_length = (average_length*num_iterations-hpwl)/(num_iterations-1);
          --num_iterations;

          resume_backup_data(pair_idx.first, pair_idx.second, move_type);
          //std::cout << "deny\n";
          //dump(std::cout); 
          //dump_backup(std::cout);
          //return;
        }
      }
    }

    current_temperature *= decay;
  }

  visualize();

  //std::ofstream outFile("./allcost", std::ios::out);

  //for (auto& ac : allcost) {
  //  outFile << ac << '\n';
  //}
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
  
  //for (auto& [key, value] : map_terminals) {
  //  assert(key == value.name);
  //  os << "Terminal " << value.name
  //     << " at x : "  << value.pos_x
  //     << ", y : "    << value.pos_y
  //     << '\n';
  //}

  //for (auto& vec : vec_nets) {
  //  os << "Net : ";
  //  for (auto& v : vec.net) {
  //    if (std::holds_alternative<Block*>(v)) {
  //      os << (std::get<Block*>(v))->name    << "(b) ";
  //    }
  //    else {
  //      os << (std::get<Terminal*>(v))->name << "(t) ";
  //    }
  //  }
  //  os << '\n';
  //}

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





} // end of namespace fp
