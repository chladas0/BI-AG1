#ifndef __PROGTEST__
#include <cassert>
#include <iomanip>
#include <cstdint>
#include <iostream>
#include <memory>
#include <limits>
#include <optional>
#include <algorithm>
#include <bitset>
#include <list>
#include <array>
#include <vector>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <map>
#include <stack>
#include <queue>
#include <random>
#include <type_traits>
using Price = unsigned long long;
using Employee = size_t;
inline constexpr Employee NO_EMPLOYEE = -1;
using Gift = size_t;

#endif

#define INF std::numeric_limits<Price>::max()

struct mem{
    Gift gift1, gift2;
    size_t sum1, sum2;
};

void topSort(size_t boss, std::vector<size_t> & result, const std::vector<std::vector<Employee>> & G){
    std::stack<std::pair<size_t, bool>> stack({{boss,false}});

    while(!stack.empty()){
        auto [v, canClose] = stack.top(); stack.pop();

        if(canClose){ result.push_back(v); continue;}

        stack.emplace(v, true);
        for(auto neigh : G[v])
                stack.emplace(neigh, false);
    }
}

void assignGifts(Employee root, std::vector<mem> & dp, std::vector<Employee> & result, const std::vector<std::vector<Employee>> & G, const std::vector<Employee> & boss){
    std::queue<Employee> q({root});

    while(!q.empty()){
        Employee current = q.front(); q.pop();

        result[current] = boss[current] != NO_EMPLOYEE && result[boss[current]] == dp[current].gift1
                ? dp[current].gift2
                : dp[current].gift1;

        for(Employee neigh : G[current])
            q.push(neigh);
    }
}


std::pair<Price, std::vector<Gift>> optimize_gifts(const std::vector<Employee>& boss, const std::vector<Price>& gift_price)
{
    std::vector<std::vector<Employee>> subordinates(boss.size(), std::vector<Employee>());
    std::pair<Price, std::vector<Gift>> result(0, std::vector<Gift>(boss.size()));
    std::vector<mem> dp(boss.size(), {0, 0, INF, INF});
    std::vector<Employee> roots;

    // create graph of subordinates and vector of roots
    for(size_t i = 0; i < boss.size(); ++i)
        if(boss[i] != NO_EMPLOYEE)
            subordinates[boss[i]].push_back(i);
        else
            roots.push_back(i);

    // find best gifts for each tree
    for(Employee root : roots)
    {
        std::vector<Employee> topSorted;
        topSort(root, topSorted, subordinates);

        for(Employee employee : topSorted)
            for(size_t gift = 0; gift < gift_price.size(); ++gift)
            {
                Price currentSum = gift_price[gift];

                // for each gift check, what can be the best cost of subtrees
                for(Employee subordinate : subordinates[employee])
                    if(dp[subordinate].gift1 != gift)
                        currentSum += dp[subordinate].sum1;
                    else
                        currentSum += dp[subordinate].sum2;

                // update the best gift for current employee
                if(currentSum < dp[employee].sum1){
                    dp[employee].sum2 = dp[employee].sum1;
                    dp[employee].gift2 = dp[employee].gift1;

                    dp[employee].sum1 = currentSum;
                    dp[employee].gift1 = gift;
                }

                else if (currentSum < dp[employee].sum2){
                    dp[employee].sum2 = currentSum;
                    dp[employee].gift2 = gift;
                }
            }
        // set best gifts into result array, the bestSum is in dp[root].firstSum
        assignGifts(root, dp, result.second, subordinates, boss);
        result.first += dp[root].sum1;
    }
    return result;
}

#ifndef __PROGTEST__

const std::tuple<Price, std::vector<Employee>, std::vector<Price>> EXAMPLES[] = {
  { 17, {1,2,3,4,NO_EMPLOYEE}, {25,4,18,3} },
  { 16, {4,4,4,4,NO_EMPLOYEE}, {25,4,18,3} },
  { 17, {4,4,3,4,NO_EMPLOYEE}, {25,4,18,3} },
  { 24, {4,4,3,4,NO_EMPLOYEE,3,3}, {25,4,18,3} },
};

#define CHECK(cond, ...) do { \
    if (cond) break; \
    printf("Test failed: " __VA_ARGS__); \
    printf("\n"); \
    return false; \
  } while (0)

bool test(Price p, const std::vector<Employee>& boss, const std::vector<Price>& gp) {
  auto&& [ sol_p, sol_g ] = optimize_gifts(boss, gp);
  CHECK(sol_g.size() == boss.size(),
    "Size of the solution: expected %zu but got %zu.", boss.size(), sol_g.size());

  Price real_p = 0;
  for (Gift g : sol_g) real_p += gp[g];
  CHECK(real_p == sol_p, "Sum of gift prices is %llu but reported price is %llu.", real_p, sol_p);

  if (0) {
    for (Employee e = 0; e < boss.size(); e++) printf(" (%zu)%zu", e, sol_g[e]);
    printf("\n");
  }

  for (Employee e = 0; e < boss.size(); e++)
    CHECK(boss[e] == NO_EMPLOYEE || sol_g[boss[e]] != sol_g[e],
      "Employee %zu and their boss %zu has same gift %zu.", e, boss[e], sol_g[e]);

  CHECK(p == sol_p, "Wrong price: expected %llu got %llu.", p, sol_p);

  return true;
}
#undef CHECK

int main() {
  int ok = 0, fail = 0;
  for (auto&& [ p, b, gp ] : EXAMPLES) (test(p, b, gp) ? ok : fail)++;
  
  if (!fail) printf("Passed all %d tests!\n", ok);
  else printf("Failed %d of %d tests.", fail, fail + ok);
}

#endif


