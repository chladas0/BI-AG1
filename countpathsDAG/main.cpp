#include <bits/stdc++.h>
using namespace std;
#define MAXN 1000005

vector<int> v[MAXN];

void add_edge(int a, int b, int fre[])
{
    v[a].push_back(b);
    fre[b]++;
}

vector<int> topological_sorting(int fre[], int n)
{
    queue<int> q;

    for (int i = 0; i < n; i++)
        if (!fre[i])
            q.push(i);

    vector<int> l;

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        l.push_back(u);

        for (int i = 0; i < v[u].size(); i++) {
            fre[v[u][i]]--;

            if (!fre[v[u][i]])
                q.push(v[u][i]);
        }
    }
    return l;
}


int numberofPaths(int source, int destination, int n, int fre[])
{
    vector<int> s = topological_sorting(fre, n);

    int dp[5] = { 0 };

    dp[source] = 1;

    for (int i = 0; i < s.size(); i++)
        for (int j = 0; j < v[s[i]].size(); j++)
            dp[v[s[i]][j]] += dp[s[i]];

    return dp[destination];
}

int main()
{
    int n = 5;
    int source = 0, destination = 4;

    int fre[5] = { 0 };

    add_edge(0, 1, fre);
    add_edge(0, 2, fre);
    add_edge(0, 3, fre);
    add_edge(0, 4, fre);
    add_edge(2, 3, fre);
    add_edge(3, 4, fre);

    cout << numberofPaths(source, destination, n, fre);
}