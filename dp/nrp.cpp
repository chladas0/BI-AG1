#include <iostream>
#include <limits>
using namespace std;


int in [] = {76,53,95,23,71,69,60,89,25,11,56,66,8,77,19,45,91,27,48,34,33};

int nrp (int i )
{
    int d = 1;

    for(int j = i + 1; j < 21; j++)
        if(in[i] < in[j])
            d  = max(d, 1 + nrp(j));
    return d;
}


int main ()
{
    int result = 0;
    for(int i = 0; i < 21; i++)
        result = max(result, nrp(i));
    cout << result << endl;

}
