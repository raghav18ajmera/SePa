#include <bits/stdc++.h>
using namespace std;
 
int main() {
 
    long long int m; // the number of elements in the set ( must be less than 63 )
    long long int n; // number of subsets to be generated ( 0 <= n <= 2**m )
    long long int cost_range; // cost of each subset lies in [0,cost_range]  
    long long int total_subsets = 1;
    for(int i = 1 ; i <= m ; i++)
    {
        total_subsets=total_subsets*2;
    } 
    // total_subsets = 2**m

    map<long long int,long long int> subsets ; // set containing random subsets
    while( subsets.size() < n )
    {
        // create random subset by choosing a random integer between [0,(2**m)-1]
        long long int random_subset = rand()%total_subsets;
        // check if the created subset is already present as a key in the map of subsets
        if( subsets.find(random_subset)==subsets.end())
        {
            // random_subset is not present, hence we can add it into our map as a key
            subsets[random_subset]=rand()%(cost_range+1);
        }
        else
        {
            random_subset++;
            random_subset=random_subset%total_subsets;
            while(subsets.find(random_subset)!=subsets.end())
            {
                random_subset++; // increment if key is found in the map
                random_subset=random_subset%total_subsets;
            }
            // now we can add it into our map as a key
            subsets[random_subset]=rand()%(cost_range+1);
        }
    }




    return 0;
}