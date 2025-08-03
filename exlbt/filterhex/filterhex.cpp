// clang++ -std=c++11 -g -o filterhex filterhex.cpp

#include <string>
#include <fstream>
#include <iostream>

#define DEBUG  0

using namespace std;

static void reportCommandUsage() {
  cout << "Usage:\n"
         << "  filterhex, or"
         << "  filterhex < input.s\n";
  cout.flush();
} 

// Return true if `Line` consists of hexadecimal numbers seperated by spaces.
//   true:  if all c in `Line` are [0..0,A..F,a..f,' ']
//   false: others
static bool checkInput(std::string &Line) {
  size_t Size = 8;
  size_t Length = 0;
  for (char c : Line) {
    if (c >= '0' && c <= '9') {
      Length++;
      continue;
    } else if (c >= 'A' && c <= 'F') {
      Length++;
      continue;
    } else if (c >= 'a' && c <= 'f') {
      Length++;
      continue; 
    } else if (c == ' ') {
      continue;
    } else {
    #if DEBUG
      cout << "error: input must in [0..9, A..F, a..f, ' '], Hex: "
           << Line << "\n";
    #endif
      return false;
    }
  }
  char Msg[128];
  snprintf(Msg, 127, "error: The length of input must be %d and seperated with "
               "blank, input: ", (int)Size);
  if (Length != Size) {
  #if DEBUG
    cout << Msg << Line << "\n";
    cout << "Example: 09 20 00 00\n";
  #endif
    return false;
  }

  return true;
}

bool getHex(string &In, string &Out) {
  size_t Begin = 0;
  size_t End = 0;

  if (In[0] == ';') {
    // skip comment.
    Out = "";
    return false;
  }
  if (In[0] != ' ') {
    // Assembly file outputed from llvm-objdump must begin with ' '.
    Out = "";
    return false;
  }
  Begin = In.find(": ");
  if (Begin == string::npos) {
    Out = "";
    return false;
  }
  End = In.find("  ", Begin + 1);
  if (End < Begin) {
    Out = "";
    return false;
  }
  size_t Length = End - Begin - 1;
  Out = In.substr(Begin+1, Length);

  return true;
}

void filterHex(istream &Is, ostream &Os) {
  string InLine;
  string OutLine;

#if DEBUG
  cerr << "\ninput: ";
#endif
  while (getline(Is, InLine)) {
    bool GetHex = getHex(InLine, OutLine);
#if DEBUG
    cerr << "InLine: " << InLine << "\n";
    cerr << "GetHex: " << GetHex << "\n";
#endif
    if (GetHex) {
      bool CheckInput = checkInput(OutLine);
    #if DEBUG
      cerr << "OutLine: " << OutLine << "\n";
      cerr << "CheckInput: " << CheckInput << "\n";
    #endif
      if (CheckInput) {
        Os << OutLine << "\n";
      }
    } // GetHex
  } // while
}

// Usage:
//   "filterhex" or
//   "filterhex < input.s"
int main (int argc, char **argv) {
  if (argc > 1) {
    reportCommandUsage();
  }

  filterHex(cin, cout);

  return 0;
}

