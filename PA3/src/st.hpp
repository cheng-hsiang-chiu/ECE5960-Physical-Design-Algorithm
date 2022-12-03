#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <regex>
#include <climits>
#include <cstdlib>
#include <cstdio>

namespace st {


struct Coordinate { 
  size_t x = 0;
  size_t y = 0;

  Coordinate() = default;

  Coordinate(const size_t xx, const size_t yy):
    x{xx}, y{yy} {}
};


struct Pin {
  std::string name;
  Coordinate coordinate;  

  Pin() = default;

  Pin(const std::string n, const Coordinate c): name{n}, coordinate{c}{}
};


class ST {

public:
  ST(const std::string&, const std::string&);

  std::string output_path;

  std::vector<Coordinate> boundary;
  
  //std::unordered_map<int, Pin> pins;
  
  std::vector<Pin> pins;

  std::vector<int> parent;

  size_t number_pins = 0;

  void run();

  void dump_solution();

  void dump() const;
private:

  int _minKey(std::vector<int>&, std::vector<bool>&);
  
  int _length(const int, const int);
};


inline ST::ST(const std::string& in_path, const std::string& out_path) {
  output_path = out_path;
  
  std::ifstream inFile(in_path, std::ios::in);
  if (!inFile) {
    std::cerr << "File could not be opended or does not exist\n";
    exit(1);
  }

  std::string line;
  std::regex reg("(\\d+)");

  /**********
   * parsing
   * ********/

  // parse the boundary
  std::getline(inFile, line);
  auto itr = std::sregex_iterator(line.begin(), line.end(), reg);
  boundary.emplace_back(stoi((*itr++).str()), stoi((*itr++).str()));
  boundary.emplace_back(stoi((*itr++).str()), stoi((*itr).str()));
  //std::cout << "Boundary = (" 
  //          << boundary[0].x << ","
  //          << boundary[0].y << ") ("
  //          << boundary[1].x << ","
  //          << boundary[1].y << ")\n";
 
  // parse the number of pins 
  std::getline(inFile, line);
  itr = std::sregex_iterator(line.begin(), line.end(), reg);
  number_pins = stoi((*itr).str()); 
  std::cout << number_pins << '\n';
  
  pins.resize(number_pins);

  // parse the pins
  std::regex reg1("\\s+");
  std::string name;
  int index = 0;
  size_t x, y;
  //std::stringstream sstream;
  for (size_t n = 0; n < number_pins; ++n) {
    std::getline(inFile, line);
    
    auto it = std::sregex_token_iterator(line.begin(), line.end(), reg1, -1);
    it++;
    name = (*it).str();
    it++;
    std::string newstr((*it).str());
    //itr = std::sregex_iterator((*it).str().begin(), (*it).str().end(), reg);
    //std::cout << (*itr).str()<<'\n';
    
    std::sscanf(newstr.c_str(), "(%zu,%zu)", &x, &y);

    //std::sscanf((itr++)->c_str(), "%zu", &x);
    //std::sscanf((itr)->c_str(), "%zu", &y);
    //sstream = (*itr++).str();
    //sstream >> x;
    //sstream = (*itr).str();
    //sstream >> y;
    //x = stoll((*itr++).str());
    //y = stoll((*itr).str());
    //std::cout << name << ' ' << x << ' ' << y << '\n';
    pins[index++] = Pin{name, Coordinate{x, y}};
  }

  parent.resize(number_pins);
  //for(const auto& [key, value] : pins) {
  //  std::cout << key << " " << value.coordinate.x << " " << value.coordinate.y << '\n';
  //}

  sort(pins.begin(),
       pins.end(),
       [](auto& pina, auto& pinb){
         return
           (pina.coordinate.x+pina.coordinate.y) < (pinb.coordinate.x+pinb.coordinate.y);
       }
  );


  //dump();
  //run();
}


inline void ST::dump() const {
  std::cout << "Boundary = (" 
            << boundary[0].x << ","
            << boundary[0].y << ") ("
            << boundary[1].x << ","
            << boundary[1].y << ")\n";

  std::cout << "Number of pins :" << number_pins << '\n';
  
  for(const auto& pin : pins) {
    std::cout << "Pin[" << pin.name << "] at (" << pin.coordinate.x << "," << pin.coordinate.y << ")\n";
  }
}


inline int ST::_minKey(std::vector<int>& key, std::vector<bool>& intree) {
  int min = INT_MAX, min_index;

  for (int v = 0; v < number_pins; ++v) {
    if (intree[v] == false && key[v] < min) {
      min = key[v];
      min_index = v;
    }
  }
  return min_index;
}


inline void ST::run() {
  std::vector<int> key(number_pins, INT_MAX);
  std::vector<bool> intree(number_pins, false);
  
  // always start with the first element in pins
  key[0] = 0;
  parent[0] = -1;

  // The Stenir tree will have number_pins vertices
  // and number_pins-1 edges
  for (int count = 0; count < number_pins-1; ++count) {
    // pick the minimum key vertex from the
    // set of vertices not yet included in the tree
    //std::cout << "count = " << count << '\n';
    int u = _minKey(key, intree);

    // add u in the tree
    intree[u] = true;

    for (int v = 0; v < number_pins; ++v) {
      if (intree[v] == false) {
        int len = _length(u, v);
        if (len < key[v]) {
          parent[v] = u;
          key[v] = len;
        }
      }
    }
  }
}

inline int ST::_length(const int u, const int v) {
  auto& pin_u = pins[u];
  auto& pin_v = pins[v];
  
  return (std::abs(static_cast<int>(pin_u.coordinate.x - pin_v.coordinate.x)) + 
          std::abs(static_cast<int>(pin_u.coordinate.y - pin_v.coordinate.y)));     
}

inline void ST::dump_solution() {
  for (int i = 1; i < number_pins; ++i) {
    std::cout << pins[parent[i]].name << " links to " << pins[i].name
              << " with length = " << _length(parent[i], i)
              << '\n';
  }

}




} // end of namespace st
