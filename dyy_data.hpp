#ifndef _DYY_DATA_HPP_
#define _DYY_DATA_HPP_

#include <vector>
#include <iostream>
#include <fstream>
#include <cassert>
#include <string>
#include <map>
#include <algorithm>
#include "const.h"


/*
  Data model used in the research of YuyangDong
  and some functions ^.-
 */

namespace dyy{
/*********************************************************************
 * Point
 ********************************************************************/
class Point
{
public:
    Coord_V coords;
    int id;
    Point(){};
    ~Point(){}
    Point(Coord_V v){coords = v;}
    Point(Coord x, Coord y)
    {
        coords.push_back(x);
        coords.push_back(y);
    }

    friend std::istream& operator>>(std::istream& in, Point& p);
    bool operator< (const Point& p) const;

public:
    void print();
};


/*********************************************************************
 * Data
 ********************************************************************/

typedef std::vector<Point> Point_V;


class Data
{
public:

    Data(){};
    ~Data(){};

    static void loadPoint(std::string fileName, Point_V &v);
};


/* update a buffer of map*/
template <typename KEY, typename VALUE>
double update_buffer(std::multimap<KEY,VALUE, std::greater<KEY> > &buffer, KEY key, VALUE value, int k)
{
    if(buffer.size() < k){
        buffer.insert(std::pair<KEY,VALUE>(key,value));
    } else {
        buffer.insert(std::pair<KEY,VALUE>(key,value));
        buffer.erase(std::prev( buffer.end() ));
    }
    return  std::prev(buffer.end())->first;
}


/*put vector*/
template <typename T>
void put_vector(const std::vector<T> &v)
{
  for(auto element : v){
    std::cout << element << ", ";
  }
  std::cout << std::endl;
}






}



#endif /*_DYY_DATA_HPP_*/
