#ifndef _DYY_GRID_HPP_
#define _DYY_GRID_HPP_

#include "const.h"
#include "dyy_Keyword.hpp"
#include <map>

namespace dyy{

class GridCell
{
public:
    GridCell(){}
    GridCell(Coord_V l, Coord_V r){leftLow = l; rightUp = r;}

    /*Objects functions*/
    void ObjAdd(int oid);
    void ObjDel(int oid);


    /*Queries functions*/
    void QryAdd(int qid);
    void QryDel(int qid);

    void print();

    void setXY(int _x, int _y){x = _x, y = _y;}

    int x;
    int y;

    Coord_V leftLow, rightUp;

    std::vector<int> Object_ids;
    std::vector<int> Queries_ids;

    std::map<std::string, double> maxwts;
    std::map<std::string, double> minwts;
};


class ObjectTable
{
public:
    ObjectTable(){};
    /*index = id*/
    SKPoint_V objects;
    std::vector<std::vector<int> > IntopKQS;

    void removeQry(int oid, int qid);
    void appendQry(int oid, int qid);
};


class QueryTable
{
public:
    QueryTable(){};
    /*index = id*/
    SKPoint_V querys;
    std::vector<double> kScores;
    std::vector< std::multimap<double, int, std::greater<double> > > topks;

    /*function top-k*/
    void updateKScore(int qid, double kscore);
    void removeObjfromTopk(int qid, int oid);
    void insertTopk(int qid, double score, int oid);
    void printTopk(int qid);

};

class Grid
{
public:
    Grid(){}
    /*Cells number = num * num*/
    Grid(int num) : number(num)
    {
        /*space dist 1.0*/
        step = MAXDIST / number;
        init();
    }

    void init();

    double get_r(double kScore, double alpha);


    /*Objects functions*/
    void ObjAdd(int oid);
    void ObjDel(int oid);

    /*Queries functions*/
    void QryAdd(int qid);
    void QryDel(int qid);
    void QryDel(int qid, double r);
    void QryAdjust(int qid, double old_kscore, double new_kscore);


    void print();

    ObjectTable* objtab;
    QueryTable* qrytab;

    double step;
    int number;

    std::vector<std::vector<GridCell> > CellS_XY;

};




}





#endif /*_DYY_GRID_HPP_*/
