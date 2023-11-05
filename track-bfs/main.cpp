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

struct ParentPath
{
    ParentPath(int index, unsigned length) : index(index), length(length) {}
    int index;
    unsigned length;
};


std::vector<Path> longest_track(size_t points, const std::vector<Path>& all_paths)
{
    const int noPath = -1;
    std::vector<std::vector<Path>> graph(points);
    std::vector<int> inDegree(points, 0);

    // create graph, graph[index] gives us possible paths from that index
    for(const auto & path : all_paths)
    {
        graph[path.from].push_back(path);
        inDegree[path.to]++;
    }

    std::deque<int> q;

    // insert all vertices with in degree 0 into the queue
    for(const auto & path : all_paths)
        if(inDegree[path.from] == 0)
            q.emplace_back(path.from);

    // vector for tracked longest distance found so far for each vertex
    std::vector<unsigned> distance(points, 0);

    // vector for backtracking the path from the end point
    std::vector<ParentPath> parent(points, ParentPath(noPath, 0));

    unsigned maxDistance = 0;
    int endPoint = -1;

    while(!q.empty())
    {
        int current = q.front();
        q.pop_front();

        for(const auto & path : graph[current])
        {
            // check new possible distance from current to the neighbour
            unsigned newDistance = distance[current] + path.length;

            // we found longer distance we update
            if(newDistance > distance[path.to])
            {
                distance[path.to] = newDistance;
                parent[path.to] = ParentPath(current, path.length);

                // we surpassed our max
                if(newDistance > maxDistance)
                {
                    maxDistance = newDistance;
                    endPoint = path.to;
                }
            }

            // we processed the neighbour so the in degree can be decreased
            inDegree[path.to]--;

            // only if the in degree is zero we can continue for that vertex
            if(inDegree[path.to] == 0)
                q.emplace_back(path.to);
        }
    }

    std::vector<Path> longestPath;
    int current = endPoint;

    // recreate the path using the parent vector
    while(parent[current].index != noPath)
    {
        ParentPath prev = parent[current];
        longestPath.emplace_back(prev.index, current, prev.length);
        current = prev.index;
    }

    // we backtracked the path, so it vice verse, we need to reverse it
    std::reverse(longestPath.begin(), longestPath.end());
    return longestPath;
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


