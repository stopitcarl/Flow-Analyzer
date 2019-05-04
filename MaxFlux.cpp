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

#define UINT unsigned int;
#define MIN(a, b) (a < b ? a : b)

using namespace std;

// ###################### Global variables ####################################################
class Node;
int suppliers;        // Number of vertices
int edges;            // Number of edges
int storage;          // Number of storage
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
    int _height, _excess;

  public:
    Node() { _connects = vector<Edge>(); }
    void addConnection(Edge edge) { _connects.push_back(edge); }
    vector<Edge> getConnections() { return _connects; }
    int getHeight() { return _height; }
    void setHeight(int h) { _height = h; }
    int getExcess() { return _excess; }
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

    // Read suppliers capacity
    Edge edge;
    for (int i = 0; i < suppliers; i++)
    {
        if (scanf("%u", &edge.cap) < 0)
            exit(-1);
        edge.dest = 2 + i;
        (*nodes[0]).addConnection(edge);
    }

    // Read mid-way stations' capacity
    for (int i = 0; i < storage; i++)
    {
        if (scanf("%u", &edge.cap) < 0)
            exit(-1);
        edge.dest = 2 + suppliers + storage + i;
        (*nodes[2 + suppliers + i]).addConnection(edge);
    }

    // Read road network
    unsigned int origin;
    while (scanf("%u %u %u", &origin, &edge.dest, &edge.cap) > 0)
        (*nodes[origin]).addConnection(edge);
}

void relabel(unsigned int u)
{
    Node node = (*nodes[u]);
    vector<Edge> adjs = node.getConnections();
    int minHeight = (*nodes[adjs[0].dest]).getHeight();
    int temp = 0;
    for (Edge e : adjs)
    {
        temp = (*nodes[e.dest]).getHeight();
        if (temp < minHeight)
            minHeight = temp;
    }

    node.setHeight(++minHeight);
}

void printStatus(vector<Node *> nodes)
{
    int size = nodes.size();

    for (int i = 0; i < size; i++)
    {
        Node node = (*nodes[i]);
        vector<Edge> edges = (*nodes[i]).getConnections();
        printf("Node %d\n\th:%d\n\te:%d\n\tConnections:\n", i+1, node.getHeight(), node.getExcess());
        for (Edge e : edges)
            printf("\t\t%d%s---%d--->%d\n", i, e.cap > 9 ? "" : " ", e.cap, e.dest);
    }
}

int main()
{
    readInput();
    relabel(2);

    printStatus(nodes);
    // Keep this code to free nodes
    for (Node *node : nodes)
        delete node;

    return 0;
}
