#include "dyy_QuadTree.hpp"

namespace dyy{


/*********************************************************************
 * Cell: the Node of Quadtree
 ********************************************************************/
Cell::Cell(Coord_VV r, Point_V pts)
    :parent(nullptr), LL(nullptr), LU(nullptr), RU(nullptr), RL(nullptr)
{
    range = r;
    points = pts;
}

Cell::Cell(Coord lmin, Coord lmax, Coord rmin, Coord rmax, Point_V pts)
    :parent(nullptr), LL(nullptr), LU(nullptr), RU(nullptr), RL(nullptr)
{
    range.push_back({lmin, lmax});
    range.push_back({rmin, rmax});
    points = pts;
}

Cell* Cell::getChild(int c_id)
{
    switch(c_id){
    case 1:
        return LU;
    case 2:
        return RU;
    case 3:
        return LL;
    case 4:
        return RL;
    default:
        return nullptr;
    }
}

void Cell::setChild(Cell* child, int c_id)
{
    if(child != nullptr){
        child->parent = this;
        switch(c_id){
        case 1:
            LU = child;
            break;
        case 2:
            RU = child;
            break;
        case 3:
            LL = child;
            break;
        case 4:
            RL = child;
            break;
        }
    }
}

bool Cell::isLeaf()
{
    return (LU == nullptr && LL == nullptr && RU == nullptr && RL == nullptr);
}

int Cell::depth()
{
    if(isLeaf())
        return 1;
    else{
        int n_LU = 0, n_LL = 0, n_RU = 0, n_RL = 0;
        if(LU)
            n_LU = LU->depth();
        if(LL)
            n_LL = LL->depth();
        if(RU)
            n_RU = RU->depth();
        if(RL)
            n_RL = RL->depth();

        return 1 + std::max({n_LU, n_LL, n_RU, n_RL});
    }
}

void Cell::setPoints(const Point_V &pts)
{
    points.clear();
    points = pts;
}

void Cell::delPoint(int p_id)
{
    int erase_index = -1;
    for(size_t i = 0; i < points.size(); i++){
        if(points[i].id == p_id){
            erase_index = points[i].id;
            break;
        }
    }
    if(erase_index != -1){
        points.erase(points.begin() + erase_index);
    }
}

void Cell::print()
{
    std::cout << depth() << ": ";
    for(size_t dim = 0; dim < 2; dim++)
        std::cout << "[" << range[dim][0] << "," << range[dim][1] << "]";
    std::cout << std::endl;
    for(size_t p_n = 0; p_n < points.size(); p_n++)
        points.at(p_n).print();

    if(LU)
        LU->print();
    if(LL)
        LL->print();
    if(RU)
        RU->print();
    if(RL)
        RL->print();
}



/*********************************************************************
 * QuadTree
 ********************************************************************/
/*Leaf max point number*/
size_t QuadTree::LEAFTHRESHOLD = 50;
/*Tree max depth*/
size_t QuadTree::MAXDEPTH = 10;
/*init Range min max (x,y)*/
double QuadTree::minX = 0;
double QuadTree::maxX = 1;
double QuadTree::minY = 0;
double QuadTree::maxY = 1;

QuadTree QuadTree::create(const Point_V &points)
{
    Cell *root = new Cell(minX,maxX,minY,maxY, points);

    QuadTree tree(root);
    int depth = 0;
    if(points.size() > LEAFTHRESHOLD && depth < MAXDEPTH)
        createChildren(root, depth + 1);

    return tree;
}

void QuadTree::createChildren(Cell *parent, size_t depth)
{
    Point_V LUpts, RUpts, LLpts, RLpts;
    Coord_VV rLU, rRU, rLL, rRL;
    Point_V pts = parent->getPoints();
    Coord_VV range = parent->getRange();

    splitPoints(pts, LUpts, RUpts, LLpts, RLpts, range, rLU, rRU, rLL, rRL);
    parent->delPoints();

    Cell *LU = new Cell(rLU, LUpts);
    parent->setChild(LU, 1);
    if(LU->getPointsNum() > LEAFTHRESHOLD && depth < MAXDEPTH)
        createChildren(LU, depth + 1);

    Cell *RU = new Cell(rRU, RUpts);
    parent->setChild(RU, 2);
    if(RU->getPointsNum() > LEAFTHRESHOLD && depth < MAXDEPTH)
        createChildren(RU, depth + 1);

    Cell *LL = new Cell(rLL, LLpts);
    parent->setChild(LL, 3);
    if(LL->getPointsNum() > LEAFTHRESHOLD && depth < MAXDEPTH)
        createChildren(LL, depth + 1);

    Cell *RL = new Cell(rRL, RLpts);
    parent->setChild(RL, 4);
    if(RL->getPointsNum() > LEAFTHRESHOLD && depth < MAXDEPTH)
        createChildren(RL, depth + 1);
}


bool QuadTree::pointInRange(const Point &p, const Coord_VV &range)
{
    bool in = true;
    if(p.coords[0] < range[0][0] || p.coords[0] > range[0][1])
        in = false;
    if(p.coords[1] < range[1][0] || p.coords[1] > range[1][1])
        in = false;
    return in;
}

void QuadTree::splitPoints(const Point_V &pts,
                           Point_V &LUpts, Point_V &RUpts,
                           Point_V &LLpts, Point_V &RLpts,
                           const Coord_VV &range,
                           Coord_VV &rLU, Coord_VV &rRU,
                           Coord_VV &rLL, Coord_VV &rRL)
{
    /*split ranges in 4 parts*/
    double a0 = range[0][0];
    double b0 = range[0][1];
    double a1 = range[1][0];
    double b1 = range[1][1];

    rLU = {{a0, (a0 + b0)*0.5}, {(a1 + b1)*0.5, b1}};
    rLL = {{a0, (a0 + b0)*0.5}, {a1, (a1 + b1)*0.5}};
    rRU = {{(a0 + b0)*0.5, b0}, {(a1 + b1)*0.5, b1}};
    rRL = {{(a0 + b0)*0.5, b0}, {a1, (a1 + b1)*0.5}};

    /*assign points*/
    for(auto &p : pts){
        if(pointInRange(p, rLU))
            LUpts.push_back(p);
        else if(pointInRange(p, rRU))
            RUpts.push_back(p);
        else if(pointInRange(p, rLL))
            LLpts.push_back(p);
        else  if(pointInRange(p, rRL))
            RLpts.push_back(p);
        else{}
    }
}

void QuadTree::print()
{
    root->print();
}




}//dyy
