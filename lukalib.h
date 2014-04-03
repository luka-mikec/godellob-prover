#ifndef LUKALIB_H
#define LUKALIB_H

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
using namespace std;

#define full(x) x.begin(), x.end()

template <class T>
void vec_del(vector<T>v )
{
    for (typename vector<T>::iterator i = v.begin(); i != v.end(); ++i)
        delete *i;
}

template <class T2, class T1>
T2 scast(T1 val)
{    stringstream _in; _in.setf(ios_base::fixed); _in << val;
     _in.setf(ios_base::fixed); T2 _out; _in >> _out; return _out;
}



#endif // LUKALIB_H
