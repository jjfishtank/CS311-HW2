#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <random>
#include <stack>

using namespace std;

const int MAX_SYMBOLS = 1e5;

class Language {
public:
  Language();
  static void initializeMaps();
  string generateCode();

private:
  static unordered_map<string, vector<string>> grammarMap;
  static unordered_map<string, vector<double>> weightMap;

  string generateCodeHelper(const string& nonTerminal);
  vector<string> tokenize(const string&);
};

unordered_map<string, vector<string>> Language::grammarMap;
unordered_map<string, vector<double>> Language::weightMap;

string Language::generateCode() {
  string start = "<prog>";
  return generateCodeHelper(start);
}

string Language::generateCodeHelper(const string& start) {
  string code;
  stack<string> symbols;
  int symbolCount = 1;
  symbols.push(start);

  while (!symbols.empty()) {
    string symbol = symbols.top();
    symbols.pop();
    symbolCount++;

    // prevent endless generation due to bad weights
    if (symbolCount > MAX_SYMBOLS) { 
      code += "\n// Symbol limit exceeded, terminating generation...";
      break;
    }

    if (grammarMap.contains(symbol)) { // non-terminal symbol
      // Choose a random production based on weights
      const auto& productions = grammarMap[symbol];
      const auto& weights = weightMap[symbol];
      random_device rand;
      mt19937 gen(rand());
      discrete_distribution<> dis(weights.begin(), weights.end());
      const string& production = productions[dis(gen)];

      // Push symbols of the chosen production onto the symbol stack in reverse order
      vector<string> newSymbols = tokenize(production);
      for (auto it = newSymbols.rbegin(); it != newSymbols.rend(); ++it) {
        symbols.push(*it);
      }
    } else { // terminal symbol
      code += symbol;
    }
  }

  return code;
}

// Parses production string for terminal and non-terminal symbols.
// Symbols delineated by '<' & '>' (Preserves whitespace.)
vector<string> Language::tokenize(const string& str) {
  vector<string> symbols;
  string symbol;
  for (char c : str) {
    if (c == '<') {
      if (!symbol.empty()) {
        symbols.push_back(symbol);
        symbol.clear();
      }
    } else if (c == '>') {
      symbol += c;
      symbols.push_back(symbol);
      symbol.clear();
      continue;
    }
    symbol += c;
  }
  if (!symbol.empty()) {
    symbols.push_back(symbol);
  }

  return symbols;
}

// Hardcoded grammar and production weight maps
void Language::initializeMaps() {
  grammarMap["<prog>"] = { "int main()\n{\n<stat_list>return 0;\n}" };
  weightMap["<prog>"] = { 1.0 };

  grammarMap["<stat_list>"] = { "<stat>", "<stat_list><stat>" };
  weightMap["<stat_list>"] = { 0.30, 0.70 };

  grammarMap["<stat>"] = { 
    "<cmpd_stat>", 
    "<if_stat>", 
    "<iter_stat>", 
    "<assgn_stat>", 
    "<decl_stat>" 
  };
  weightMap["<stat>"] = { 0.1, 0.1, 0.1, 0.35, 0.35 };

  grammarMap["<cmpd_stat>"] = { "{\n<stat_list>}\n" };
  weightMap["<cmpd_stat>"] = { 1.0 };

  grammarMap["<if_stat>"] = { 
    "if (<exp>)\n<stat>", 
    "if (<exp>)\n<cmpd_stat>", 
    "if (<exp>)\n<stat>else <stat>", 
    "if (<exp>)\n<cmpd_stat>else <stat>", 
    "if (<exp>)\n<stat>else <cmpd_stat>", 
    "if (<exp>)\n<cmpd_stat>else <cmpd_stat>" 
  };
  weightMap["<if_stat>"] = { 0.25, 0.15, 0.2, 0.15, 0.15, 0.1 };

  grammarMap["<iter_stat>"] = { 
    "while (<exp>)\n{\n<stat>}\n", 
    "while (<exp>)\n{\n<cmpd_stat>}\n" };
  weightMap["<iter_stat>"] = { 2.0 / 3.0, 1.0 / 3.0 };

  grammarMap["<assgn_stat>"] = { "<id> = <exp>;\n" };
  weightMap["<assgn_stat>"] = { 1.0 };

  grammarMap["<decl_stat>"] = { "<type> <id>;\n", "<type> <assgn_stat>" };
  weightMap["<decl_stat>"] = { 0.5, 0.5 };

  grammarMap["<exp>"] = { "<exp> <op> <exp>", "<id>", "<const>" };
  weightMap["<exp>"] = { 1.0 / 3.0, 1.0 / 3.0, 1.0 / 3.0 };

  grammarMap["<op>"] = { "+", "-", "*", "/" };
  weightMap["<op>"] = { 0.25, 0.25, 0.25, 0.25 };

  grammarMap["<type>"] = { "int", "double" };
  weightMap["<type>"] = { 0.5, 0.5 };

  grammarMap["<id>"] = { "<char><char_digit_seq>" };
  weightMap["<id>"] = { 1.0 };

  grammarMap["<const>"] = { "<digit><digit_seq>" };
  weightMap["<const>"] = { 1.0 };

  grammarMap["<char_digit_seq>"] = { 
    "", 
    "<char><char_digit_seq>", 
    "<digit><char_digit_seq>" 
  };
  weightMap["<char_digit_seq>"] = { 1.0 / 3.0, 1.0 / 3.0, 1.0 / 3.0 };

  grammarMap["<digit_seq>"] = { "", "<digit><digit_seq>" };
  weightMap["<digit_seq>"] = { 0.5, 0.5};

  grammarMap["<char>"] = { 
    "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n","o", 
    "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "A", "B", "C", "D", 
    "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", 
    "T", "U", "V", "W", "X", "Y", "Z", "_" 
  };
  weightMap["<char>"] = vector<double>(53, 1.0 / 53.0);

  grammarMap["<digit>"] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };
  weightMap["<digit>"] = vector<double>(10, 1.0 / 10.0);
}

Language::Language() {
  
}