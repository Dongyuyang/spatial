#include "dyy_BallTree.hpp"
#include <random>



namespace dyy{

/*********************************************************************
 * Ball
 ********************************************************************/

Ball::Ball(Point_V p)
    : parent(NULL), leftChild(NULL), rightChild(NULL)
{
    points = p;
    calculateCenter();
    calculateRadius();
    setPointNum(p.size());
}


void Ball::setLeftChild(Ball *left)
{
    leftChild = left;
    if(left != NULL)
        left->parent = this;
}

void Ball::setRightChild(Ball *right)
{
    rightChild = right;
    if(right != NULL)
        right->parent = this;
}

int Ball::numNodes()
{
    int num = 1;
    if(leftChild != NULL)
        num += leftChild->numNodes();
    if(rightChild != NULL)
        num += rightChild->numNodes();
    return num;
}

int Ball::depth()
{
    if(isLeaf())
        return 1;
    else
        return 1 + std::max(leftChild->depth(), rightChild->depth());
}

void Ball::calculateCenter()
{
    center.coords.clear();
    center.coords.resize(DIM);
    for(auto &p : points){
        for(int dim = 0; dim < DIM; dim++)
            center.coords[dim] += p.coords[dim];
    }

    for(int dim = 0; dim < DIM; dim++)
        center.coords[dim] /= points.size();
}

void Ball::calculateRadius()
{
    double r = 0.0;
    for(auto &p : points){
        r = std::max(r, dist2(center,p));
    }
    radius = r;
}

void Ball::print()
{
    std::cout << "Depth: " << depth() <<
      ", r: " << radius << ", aggNodeNum: " << pointsNum << std::endl;
    if(leftChild != NULL)
        leftChild->print();
    if(rightChild != NULL)
        rightChild->print();
}


/*********************************************************************
 * BallTree
 ********************************************************************/
size_t BallTree::LEAFTHRESHOLD = 55;
size_t BallTree::MAXDEPTH = 20;


BallTree BallTree::create(Point_V points)
{

    Ball *root = new Ball(points);

    BallTree tree(root);

    int depth = 0;
    if(points.size() > LEAFTHRESHOLD && depth < MAXDEPTH)
        createChildren(root, depth + 1);

    return tree;
}


void BallTree::createChildren(Ball *parent, size_t depth)
{
    Point_V leftPoints, rightPoints;
    auto p = parent->getPoints();
    /*need opt*/
    splitPoints(p,leftPoints, rightPoints);
    parent->clearPoints();

    Ball* leftChild = new Ball(leftPoints);
    parent->setLeftChild(leftChild);

    if(leftChild->getPointNum() > LEAFTHRESHOLD && depth < MAXDEPTH)
        createChildren(leftChild, depth+1);
    Ball* rightChild = new Ball(rightPoints);

    parent->setRightChild(rightChild);
    if(rightChild->getPointNum() > LEAFTHRESHOLD && depth < MAXDEPTH)
        createChildren(rightChild, depth+1);
}

static int random(int low, int up)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(low, up);

    return dis(gen);
}


void BallTree::splitPoints(Point_V &parentPoints,
                           Point_V &leftPoints,
                           Point_V &rightPoints)
{
    leftPoints.clear(); rightPoints.clear();
    int rand = random(0,parentPoints.size() - 1);
    Point x = parentPoints[rand];

    int fa_index = 0;
    double mindist = 0;
    for(size_t i = 0; i < parentPoints.size(); i++){
        double d = dist2(x, parentPoints[i]);
        if(d > mindist){
            mindist = d;
            fa_index = i;
        }
    }
    Point A = parentPoints[fa_index];

    int fb_index = 0;
    mindist = 0;
    for(size_t i = 0; i < parentPoints.size(); i++){
        double d = dist2(A, parentPoints[i]);
        if(d > mindist){
            mindist = d;
            fb_index = i;
        }
    }
    Point B = parentPoints[fb_index];

    for(auto &p : parentPoints){
        double distA = dist2(A, p);
        double distB = dist2(B, p);

        if(distA <= distB)
            leftPoints.push_back(p);
        else
            rightPoints.push_back(p);
    }
}


void BallTree::print()
{
    root->print();
}




}/*dyy*/

