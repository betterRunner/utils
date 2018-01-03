#include <stdio.h>
#include <iostream>
#include <fstream>

#include "al_kmeans.hpp"

int read_array(const std::string &file_name, std::vector<int> &out)
{
  std::ifstream in(file_name.c_str(), std::ios::in);
  if(!in.is_open())
  {
    return -1;
  }
  int m=0;
  while(!in.eof())
  {
    int tmp;
    in>>tmp;
    out.push_back(tmp);
//    char tmp[256];
//    in.getline(tmp, 256);
    m++;
  }
  in.close();

  return 0;
}

int main()
{
  int tmp;
  std::vector<int> arr;

  read_array("box_00000010.txt", arr);
//  std::vector<int> arr(10);
//  for(int m=0; m<10; m++)
//  {
//    arr[m] = m;
//  }
  al_kmeans::run_kmeans(arr, 2);

  getchar();

  return 0;
}
