#include "dyy_data.hpp"




namespace dyy{

/*********************************************************************
 * Point
 ********************************************************************/
std::istream& operator>>(std::istream &in, Point &p)
{
    p.coords.resize(DIM);
    for(size_t d = 0; d < DIM; d++)
        in >> p.coords[d];
    return in;
}

bool Point::operator<(const dyy::Point &p) const
{
    for(size_t dim = 0; dim < DIM; dim++)
        if(coords[dim] != p.coords[dim])
            return coords[dim] < p.coords[dim];
    return true;
}

void Point::print()
{
    std::cout << "<" << coords[0];
    for(size_t dim = 1; dim < DIM; dim++)
        std::cout << "," << coords[dim];
    std::cout << ">" << std::endl;
}

/*********************************************************************
 * Data
 ********************************************************************/

void Data::loadPoint(std::string data_file,  Point_V &points)
{
    points.clear();
    std::ifstream in(data_file.c_str());
    assert(in.is_open());
    int id = 0;
    while(true){
        Point point;
        in >> point;
        if(in){
            point.id = id++;
            points.push_back(point);
        }
        else
            break;
    }
    in.close();
}

}//dyy
