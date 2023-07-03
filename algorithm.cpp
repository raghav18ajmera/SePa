#include <fstream>
#include <map>
#include <unordered_map>
#include <iostream>
#include <utility>
#include <cmath>
#include <set>
#include <vector>
#include <chrono>
#include <algorithm>
#include <cstdio>
#include <string> 
#include <bitset>

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


int get_row_bitset(bitset<200> b) 
{
    for(int i=0;i<200;i++)
    {
        if(b.test(i)){return i;}
    }
}

unordered_map<bitset<200>, long long int> read_from_bits_file(long long int *ptr_m,long long int *ptr_n,long long int *ptr_cost_range, long long int *ptr_total_subsets,string file_name)
{
    long long int m; //number of elements 
    long long int n; //number of subsets
    long long int cost_range; // cost of each subset lies in [0,cost_range] 
    long long int total_subsets = 1;
    unordered_map<bitset<200>,long long int> subset_cost; 
    string line;
    ifstream myfile (file_name);
    getline(myfile,line);
    sscanf(line.c_str(), "%lld %lld", &m,&n);

    for(int i = 1 ; i <= m ; i++)
    {
        total_subsets=total_subsets*2;
    } 
    // total_subsets = 2**m

    //  reading cost of each subset from file
    for(int i =1 ;i<=n;i++)
    {
        getline(myfile,line);
        string key=line;
        getline(myfile,line);
        long long int value;
        sscanf(line.c_str(), "%lld", &value);
        bitset<200> dummy_bitset(0);
        for(int i=key.length()-1;i>=0;i--)
        {
            int j=key.length()-i-1;
            if(key[i]=='1')
            {
                dummy_bitset.set(j);
            }
        }
        subset_cost[dummy_bitset]=value;
    }
    getline(myfile,line);
    long long int answer_by_scip;
     sscanf(line.c_str(), "%lld", &answer_by_scip);
    myfile.close();

    *ptr_m=m;
    *ptr_n=n;
    *ptr_cost_range=cost_range;
    *ptr_total_subsets=total_subsets;
    

    return subset_cost;

}

unordered_map<long long int,long long int> read_from_file(long long int *ptr_m,long long int *ptr_n,long long int *ptr_cost_range, long long int *ptr_total_subsets)
{
    long long int m; //number of elements 
    long long int n; //number of subsets
    long long int cost_range; // cost of each subset lies in [0,cost_range] 
    long long int total_subsets = 1;
   
    unordered_map<long long int,long long int> subset_cost; 
    string line;
    ifstream myfile ("random_subsets.txt");
    getline(myfile,line);
    sscanf(line.c_str(), "%lld %lld", &m,&n); // get vakue of m
    
    for(int i = 1 ; i <= m ; i++)
    {
        total_subsets=total_subsets*2;
    } 
    // total_subsets = 2**m

    //  reading cost of each subset from file
    for(int i =1 ;i<=n;i++)
    {
        getline(myfile,line);
        long long int key, value;
        sscanf(line.c_str(), "%lld %lld", &key, &value);
        subset_cost[key]=value;
    }
    myfile.close();

    *ptr_m=m;
    *ptr_n=n;
    *ptr_cost_range=cost_range;
    *ptr_total_subsets=total_subsets;

    return subset_cost;

}

vector<pair<long long int,long long int> > sort_wrt_cost(unordered_map<long long int,long long int> subset_cost, bool normalize)
{
    // sorting the subsets with respect to their cost
    vector<pair<long long int,long long int> > sorted_values;
    for(auto i:subset_cost)
    {
        pair<double, long long int> p;
        
        long long int count_of_elements = __builtin_popcount(i.first);
        double divisor = normalize ? count_of_elements + 1 : 1;
        double cost_per_element = i.second / (divisor); 

        p = make_pair(cost_per_element, i.first);
        sorted_values.push_back(p);
    }
    sort(sorted_values.begin(),sorted_values.end());

    return sorted_values;

}

vector<pair<long long int,bitset<200> > > sort_wrt_cost_in_bitset(unordered_map<bitset<200>,long long int> subset_cost)
{
    // sorting the subsets with respect to their cost
    vector<pair<long long int , bitset<200> > > sorted_bitsets;
    vector<pair<long long int,long long int > >sorted_values;
    unordered_map<long long int,bitset<200> > indx_map;
    long long int indx_map_key=0;
    for(auto i:subset_cost)
    {
        pair<long long int,long long int > p;
        p = make_pair(i.second, indx_map_key);
        indx_map[indx_map_key]=i.first;
        indx_map_key++;
        sorted_values.push_back(p);
    }
    sort(sorted_values.begin(),sorted_values.end());
    for(int i=0;i<sorted_values.size();i++)
    {
        pair<long long int ,long long int> dummy;
        dummy=sorted_values[i];
        sorted_bitsets.push_back(make_pair(dummy.first,indx_map[dummy.second]));
    }

    return sorted_bitsets;

}

int reduction_one(long long int m,unordered_map<long long int,long long int> subset_cost)
{
    // creating the matrix with entries 1/0, denoting if a element is present in the respective subset or not.
    vector<long long int> matrix_columns;
    vector<vector<int> > matrix(m); // creating matrix 
    for(auto i:subset_cost)
    {
        matrix_columns.push_back(i.first);
    }
    
    //filling the entries of the matrix
    for(int i=0;i<matrix_columns.size();i++)
    {
        long long int dummy = matrix_columns[i];
        for(int j=0;j<m;j++)
        {
            if(dummy%2==1)
            {
                matrix[j].push_back(1);
            }
            else
            {
                matrix[j].push_back(0);
            }
            dummy=dummy/2;
        }
    }
    
    // Reduction : if any row is a null vector, then there exists no solution.
    int flag=0; // if any row is null vector, make this flag = 1.
    for(int i=0;i<m;i++)
    {   
        int j;
        for(j=0;j<matrix_columns.size();j++)
        {
            if(matrix[i][j]==1){break;}
        }
        if(j==matrix_columns.size())
        {
            flag=1;
            cout << "Found a null vector. No solution exists.";
            break;
        }
    }

    return flag;

}

void reduction_two(long long int m,unordered_map<long long int,long long int> subset_cost, long long int *ptr1, long long int *ptr2)
{
    // creating the matrix with entries 1/0, denoting if a element is present in the respective subset or not.
    vector<long long int> matrix_columns;
    long long int pre_partial_sol=0;
    long long int pre_cost=0;
    vector<vector<int> > matrix(m); // creating matrix 
    for(auto i:subset_cost)
    {
        matrix_columns.push_back(i.first);
    }
    
    //filling the entries of the matrix
    for(int i=0;i<matrix_columns.size();i++)
    {
        long long int dummy = matrix_columns[i];
        for(int j=0;j<m;j++)
        {
            if(dummy%2==1)
            {
                matrix[j].push_back(1);
            }
            else
            {
                matrix[j].push_back(0);
            }
            dummy=dummy/2;
        }
    }
    // Reduction : if a row is a unit vector with a 1 in column t then in every solution x[t] = 1.
    
    for(int i=0;i<m;i++)
    {   
        int count=0;
        long long int index = -1;
        for(int j=0;j<matrix_columns.size();j++)
        {
            if(matrix[i][j]==1){count++;}
        }
        if(count==1)
        {
            for(int j=0;j<matrix_columns.size();j++)
            {
                if(matrix[i][j]==1){index=j;break;}
            }
            pre_partial_sol=matrix_columns[index]|pre_partial_sol;
            
            pre_cost+=subset_cost[matrix_columns[index]];
        }
        
    }
    *ptr1=pre_partial_sol;
    *ptr2=pre_cost;
    return;

}

set<int> reduction_three(long long int m,unordered_map<long long int,long long int> subset_cost)
{
     // creating the matrix with entries 1/0, denoting if a element is present in the respective subset or not.
    vector<long long int> matrix_columns;
    vector<vector<int> > matrix(m); // creating matrix 
    for(auto i:subset_cost)
    {
        matrix_columns.push_back(i.first);
    }
    
    //filling the entries of the matrix
    for(int i=0;i<matrix_columns.size();i++)
    {
        long long int dummy = matrix_columns[i];
        for(int j=0;j<m;j++)
        {
            if(dummy%2==1)
            {
                matrix[j].push_back(1);
            }
            else
            {
                matrix[j].push_back(0);
            }
            dummy=dummy/2;
        }
    }

    // Reduction : if a row[k1] >= row[k2], then row k1 can be deleted.
    set<int> deleted_rows;
    int flag_deleted_row=0;
    long long int deleted_rows_in_binary=0;
    for(int i=1;i<m;i++)
    {
        for(int j=0;j<m;j++)
        {
            if(i!=j)
            {   
                long long int itr;
                for(itr=0;itr<matrix_columns.size();itr++)
                {
                    if(matrix[i][itr]<matrix[j][itr]){break;}
                }
                if(itr==matrix_columns.size())
                {
                    deleted_rows.insert(i);
                    flag_deleted_row=1;
                    long long int dum_row=pow(2,i);
                    //pre_partial_sol=pre_partial_sol|dum_row;
                    deleted_rows_in_binary=deleted_rows_in_binary|dum_row;
                    break;
                }
            }
        }
        if(flag_deleted_row==1)
        {
            break;
        }
    }

    return deleted_rows;

}

void algorithm(long long int m,int flag,long long int total_subsets,long long int pre_partial_sol,long long int pre_cost,unordered_map<long long int,long long int> subset_cost,vector<pair<long long int,long long int> > sorted_values,set<int> deleted_rows)
{
    vector<vector<long long int> > v(m+2); //index position for each list
    long long int arr[m+2];
    for(int i=0;i<=m+1;i++)
    {
        arr[i]=0; //initialised with 0
    }

    //creating lists 
    for(int i=0;i<sorted_values.size();i++)
    {
        pair<long long int,long long int> i1=sorted_values[i];
        if((get<1>(i1))!=0){
        v[get_row(get<1>(i1))].push_back(get<1>(i1));}
    }

    long long int partial_sol=pre_partial_sol,partial_cost=pre_cost,best_sol=-1; //intialization 
    float best_cost=numeric_limits<float>::infinity();
    vector<long long int> best_sol_vec;
    vector<long long int> subsets_in_partial_sol; // vectoe containing subsets present in partial_sol
    unsigned long long int counter=0;
    auto start = std::chrono::high_resolution_clock::now();

    while(flag==0)
    {
        counter++;
        if (counter%10000==0)
        {
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
            cout<< duration << "s at iteration " << counter <<endl;
            
        }
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
            if(partial_sol==total_subsets-1)
            {
                best_cost=partial_cost;
                best_sol=1;
                best_sol_vec=subsets_in_partial_sol;
                cout << "Found sol with cost " << best_cost << endl;
            }
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
    if(best_sol==-1||flag==1)
    {
        cout<<"no solution found\n";
    }
    else
    {
        cout<<"minimum cost : "<<best_cost<<"\n";
        cout<<"corresponding subsets : ";
        for(int i=0;i<best_sol_vec.size();i++)
        {
            cout<<best_sol_vec[i]<<" ";
        }
        cout<<"\n";
    }

}

long long int algorithm_with_bitsets(long long int m,int flag,long long int total_subsets,long long int pre_partial_sol,long long int pre_cost,unordered_map<bitset<200>,long long int> subset_cost,vector<pair<long long int,bitset<200> > > sorted_values,set<int> deleted_rows)
{
    
    vector<vector<bitset<200> > > v(m+2); //index position for each list
    bitset<200> all_one(0);
    for(int i=0;i<m;i++)
    {
        all_one.set(i);
    }
    long long int arr[m+2];
    for(int i=0;i<=m+1;i++)
    {
        arr[i]=0; //initialised with 0
    }

    //creating lists 
    for(int i=0;i<sorted_values.size();i++)
    {
        pair<long long int,bitset<200> > i1=sorted_values[i];
        if((get<1>(i1)).count()!=0){
        v[get_row_bitset(get<1>(i1))].push_back(get<1>(i1));}
    }

    long long int partial_cost=0,best_sol=-1; //intialization
    bitset<200> partial_sol(0); 
    float best_cost=numeric_limits<float>::infinity();
    vector<bitset<200> > best_sol_vec;
    vector<bitset<200> > subsets_in_partial_sol; // vectoe containing subsets present in partial_sol
    unsigned long long int counter=0;
    auto start = std::chrono::high_resolution_clock::now();

    while(flag==0)
    {
        counter++;
        if (counter%10000==0)
        {
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
            cout<< duration << "s at iteration " << counter <<endl;
            
        }
        long long int list_no; 
        bitset<200> dummy;
        list_no=0;
        dummy=partial_sol;
        if(!(dummy.all()))
        {
            for(int i=0;i<200;i++)
            {
                if(!(dummy.test(i))){list_no=i;break;}
            }
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
            if((partial_sol&all_one)==all_one)
            {
                best_cost=partial_cost;
                best_sol=1;
                best_sol_vec=subsets_in_partial_sol;
                cout << "Found sol with cost " << best_cost << endl;
            }
            if(subsets_in_partial_sol.size()==0)
            {
                break; //terminate
            }
            
            bitset<200> subset_to_be_removed = subsets_in_partial_sol[subsets_in_partial_sol.size()-1];
            subsets_in_partial_sol.pop_back();
            partial_sol^=subset_to_be_removed;
            partial_cost=partial_cost-subset_cost[subset_to_be_removed];
        }
        else
        {
            arr[list_no]++; 
            bitset<200> subset_to_be_added = v[list_no][find_subset_index];
            subsets_in_partial_sol.push_back(subset_to_be_added);
            partial_sol=partial_sol|subset_to_be_added;
            partial_cost=partial_cost+subset_cost[subset_to_be_added];
            if((partial_sol&all_one)==all_one)
            {
                best_cost=partial_cost;
                best_sol=1;
                best_sol_vec=subsets_in_partial_sol;
            }
            
        }
    }
    if(best_sol==-1||flag==1)
    {
        return -1;
    }
    else
    {
        return best_cost;
    }

}

void compare(long long int algo_answer,string file_name2)
{
    string line;
    ifstream myfile (file_name2);
    float scip_answer;
    getline(myfile,line);
    sscanf(line.c_str(), "%f", &scip_answer);
    // compare results ,scip_time, algo_time,etc
    cout<<algo_answer<<"\n";
    return;
}

void tests(string file_name1,string file_name2){

    long long int m; //number of elements 
    long long int n; //number of subsets
    long long int cost_range; //cost of each subset lies in [0,cost_range] 
    long long int total_subsets; // 2**m
    
   
    unordered_map<long long int,long long int> subset_cost; // assume this comes from the file random_subsets.txt (that generates random set partition problem)
    unordered_map<bitset<200>,long long int> subset_cost_bitset; 
    //subset_cost=read_from_file(&m,&n,&cost_range,&total_subsets);
    subset_cost_bitset=read_from_bits_file(&m,&n,&cost_range,&total_subsets,file_name1);
    auto start = std::chrono::high_resolution_clock::now();
    vector<pair<long long int,long long int> > sorted_values;
    vector<pair<long long int,bitset<200>> > sorted_values_bitset;
   // sorted_values= sort_wrt_cost(subset_cost, false); // sorting the subsets with respect to their cost
    sorted_values_bitset=sort_wrt_cost_in_bitset(subset_cost_bitset);
    
    // Reductions
    int flag=0;
    //reduction_one(m,subset_cost); // set flag = 0 if you do not want to use reduction 1.
    long long int pre_partial_sol=0;
    long long int pre_cost=0;
   // reduction_two(m,subset_cost,&pre_partial_sol,&pre_cost); // call this function if you want to use reduction 2.
    set<int> deleted_rows; 
   // deleted_rows = reduction_three(m,subset_cost); // call this function if you want to use reduction 3.

    //algorithm(m,flag,total_subsets,pre_partial_sol,pre_cost,subset_cost,sorted_values,deleted_rows); // this function solves the set partition problem
    long long algo_answer=algorithm_with_bitsets(m,flag,total_subsets,pre_partial_sol,pre_cost,subset_cost_bitset,sorted_values_bitset,deleted_rows);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start).count(); // duration is time taken by algorithm
    compare(algo_answer,file_name2);
    return ;
}

int main()
{
    // call tests here
    tests("4_10_1.txt","4_10_1_output.txt");
    return 0;
}