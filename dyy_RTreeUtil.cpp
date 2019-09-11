#include "dyy_RTreeUtil.hpp"
#include <queue>

namespace dyy{


void RStarTreeUtil::buildTree(Point_V &points, Entry_V &entries, RStarTree *tree)
{
	entries.clear();
	for(size_t ip = 0; ip < points.size(); ip++){
		Data_P datap = &points.at(ip);
		Mbr mbr(points.at(ip).coords);
		LeafNodeEntry entry(mbr, datap);
		entries.push_back(entry);
	}

	std::cout << entries.size() << " entries created" << std::endl;

	for(size_t ie = 0; ie < entries.size(); ie++)
		tree->insertData(&entries.at(ie));

	std::cout << tree->root->aggregate << " entries created" << std::endl;
}


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

std::vector<Point*> RStarTreeUtil::rangeQuery(RStarTree &tree, Point &point, double range)
{
	std::priority_queue<PDEntry, std::vector<PDEntry>, PDEntry::ascending> pq;
    pq.push(PDEntry(tree.root,1));

	std::vector<Point*> result;

    while(!pq.empty()){
        PDEntry e = pq.top();
        pq.pop();

        if(e.nodePtr->level){ //non leaf
            Node_P_V &children = *e.nodePtr->children;
            for(size_t ic = 0; ic < children.size(); ic++){
                Node_P childptr = children.at(ic);
                double dis = minL2(point, childptr->mbrn);
                if(dis <= range){
                    pq.push(PDEntry(childptr,dis));
                }
            }
        } else { // leaf
            Entry_P_V &entries = *e.nodePtr->entries;
            for(size_t ie = 0; ie < entries.size(); ie++){
                Entry_P entryPtr = entries.at(ie);
                double dis = L2(point, entryPtr->mbre);
                if(dis <= range){
					std::cout << "tree dis: " << dis << std::endl;
					result.push_back(static_cast<Point *>(entryPtr->data));
				}
			}
        }
    }
    return result;
}


}/*dyy*/
