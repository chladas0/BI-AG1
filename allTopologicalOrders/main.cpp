#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <deque>

using namespace std;

// nice complexity of O(n!)
void backtrack(set<int>& visited, deque<int>& path, vector<vector<int>>& result, const map<int, vector<int>>& graph) {
    // exit condition, reached the size
    if (visited.size() == graph.size()) {
        for(auto x : path)
            cout << x << " -> ";
        cout << endl;
        return;
    }

    for (const auto& node : graph)
    {
        // if current node is not visited and all of his neighbours were visited
        if (visited.find(node.first) == visited.end() &&
        // add him to the path and make all top orders with him at this place
        all_of(node.second.begin(), node.second.end(), [&visited](int neigh) { return visited.find(neigh) != visited.end(); }))

        {
            visited.insert(node.first);
            path.push_front(node.first);
            backtrack(visited, path, result, graph);
            visited.erase(node.first);
            path.pop_front();
        }
    }
}

void print_all_topological_orders(const map<int, vector<int>>& graph) {
    vector<vector<int>> result;
    set<int> visited;
    deque<int> path;
    backtrack(visited, path, result, graph);
}

int main() {
    map<int, vector<int>> graph = {
            {1, {2, 3}},
            {2, {4}},
            {3, {5}},
            {4, {6}},
            {5, {6}},
            {6, {}}
    };
    print_all_topological_orders(graph);
    return 0;
}
