#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <cstdlib>
#include "graph.hpp"
#include <set>
#include <map>
#include <ctime>

/*
void parsing(std::string input_file, double& r_factor, hypergraph& graph) {
  std::ifstream inClientFile(input_file, std::ios::in);

  // exit program if ifstream could not open file
  if (!inClientFile) {
    std::cerr << "File could not be opened or does not exist\n";
    exit(1);
  }

  std::string line;
  std::getline(inClientFile, line);
  r_factor = std::stof(line);
  std::cout << "r factor = " << r_factor << '\n';
  
  std::string net;

  while(std::getline(inClientFile, line)) {
    std::stringstream ss(line);
    while (std::getline(ss, line, ' ')) {
      if (line != ";" && line != "NET") {
        if (line[0] == 'n') {
          net = line;
          graph.data[net] = std::vector<cell>();
        }
        else {
          cell c;
          c.name = line;
          c.partition = std::rand()%2;
          graph.data[net].emplace_back(c);
        }
        std::cout << line << '\n';
      }
    }
  }
}
*/

int main(int argc, char** argv) {

  if (argc != 3) {
    std::cout << "------Wrong input------\n";
    std::cout << "./fm input_file output_file\n";
  }

  std::srand(std::time(nullptr));

  std::string input_file(argv[1]);

  Hypergraph hypergraph(input_file);

  std::cout << "r factor = " << hypergraph.r_factor << '\n';

  std::cout << "num of nets = " << hypergraph.num_nets() << '\n';

  std::cout << "num of cells = " << hypergraph.num_cells() << '\n';
   
  //hypergraph.traverse();

  std::cout << hypergraph.area_lower_bound 
            << " <= area <= " 
            << hypergraph.area_upper_bound << '\n';

  std::cout << "hypergraph.max_gain = " << hypergraph.max_gain << '\n';
  hypergraph.display_partition();

  hypergraph.display_cut();
  
  hypergraph.display_gain();
   
  hypergraph.display_connected_cells();

  hypergraph.display_bucket();

  hypergraph.run_fm();

  return 0;
}
