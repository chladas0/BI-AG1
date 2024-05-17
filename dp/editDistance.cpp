#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

string x = "progtest";
string y = "peklo";
int m = 7;
int n = 5;


vector<char> getMoves(int num, char c)
{
    vector<char> result;
    for(int i = 0; i < num; i++)
        result.push_back(c);
    return result;
}

pair<int, vector<char>> EVR (int i, int j)
{
    if(i > m) return {n - j + 1, getMoves(n-j+1, 'D')};
    if(j > n) return {m - i + 1, getMoves(m-i+1, 'D')};

    auto edited = EVR(i+1, j+1); // they are the same
    edited.second.push_back('K');


    if(x[i] != y[j]) //edit x[i] to be y[j]
    {
        edited.first++;
        edited.second.pop_back();
        edited.second.push_back('E');
    }

    auto deleted  = EVR(i + 1, j); // delete 
    deleted.first++;
    deleted.second.push_back('D');
    
    auto inserted = EVR(i, j + 1); // insert y[j] before x[i]
    inserted.first++;
    inserted.second.push_back('I');


    if(deleted < edited && deleted < inserted)
        return deleted;

    if(inserted < edited && inserted < deleted)
        return inserted;
    return edited;
}


int main ()
{
    auto result = EVR(0, 0);
    reverse(result.second.begin(), result.second.end());
    cout << "EVR is : " << result.first << endl
         << "The moves are : ";

    for(auto i : result.second)
        cout << i << ", ";
    cout << endl;
}
