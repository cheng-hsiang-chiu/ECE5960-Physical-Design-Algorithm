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



namespace fp {



class Block {
public:
  size_t lower_left_x = 0;
  size_t lower_left_y = 0;
  size_t upper_right_x = 0;
  size_t upper_right_y = 0;
  size_t width = 0;
  size_t height = 0;
  std::string name;

  std::vector<Block*> rightof;
  std::vector<Block*> aboveof;

  int idx_positive_sequence = -1;
  int idx_negative_sequence = -1;
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





class SP {
public:
  std::unordered_map<std::string, Block> map_blocks;
  std::unordered_map<std::string, Terminal> map_terminals;
  std::vector<Net> vec_nets;

  std::vector<Block*> positive_sequence;
  std::vector<Block*> negative_sequence;

  size_t outline_width;
  size_t outline_height;
  size_t num_blocks;
  size_t num_terminals;
  size_t num_nets;
  
  double alpha = 0;

  // parameters for Simulated Annealing
  const double initial_temperature = 100.0;

  const double frozen_temperature = 0.1;

  const double decay = 0.85; 
    
  const size_t iterations_per_temperature = 1000;
  
  SP() = default;

  SP(double, const std::string&, const std::string&, const std::string&);

  void dump(std::ostream&) const;

  void initialize_sequence();

  void move1(const Sequence);

  void move2();

  void run();

  void construct_relative_locations();
  
  void compute_area();

  void spfa();
};


inline SP::SP(double a, const std::string& input_block_path, 
              const std::string& input_net_path,
              const std::string& output_path) {
  
  std::srand(std::time(nullptr)); 
  
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
  //construct_relative_locations();

  dump(std::cout);
}


// initialize positive and negative sequences
inline void SP::initialize_sequence() {
  positive_sequence.resize(num_blocks); 
  negative_sequence.resize(num_blocks);
  
  size_t idx = std::rand()%num_blocks; 
  for (auto& [key, value] : map_blocks) {
    while (positive_sequence[idx] != nullptr) {
      idx = std::rand()%num_blocks;
    }
    positive_sequence[idx] = &(map_blocks[key]);
    map_blocks[key].idx_positive_sequence = idx;

    idx = std::rand()%num_blocks;
    while (negative_sequence[idx] != nullptr) {
      idx = std::rand()%num_blocks;
    }
    negative_sequence[idx] = &(map_blocks[key]);
    map_blocks[key].idx_negative_sequence = idx;
  }
}


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
}


// compute area
inline void SP::compute_area() {
  
}


// move 1 : swap a random pair of blocks in the positive sequence or negative sequence or both
inline void SP::move1(const Sequence sequence) {
  size_t id1 = std::rand()%num_blocks;
  size_t id2 = std::rand()%num_blocks;

  while (id1 == id2) {
    id2 = std::rand()%num_blocks;
  }

  switch (sequence) {
    case Sequence::POS:
      std::swap(positive_sequence[id1], positive_sequence[id2]);
    break;

    case Sequence::NEG:
      std::swap(negative_sequence[id1], negative_sequence[id2]);
    break;

    case Sequence::BOTH:
      std::swap(positive_sequence[id1], positive_sequence[id2]);
      
      auto itr_neg_1 = std::find(
        negative_sequence.begin(), 
        negative_sequence.end(), 
        positive_sequence[id1]);
      auto itr_neg_2 = std::find(
        negative_sequence.begin(), 
        negative_sequence.end(), 
        positive_sequence[id2]);   
      
      std::swap(*itr_neg_1, *itr_neg_2);
    break;
  }
}


// move 2 : rotate a randomly selected block
inline void SP::move2() {
  size_t id = std::rand()%num_blocks;

  switch(std::rand()%2) {
    case 0:
      std::swap(positive_sequence[id]->width, 
                positive_sequence[id]->height);
    break;

    case 1:
      std::swap(negative_sequence[id]->width, 
                negative_sequence[id]->height);
    break;
  }
}


// run SP
inline void SP::run() {
  double current_temperature = initial_temperature;
  
  while (current_temperature <= frozen_temperature) {
    for (size_t ite = 0; ite < iterations_per_temperature; ++ite) {

    }

    current_temperature *= decay;
  }
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
       << ", ur.x : "     << value.upper_right_x
       << ", ur.y : "     << value.upper_right_y
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
}











} // end of namespace fp
