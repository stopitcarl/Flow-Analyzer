#include <algorithm>
#include <cmath>
#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <queue>

#define MIN(a, b) (a < b ? a : b)
#define HEIGHT(a) (nodes[a]->getHeight())
#define IS_STATION(a) (a >= (2 + suppliers))
#define IS_STATION_OUT(a) (a >= (2 + suppliers + storage))

using namespace std;

// ###################### Global variables ####################################################
class Node;
int suppliers; // Number of suppliers
int storage;   // Number of storage
int nodesNum;
vector<Node *> nodes; // Vector of nodes
queue<int> L;         // Queue for Relabel to front
bool *found;          // Array that contains the direct parent of each vertex
bool *isInL;          // Checklist if a vertex is in the stack

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
    Node(int id) : _height(0), _excess(0), _id(id) { _connects = vector<Edge *>(); }
    void addConnection(Edge *edge) { _connects.push_back(edge); }
    vector<Edge *> getConnections() { return _connects; }
    int getHeight() { return _height; }
    void setHeight(int h) { _height = h; }
    int *getExcess() { return &_excess; }
    void addExcess(int e) { _excess += e; }
    int getIndex() { return _id; }
    string getId()
    {
        if (_id == 0)
            return "source";
        else if (_id == 1)
            return "hiper";
        else if (_id <= suppliers + 1)
            return to_string(_id) + " (supplier)";
        else
        {
            if (_id < suppliers + 2 + storage)
                return to_string(_id) + "_in";
            else
                return to_string(_id - storage) + "_out";
        }
    }
    ~Node()
    {
        // No freeing up memory cause why waste time
        /*for (Edge *edge : _connects)
            delete edge;*/
    }
};

/*
Returns: the edge to lowest node
*/
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

/*
Returns: amount of excess pushed trough edge e
*/
int push(Edge *e, int *push)
{

    int amountPushed = *push > e->cap ? e->cap : *push;

    // Change variable capacity
    *push -= amountPushed;
    e->back->cap += amountPushed;
    e->cap -= amountPushed;
    nodes[e->dest]->addExcess(amountPushed);

    if (!isInL[e->dest])
    {
        L.push(e->dest);
        isInL[e->dest] = true;
    }

    return amountPushed;
}

void relabelToFront()
{
    // Initialize node status array
    found = new bool[nodesNum]{0};

    // Create list L
    isInL = new bool[nodesNum]{0};
    isInL[0] = isInL[1] = true; // Kids, here's a little lesson in trickery
    L = queue<int>();
    for (int i = 0; i < suppliers; i++)
    {
        L.push(2 + i);
        isInL[2 + i] = true;
    }

    vector<Edge *> adjs; // List of neighbours
    Node *node;
    while (!L.empty())
    {
        node = nodes[L.front()];
        adjs = node->getConnections();
        int *excess = node->getExcess();
        while (*excess > 0)
        {
            for (Edge *e : adjs)
            {
                if (e->cap > 0 && HEIGHT(e->dest) < node->getHeight()) // IF: edge has capacity and is lower
                    push(e, excess);                                   // THEN: push excess through it

                if (*excess == 0) // IF: There's nor more excess to push
                    break;        // THEN: Leave loop
            }
            if (*excess > 0)
                push(relabel(node), excess);
        }
        isInL[node->getIndex()] = false;
        L.pop();
    }
}

int checkOutGoingFlow(Node *node)
{
    int flow = 0;
    for (Edge *e : node->getConnections())
        flow += e->cap;
    return flow;
}

bool compareEdges(int *&first, int *&second)
{
    return first[0] < second[0] || (first[0] == second[0] && first[1] < second[1]);
}

void bfs(int startingNode)
{
    // Create a queue for BFS
    queue<int> queue;

    // Mark the sink node as visited
    found[startingNode] = found[0] = true;
    queue.push(startingNode);

    // Create bool array to check if a given node is an augmented station
    bool augStations[storage * 2 + suppliers + 2]{0};
    int numStations = 0;

    // List of adjencies
    vector<Edge *> edges;

    list<int *> minimumCut = list<int *>();
    while (!queue.empty())
    {
        // Dequeue a vertex from queue and print it
        startingNode = queue.front();
        queue.pop();
        edges = nodes[startingNode]->getConnections();
        for (Edge *edge : edges)
        {
            if (!found[edge->dest])
            {
                if (edge->cap > 0 && edge->back->cap == 0)
                {
                    if (IS_STATION(startingNode) && IS_STATION(edge->dest) && abs(edge->dest - startingNode) == storage)
                    {
                        augStations[startingNode] = augStations[edge->dest] = true;
                        numStations++;
                    }
                    else
                        minimumCut.push_back(new int[2]{edge->dest, startingNode});
                }
                else
                {
                    if (augStations[startingNode] || augStations[edge->dest])
                        numStations--;

                    augStations[startingNode] = augStations[edge->dest] = false;
                    found[edge->dest] = true;
                    queue.push(edge->dest);
                }
            }
        }
    }

    int *edge;
    for (auto it = minimumCut.begin(); it != minimumCut.end(); it++)
    {
        edge = *it;
        if (found[edge[1]] && found[edge[0]]) // If both vertexes were found
        {
            it = minimumCut.erase(it);
            --it;
        }
        else if (augStations[edge[0]] || augStations[edge[1]]) // If either one of its nodes is an augmented station
        {
            it = minimumCut.erase(it);
            --it;
        }
        else if (IS_STATION_OUT(edge[0])) // TODO: comment (yea ironic I know, shut up)
            edge[0] -= storage;
    }

    int stationLimit = 2 + suppliers + storage;
    if (numStations > 0)
        for (int i = 2 + suppliers; i < stationLimit; i++)
            if (augStations[i])
            {
                printf("%d", i);
                numStations--;
                if (numStations > 0)
                    printf(" ");
            }
    printf("\n");

    minimumCut.sort(compareEdges);
    for (int *edge : minimumCut)
        printf("%d %d\n", edge[0], edge[1]);
}

int readInput()
{
    int nodesNum;
    int edges;

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
        edge->cap = 0;

        nodes[2 + i]->addExcess(backEdge->cap);
        nodes[0]->addConnection(edge);
        nodes[2 + i]->addConnection(backEdge); // Part of residual graph
    }

    // Read storage stations capacity
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
        // Link storage_in and storage_out
        edge->dest = storage_out;
        backEdge->dest = storage_in;
        backEdge->cap = 0;
        // Add edges to nodes
        nodes[storage_in]->addConnection(edge);
        nodes[storage_out]->addConnection(backEdge);
    }

    // Read road network
    int origin;
    edge = new Edge, backEdge = new Edge;
    while (scanf("%u %u %u", &origin, &edge->dest, &edge->cap) > 0)
    {
        if (origin >= 2 + suppliers) // IF: edge leaves from storage
            origin += storage;       // THEN: offset storage's index to its 'out' node

        // Connect two edges
        edge->back = backEdge;
        backEdge->back = edge;
        // Initialize the backedge
        backEdge->dest = origin;
        backEdge->cap = 0;
        // Add edges to nodes
        nodes[origin]->addConnection(edge);
        nodes[edge->dest]->addConnection(backEdge);
        edge = new Edge, backEdge = new Edge;
    }

    return nodesNum;
}

int main()
{
    nodesNum = readInput();

    relabelToFront();

    printf("%d\n", checkOutGoingFlow(nodes[1])); // Print max flow
    bfs(1);

    /* 
    // Keep this code to free nodes
       for (Node *node : nodes)
        delete node;

    delete[] isInL;
    delete[] found;
    */

    return 0;
}
