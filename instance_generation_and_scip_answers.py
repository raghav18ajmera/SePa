from pyscipopt import Model, quicksum, SCIP_PARAMSETTING
import random
import sys


# checks if i is present in subset j
def ele_in_subset(i,j):
    x=j
    for dum in range(0,i):
        x=x//2
    if x%2 == 1 :
        return True
    else :
        return False
    
# generates random subsets
def generate_subsets(m,n,cost,p):
    N = pow(2,m) # total possible subsets
    Elements={}
    for i in range(0,m):
        Elements[i]=1

    Profit={}
    while len(Profit)<n :
        dummy=0
        for i in range(0,m):
            random_num=random.randint(1,10)
            if(random_num<=p) :
                dummy=dummy+pow(2,i)
        if dummy in Profit:
            continue
        elif dummy!=0:
            Profit[dummy]=random.randint(1,C) 
            # print(dummy," ",Profit[dummy])

    return (Elements,Profit)

# Solve the set partition problem using scip with pre solving on
def solve_using_scip(Elements,Profit):
    #Initialize model
    model = Model("Sepa")
    Ele_key = Elements.keys()
    Profit_key = Profit.keys()

    Use ={}
    for i in Elements:
        for j in Profit:
            if ele_in_subset(i,j):
                Use[(i,j)]=1
            else:
                Use[(i,j)]=0
    
    # Create BINARY variables
    x = {}
    for j in Profit_key:
        x[j] = model.addVar(vtype="B", name="x(%s)"%j)
    
    # Create constraints
    c = {}
    for i in Ele_key:
        c[i] = model.addCons(quicksum(Use[i,j]*x[j] for j in Profit_key) == Elements[i], name="Use(%s)"%i)
    
    # Objective ( TO MINIMIZE )
    model.setObjective(quicksum(Profit[j]*x[j] for j in Profit_key), "minimize")
    
    model.optimize()

    # need to return lower and upper bonds, time taken by scip and subsets present in optimal solution
    # terminate it 1 hour time!
    
    if model.getStatus() == "optimal":
        print("Optimal value:", model.getObjVal())
        return model.getObjVal()
    

    else:
        return -1

def save_instance_to_file(path: str, elements, profit): 
    with open(path, "w") as file:
        file.write(f"{len(elements)} {len(profit)}\n")
        for subset_number, profit in profit.items():
            dum=subset_number
            l=[]
            if(dum==0):
                l.append(0)
            while(dum>0):
                l.append(dum%2)
                dum=dum//2
            l.reverse()
            for i in l:
                file.write(f"{i}")
            file.write(f"\n")
            file.write(f"{profit}\n")

def save_instance_to_file2(path: str,answer):
    with open(path, "w") as file:
        file.write(f"{answer}\n")
        


if __name__ == "__main__": 
    random.seed(1)

    # range of cost is integers in [0,C] 
    C = 200 # input

    # change range of i,j,p as required 
    i=4
    while(i<6):
        j=10
        while ((j<=pow(2,i)) and (j<15)):
            #probability of a element to be present in each subset is p/10
            p=1
            while(p<3):
                (Elements,Profit)=generate_subsets(i,j,C,p)
                file_name=str(i)+"_"+str(j)+"_"+str(p)+".txt"
                file_name2=str(i)+"_"+str(j)+"_"+str(p)+"_output.txt"
                answer=solve_using_scip(Elements,Profit)
                save_instance_to_file(file_name, Elements, Profit)
                save_instance_to_file2(file_name2,answer)
                p=p+1
            j=j+3
        i=i+1