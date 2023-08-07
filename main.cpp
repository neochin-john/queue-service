#include "Cmd.h"
#include <iostream>
#include <list>
#include <sstream>
using namespace std;

list<string> ParseCmd(string cmdLine) {
  list<string> cmds;
  istringstream ss(cmdLine);
  string part;
  while (ss >> part)
    cmds.push_back(part);

  return cmds;
}

int main() {
  cout << "欢迎使用排队系统" << endl;
  string cmdLine;
  while (!cin.eof()) {
    cout << ">";
    getline(cin, cmdLine);
    if (cmdLine == "") {
      continue;
    }

    list<string> cmds = ParseCmd(cmdLine);
    Cmd::Instance()->Exec(cmds);
  }
}
