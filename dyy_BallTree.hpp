#ifndef _DYY_BALLTREE_HPP_
#define _DYY_BALLTREE_HPP_

#include "dyy_data.hpp"
#include "const.h"
#include <cmath>



namespace dyy{


/*********************************************************************
 * Ball i am here to test surume
 ********************************************************************/

class Ball
{
public:
    Ball(Point_V p);

    Ball* getParent(){return parent;}
    Ball* getLeftChild(){return leftChild;}
    Ball* getRightChild(){return rightChild;}

    void setLeftChild(Ball *left);
    void setRightChild(Ball *right);

    bool isLeaf(){return (leftChild == NULL && rightChild == NULL);}
    int numNodes();
    int depth();


    void clearPoints(){points.clear();}

    Point_V getPoints(){return points;}
    Point getCenter(){return center;}
    double getRadius(){return radius;}

    void setPointNum(size_t num){pointsNum = num;}
    size_t getPointNum(){return pointsNum;}

    static double dist(Point &a, Point &b);
    static double dist2(Point &a, Point &b);

    void print();

    Point center;
    double radius;
    Point_V points;

    size_t pointsNum;

    Ball* parent;
    Ball* leftChild;
    Ball* rightChild;

    void calculateCenter();
    void calculateRadius();
};

inline double Ball::dist(Point &a, Point &b)
{
    double dis = 0;
    for(int dim = 0; dim < DIM; dim++)
        dis += (a.coords[dim] - b.coords[dim]) * (a.coords[dim] - b.coords[dim]);
    return dis;
}

inline double Ball::dist2(Point &a, Point &b)
{
    return std::sqrt(Ball::dist(a, b));
}




/*********************************************************************
 * BallTree (ConeTree) based on the paper
 P. Ram & A. Gray,
 "Maximum Inner-Product Search using Cone trees." kdd12
 ********************************************************************/
class BallTree
{

public:

    static size_t LEAFTHRESHOLD;
    static size_t MAXDEPTH;

    BallTree(Ball* b){root = b;}

    Ball* getRoot(){return root;}
    int numNodes(){return root->numNodes();}
    int depth(){return root->depth();}

    static BallTree create(Point_V points);
    static void createChildren(Ball* parent,size_t depth);
    static void splitPoints(Point_V &parentPoints,
                            Point_V &leftPoints,
                            Point_V &rightPoints);

    /*static functions*/
    static double dist(Point &a, Point &b);
    static double dist2(Point &a, Point &b);

    void print();

private:
    Ball* root;

};


inline double BallTree::dist(Point &a, Point &b)
{
    double dis = 0;
    for(int dim = 0; dim < DIM; dim++)
        dis += (a.coords[dim] - b.coords[dim]) * (a.coords[dim] - b.coords[dim]);
    return dis;
}

inline double BallTree::dist2(Point &a, Point &b)
{
    return std::sqrt(BallTree::dist(a, b));
}













}/*namespace dyy*/

#endif /*_DYY_BINARYTREE_HPP_*/
