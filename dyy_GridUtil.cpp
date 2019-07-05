#include "dyy_GridUtil.hpp"
#include <algorithm>
#include <list>
#include <numeric>
#include <fstream>
#include <cassert>
#include "json.hpp"

namespace dyy{

/*set minues*/
static std::vector<int> AminuesB(std::vector<int> a, std::vector<int> b)
{
    a.erase(std::set_difference(a.begin(), a.end(), b.begin(), b.end(), a.begin()), a.end());
    return a;
}

/*Get intersection of two vector*/
static std::vector<int> GetIntersection(std::vector<int> a, std::vector<int> b)
{
    std::vector<int> result(a.size() + b.size());
    auto it=std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), result.begin());
    result.resize(it - result.begin());
    std::sort(result.begin(), result.end());
    return result;
}

/*Get union of two vector*/
static std::vector<int> GetUnion(std::vector<int> a, std::vector<int> b)
{
    std::vector<int> result(a.size() + b.size());
    auto it=std::set_union(a.begin(), a.end(), b.begin(), b.end(), result.begin());
    result.resize(it - result.begin());
    std::sort(result.begin(), result.end());
    return result;
}

void GridUtil::loadSKPoint(std::string data_file)
{
    grid.objtab->objects.clear();
    std::ifstream in(data_file.c_str());
    assert(in.is_open());
    int id = 0;
    SKPoint_V qs;
    while(true){
        SKPoint p;
        in >> p;
        if(in){
            p.id = id++;
            p.wts = randomWts();
            ObjAdd(p);
            if(id % QRATIO == 0){
                qs.push_back(p);
            }
        }
        else
            break;
    }
    in.close();

    /*add q*/
    for(auto &q : qs){
        QryAdd(q);
    }
}

template<typename Out>
void split(const std::string &s, char delim, Out result) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

static std::map<std::string, double> getWtsMap(std::vector<std::string> &keywords,
                                               std::vector<std::string> &wts)
{
    assert(keywords.size() == wts.size());
    std::map<std::string, double> result;
    int count = 1;
    for(size_t index = 0; index < keywords.size(); index++){
        result.insert(std::pair<std::string, double>(keywords[index],
                                                     std::stod(wts[index])));
        if(++count > MAXKWNUM)
            break;
    }

    return result;
}

using JSON = nlohmann::json;
void GridUtil::loadSKPointJson(fileParam param)
{
    grid.objtab->objects.clear();

    std::ifstream oixyf(param.o_init_spatial.c_str());
    std::ifstream oiwtf(param.o_init_wts.c_str());
    int id = 0;
    JSON json;
    int size = DATASIZE;

    //read init objects
    while(size-- > 0){
        SKPoint p;
        oixyf >> p;
        oiwtf >> json;
        if(oixyf && oiwtf){
            p.id = id++;
            auto k_set = split(json["text"], ' ');
            auto k_wts = split(json["wts"], ' ');
            p.wts = getWtsMap(k_set, k_wts);
            ObjAdd(p);
        }
        else
            break;
    }
    oixyf.close();
    oiwtf.close();

    size = DATASIZE/QRATIO;
    std::ifstream qxyf(param.q_spatial.c_str());
    std::ifstream qwtf(param.q_wts.c_str());
    //read init queries
    while(size-- > 0){
        SKPoint q;
        qxyf >> q;
        qwtf >> json;
        if(qxyf){
            auto k_set = split(json["text"], ' ');
            auto k_wts = split(json["wts"], ' ');
            q.wts = getWtsMap(k_set, k_wts);
            QryAdd(q);
        }
        else
            break;
    }
    qxyf.close();
    qwtf.close();

    std::ifstream omxyf(param.o_move_spatial.c_str());
    std::ifstream omwts(param.o_move_wts.c_str());
    //read moving objects
    while(true){
        SKPoint p;
        omxyf >> p;
        omwts >> json;
        if(omxyf){
            auto k_set = split(json["text"], ' ');
            auto k_wts = split(json["wts"], ' ');
            p.id = json["user_id"];
            p.wts = getWtsMap(k_set, k_wts);
            movingObjs.push_back(p);
        }
        else
            break;
    }
}


void GridUtil::monitor(SKPoint &new_obj, int oid)
{
    switch(type){
    case KMAX:
        Topkmax(new_obj, oid);
        break;
    case KGRD:
        TopkGrid(new_obj, oid);
        break;
    case CB:
        CellBase(new_obj, oid);
        break;
    default:
        break;
    }
}


std::vector<int> GridUtil::getAffectQueries(SKPoint &new_obj)
{
    std::vector<int> affectQ;
    /*TBD*/
    int x = new_obj.coords[0] / grid.step;
    int y = new_obj.coords[1] / grid.step;
    GridCell *cell = &(grid.CellS_XY[x][y]);
    for (auto qid : cell->Queries_ids) {
        double score = simst(grid.qrytab->querys[qid], new_obj);
        if (score > grid.qrytab->kScores[qid])
            affectQ.push_back(qid);
    }
    return affectQ;
}

void GridUtil::Topkmax(SKPoint &new_obj, int oid)
{
    grid.objtab->objects[oid] = new_obj;
    for(size_t iq = 0; iq < grid.qrytab->querys.size();iq++){
        grid.qrytab->removeObjfromTopk(iq, oid);
        if(grid.qrytab->topks[iq].size() < grid.qrytab->querys[iq].k){
            grid.qrytab->topks[iq] = BFTopK(grid.qrytab->querys[iq], grid.qrytab->querys[iq].kmax);
            grid.qrytab->kScores[iq] = std::prev(grid.qrytab->topks[iq].end())->first;
        }
        double score = simst(grid.qrytab->querys[iq], new_obj);
        bool in = false;
        if(score >= grid.qrytab->kScores[iq]){
            for(auto it = grid.qrytab->topks[iq].begin(); it != grid.qrytab->topks[iq].end(); it++){
                if(it->second == oid){
                    in = true;
                    break;
                }
            }
            if(!in)
                grid.qrytab->insertTopk(iq, score, oid);
        }
    }
}

void GridUtil::TopkGrid(SKPoint &new_obj, int oid)
{
    for(size_t iq = 0; iq < grid.qrytab->querys.size();iq++){
        grid.qrytab->removeObjfromTopk(iq, oid);
        if(grid.qrytab->topks[iq].size() < grid.qrytab->querys[iq].k){
            grid.qrytab->topks[iq] = GridTopk(grid.qrytab->querys[iq], grid.qrytab->querys[iq].kmax);
            grid.qrytab->kScores[iq] = std::prev(grid.qrytab->topks[iq].end())->first;
        }
        double score = simst(grid.qrytab->querys[iq], new_obj);
        if(score > grid.qrytab->kScores[iq]){
            grid.qrytab->insertTopk(iq, score, oid);
        }
    }
}

void GridUtil::CellBase(SKPoint &new_obj, int oid)
{
    for(size_t iq = 0; iq < grid.qrytab->querys.size();iq++){
        grid.qrytab->removeObjfromTopk(iq, oid);
        double score = simst(grid.qrytab->querys[iq], new_obj);

        if(grid.qrytab->topks[iq].size() < grid.qrytab->querys[iq].k){
            /*only find (k+1)-th with cell*/
            auto kplusone = Gridkplusone(grid.qrytab->querys[iq]);
            if(score > kplusone.first)
                grid.qrytab->insertTopk(iq, score, oid);
            else
                grid.qrytab->insertTopk(iq, kplusone.first, kplusone.second);
        } else {
            if(score > grid.qrytab->kScores[iq]){
                grid.qrytab->insertTopk(iq, score, oid);
            }
        }
    }
}


GridCell* GridUtil::getCellbyObj(SKPoint &obj)
{
    return &(grid.CellS_XY[obj.coords[0] / grid.step][obj.coords[1] / grid.step]);
}



void GridUtil::ObjAdd(SKPoint &obj)
{
    grid.objtab->objects.push_back(obj);
    int oid = grid.objtab->objects.size()  - 1;
    grid.ObjAdd(oid);
    grid.objtab->IntopKQS.push_back({});
}

void GridUtil::QryAdd(SKPoint &qry)
{
    grid.qrytab->querys.push_back(qry);
    int qid = grid.qrytab->querys.size() - 1;
    //auto topk = BFTopK(qry, qry.k);
    auto topk = GridTopk(qry,qry.k);
    double kscore = std::prev(topk.end())->first;
    grid.qrytab->topks.push_back(topk);
    grid.qrytab->kScores.push_back(kscore);
    grid.QryAdd(qid);

    for(auto it = topk.begin(); it != topk.end(); it++)
        grid.objtab->IntopKQS[it->second].push_back(qid);
}

std::multimap<double, int, std::greater<double> > GridUtil::BFTopK(SKPoint &qry, int k)
{
    std::multimap<double, int, std::greater<double> > result;
    for(size_t io = 0; io < grid.objtab->objects.size(); io++){
        double score = simst(qry, grid.objtab->objects[io]);
        if(result.size() < k){
            result.insert(std::pair<double, int>(score, io));
        } else {
            double kscore = std::prev(result.end())->first;
            if(score > kscore){
                result.insert(std::pair<double, int>(score, io));
                result.erase(std::prev(result.end()));
            }
        }
    }
    return result;
}

std::multimap<double, int, std::greater<double> > GridUtil::getKCand(SKPoint &q,
                                                                     PCL &pcl)
{
    std::multimap<double, int, std::greater<double> > result;
    double kscore = INF_N;

    for(auto it = pcl.list.begin(); it != pcl.list.end(); it++){
        if(it->first > kscore){
            for(auto oid : it->second->Object_ids){
                double score = simst(q, grid.objtab->objects[oid]);
                if(result.size() < q.k){
                    result.insert(std::pair<double, int>(score, oid));
                } else {
                    kscore = std::prev(result.end())->first;
                    if(score > kscore){
                        result.insert(std::pair<double, int>(score, oid));
                        result.erase(std::prev(result.end()));
                    }
                }
            }
        }
    }
    return result;
}

 std::multimap<double, int, std::greater<double> > GridUtil::GridTopk(SKPoint &q, int k)
{
    std::multimap<double, int, std::greater<double> > result;
    double kscore = INF_N;

    for(size_t x = 0; x < grid.CellS_XY.size(); x++){
        for(size_t y = 0; y < grid.CellS_XY[x].size(); y++){
            GridCell * cell = &(grid.CellS_XY[x][y]);
            double maxs = maxscore(q, *cell);
            if(maxs > kscore){
                for(auto oid : cell->Object_ids){
                    double score = simst(q, grid.objtab->objects[oid]);
                    if(result.size() < k){
                        result.insert(std::pair<double, int>(score, oid));
                    } else {
                        kscore = std::prev(result.end())->first;
                        if(score > kscore){
                            result.insert(std::pair<double, int>(score, oid));
                            result.erase(std::prev(result.end()));
                        }
                    }
                }
            }
        }
    }
    return result;
}


std::pair<double, int> GridUtil::Gridkplusone(SKPoint &q)
{
    int bestoid = -1;
    double threshold = std::prev(grid.qrytab->topks[q.id].end())->first;

    for(size_t x = 0; x < grid.CellS_XY.size(); x++){
        for(size_t y = 0; y < grid.CellS_XY[x].size(); y++){
            GridCell * cell = &(grid.CellS_XY[x][y]);
            double maxs = maxscore(q, *cell);
            if(maxs > threshold){
                for(auto oid : cell->Object_ids){
                    double score = simst(q, grid.objtab->objects[oid]);
                    if(score > threshold){
                        bestoid = oid;
                        threshold = score;
                    }
                }
            }
        }
    }
    return std::pair<double, int>(bestoid, threshold);
}


}
