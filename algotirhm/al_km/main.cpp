#include <iostream>
#include <cstring>
#include <cstdio>

#include "al_km.hpp"

using namespace std;

int main()
{
		vector<double> v1 = {1,2,3};
		vector<double> v2 = {1,3,2};
		vector<double> v3 = {3,2,1};
		vector<vector<double>> vv;
		vv.push_back(v1);
		vv.push_back(v2);
		vv.push_back(v3);
		al::al_km_t<double> al_km(vv);
		vector<int> v_match = al_km.run();
    for(int m=0; m<v_match.size(); m++)
		{
			printf("t:%d match d:%d\t", m, v_match[m]);
		}

    return 0;
}

