#include "dyy_Keyword.hpp"
#include <random>

/*********************************************************************
 *Spatial Keyword Point SKPoint
 ********************************************************************/
namespace dyy{

void SKPoint::print()
{
    std::cout << "<" << coords[0];
    for(size_t dim = 1; dim < DIM; dim++)
        std::cout << "," << coords[dim];
    std::cout << ">" << std::endl;
}

std::istream& operator>>(std::istream &in, SKPoint &p)
{
    p.coords.resize(DIM);
    for(size_t d = 0; d < DIM; d++)
        in >> p.coords[d];
    return in;
}

/**/

void loadSKPoint(std::string data_file, SKPoint_V &objs)
{
    objs.clear();
    std::ifstream in(data_file.c_str());
    assert(in.is_open());
    int id = 0;
    while(true){
        SKPoint p;
        in >> p;
        if(in){
            p.id = id++;
            objs.push_back(p);
        }
        else
            break;
    }
    in.close();
}

static int get_rand_int(int low, int up)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(low, up);

    return dis(gen);
}

std::map<std::string, double> randomWts()
{
    int index = get_rand_int(1, 6);
    std::string k1 = std::to_string(index);
    std::string k2 = std::to_string(index);

    double w1 = (double)index / 10;
    double w2 = 1 - w1;

    std::map<std::string, double> result;
    result.insert(std::pair<std::string, double>(k1,w1));
    result.insert(std::pair<std::string, double>(k2,w2));

    return result;
}

/**/

/*********************************************************************
 *Inverted File
 ********************************************************************/

size_t InvertedFile::inKeySet(Keyword k)
{
    std::cout << "hi";
    int index = -1;
    for(size_t i = 0; i < keywords.size(); i++){
        if(k.compare(keywords[i]) == 0)
            index = i;
    }
    return index;
}

void InvertedFile::init(const SKPoint_V &sk_v)
{
    std::cout << "hi";
    for(auto &&skp : sk_v){
        for(auto &k : skp.keywords){
            size_t index = inKeySet(k);
            if(index){
                postingList[index].push_back(skp.id);
            } else {
                keywords.push_back(k);
                postingList.push_back({skp.id});
            }
        }
        std::cout << "hi";
    }
}

void InvertedFile::print()
{
    for(size_t i = 0; i < postingList.size(); i++){
        std::cout << keywords[i] << ": ";
        /*for(size_t j = 0; j < postingList[i].size(); j++){
            std::cout << postingList[i][j] << ", ";
        }
        std::cout << std::endl;*/
    }
}


























}
