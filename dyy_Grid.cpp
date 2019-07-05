#include "dyy_Grid.hpp"
#include <iostream>


namespace dyy{

/*********************************************************************
 *Cell
 ********************************************************************/
void GridCell::ObjAdd(int oid)
{
    Object_ids.push_back(oid);
}

void GridCell::ObjDel(int oid)
{
    Object_ids.erase(std::remove(Object_ids.begin(), Object_ids.end(), oid),
                     Object_ids.end());
}

/*need opt, block based invtfile*/
void GridCell::QryAdd(int qid)
{
    Queries_ids.push_back(qid);
}

void GridCell::QryDel(int qid)
{
    Queries_ids.erase(std::remove(Queries_ids.begin(), Queries_ids.end(), qid),
                      Queries_ids.end());
}

void GridCell::print()
{
    std::cout << "Queries num: ";
    std::cout << Queries_ids.size();
    /*for(auto qid : Queries_ids)
      std::cout << qid << ",";*/

    std::cout << std::endl;

    std::cout << "Objects num: ";
    std::cout << Object_ids.size();
    /*for(auto oid : Object_ids)
    std::cout << oid << ",";*/

    std::cout << std::endl;
    std::cout << "Range: ";
    std::cout << "<" << leftLow[0] << "," << leftLow[1] << ">";
    std::cout << " - ";
    std::cout << "<" << rightUp[0] << "," << rightUp[1] << ">";
    std::cout << std::endl;
    std::cout << "maxwts: ";
    if(!maxwts.empty()){
        for(auto it = maxwts.begin(); it != maxwts.end(); it++)
            std::cout << it->first << "(" << it->second << "), ";
    }
    std::cout << std::endl << "minwts: ";
    if(!minwts.empty()){
        for(auto it = minwts.begin(); it != minwts.end(); it++)
            std::cout << it->first << "(" << it->second << "), ";
    }
    std::cout << std::endl << "-----------------------" << std::endl;
}



/*********************************************************************
 *Grid (many cells)
 ********************************************************************/
void Grid::init()
{
    CellS_XY.resize(number);
    for(int x = 0; x < number; x++){
        CellS_XY[x].resize(number);
        for(int y = 0; y < number; y++){
            auto l = {x * step, y * step};
            auto r = {(x+1) * step, (y+1) * step};
            GridCell gc(l,r);
            gc.setXY(x, y);
            CellS_XY[x][y] = gc;
        }
    }
}


void Grid::ObjAdd(int oid)
{
    SKPoint *obj = &(objtab->objects[oid]);
    int x = obj->coords[0] / step;
    int y = obj->coords[1] / step;
    GridCell *cell = &(CellS_XY[x][y]);
    cell->ObjAdd(oid);

    /*update maxwts minwts*/
    for(auto it = obj->wts.begin(); it != obj->wts.end(); it++){
        auto key_wt = cell->maxwts.find(it->first);
        if(key_wt != cell->maxwts.end())
            key_wt->second = std::max(key_wt->second, it->second);
        else
            cell->maxwts.insert(*it);

        key_wt = cell->minwts.find(it->first);
        if(key_wt != cell->minwts.end())
            key_wt->second = std::min(key_wt->second, it->second);
        else
            cell->minwts.insert(*it);
    }
}

void Grid::ObjDel(int oid)
{
    SKPoint *obj = &(objtab->objects[oid]);
    int x = obj->coords[0] / step;
    int y = obj->coords[1] / step;
    GridCell *cell = &(CellS_XY[x][y]);
    cell->ObjDel(oid);

    for(auto it = obj->wts.begin(); it != obj->wts.end(); it++){
        /*recreate maxwts*/
        if(cell->maxwts[it->first] == it->second){
            cell->maxwts.erase(it->first);
            for(auto cell_o_id : cell->Object_ids){
                auto *object = &(objtab->objects[cell_o_id]);
                auto key_wt = object->wts.find(it->first);

                if(key_wt != object->wts.end()){
                    auto mwts_it = cell->maxwts.find(it->first);
                    if(mwts_it != cell->maxwts.end())
                        mwts_it->second = std::max(mwts_it->second, key_wt->second);
                    else
                        cell->maxwts.insert(*key_wt);
                }
            }
        }
        /*recreate minwts*/
        if(cell->minwts[it->first] == it->second){
            cell->minwts.erase(it->first);
            for(auto cell_o_id : cell->Object_ids){
                auto *object = &(objtab->objects[cell_o_id]);
                auto key_wt = object->wts.find(it->first);

                if(key_wt != object->wts.end()){
                    auto mwts_it = cell->minwts.find(it->first);
                    if(mwts_it != cell->minwts.end())
                        mwts_it->second = std::min(mwts_it->second, key_wt->second);
                    else
                        cell->minwts.insert(*key_wt);
                }
            }
        }
    }
}

/*need opt*/
void Grid::QryAdd(int qid)
{
    SKPoint *qry = &(qrytab->querys[qid]);
    int x = qry->coords[0] / step;
    int y = qry->coords[1] /step;
    CellS_XY[x][y].QryAdd(qid);

    double r = get_r(qrytab->kScores[qid], qry->alpha);
    int GoStep = r / step;

    int x_begin = (x - GoStep > 0) ? (x - GoStep) : 0;
    int y_begin = (y - GoStep > 0) ? (y - GoStep) : 0;
    int x_end   = (x + GoStep < number) ? (x + GoStep) : number;
    int y_end   = (y + GoStep < number) ? (y + GoStep) : number;

    for(int ix = x_begin; ix < x_end; ix++){
        for(int iy = y_begin; iy < y_end; iy++){
            if(ix != x && iy != y)
                CellS_XY[ix][iy].QryAdd(qid);
        }
    }
}

double Grid::get_r(double kScore, double alpha)
{
    return MAXDIST - ((kScore - 1 + alpha) / alpha )* MAXDIST;
}

/*need opt*/
void Grid::QryDel(int qid)
{
    SKPoint *qry = &(qrytab->querys[qid]);
    int x = qry->coords[0] / step;
    int y = qry->coords[1] /step;
    CellS_XY[x][y].QryDel(qid);

    double r = get_r(qrytab->kScores[qid], qry->alpha);
    int GoStep = r / step;

    int x_begin = (x - GoStep > 0) ? (x - GoStep) : 0;
    int y_begin = (y - GoStep > 0) ? (y - GoStep) : 0;
    int x_end   = (x + GoStep < number) ? (x + GoStep) : number;
    int y_end   = (y + GoStep < number) ? (y + GoStep) : number;

    for(int ix = x_begin; ix < x_end; ix++){
        for(int iy = y_begin; iy < y_end; iy++){
            if(ix != x && iy != y)
                CellS_XY[ix][iy].QryDel(qid);
        }
    }
}

void Grid::QryDel(int qid, double r)
{
    SKPoint *qry = &(qrytab->querys[qid]);
    int x = qry->coords[0] / step;
    int y = qry->coords[1] /step;
    CellS_XY[x][y].QryDel(qid);

    int GoStep = r / step;

    int x_begin = (x - GoStep > 0) ? (x - GoStep) : 0;
    int y_begin = (y - GoStep > 0) ? (y - GoStep) : 0;
    int x_end   = (x + GoStep < number) ? (x + GoStep) : number;
    int y_end   = (y + GoStep < number) ? (y + GoStep) : number;

    for(int ix = x_begin; ix < x_end; ix++){
        for(int iy = y_begin; iy < y_end; iy++){
            if(ix != x && iy != y)
                CellS_XY[ix][iy].QryDel(qid);
        }
    }
}

void Grid::QryAdjust(int qid, double old_kscore, double new_kscore)
{
    SKPoint *qry = &(qrytab->querys[qid]);
    double old_r = get_r(old_kscore, qry->alpha);
    double new_r = get_r(new_kscore, qry->alpha);
    int GoStep = (new_r - old_r) /step;

    if(GoStep != 0){
        QryDel(qid, old_r);
        QryAdd(qid);
    }
}

void Grid::print()
{
    std::cout << "Cells number: " << number * number;
    std::cout << std::endl << "-----------------------" << std::endl;
    for(int x = 0; x < number; x++){
        for(int y = 0; y < number; y++){
            CellS_XY[x][y].print();
        }
    }
}





/*********************************************************************
 *QueryTable
 ********************************************************************/

void QueryTable::updateKScore(int qid, double kscore)
{
    kScores[qid] = kscore;
}

void QueryTable::removeObjfromTopk(int qid, int oid)
{
    for(auto it = topks[qid].begin(); it != topks[qid].end(); it++){
        if(it->second == oid){
            topks[qid].erase(it);
            break;
        }
    }
}

void QueryTable::insertTopk(int qid, double score, int oid)
{
    double new_kScore = update_buffer(topks[qid], score, oid, querys[qid].k);
    kScores[qid] = new_kScore;
}

void QueryTable::printTopk(int qid)
{
    for (auto it = topks[qid].begin(); it != topks[qid].end(); it++) {
        std::cout << "oId: " << it->second;
        std::cout << ", score: " << it->first << std::endl;
    }
}


/*********************************************************************
 *ObjectTable
 ********************************************************************/

void ObjectTable::removeQry(int oid, int qid)
{
    IntopKQS[oid].erase(std::remove(IntopKQS[oid].begin(), IntopKQS[oid].end(), qid),
                        IntopKQS[oid].end());
}

void ObjectTable::appendQry(int oid, int qid)
{
    IntopKQS[oid].push_back(qid);
}






}//dyy
