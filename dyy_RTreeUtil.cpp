#include "dyy_RTreeUtil.hpp"
#include <queue>

namespace dyy{


Point RStarTreeUtil::NNquery(RStarTree &tree, Point &point)
{
    std::priority_queue<PDEntry, std::vector<PDEntry>, PDEntry::ascending> pq;
    pq.push(PDEntry(tree.root,1));

    double mindist = std::numeric_limits<double>::max();
    Point *NN_point;

    while(!pq.empty()){
        PDEntry e = pq.top();
        pq.pop();

        if(e.nodePtr->level){ //non leaf
            Node_P_V &children = *e.nodePtr->children;
            for(size_t ic = 0; ic < children.size(); ic++){
                Node_P childptr = children.at(ic);
                double dis = minDis2(point, childptr->mbrn);
                if(dis < mindist){
                    pq.push(PDEntry(childptr,dis));
                }
            }
        } else { // leaf
            Entry_P_V &entries = *e.nodePtr->entries;
            for(size_t ie = 0; ie < entries.size(); ie++){
                Entry_P entryPtr = entries.at(ie);
                double dis = dis2(point, entryPtr->mbre);
                //std::cout << "tree dis: " << dis << std::endl;
                if(dis < mindist){
                    mindist = dis;
                    NN_point = static_cast<Point *>(entryPtr->data);
                }
            }
        }
    }
    return *NN_point;
}


}/*dyy*/
