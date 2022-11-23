#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <regex>


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
  
  std::unordered_map<std::string, Pin> pins;
  size_t number_pins = 0;

  void run();

  void dump_solution() const;

  void dump() const;
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
  //std::cout << number_pins << '\n';

  // parse the pins
  std::regex reg1("\\s+");
  std::string name;
  size_t x, y;
  for (size_t n = 0; n < number_pins; ++n) {
    std::getline(inFile, line);
    auto it = std::sregex_token_iterator(line.begin(), line.end(), reg1, -1);
    it++;
    name = (*it).str();
    it++;
    itr = std::sregex_iterator((*it).str().begin(), (*it).str().end(), reg);
    x = stoi((*itr++).str());
    y = stoi((*itr).str());
    pins[name] = Pin{name, Coordinate{x, y}};
  }
  //for(const auto& [key, value] : pins) {
  //  std::cout << key << " " << value.coordinate.x << " " << value.coordinate.y << '\n';
  //}

  dump();
}


inline void ST::dump() const {
  std::cout << "Boundary = (" 
            << boundary[0].x << ","
            << boundary[0].y << ") ("
            << boundary[1].x << ","
            << boundary[1].y << ")\n";

  std::cout << "Number of pins :" << number_pins << '\n';
  
  for(const auto& [key, value] : pins) {
    std::cout << "Pin[" << key << "] at (" << value.coordinate.x << "," << value.coordinate.y << ")\n";
  }
}

inline void ST::run() {}

inline void ST::dump_solution() const {}




} // end of namespace st
