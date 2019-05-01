/* Relabel to front
    Vertice
        lista de adjancencias
    
    Adjacencia
        capacidade

        Vertice:out : {
            adja cencias: [3: in, 5: in ,7 ,8] // length = v
            capacidade: [2, 9, 3, 4]

            
        }
    (A-in)--100-->(A-out)-->

    Vertice {
        push : function {}
    }

    armazem : [2, 6, 8] / <- heap
    [0,0,0,0,1,0,0,0,1,0]

    vertices : [1,2,3,4,5,6,7,8,9]

    (s)-20/500->(1)-20/20->(2)-20/100->(3)-20/20->(4)-20/100->(t)   //Relabel O(V³) + BFS
                 |          |
                (8)--0/30--(9)

    
*/

#include <vector>
#include <stack>
#include <iostream>

#define unsigned int Int;

using namespace std;

// ###################### Global variables ####################################################
class Node;
int suppliers; // Number of vertices
int edges;     // Number of edges
int storage;   // Number of storage
unsigned int *storage;
vector<Node *> nodes; // Vector of vertices
stack<int> stackL;    // Stack
int *parent;          // Array that contains the direct parent of each vertex
bool *stackU;         // Checklist if a vertex is in the stack

struct Edge
{
    unsigned int dest;
    unsigned int cap;
};

// ###################### Node ################################################################
class Node
{
    vector<Edge> _connects; // Vector of adjacencies of the vertex

  public:
    Node() { _connects = vector<Edge>(); }
    void addConnection(Edge edge) { _connects.push_back(edge); }
    vector<Edge> getConnections() { return _connects; }
    ~Node() { _connects.clear(); }
};

void readInput()
{
    int suppliers, storage, edges, nodesNum;

    // Read the number of vertexes and edges
    if (!scanf("%u %u %u", &suppliers, &storage, &edges))
        exit(-1);

    // Calc the number of nodes
    nodesNum = 1 + suppliers + storage * 2;
    // Shorten the nodes list
    nodes.resize(nodesNum);

    // Initialize the nodes
    for (int i = 0; i < nodesNum; i++)
        nodes[i] = new Node();

    // Read suppliers
    Edge edge;
    for (int i = 0; i < suppliers; i++)
    {
        scanf("%u", &edge.cap);
        edge.dest = 2 + i;
        (*nodes[0]).addConnection(edge);
    }

    for (int i = 0; i < storage; i++)
    {
        scanf("%u", &edge.cap);
        edge.dest = 2 + suppliers + storage + i;
        (*nodes[2 + suppliers + i]).addConnection(edge);
    }

    // Update the adjacencies list of each vertex

    while (scanf("%u %u", &edge[0], &edge[1]) > 0)
    {
        --connectNum;
        (*nodes[routes[0] - 1]).addConnection(routes[1] - 1);
        (*nodes[routes[1] - 1]).addConnection(routes[0] - 1);
    }

    if (connectNum != 0)
        exit(-1);
}

int main()
{

    return 0;
}
