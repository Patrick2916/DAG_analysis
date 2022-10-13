//
// Created by SUN Yongzheng on 2022/9/20.
//

#ifndef DAG_ANALYSIS_DAGVERTEX_H
#define DAG_ANALYSIS_DAGVERTEX_H

#include <vector>
#include <algorithm>
#include <set>

namespace DAG {

    class DAGVer {
    public:
        int id = -1;
        int type = -1;
        int prio = -1;
        double c = -1;
        double l = -1;

        std::set<int> succ;
        std::set<int> pred;

        DAGVer() = default;
        DAGVer(double _c): c(_c) {}
        DAGVer(int _type, double _c): type(_type), c(_c) {}
        DAGVer(int _id, int _type, double _c): id(_id), type(_type), c(_c) {}
        void copyDAGVer(const DAGVer* obj) {
            id = obj->id;
            type = obj->type;
            c = obj->c;
            l = obj->l;
            prio = obj->prio;
            succ = obj->succ;
            pred = obj->pred;
        }
        ~DAGVer() = default;
    };
}
#endif //DAG_ANALYSIS_DAGVERTEX_H
