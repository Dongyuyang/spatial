#ifndef _DYY_KEYWORD_HPP_
#define _DYY_KEYWORD_HPP_

#include <string>
#include <map>
#include "dyy_data.hpp"

namespace dyy{

typedef std::string Keyword;
typedef std::vector<Keyword> Keyword_V;


/*********************************************************************
 *Spatial Keyword Point SKPoint
 ********************************************************************/

enum KEYWORD_SET{sushi, ramen, beaf, chicken, fish, chinese, curry,
               last};



std::map<std::string, double> randomWts();


class SKPoint : public Point
{
public:
    Keyword_V keywords;
    std::map<std::string, double> wts;
    double alpha = ALPHA;
    int k = KNUM;
    int kmax = KMAXCOF * k;

    SKPoint(){};
    ~SKPoint(){}

    SKPoint(Coord_V lo, Keyword_V key) : Point(lo) {keywords = key;}
    SKPoint(Coord x, Coord y, Keyword_V key) : Point(x,y) {keywords = key;}
    SKPoint(Coord x, Coord y, std::map<std::string, double> weights) : Point(x,y)
    {
        wts = weights;
    }

    void print();

    friend std::istream& operator>>(std::istream& in, Point &p);
};

typedef std::vector<SKPoint> SKPoint_V;
void loadSKPoint(std::string data_file, SKPoint_V &objs);



/*********************************************************************
 *Inverted File
 *Member keywords union keywords
 *Member postingList the points of each keyword, same index of keywords.
 ********************************************************************/

class InvertedFile
{
public:
    InvertedFile(){};
    InvertedFile(const SKPoint_V &sk_v){
        std::cout << "hi";
        init(sk_v);}

    void init(const SKPoint_V &sk_v);
    size_t inKeySet(Keyword k);

    Keyword_V keywords;
    std::vector<std::vector<int> > postingList;

    void print();
};





}//namespace dyy

#endif /*_DYY_KEYWORD_HPP_*/
