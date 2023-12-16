#include <iostream>
#include <vector>
#include <list>
using namespace std;

struct Edge {
    int src, dest;
};

class Graph
{
public:
    vector<vector<int>> adjList;
    vector<int> indegree;

    Graph(vector<Edge> const &edges, int n){
        adjList.resize(n); indegree.resize(n);
        for (auto &edge: edges){
            adjList[edge.src].push_back(edge.dest);
            indegree[edge.dest]++;
        }
    }
};

void printPath(list<int> list)
{
    while (!list.empty()){
        cout << list.front() << ' ';
        list.pop_front();
    }
    cout << endl;
}

// Recursive function to find all topological orderings of a given DAG
void findAllTopologicalOrderings(Graph &graph, list<int> &path, vector<bool> &discovered, int n)
{
    // do for every vertex
    for (int v = 0; v < n; v++)
    {
        // proceed only if the current node's in-degree is 0 and the current node is not processed yet
        if (graph.indegree[v] == 0 && !discovered[v])
        {
            // for every adjacent vertex `u` of `v`, reduce the in-degree of `u` by 1
            for (int u: graph.adjList[v])
                graph.indegree[u]--;

            path.push_back(v);
            discovered[v] = true;

            findAllTopologicalOrderings(graph, path, discovered, n);

            // backtrack: reset in-degree information for the current node
            for (int u: graph.adjList[v])
                graph.indegree[u]++;

            path.pop_back();
            discovered[v] = false;
        }
    }
    if (path.size() == n)
        printPath(path);
}

void printAllTopologicalOrders(Graph &graph)
{
    vector<bool> discovered(graph.adjList.size());
    list<int> path;
    findAllTopologicalOrderings(graph, path, discovered, (int)graph.adjList.size());
}

int main()
{
    vector<Edge> edges ={
                    {0, 6}, {1, 2}, {1, 4}, {1, 6}, {3, 0},
                    {3, 4}, {5, 1}, {7, 0}, {7, 1}};
    int n = 8;
    Graph graph(edges, n);
    printAllTopologicalOrders(graph);
    return 0;
}