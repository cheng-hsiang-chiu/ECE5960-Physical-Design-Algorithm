#include <iostream>
#include "st.hpp"




int main(int argc, char** argv) {

  if (argc != 3) {
    std::cerr << "Wrong commands\n";
    exit(-1);
  }

  std::string input_path(argv[1]);
  std::string output_path(argv[2]);

  st::ST st(input_path, output_path);
  st.run();
  st.dump_solution();

  return 0;

}
