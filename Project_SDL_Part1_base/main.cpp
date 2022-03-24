#include "Project_SDL1.h"
#include <stdio.h>
#include <string>

int main(int argc, char* argv[]) {

  std::cout << "Starting up the application" << std::endl;

  if (argc != 4)
    throw std::runtime_error("Need three arguments - "
                             "number of sheep, number of wolves, "
                             "simulation time\n");

  init();

  std::cout << "Done with initilization" << std::endl;

  application my_app(std::stoul(argv[1]), std::stoul(argv[2]));
  
  std::cout << "Created window" << std::endl;
  std::cout << "" << std::endl;
  std::cout << "Bienvenue dans sheep game " << std::endl;
  std::cout << "Le but est simple survivre en essayant" << std::endl;
  std::cout << "de sauver le maximum de mouton..." << std::endl;
  std::cout << "Bonne chance !" << std::endl;

  int retval = my_app.loop(std::stoul(argv[3]));
  std::cout << "Exiting application with code " << retval << std::endl;

  return retval;
}