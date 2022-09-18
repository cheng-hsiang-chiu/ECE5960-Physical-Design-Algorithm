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

int main(int argc, char** argv) {

  if (argc != 3) {
    std::cout << "------Wrong input------\n";
    std::cout << "./fm input_file output_file\n";
  }

  std::srand(std::time(nullptr));

  std::string input_file(argv[1]);
  
  std::string output_file(argv[2]);

  Hypergraph hypergraph(input_file, output_file);

  std::cout << "  r factor = " << hypergraph.r_factor << '\n';

  std::cout << "  num of nets = " << hypergraph.num_nets() << '\n';

  std::cout << "  num of cells = " << hypergraph.num_cells() << '\n';

  std::cout << "  " 
            << hypergraph.area_lower_bound 
            << " <= area <= " 
            << hypergraph.area_upper_bound << '\n';

  std::cout << "  max_gain = " << hypergraph.max_gain << '\n';
  
  std::cout << "  min_gain = " << hypergraph.min_gain << '\n';
  
  hypergraph.run_fm();

  hypergraph.output_answer();

  return 0;
}
