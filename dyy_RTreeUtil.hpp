#ifndef _DYY_RTREEUTIL_HPP_
#define _DYY_RTREEUTIL_HPP_

#include "dyy_data.hpp"
#include "dyy_RstarTree.hpp"
#include <cmath>

namespace dyy{

/*********************************************************************
 * Point + dis Entry
 ********************************************************************/

class PDEntry
{
public:
    Node_P nodePtr;
    double dis;

    PDEntry(Node_P _nodePtr, double _dis) : nodePtr(_nodePtr), dis(_dis){};

    struct ascending : public std::binary_function<PDEntry , PDEntry, bool>
    {
        bool operator()(const PDEntry &_a, const PDEntry &_b) const
        {return _a.dis < _b.dis;}
    };

};

/*********************************************************************
 * RStarTreeUtil
 ********************************************************************/

class RStarTreeUtil
{
public:
    RStarTreeUtil(){}

	static void buildTree(Point_V& points, Entry_V& entries, RStarTree* tree);

    /*Nearest neighbor query*/
    static Point NNquery(RStarTree &tree, Point &point);

	/*Range query*/
	static std::vector<Point*> rangeQuery(RStarTree &tree, Point &point, double range);
	
    //inline functions
    //Euclid distance
	/*without sqrt*/
    static double minDis2(Point &point, Mbr &mbr);
    static double maxDis2(Point &point, Mbr &mbr);
    static double dis2(Point &point, Mbr &mbr);

	/*with sqrt*/
	static double minL2(Point &point, Mbr &mbr);
	static double maxL2(Point &point, Mbr &mbr);
	static double L2(Point &point, Mbr &mbr);

    //Inner product
    static double dot(Point &a, Point &b);
    static double dotMbrLow(Point &point, Mbr &mbr);
    static double dotMbrUp(Point &point, Mbr &mbr);
    static double dotMMLow(Mbr &a, Mbr &b);
    static double dotMMUp(Mbr &a, Mbr &b);

};

inline double RStarTreeUtil::dot(Point &a, Point &b)
{
    double score = 0;
    for(size_t dim = 0; dim < DIM; dim++)
        score += a.coords[dim] * b.coords[dim];
    return score;
}

inline double RStarTreeUtil::dotMbrLow(Point &point, Mbr &mbr)
{
    double score = 0;
    for(size_t dim = 0; dim < DIM; dim++)
        score += point.coords[dim] * mbr.coord[dim][0];
    return score;
}

inline double RStarTreeUtil::dotMbrUp(Point &point, Mbr &mbr)
{
    double score = 0;
    for(size_t dim = 0; dim < DIM; dim++)
        score += point.coords[dim] * mbr.coord[dim][1];
    return score;
}

inline double RStarTreeUtil::dotMMLow(Mbr &a, Mbr &b)
{
    double score = 0;
    for(size_t dim = 0; dim < DIM; dim++)
        score += a.coord[dim][0] * b.coord[dim][0];
    return score;
}

inline double RStarTreeUtil::dotMMUp(Mbr &a, Mbr &b)
{
    double score = 0;
    for(size_t dim = 0; dim < DIM; dim++)
        score += a.coord[dim][1] * b.coord[dim][1];
    return score;
}

inline double RStarTreeUtil::minDis2(Point &point, Mbr &mbr)
{
    double dis = 0;
    for(size_t dim = 0; dim < DIM; dim++){
        if(mbr.coord[dim][0] > point.coords[dim] ||
           mbr.coord[dim][1] < point.coords[dim])
            {
                double diff =
                    std::min(std::abs(mbr.coord[dim][0] - point.coords[dim]),
                             std::abs(mbr.coord[dim][1] - point.coords[dim])
                             );
                dis += diff * diff;
            }
    }
    return dis;
}

inline double RStarTreeUtil::maxDis2(Point &point, Mbr &mbr)
{
    double dis = 0;
    for(size_t dim = 0; dim < DIM; dim++){
        double diff =
            std::max(std::abs(mbr.coord[dim][0] - point.coords[dim]),
                     std::abs(mbr.coord[dim][1] - point.coords[dim]));
        dis += diff * diff;
    }
    return dis;
}

inline double RStarTreeUtil::dis2(Point &point, Mbr &mbr)
{
    double dis = 0;
    for(size_t dim = 0; dim < DIM; dim++){
        double diff = mbr.coord[dim][0] - point.coords[dim];
        dis += diff * diff;
    }
    return dis;
}

	
inline double RStarTreeUtil::L2(Point &point, Mbr &mbr)
{
    double dis = 0;
    for(size_t dim = 0; dim < DIM; dim++){
        double diff = mbr.coord[dim][0] - point.coords[dim];
        dis += diff * diff;
    }
    return std::sqrt(dis);
}

inline double RStarTreeUtil::minL2(Point &point, Mbr &mbr)
{
	double dis = 0;
    for(size_t dim = 0; dim < DIM; dim++){
        if(mbr.coord[dim][0] > point.coords[dim] ||
           mbr.coord[dim][1] < point.coords[dim])
            {
                double diff =
                    std::min(std::abs(mbr.coord[dim][0] - point.coords[dim]),
                             std::abs(mbr.coord[dim][1] - point.coords[dim])
                             );
                dis += diff * diff;
            }
    }
    return std::sqrt(dis);
}

inline double RStarTreeUtil::maxL2(Point &point, Mbr &mbr)
{
    double dis = 0;
    for(size_t dim = 0; dim < DIM; dim++){
        double diff =
            std::max(std::abs(mbr.coord[dim][0] - point.coords[dim]),
                     std::abs(mbr.coord[dim][1] - point.coords[dim]));
        dis += diff * diff;
    }
    return std::sqrt(dis);
}



}






#endif /*_DYY_RTREEUTIL_HPP_*/
