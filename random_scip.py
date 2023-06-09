from pyscipopt import Model, quicksum, SCIP_PARAMSETTING
import random
import sys
random.seed(1)

original_stdout = sys.stdout # Save a reference to the original standard output
f = open("random_subsets.txt","w")
sys.stdout = f # Change the standard output to the file we created.
 
#Initialize model
model = Model("Sepa")

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
            Profit[dummy]=random.randint(0,C) 
            print(dummy," ",Profit[dummy])

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

# m = number of elements 
m = 4 # input
print(m)

# n = number of subset, n <= 2^m
n = 10 # input
print(n)

# range of cost is integers in [0,C] 
C = 200 # input
print(C)

(Elements,Profit)=generate_subsets(m,n,C)
solve_using_scip(Elements,Profit) 

f.close()
sys.stdout = original_stdout # Reset the standard output to its original value