 
from pyscipopt import Model, quicksum, SCIP_PARAMSETTING
 
#Initialize model
model = Model("Sepa")
model.setPresolve(SCIP_PARAMSETTING.OFF)
 
Elements = {"A":1, "B":1, "C":1, "D":1, "E":1}
Ele_key = Elements.keys()
 
Profit = {"ABC":100, "BCD":150, "CDE":120, "AE":80, "AB":50, "BD":70, "DE":60, "BCE":110, "ACD":140}
Profit_key = Profit.keys()
 
Use = {
    ("A","ABC"):1,
    ("A","BCD"):0,
    ("A","CDE"):0,
    ("A","AE"):1,
    ("A","AB"):1,
    ("A","BD"):0,
    ("A","DE"):0,
    ("A","BCE"):0,
    ("A","ACD"):1,
    
    ("B","ABC"):1,
    ("B","BCD"):1,
    ("B","CDE"):0,
    ("B","AE"):0,
    ("B","AB"):1,
    ("B","BD"):1,
    ("B","DE"):0,
    ("B","BCE"):1,
    ("B","ACD"):0,

    ("C","ABC"):1,
    ("C","BCD"):1,
    ("C","CDE"):1,
    ("C","AE"):0,
    ("C","AB"):0,
    ("C","BD"):0,
    ("C","DE"):0,
    ("C","BCE"):1,
    ("C","ACD"):1,

    ("D","ABC"):0,
    ("D","BCD"):1,
    ("D","CDE"):1,
    ("D","AE"):0,
    ("D","AB"):0,
    ("D","BD"):1,
    ("D","DE"):1,
    ("D","BCE"):0,
    ("D","ACD"):1,

    ("E","ABC"):0,
    ("E","BCD"):0,
    ("E","CDE"):1,
    ("E","AE"):1,
    ("E","AB"):0,
    ("E","BD"):0,
    ("E","DE"):1,
    ("E","BCE"):1,
    ("E","ACD"):0,
    }
 
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