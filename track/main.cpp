#ifndef __PROGTEST__
#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <cstdint>
#include <deque>
#include <iomanip>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <queue>
#include <random>
#include <set>
#include <stack>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

enum Point : size_t {};

struct Path {
    Point from, to;
    unsigned length;

    Path(size_t f, size_t t, unsigned l) : from{f}, to{t}, length{l} {}

    friend bool operator == (const Path& a, const Path& b) {
        return std::tie(a.from, a.to, a.length) == std::tie(b.from, b.to, b.length);
    }

    friend bool operator != (const Path& a, const Path& b) { return !(a == b); }
};

#endif

struct Node
{
    Node() = default;
    Node(unsigned value, size_t vertex) : value(value), vertex(vertex) {}
    unsigned value{};
    size_t vertex{};
};

void topsortDFS(size_t s, std::vector<bool> & visited, std::deque<size_t> & result, const std::vector<std::vector<Node>> & G)
{
    std::stack<std::pair<size_t, bool>> stack({{s,false}});

    while(!stack.empty())
    {
        auto [v, canClose] = stack.top(); stack.pop();

        if(canClose)
        {
            result.push_front(v);
            continue;
        }

        if(visited[v]) continue;

        stack.emplace(v, true);
        visited[v] = true;

        for(const auto & n : G[v])
            if(!visited[n.vertex])
                stack.emplace(n.vertex, false);
    }
}

std::vector<size_t> topsort(const std::vector<std::vector<Node>> & G, const std::unordered_set<size_t> & reachable, size_t points)
{
    std::vector<bool> visited (points, false);
    std::deque<size_t> result;

    for(const auto & vertex : reachable)
        if(!visited[vertex])
            topsortDFS(vertex, visited, result, G);

    return {result.begin(), result.end()};
}


std::vector<Path> longest_track(size_t points, const std::vector<Path>& all_paths)
{
    const Node noPath (0, std::numeric_limits<size_t>::max());

    // construct G for fast indexing
    std::vector<std::vector<Node>> G (points, std::vector<Node>());
    std::unordered_set<size_t> reachable;

    for(const auto & path : all_paths)
    {
        G[path.from].emplace_back(path.length, path.to);
        reachable.emplace(path.from);
    }

    // create array for saving best distances for each vertex
    std::vector<size_t> distances (points, 0);

    // use topsort so we can update distances in the right order
    std::vector<size_t> topSorted (topsort(G, reachable, points));

    // store previous vertices to be able to reconstruct the path
    std::vector<Node> prev (points, noPath);

    size_t bestEnd = 0, maxLength = 0;

    for(const auto & cur : topSorted)
        // update all neighbours if we get there in better length
        for(const auto & n : G[cur])
            // found a longer path into n
            if(distances[n.vertex] < distances[cur] + n.value)
            {
                distances[n.vertex] = distances[cur] + n.value;
                prev[n.vertex] = {n.value, cur};

                if(distances[n.vertex] > maxLength)
                {
                    maxLength = distances[n.vertex];
                    bestEnd = n.vertex;
                }
            }

    // Reconstruct the path from prev
    std::deque<Path> result;
    size_t cur = bestEnd;

    while (cur != noPath.vertex)
    {
        size_t previous = prev[cur].vertex;
        if (previous != noPath.vertex)
            result.emplace_front(prev[cur].vertex, cur, prev[cur].value);
        cur = previous;
    }
return {result.begin(), result.end()};
}


#ifndef __PROGTEST__

struct Test {
    unsigned longest_track;
    size_t points;
    std::vector<Path> all_paths;
};

inline const Test TESTS[] = {
        {13, 5, { {3,2,10}, {3,0,9}, {0,2,3}, {2,4,1} } },
        {11, 5, { {3,2,10}, {3,1,4}, {1,2,3}, {2,4,1} } },
        {16, 8, { {3,2,10}, {3,1,1}, {1,2,3}, {1,4,15} } },
};

#define CHECK(cond, ...) do { \
    if (cond) break; \
    printf("Fail: " __VA_ARGS__); \
    printf("\n"); \
    return false; \
  } while (0)

bool run_test(const Test& t) {
    auto sol = longest_track(t.points, t.all_paths);

    unsigned length = 0;
    for (auto [ _, __, l ] : sol) length += l;

    CHECK(t.longest_track == length,
          "Wrong length: got %u but expected %u", length, t.longest_track);

    for (size_t i = 0; i < sol.size(); i++) {
        CHECK(std::count(t.all_paths.begin(), t.all_paths.end(), sol[i]),
              "Solution contains non-existent path: %zu -> %zu (%u)",
              sol[i].from, sol[i].to, sol[i].length);

        if (i > 0) CHECK(sol[i].from == sol[i-1].to,
                         "Paths are not consecutive: %zu != %zu", sol[i-1].to, sol[i].from);
    }
    return true;
}
#undef CHECK

int main() {
    int ok = 0, fail = 0;

    for (auto&& t : TESTS) (run_test(t) ? ok : fail)++;

    if (!fail) printf("Passed all %i tests!\n", ok);
    else printf("Failed %u of %u tests.\n", fail, fail + ok);
}

#endif
