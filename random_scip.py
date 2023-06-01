 
from pyscipopt import Model, quicksum, SCIP_PARAMSETTING
import random
 
#Initialize model
model = Model("Sepa")
model.setPresolve(SCIP_PARAMSETTING.OFF)

# m = number of elements 
m = 10 # input

print(m)

# n = number of subset, n <= 2^m
n = 40 # input

print(n)

# range of cost is integers in [0,C] 
C = 200 # input

print(C)

N = pow(2,m) # total possible subsets

 
Elements={}
for i in range(0,m):
    Elements[i]=1
Ele_key = Elements.keys()

 
Profit={}
while len(Profit)<n :
    dummy = random.randint(0,N-1)
    if dummy in Profit:
        continue
    else:
        Profit[dummy]=random.randint(0,C) 
        print(dummy," ",Profit[dummy])
Profit_key = Profit.keys()


# checks if i is present in subset j
def ele_in_subset(i,j):
    x=j
    for dum in range(0,i):
        x=x//2
    if x%2 == 1 :
        return True
    else :
        return False
    
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
 
    for j in x:
        print(x[j].name, "=", model.getVal(x[j]), " (red. cost: ", model.getVarRedcost(x[j]), ")")
    for i in c:
        try:
            dual = model.getDualsolLinear(c[i])
        except:
            dual = None
        print("dual of", c[i].name, ":", dual)
else:
    print("Problem could not be solved to optimality")