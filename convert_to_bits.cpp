#include <bits/stdc++.h>
#include <fstream>
using namespace std;

int main()
{
    long long int m; //number of elements 
    long long int n; //number of subsets
    long long int cost_range; // cost of each subset lies in [0,cost_range] 
    
   
    map<long long int,long long int> subset_cost; 
    string line;
    ifstream myfile ("random_subsets.txt");
    ofstream MyFile("random_subsets_bits.txt");
    getline(myfile,line);
    sscanf(line.c_str(), "%lld %lld %lld", &m,&n,&cost_range); 
    MyFile << m << " " << n << " " << cost_range << "\n";
    
    //  reading cost of each subset from file
    for(int i =1 ;i<=n;i++)
    {
        getline(myfile,line);
        long long int key, value;
        sscanf(line.c_str(), "%lld %lld", &key, &value);
        vector<int> dummy;
        long long int x=key;
        if(x==0){dummy.push_back(0);}
        while(x>0)
        {
            dummy.push_back(x%2);
            x=x/2;
        }
        while(dummy.size())
        {
            MyFile<<dummy[dummy.size()-1];
            dummy.pop_back();
        }
        MyFile<<"\n"<<value<<"\n";
        subset_cost[key]=value;
    }
    myfile.close();
    MyFile.close();
    return 0;
}