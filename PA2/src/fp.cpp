#include <iostream>
#include "sp.hpp"
#include <string>
#include <cstdlib>


int main(int argc, char** argv) {

  if (argc != 5) {
    std::cerr << "Wrong commands\n";
    exit(-1);
  }

  double alpha = std::atof(argv[1]);
  std::string input_block_path(argv[2]);
  std::string input_net_path(argv[3]);
  std::string output_path(argv[4]);

  fp::SP sp(alpha, input_block_path, input_net_path, output_path);
  sp.run();
  sp.dump_solution();
  return 0;
}
