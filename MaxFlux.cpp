#include <vector>
#include <queue>
#include <list>
#include <string>
#include <iostream>
#include <cmath>

#define MIN(a, b) (a < b ? a : b)
#define HEIGHT(a) (nodes[a]->getHeight())
#define IS_STATION(a) (a >= (2 + suppliers))

using namespace std;

// ###################### Global variables ####################################################
class Node;
int suppliers;        // Number of suppliers
int storage;          // Number of storage
vector<Node *> nodes; // Vector of nodes
queue<Node *> L;      // Queue for Relabel to front
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
        /*for (Edge *edge : _connects)
            delete edge;*/
    }
};

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

    if (!isInL[e->dest] && amountPushed > 0)
    {
        L.push(nodes[e->dest]);
        isInL[e->dest] = true;
    }

    return amountPushed;
}

void relabelToFront()
{
    // Create list L
    L = queue<Node *>();
    for (int i = 0; i < suppliers; i++)
    {
        L.push(nodes[2 + i]);
        isInL[2 + i] = true;
    }

    vector<Edge *> adjs; // List of neighbours
    Node *node;
    while (!L.empty())
    {
        node = L.front();
        adjs = node->getConnections();
        int *excess = node->getExcess();
        while (*excess > 0)
        {
            //   cout << "Node " << node->getId() << endl;

            for (Edge *e : adjs)
            {
                if (e->cap > 0 && HEIGHT(e->dest) < node->getHeight()) // IF: edge has capacity and is lower
                    push(e, excess);                                   // THEN: push excess through it

                if (*excess == 0)
                    break;
            }

            if (*excess > 0)
                push(relabel(node), excess);
        }

        isInL[node->getIndex()] = false;
        L.pop();
    }
    //cout << " Done with RELABEL TO FRONT" << endl;
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
    // Mark all the vertices as not visited

    // Create a queue for BFS
    queue<int> queue;

    // Mark the current node as visited and enqueue it
    found[startingNode] = found[0] = true;

    bool augStations[2 + suppliers + storage * 2]{0};

    queue.push(startingNode);

    // List of adjencies
    vector<Edge *> edges;
    list<int *> minimumCut = list<int *>();
    list<int> stations = list<int>();

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
                    if (startingNode >= (suppliers + 2) && edge->dest >= (suppliers + 2) && abs(edge->dest - startingNode) == storage)
                        augStations[startingNode] = augStations[edge->dest] = true;

                    minimumCut.push_back(new int[2]{edge->dest, startingNode});
                }
                else
                {
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
        else if (edge[0] >= (suppliers + 2) && edge[1] >= (suppliers + 2) && abs(edge[1] - edge[0]) == storage) // If it's a station-to-station edge
        {
            stations.push_back(MIN(edge[0], edge[1]));
            it = minimumCut.erase(it);
            --it;
        }
        else if (augStations[edge[0]] || augStations[edge[1]]) // ignore this
        {
            it = minimumCut.erase(it);
            --it;
        }
        else if (edge[0] >= suppliers + 2 + storage)
            edge[0] -= storage;
        //        else if (edge[1] >= suppliers + 2 + storage)
        //          edge[1] -= storage;
    }

    if (!stations.empty())
    {
        stations.sort();
        list<int>::iterator it = stations.begin();
        cout << *it;
        ++it;
        while (it != stations.end())
        {
            cout << ' ' << *it;
            ++it;
        }
    }
    cout << endl;

    minimumCut.sort(compareEdges);
    for (auto edge : minimumCut)
        cout << edge[0] << " " << edge[1] << endl;
}

void printNode(Node *node)
{
    vector<Edge *> edges = node->getConnections();
    cout << "Node " << node->getId() << "\th: " << node->getHeight() << "\te: " << *node->getExcess()
         << endl;
    for (Edge *e : edges)
        cout << "\t" << node->getId()
             << "---" << e->cap << "--->" << nodes[e->dest]->getId() << endl;
}

void printStatus(vector<Node *> nodes)
{
    int size = nodes.size();
    cout << "Size: " << size << endl;
    for (int i = 0; i < size; i++)
        printNode(nodes[i]);
}

int readInput()
{
    int nodesNum;
    int edges;
    //cout << "reading line 1 (suppliers, gas stations, number of roads)" << endl;

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
    //cout << "reading line 2 (suppliers)" << endl;
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

    //cout << "reading line 3 (gas stations capacity)" << endl;
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
        // Link storage_in and storage_out
        edge->dest = storage_out;
        backEdge->dest = storage_in;
        backEdge->cap = 0;

        nodes[storage_in]->addConnection(edge);
        nodes[storage_out]->addConnection(backEdge);
    }

    // Read road network
    //cout << "reading line 4 and onwards (roads)" << endl;
    int origin;
    edge = new Edge, backEdge = new Edge;
    while (scanf("%u %u %u", &origin, &edge->dest, &edge->cap) > 0)
    {

        if (origin >= 2 + suppliers) // If edge goes out of storage for storage_in and storage_out
            origin += storage;

        backEdge->dest = origin;

        edge->back = backEdge;
        backEdge->back = edge;

        backEdge->cap = 0;

        nodes[origin]->addConnection(edge);
        nodes[edge->dest]->addConnection(backEdge);
        edge = new Edge, backEdge = new Edge;
    }
    delete edge, delete backEdge;

    return nodesNum;
}

int main()
{
    int nodesNum = readInput();

    isInL = new bool[nodesNum]{0};
    isInL[0] = isInL[1] = true; // Kids, here's a little lesson in trickery
    found = new bool[nodesNum]{0};

    //    printStatus(nodes);

    /*printNode(nodes[37]);
    printNode(nodes[37 + storage]);
    printNode(nodes[47]);
    printNode(nodes[47 + storage]);
    */
    //    cout << "################## RELABEL TO FRONT ##########################" << endl;
    relabelToFront();
    /*
    printNode(nodes[37]);
    printNode(nodes[37 + storage]);
    printNode(nodes[47]);
    printNode(nodes[47 + storage]);
*/
    cout << checkOutGoingFlow(nodes[1]) << endl; // Print max flow
    bfs(1);

    // Keep this code to free nodes
    /*    for (Node *node : nodes)
        delete node;

    delete[] isInL;
    delete[] found;
    */

    return 0;
}
