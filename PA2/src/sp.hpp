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


namespace fp {

// random generator
static size_t random_value() {
  static std::minstd_rand engine{std::random_device{}()};
  static std::uniform_int_distribution<size_t> distribution;
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
 
  // source and terminus blocks for horizontal/vertical constraint graph
  Block source;
  Block terminus;
 
  // bounding box width and height and its backups
  int bb_width = 0;
  int bb_height = 0;
  int backup_bb_width = 0;
  int backup_bb_height = 0; 
  


  SP() = default;

  SP(double, const std::string&, const std::string&, const std::string&);

  void dump(std::ostream&) const;

  void visualize() const;

  void initialize_sequence();

  void move1(const Sequence);

  void move2();

  void run();

  double run_one();

  void construct_relative_locations();
  
  std::vector<int> spfa(const Orientation);
  
  void compute_block_locations(std::vector<int>&, const Orientation);

  void compute_area(const std::vector<int>&);

  int compute_hpwl() const;

  void initialize_backup_data();
};


inline SP::SP(double a, const std::string& input_block_path, 
              const std::string& input_net_path,
              const std::string& output_path) {

  source.name = "source";
  terminus.name = "terminus";

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
  construct_relative_locations();

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


inline std::vector<int> SP::spfa(const Orientation orientation) {
  std::vector<int> distance(num_blocks+2);
  std::fill_n(distance.begin(), num_blocks+2, INT_MAX);
  distance[0] = 0;
  
  std::deque<Block*> Q;

  Q.push_back(&source);

  while (!Q.empty()) {
    Block* u = Q.front();
    Q.pop_front();
   
    std::vector<Block*>& vec = u->rightof; 
    if (orientation == Orientation::Vertical) {
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
// and overall width and height
inline void SP::compute_block_locations(std::vector<int>& distance,
  const Orientation orientation) {
  
  switch (orientation) {
    // horizontal
    case 0:
      for (auto& [key, value] : map_blocks) {
        value.lower_left_x = 
          -1*distance[value.idx_positive_sequence+1] - value.width;

        //bb_width = bb_width > -1*distance[value.idx_positive_sequence+1]
        //           ? bb_width : -1*distance[value.idx_positive_sequence+1];  
      }
    break;

    // vertical
    case 1:
      for (auto& [key, value] : map_blocks) {
        value.lower_left_y = 
          -1*distance[value.idx_positive_sequence+1] - value.height;
        
        //bb_height = bb_height > -1*distance[value.idx_positive_sequence+1] 
        //            ? bb_height : -1*distance[value.idx_positive_sequence+1];  
      }
    break;
  }
}


// compute the bounding box area 
inline void SP::compute_area(const std::vector<int>& distance) {
  for (auto& [key, value] : map_blocks) {

    bb_width = bb_width > -1*distance[value.idx_positive_sequence+1]
               ? bb_width : -1*distance[value.idx_positive_sequence+1];  
    bb_height = bb_height > -1*distance[value.idx_positive_sequence+1] 
               ? bb_height : -1*distance[value.idx_positive_sequence+1];  
  }
}


// move 1 : swap a random pair of blocks in the positive sequence
// or negative sequence or both
inline void SP::move1(const Sequence sequence) {

  //size_t id1 = std::rand()%num_blocks;
  //size_t id2 = std::rand()%num_blocks;
  size_t id1 = random_value()%num_blocks;
  size_t id2 = random_value()%num_blocks;

  while (id1 == id2) {
    //id2 = std::rand()%num_blocks;
    id2 = random_value()%num_blocks;
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
     
      std::swap(negative_sequence[positive_sequence[id1]->idx_negative_sequence],
                negative_sequence[positive_sequence[id2]->idx_negative_sequence]); 
      //auto itr_neg_1 = std::find(
      //  negative_sequence.begin(), 
      //  negative_sequence.end(), 
      //  positive_sequence[id1]);
      //auto itr_neg_2 = std::find(
      //  negative_sequence.begin(), 
      //  negative_sequence.end(), 
      //  positive_sequence[id2]);   
      //
      //std::swap(*itr_neg_1, *itr_neg_2);
    break;
  }
}


// move 2 : rotate a randomly selected block
inline void SP::move2() {
  //size_t id = std::rand()%num_blocks;
  size_t id = random_value()%num_blocks;

  //switch (std::rand()%2) {
  switch (random_value()%2) {
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
}


// run SP one time
inline double SP::run_one() {
  std::vector<int> distance = spfa(Orientation::Horizontal);
  compute_block_locations(distance, Orientation::Horizontal);
  distance = spfa(Orientation::Vertical);
  compute_block_locations(distance, Orientation::Vertical);
  
  compute_area(distance); 
  
  int hpwl = compute_hpwl();
  //dump(std::cout); 
  //std::cout << "hpwl = " << hpwl << '\n';
  
  return alpha*bb_width*bb_height + (1-alpha) * hpwl; 
}



// run SP with SA
inline void SP::run() {
  double current_temperature = initial_temperature;
 
  // first run
  double backup_cost = run_one();
  double cost = backup_cost;
  
  // backup data  
  std::vector<Block*> backup_positive_sequence = positive_sequence;
  std::vector<Block*> backup_negative_sequence = negative_sequence;
  initialize_backup_data(); 
   
  std::uniform_int_distribution<> distr(0, 1); 
  // SA 
   
  while (current_temperature > frozen_temperature) {
    for (size_t ite = 0; ite < iterations_per_temperature; ++ite) {
      
      switch (random_value()%2) {
        // a new neighbor from move1
        case 0:
          // move1();
          std::cout << "iteration " << ite << ", temperature : " << current_temperature << ", move 1 chosen" << '\n';
        break;
        // a new neighbor from move2 
        case 1:
          //move2();
          std::cout << "iteration " << ite << ", temperature : " << current_temperature << ", move 2 chosen" << '\n';
        break;
      }
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








} // end of namespace fp
