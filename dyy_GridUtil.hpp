#ifndef _DYY_GRIDUTIL_HPP_
#define _DYY_GRIDUTIL_HPP_

#include <cmath>
#include <map>
#include "const.h"
#include "dyy_Keyword.hpp"
#include "dyy_Grid.hpp"


namespace dyy{

enum MonType{KMAX, KGRD, CB};

class fileParam
{
public:
    std::string o_init_spatial;
    std::string o_init_wts;
    std::string o_move_spatial;
    std::string o_move_wts;

    std::string q_spatial;
    std::string q_wts;
};

class GridUtil
{
public:
    GridUtil(MonType t){type = t;}

    /*loadData*/
    void loadSKPoint(std::string data_file);
    void loadSKPointJson(fileParam param);

    //SimS: Nomarlized Eculid similarity
    static double dist(Coord_V &a, Coord_V &b);
    static double maxdist(SKPoint &q, GridCell &cell);
    static double mindist(SKPoint &q, GridCell &cell);
    //SimT: dot product
    static double dot(std::map<std::string, double> &a,
                      std::map<std::string, double> &b);
    static double mindot(std::map<std::string, double> &qwts,
                         std::map<std::string, double> &cellminwts);

    //SimST = alpha * SimS + (1 - alpha) * SimT
    static double simst(SKPoint &q, SKPoint &o);
    /*score with cell and q*/
    static double maxscore(SKPoint &q, GridCell &cell);
    static double minscore(SKPoint &q, GridCell &cell);

    /*init Obj Query*/
    void ObjAdd(SKPoint &obj);
    void QryAdd(SKPoint &qry);

    GridCell* getCellbyObj(SKPoint &obj);

    std::vector<int> getAffectQueries(SKPoint &new_obj);

    /*top-1 topk re-query*/
    std::multimap<double, int, std::greater<double> > BFTopK(SKPoint &qry, int k);
    std::multimap<double, int, std::greater<double> > GridTopk(SKPoint &q, int k);
    std::pair<double, int> Gridkplusone(SKPoint &q);


    void monitor(SKPoint &new_obj, int oid);

    /*BaseLine topkmax*/
    void Topkmax(SKPoint &new_obj, int oid);

    /*BaseLine Grid topk*/
    void TopkGrid(SKPoint &new_obj, int oid);

    /*cell-base*/
    void CellBase(SKPoint &new_obj, int oid);

    /*member*/
    Grid grid;
    MonType type;
};

inline double GridUtil::minscore(SKPoint &q, dyy::GridCell &cell)
{
    double minSimS = GridUtil::maxdist(q, cell);
    double minSimT = GridUtil::mindot(q.wts, cell.minwts);
    if(minSimT)
        return (q.alpha) * minSimS + (1 - q.alpha) * minSimT;
    else
        return 0;
}

inline double GridUtil::maxscore(SKPoint &q, GridCell &cell)
{
    double maxSimS = GridUtil::mindist(q, cell);
    double maxSimT = GridUtil::dot(cell.maxwts, q.wts);
    if(maxSimT)
        return (q.alpha) * maxSimS + (1 - q.alpha) * maxSimT;
    else
        return 0;
}

inline double GridUtil::maxdist(SKPoint &q, GridCell &cell)
{
    double dis = 0;
    for(size_t dim = 0; dim < DIM; dim++){
        double diff =
            std::max(std::abs(cell.leftLow[dim] - q.coords[dim]),
                     std::abs(cell.rightUp[dim] - q.coords[dim])
                     );
        dis += diff * diff;
    }
    return (1 - std::sqrt(dis) / MAXDIST);
}

inline double GridUtil::mindist(SKPoint &q, GridCell &cell)
{
    double dis = 0;
    for(size_t dim = 0; dim < DIM; dim++){
        if(cell.leftLow[dim] > q.coords[dim] ||
           cell.rightUp[dim] < q.coords[dim])
            {
                double diff =
                    std::min(std::abs(cell.leftLow[dim] - q.coords[dim]),
                             std::abs(cell.rightUp[dim] - q.coords[dim])
                             );
                dis += diff * diff;
            }
    }
    return (1 - std::sqrt(dis) / MAXDIST);
}

inline double GridUtil::simst(SKPoint &q, SKPoint &o)
{
    double sims = GridUtil::dist(q.coords, o.coords);
    double simt = GridUtil::dot(q.wts, o.wts);
    if(simt)
        return (q.alpha * sims + (1 - q.alpha) * simt);
    else
        return 0;
}


inline double GridUtil::dist(Coord_V &a, Coord_V &b)
{
    double dis = 0;
    for(size_t dim = 0; dim < DIM; dim++){
        double diff = a[dim] - b[dim];
        dis += diff * diff;
    }
    return (1 - std::sqrt(dis) / MAXDIST);
}

inline double GridUtil::dot(std::map<std::string, double> &a,
                            std::map<std::string, double> &b)
{
    double sim = 0;
    for(auto it = a.begin(); it != a.end(); it++){
        auto hit = b.find(it->first);
        if(hit != b.end())
            sim += it->second * hit->second;
    }
    return sim;
}

inline double GridUtil::mindot(std::map<std::string, double> &qwts,
                               std::map<std::string, double> &cellminwts)
{
    double min = INF_P;
    double sim = 0;
    for(auto it = qwts.begin(); it != qwts.end(); it++){
        auto hit = cellminwts.find(it->first);
        if(hit != cellminwts.end()){
            sim = std::min(min, it->second * hit->second);
            min = sim;
        }
    }
    return sim;
}










}

#endif /*_DYY_GRIDUTIL_HPP_*/
