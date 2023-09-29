/*
 * Author: Jeremy Renati
 * Date: 09/28/2023
 * Writes code generated from Language object to "out/<executable dir>/random code.txt"
 */
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