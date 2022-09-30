#pragma once

#include <vector>
#include <variant>
#include <fstream>
#include <regex>
#include <unordered_map>
#include <cassert>

namespace sp {



class Block {
public:
  size_t lower_left_x = 0;
  size_t lower_left_y = 0;
  size_t upper_right_x = 0;
  size_t upper_right_y = 0;
  size_t width = 0;
  size_t height = 0;
  std::string name;
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




class SP {
public:
  std::unordered_map<std::string, Block> map_blocks;
  std::unordered_map<std::string, Terminal> map_terminals;
  std::vector<Net> vec_nets;

  size_t outline_width;
  size_t outline_height;
  size_t num_blocks;
  size_t num_terminals;
  size_t num_nets;
  double alpha = 0;

  SP() = default;

  SP(double, const std::string&, const std::string&, const std::string&);

  void dump(std::ostream&) const;

};



inline SP::SP(double a, const std::string& input_block_path, 
              const std::string& input_net_path, const std::string& output_path) {
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


  dump(std::cout);
}


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
}











} // end of namespace sp
