#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <vector>



inline void initialize(std::string input_file, std::vector<cell*>& bucket) {
  // ifstream constructor opens the file
  std::ifstream inClientFile(input_file, std::ios::in);

  // exit program if ifstream could not open file
  if (!inClientFile) {
    std::cerr << "File could not be opened or does not exist\n";
    exit(1);
  }

  std::string line;
  std::getline(inClientFile, line);
  std::cout << "r factor = " << line << '\n';
  while(std::getline(inClientFile, line)) {
    std::stringstream ss(line);
    while (std::getline(ss, line, ' ')) {
      if (line != ";" && line != "NET") {
        std::cout << line << '\n';
      }
    }
  }
  
}

