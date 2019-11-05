# Jack Wharton
# Cpt_S 350: Spring 2019
# ============================================================================ #
# This program contains an algorithm to solve the following question using a
# symbolic representation of the graph
#
# Given G as a graph of 32 nodes where, for all nodes nodes i and j in G,
# there is an edge from i to j iff (i+3) % 32 = j % 32 or (i+8) % 32 = j % 32
#
# [even] = the set of nodes that are even = {0,2,4,6,8,10,12,...,30}
# [prime] = the set of nodes that are prime = {3,5,7,11,13,17,19,23,29,31}
#
# Question:
# for each node u in [prime], is there a node v in [even] such that
# u can reach v in an even number of steps.
# ============================================================================ #

from pyeda.inter import *

# =================== CREATE BOOLEAN GRAPH REPRESENTATION ==================== #

# Helper function: takes two node values and returns true if there
# is an edge from i to j according to the rules of G
def isEdge(i, j):
    if (i + 3) % 32 == j % 32 or (i + 8) % 32 == j % 32:
        return True
    return False


# For a graph with 32 nodes, we need log2(32) = 5 boolean bits to express each
# node as a unique sequence of boolean values

# Subsequently, to represent an edge we need 10 boolean variables,
# x1, x2, x3, x4, x5 -> y1 y2 y3 y4 y5, which can be represented as
# a boolean expression.

def createEdgeExp(i, j):
    # Create the 10 boolean variables to be used
    x1, x2, x3, x4, x5 = map(exprvar, "x1 x2 x3 x4 x5".split())
    y1, y2, y3, y4, y5 = map(exprvar, "y1 y2 y3 y4 y5".split())

    # use bit comparison to assign negations for the 0s of the boolean (binary)
    # representation of node i
    if not (i & 1<<4):
        x1 = ~x1
    if not (i & 1<<3):
        x2 = ~x2
    if not (i & 1<<2):
        x3 = ~x3
    if not (i & 1<<1):
        x4 = ~x4
    if not (i & 1):
        x5 = ~x5

    # use bit comparison to assign negations for the 0s of the boolean (binary)
    # representation of node j
    if not (j & 1<<4):
        y1 = ~y1
    if not (j & 1<<3):
        y2 = ~y2
    if not (j & 1<<2):
        y3 = ~y3
    if not (j & 1<<1):
        y4 = ~y4
    if not (j & 1):
        y5 = ~y5

    # We can now combine all these variable into a boolean expression
    # representing this edge

    # PyEDA overloads & with AND logic operator
    exp = x1 & x2 & x3 & x4 & x5 & y1 & y2 & y3 & y4 & y5

    return exp


# To represent the entire graph as a boolean expression, we need to create a
# expression as: R = (i1 -> j1) OR (i1 -> j2) OR ... for each edge that
# appears in G, substituting the boolean edge expression for each edge.

def createGraphExp():
    R = expr2bdd(expr('0'))   # R is the boolean representation of the graph

    # for each edge in the graph,
    # add the boolean expression for that edge to the expression for G
    for i in range(0, 32):
        for j in range(0, 32):
            if isEdge(i, j):
                R = R | createEdgeExp(i, j) # OR the edge expression to R

    return R    # return the boolean graph representation


# ===================== USE BOOLEAN GRAPH REPRESENTATION ===================== #

# When R is contructed in this way, each row in the truth table of R represents
# a possible edge on G, with the value of R saying whether that edge appears
# in G or not. Using this, we can construct an algorithm to determine if
# there is a even-length path between any two nodes in G.

# Note: from here on, we will be treating each expression as a Binary decision
# tree (BDD) over 2(log2(32)) = 10 BDD variables: x1,...,x5, y1,...,y5.
# This will greatly simplify the size of our graph and make the running
# time much better.

def Compose(HH, RR):
    # Here we want to construct an expression determining if there exists
    # a node k such that H(i -> k) is true and R(k -> j) is true for
    # an i and j. (or if i can reach j in exactly 2 steps

    # That is, we are making an expression to say if there exists a node with
    # values z1,...,z5 such that R(x1,...,x5,z1,...,z5) AND
    # RR(z1,...,z5,y1,...,y5) is true

    # Get the input BDD variables of HH (same for RR)
    x1, x2, x3, x4, x5 = map(bddvar, 'x1 x2 x3 x4 x5'.split())
    y1, y2, y3, y4, y5 = map(bddvar, 'y1 y2 y3 y4 y5'.split())

    # Temporary BDD variables for node k
    z1, z2, z3, z4, z5 = map(bddvar, 'z1 z2 z3 z4 z5'.split())

    # Replace node j (v...z) with temporary k node
    f1 = HH.compose({y1: z1, y2: z2, y3: z3, y4: z4, y5: z5})

    # Replace node i (a...e) with temporary k node
    f2 = RR.compose({x1: z1, x2: z2, x3: z3, x4: z4, x5: z5})

    # Create a new expression as f1 AND f2 over 3 sets of boolean variables,
    # representing the existance of a path from i to k to j
    f = f1 & f2

    # We only want to know if k exists, so we use the smoothing
    # method to simplify the expression to when this is true.
    return f.smoothing({z1, z2, z3, z4, z5})


# We will nto define the process for creating the transitive closure of R,
# defined as R v Compose(R, R) v Compose(R, Compose(R, R)) v ...
# meaning from i to j there a path of length 1 or length 2 or length 3 ...
# => there is a path from i to j

def transitiveClosure(RR):
    H1 = RR
    H2 = expr2bdd(expr('0'))        # Empty BDD

    # In a graph with k nodes, the maximum length of the shortest path between
    # any two nodes is k, as if it passes any node ore than once it isn't the
    # stortest path. So we only need to check the lengths until that point,
    # when f1 and f2 are equivalent.

    while not (H1.equivalent(H2)):
        H2 = H1
        H1 = H2 | Compose(H2, RR)

    # f1 is now the transitive closure of R
    return H1

# However, in this problem we are concerned with all paths that have EVEN
# length. To account for this wee need to put Compose(R, R) into
# transitiveClosure() as R, so we get paths of length 2 v 4 v 6 v ...

# ======================== SOLUTION HELPER FUNCTIONS ========================= #

# Creates a single node expression over x1,..,x5, for use in creating [Prime]
def createPrimeNode(i):
    # Create the 5 boolean variables to be used
    x1, x2, x3, x4, x5 = map(exprvar, "x1 x2 x3 x4 x5".split())

    # use bit comparison to assign negations for the 0s of the boolean (binary)
    # representation of node i
    if not (i & 1<<4):
        x1 = ~x1
    if not (i & 1<<3):
        x2 = ~x2
    if not (i & 1<<2):
        x3 = ~x3
    if not (i & 1<<1):
        x4 = ~x4
    if not (i & 1):
        x5 = ~x5

    # PyEDA overloads & with AND logic operator
    exp = x1 & x2 & x3 & x4 & x5

    return exp


# Creates a single node expression over y1,..y5, for use in creating [Even]
def createEvenNode(i):
    # Create the 5 boolean variables to be used
    y1, y2, y3, y4, y5 = map(exprvar, "y1 y2 y3 y4 y5".split())

    # use bit comparison to assign negations for the 0s of the boolean (binary)
    # representation of node j
    if not (i & 1<<4):
        y1 = ~y1
    if not (i & 1<<3):
        y2 = ~y2
    if not (i & 1<<2):
        y3 = ~y3
    if not (i & 1<<1):
        y4 = ~y4
    if not (i & 1):
        y5 = ~y5

    # PyEDA overloads & with AND logic operator
    exp = y1 & y2 & y3 & y4 & y5

    return exp


def logicImply(E1, E2):
    # logically, E1 implies E2 means (E1 AND E2) OR NOT(E1)
    return (E1 & E2) | (~E1)

# ================================= SOLUTION ================================= #

print("Generating Graph Boolean Expression...")
R = createGraphExp()                    # Create boolean representation of G

print("Converting Expression to BDD...")
RR = expr2bdd(R)                        # Convert the boolean expression to BDD

print("Computing R compose R...")
RR2 = Compose(RR, RR)                   # 2-step BDD expression

print("Computing Transitive Closure...")
RREven = transitiveClosure(RR2)         # Compute transitive closure of RR2

# Now we have RREven, the BDD expression telling us whether two nodes have
# a path with an even number of steps. Now, we need to formulate an expression
# to say if for all nodes u in [prime] there exists a node v in [even] such
# that u can reach v in an even number of steps

# to do this cleanly, we will first translate the sets [prime] and [even] into
# the BDD expressions PP and EE respectively

# Create the BDD expression for [Even] and [Prime]
EE = expr2bdd(expr('0'))
PP = expr2bdd(expr('0'))

print("Creating [Even] and [Prime] expressions...")

for i in range(0, 32, 2):               # even numbers 0-30
    EE = EE | expr2bdd(createEvenNode(i))

for i in [3,5,7,11,13,17,19,23,29,31]:  # prime numbers 0-31
    PP = PP | expr2bdd(createPrimeNode(i))

# Now we must contruct the expression for {for all nodes u in [prime] there
# exists a node v in [even] such that u can reach v in an even number of steps}

# This can be done as:
# For all x1..x5 (PP(x1..x5) IMPLIES there exists y1..y5 such that
# EE(y1..y5) AND RREven(x1..x5, y1..y5))

print("Calculating Solution...")

x1, x2, x3, x4, x5 = map(bddvar, "x1 x2 x3 x4 x5".split())
y1, y2, y3, y4, y5 = map(bddvar, "y1 y2 y3 y4 y5".split())

# Exists y1..y5 : EE(y1..y5) AND RREven(x1..x5, y1..y5)
QQ = (EE & RREven).smoothing({y1, y2, y3, y4, y5})

# PP(x1..x5) IMPLIES QQ(x1..x5)
SS = logicImply(PP, QQ)

# Forall x1..x5, SS is true
# Equivalent to NOT there exists x1..x5 such that NOT SS
AA = ~((~SS).smoothing({x1, x2, x3, x4, x5}))

# AA is now the awnser to the given problem (0 or 1)
if (AA == expr2bdd(expr('1'))):
    print('Success: there IS an even path to a node in [even] for every node in [prime]')
else:
    print('Failure: there is NOT an even path to a node in [even] for every node in [prime]')
