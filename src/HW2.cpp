#include "HW2.h"
#include <fstream>

int main() {
  ofstream file("random code.txt");
  Language::initializeMaps();
  Language myLanguage;
  file << myLanguage.generateCode();
  file.close();
  return 0;
}