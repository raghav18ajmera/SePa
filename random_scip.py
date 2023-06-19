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
def generate_subsets(m,n,cost):
    N = pow(2,m) # total possible subsets
    Elements={}
    for i in range(0,m):
        Elements[i]=1

    Profit={}
    while len(Profit)<n :
        dummy = random.randint(0,N-1)
        if dummy in Profit:
            continue
        else:
            Profit[dummy]=random.randint(1,C) 
            # print(dummy," ",Profit[dummy])

    return (Elements,Profit)

# Solve the set partition problem using scip with pre solving on
def solve_using_scip(Elements,Profit):
    N = pow(2,m) # total possible subsets
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
    
    # Objective ( TO MINIMIZE)
    model.setObjective(quicksum(Profit[j]*x[j] for j in Profit_key), "minimize")
    
    model.optimize()
    
    if model.getStatus() == "optimal":
        print("Optimal value:", model.getObjVal())
    

    else:
        print("Problem could not be solved to optimality")

def save_instance_to_file(path: str, elements, profit): 
    with open(path, "w") as file:
        file.write(f"{len(elements)} {len(profit)}\n")
        for subset_number, profit in profit.items():
            file.write(f"{subset_number} {profit}\n")


if __name__ == "__main__":
    random.seed(1)
    
    #Initialize model
    model = Model("Sepa")
    # m = number of elements 
    m = 10 # input

    # n = number of subset, n <= 2^m
    n = 300 # input

    # range of cost is integers in [0,C] 
    C = 200 # input

    (Elements,Profit)=generate_subsets(m,n,C)
    save_instance_to_file("random_subsets.txt", Elements, Profit)
    solve_using_scip(Elements,Profit)