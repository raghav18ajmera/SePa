#include <bits/stdc++.h>
#include <fstream>
using namespace std;

int get_row(long long int num)
{
    int row=0;
    while(num%2==0)
    {
        num=num/2;
        row++;
    }
    return row;
}



int main(){
    
    int m=10; //number of elements (m=5 is for testing)
    long long int n=200; //number of subsets (n=10 is for testing)
    long long int cost_range = 200; // cost of each subset lies in [0,cost_range] 
    long long int total_subsets = 1;
    for(int i = 1 ; i <= m ; i++)
    {
        total_subsets=total_subsets*2;
    } 
    // total_subsets = 2**m
    
    map<long long int,long long int> subset_cost; // assume this comes from the file random_sepa_cpp (that generates random set partition problem)
    string line;
    ifstream myfile ("random_subsets.txt");
    for(int i =1 ;i<=n;i++)
    {
        getline(myfile,line);
        long long int key, value;
        sscanf(line.c_str(), "%lld %lld", &key, &value);
        subset_cost[key]=value;
    }
    myfile.close();
    
    
    
    
    
   
    
    
    
    vector<vector<long long int>> v(m+2);
    long long int arr[m+2];
    for(int i=0;i<=m+1;i++)
    {
        arr[i]=0;
    }

    for(auto i1:subset_cost)
    {
        v[get_row(i1.first)].push_back(i1.first);
    }
    long long int partial_sol=0,partial_cost=0,best_sol=-1;
    float best_cost=numeric_limits<float>::infinity();
    vector<long long int> best_sol_vec;
    vector<long long int> subsets_in_partial_sol;
    while(true)
    {
        long long int list_no,dummy; 
        list_no=0;
        dummy=partial_sol;
        while(dummy%2==1)
        {
            list_no++;
            dummy=dummy/2;
        }
        long long int find_subset_index=-1;
        for(long long int i=arr[list_no];i<v[list_no].size();i++)
        {
            
            if(partial_cost+subset_cost[v[list_no][i]]<best_cost&&((partial_sol&v[list_no][i])==0))
            {
                find_subset_index=i;
                break;
            }
        }
        if(find_subset_index==-1)
        {
            arr[list_no]=0;
            if(subsets_in_partial_sol.size()==0)
            {
                break; //terminate
            }
            long long int subset_to_be_removed = subsets_in_partial_sol[subsets_in_partial_sol.size()-1];
            subsets_in_partial_sol.pop_back();
            partial_sol=partial_sol-subset_to_be_removed;
            partial_cost=partial_cost-subset_cost[subset_to_be_removed];
        }
        else
        {
            arr[list_no]++; 
            long long int subset_to_be_added = v[list_no][find_subset_index];
            subsets_in_partial_sol.push_back(subset_to_be_added);
            partial_sol=partial_sol+subset_to_be_added;
            partial_cost=partial_cost+subset_cost[subset_to_be_added];
            if(partial_sol==total_subsets-1)
            {
                best_cost=partial_cost;
                best_sol=1;
                best_sol_vec=subsets_in_partial_sol;
            }
            
        }
    }
    if(best_sol==-1)
    {
        cout<<"no solution found\n";
    }
    else
    {
        cout<<"minimum cost : "<<best_cost<<"\n";
        //cout<<"corresponding subsets : ";
        //for(int i=0;i<best_sol_vec.size();i++)
        //{
        //    cout<<best_sol_vec[i]<<" ";
        //}
        //cout<<"\n";
    }



    return 0;
}