#ifndef _DYY_QUADTREE_HPP_
#define _DYY_QUADTREE_HPP_

#include "dyy_data.hpp"
#include "const.h"
#include <cmath>

namespace dyy{

/*********************************************************************
 * Cell: the Node of Quadtree
  |-------------|
  |  LU  |  RU  |
  |  1   |  2   |
  |------------=| Chilren id
  |  LL  |  RL  |
  |  3   |  4   |
  |-------------|
 ********************************************************************/
class Cell
{
public:
    Cell(){};
    Cell(Coord_VV r, Point_V pts);
    Cell(Coord lmin, Coord lmax, Coord rmin, Coord rmax, Point_V pts);

    Cell* getParent(){return parent;}
    Cell* getChild(int c_id);
    void setChild(Cell* child, int c_id);

    void addPoint(Point p){points.push_back(p);}
    void setPoints(const Point_V &pts);
    void delPoint(int p_id);
    void delPoints(){points.clear();}

    Point_V getPoints(){return points;}
    Coord_VV getRange(){return range;}

    int getPointsNum(){return points.size();}

    int depth();
    bool isLeaf();
    void print();

    Cell* parent;
    /*Children left right, up low*/
    Cell* LU, *LL, *RU, *RL;
    Coord_VV range;
    Point_V points;
};


/*********************************************************************
 Quad-tree, Cell as tree node
 ********************************************************************/
class QuadTree
{
public:
    static double minX, minY, maxX, maxY;
    static size_t LEAFTHRESHOLD;
    static size_t MAXDEPTH;

    QuadTree(Cell *c){root = c;}

    Cell* getRoot(){return root;}
    int depth(){return root->depth();}

    static bool pointInRange(const Point &p, const Coord_VV &range);

    static QuadTree create(const Point_V &points);

    static void createChildren(Cell* parent, size_t depth);
    static void splitPoints(const Point_V &pts,
                            Point_V &LUpts,
                            Point_V &RUpts,
                            Point_V &LLpts,
                            Point_V &RLpts,
                            const Coord_VV &range,
                            Coord_VV &rLU,
                            Coord_VV &rRU,
                            Coord_VV &rLL,
                            Coord_VV &rRL);

    void print();

private:
    Cell* root;
};









}//namespace dyy


#endif /*_DYY_QUADTREE_HPP_*/
