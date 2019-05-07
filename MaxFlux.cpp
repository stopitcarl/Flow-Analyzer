#include <vector>
#include <list>
#include <string>
#include <iostream>

#define MIN(a, b) (a < b ? a : b)
#define HEIGHT(a) (nodes[a]->getHeight())

using namespace std;

// ###################### Global variables ####################################################
class Node;
int suppliers;        // Number of vertices
int edges;            // Number of edges
int storage;          // Number of storage
vector<Node *> nodes; // Vector of vertices
list<Node *> L;       // Stack
int *parent;          // Array that contains the direct parent of each vertex
bool *stackU;         // Checklist if a vertex is in the stack

struct Edge
{
    int dest;
    int cap;
    Edge *back;
};

// ###################### Node ################################################################
class Node
{
    vector<Edge *> _connects; // Vector of adjacencies of the vertex
    int _height, _excess, _id;

  public:
    Node(int id) : _id(id) { _connects = vector<Edge *>(); }
    void addConnection(Edge *edge) { _connects.push_back(edge); }
    vector<Edge *> getConnections() { return _connects; }
    int getHeight() { return _height; }
    void setHeight(int h) { _height = h; }
    int *getExcess() { return &_excess; }
    void addExcess(int e) { _excess += e; }
    string getId()
    {
        if (_id == 0)
            return "source";
        else if (_id == 1)
            return "hiper";
        else if (_id <= suppliers + 1)
            return to_string(_id);
        else
        {
            if (_id < suppliers + 2 + storage)
                return to_string(_id) + "_in";
            else
                return to_string(_id - storage) + "_out";
        }
    }
    ~Node() { _connects.clear(); }
};

void readInput()
{
    int nodesNum;
    printf("reading line 1 (suppliers, gas stations, number of roads)\n");

    // Read the number of vertexes and edges
    if (!scanf("%u %u %u", &suppliers, &storage, &edges))
        exit(-1);

    // Calc the number of nodes
    nodesNum = 2 + suppliers + storage * 2;
    // Shorten the nodes list
    nodes.resize(nodesNum);

    // Initialize the nodes
    for (int i = 0; i < nodesNum; i++)
        nodes[i] = new Node(i);
    nodes[0]->setHeight(nodes.size());

    // Read suppliers capacity
    printf("reading line 2 (suppliers)\n");
    Edge *edge, *backEdge;
    for (int i = 0; i < suppliers; i++)
    {
        edge = new Edge, backEdge = new Edge;
        if (scanf("%u", &backEdge->cap) < 0)
            exit(-1);
        edge->dest = 2 + i;
        edge->back = backEdge;
        backEdge->back = edge;
        backEdge->dest = 0;
        nodes[2 + i]->addExcess(backEdge->cap);
        nodes[0]->addConnection(edge);
        nodes[2 + i]->addConnection(backEdge); // Part of residual graph
    }

    printf("reading line 3 (gas stations capacity)\n");
    // Read mid-way stations' capacity
    for (int i = 0; i < storage; i++)
    {
        edge = new Edge, backEdge = new Edge;
        int storage_in = 2 + suppliers + i;
        int storage_out = storage_in + storage;
        if (scanf("%u", &edge->cap) < 0)
            exit(-1);
        // Connect two edges
        edge->back = backEdge;
        backEdge->back = edge;
        // Connect two edges
        edge->dest = storage_out;
        backEdge->dest = storage_in;

        nodes[storage_in]->addConnection(edge);
        nodes[storage_out]->addConnection(backEdge);
    }

    // Read road network
    printf("reading line 4 and onwards (roads)\n");
    int origin;
    edge = new Edge, backEdge = new Edge;
    while (scanf("%u %u %u", &origin, &edge->dest, &edge->cap) > 0)
    {

        if (origin > 1 + suppliers) // If edge goes out of storage for storage_in and storage_out
            origin += storage;

        edge->back = backEdge;
        backEdge->back = edge;
        backEdge->dest = origin;

        nodes[origin]->addConnection(edge);
        nodes[edge->dest]->addConnection(backEdge);
        edge = new Edge, backEdge = new Edge;
    }
    delete edge, delete backEdge;
}

Edge *relabel(Node *node)
{
    vector<Edge *> adjs = node->getConnections();
    int temp, minHeight = -1;
    Edge *edge = adjs[0];
    for (Edge *e : adjs)
        if (e->cap > 0)
        {
            temp = HEIGHT(e->dest);
            if (temp < minHeight || minHeight < 0)
            {
                minHeight = temp;
                edge = e;
            }
        }

    node->setHeight(minHeight + 1);
    return edge;
}

int push(Edge *e, int *push)
{

    int amountPushed = *push > e->cap ? e->cap : *push;

    // Change variable capacity
    *push -= amountPushed;
    e->back->cap += amountPushed;
    e->cap -= amountPushed;
    nodes[e->dest]->addExcess(amountPushed);

    return amountPushed;
}

void relabelToFront()
{
    // Create list L
    L = list<Node *>(nodes.begin(), nodes.end());
    L.pop_front(); // remove source
    L.pop_front(); // remove sink

    vector<Edge *> adjs; // List of neirbours
    Node *node;
    auto it = L.begin();
    while (it != L.end())
    {
        node = *it;
        adjs = node->getConnections();
        int *excess = node->getExcess();
        while (*excess > 0)
        {
            //   cout << "Node " << node->getId() << endl;

            for (Edge *e : adjs)
                if (e->cap > 0 && HEIGHT(e->dest) < node->getHeight())
                {
                    push(e, excess);
                }
            if (*excess > 0)
            {
                push(relabel(node), excess);
                if (it != L.begin()) // If node is not already in L's front, move to it to front
                {
                    L.splice(L.begin(), L, it);
                    it = L.begin();
                }
            }
        }
        ++it;
    }
}

void printNode(Node *node)
{
    vector<Edge *> edges = node->getConnections();
    cout << "Node " << node->getId() << "\th: " << node->getHeight() << "\te: " << *node->getExcess()
         << endl;
    for (Edge *e : edges)
        cout << "\t" << node->getId() << "---" << e->cap << "--->" << e->dest << endl;
}

void printStatus(vector<Node *> nodes)
{
    int size = nodes.size();
    printf("Size: %d\n", size);
    for (int i = 0; i < size; i++)
        printNode(nodes[i]);
}

int main()
{
    readInput();

    //printStatus(nodes);
    cout << "################## RELABEL TO FRONT ##########################" << endl;
    relabelToFront();

    printStatus(nodes);

    // Keep this code to free nodes
    for (Node *node : nodes)
        delete node;

    return 0;
}
