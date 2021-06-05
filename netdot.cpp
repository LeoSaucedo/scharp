/**
 * `dotnet` command passthrough.
 * Carlos Saucedo, 2021
 **/

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <unistd.h>

using namespace std;
ifstream ifs;
ofstream ofs;
stringstream buffer;

/**
 * Executes a command and captures the output.
 * @return the stdout of the command.
 **/
string exec(const char *cmd)
{
  array<char, 128> buffer;
  string result;
  unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
  if (!pipe)
  {
    throw runtime_error("popen() failed!");
  }
  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
  {
    result += buffer.data();
  }
  return result;
}

/**
 * Swaps all c's for s's and all `dotnet`s with `netdot`s.
 * @return the string with the swap.
 **/
string swap(string s)
{
  replace(s.begin(), s.end(), 's', '\v');
  replace(s.begin(), s.end(), 'c', 's');
  replace(s.begin(), s.end(), '\v', 'c');
  replace(s.begin(), s.end(), 'S', '\v');
  replace(s.begin(), s.end(), 'C', 'S');
  replace(s.begin(), s.end(), '\v', 'C');
  while (s.find("dotnet") != string::npos)
    s.replace(s.find("dotnet"), 6, "netdot");
  return s;
}

int main(int argc, char *argv[])
{

  // Build the arguments.
  string args = "";
  for (int i = 1; i < argc; i++)
  {
    args += argv[i];
    args += " ";
  }
  string command = "dotnet " + args;
  if (command.find("dotnet run") != string::npos)
  {
    // Replace Program.sc with Program.cs
    ifs.open("Program.sc");
    buffer.str("");
    buffer << ifs.rdbuf();
    ifs.close();
    // remove("Program.sc");
    ofs.open("Program.cs", ofstream::out | ofstream::trunc);
    ofs << swap(buffer.str());
    ofs.close();

    // Replace .scproj with .csproj
    char cwd[4096];
    getcwd(cwd, sizeof(cwd));
    string path(cwd);
    string base_filename = path.substr(path.find_last_of("/\\") + 1);
    base_filename += ".scproj";
    ifs.open(base_filename.c_str());
    buffer.str("");
    buffer << ifs.rdbuf();
    ifs.close();
    // remove(base_filename.c_str());
    string new_filename = path.substr(path.find_last_of("/\\") + 1);
    new_filename += ".csproj";
    ofs.open(new_filename.c_str());
    ofs << swap(buffer.str());
    ofs.close();
  }
  string output = exec(command.c_str());
  cout << swap(output) << endl;
  if (command.find("dotnet run") != string::npos)
  {
    char cwd[4096];
    getcwd(cwd, sizeof(cwd));
    string path(cwd);
    string new_filename = path.substr(path.find_last_of("/\\") + 1);
    new_filename += ".csproj";
    remove(new_filename.c_str());
    remove("Program.cs");
  }
  // Capture creation of a console app.
  if (command.find("dotnet new console -o") != string::npos)
  {
    // Replacing csproj with scproj.
    string csprojfile = argv[4];
    csprojfile += "/";
    csprojfile += argv[4];
    csprojfile += ".csproj";

    ifs.open(csprojfile.c_str());
    buffer.str("");
    buffer << ifs.rdbuf();
    ifs.close();
    remove(csprojfile.c_str());

    string scprojname = argv[4];
    scprojname += "/";
    scprojname += argv[4];
    scprojname += ".scproj";
    ofs.open(scprojname.c_str(), ofstream::out | ofstream::trunc);
    ofs << swap(buffer.str());
    ofs.close();

    // Replacing Program.cs with Program.sc
    string cspath = argv[4];
    cspath += "/Program.cs";

    ifs.open(cspath.c_str());
    buffer.str("");
    buffer << ifs.rdbuf();
    ifs.close();
    remove(cspath.c_str());

    ofstream csfile;
    string scpath = argv[4];
    scpath += "/Program.sc";
    csfile.open(scpath.c_str(), ofstream::out | ofstream::trunc);
    csfile << swap(buffer.str());
    csfile.close();
  }
}