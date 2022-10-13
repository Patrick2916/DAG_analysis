//
// Created by SUN Yongzheng on 2022/9/20.
//

#ifndef DAG_ANALYSIS_DAGTASK_H
#define DAG_ANALYSIS_DAGTASK_H
#include "DAGVertex.h"
#include <vector>
#include <map>
#include <iostream>
#include <cstring>
#include <set>

namespace DAG {

    class DAGTask {
    public:
        std::vector<DAGVer*> V;
        std::vector<int> topoOrder;

        int n_V = 0;
        double T = 0;
        double D = 0;
        double L = 0;
        double vol = 0;
        double u = 0;
        double pr = 0;

        DAGTask() = default;
        DAGTask(const double _T, const double _D) : T(_T), D(_D) {};
        void copyDAGTask(const DAGTask* obj){
            n_V = obj->n_V;
            T = obj->T;
            D = obj->D;
            L = obj->L;
            vol = obj->vol;
            u = obj->u;
            topoOrder = obj->topoOrder;
            for (auto &v: obj->V){
                DAGVer* new_ver = new DAGVer();
                new_ver->copyDAGVer(v);
                V.push_back(new_ver);
            }
        }
        ~DAGTask() = default;



        void makeItADAG();

        bool isAncestor(DAGVer *ver, DAGVer* tar);
        bool isDescendant(DAGVer* ver, DAGVer* tar);
        void getAncestorsSet(DAGVer* ver, std::set<int>& ans);
        void getDescendantsSet(DAGVer* ver, std::set<int>& des);
        void getInterferenceSet(DAGVer* ver, std::set<int>& ivs);
        void getLength();
        bool checkIndexAndIdsAreEqual();
        void getTopologicalOrder();
    };

    class DAG_Type : public DAGTask{
    public:
        std::vector<int> n_type;
        std::vector<double> vol_s;

        DAG_Type() = default;
        DAG_Type(const double _T, const double _D) : DAGTask(_T, _D) {};
        void copyDAGTask(const DAG_Type* obj){
            n_V = obj->n_V;
            T = obj->T;
            D = obj->D;
            L = obj->L;
            vol = obj->vol;
            u = obj->u;
            topoOrder = obj->topoOrder;
            n_type = obj->n_type;
            vol_s = obj->vol_s;
            for (auto &v: obj->V){
                DAGVer* new_ver = new DAGVer();
                new_ver->copyDAGVer(v);
                V.push_back(new_ver);
            }
        }
        ~DAG_Type() = default;

        void printDAGInfo();
        void generateDAGWithType(double set_u = -1, int set_nv = -1, double set_pr = -1, int set_s = -1);
        void generateDAGWithType_U(double set_u) {
            generateDAGWithType(set_u, -1, -1, -1);
        }
        void generateDAGWithType_V(int set_nv) {
            generateDAGWithType(-1, set_nv, -1, -1);
        }
        void generateDAGWithType_P(double set_pr) {
            generateDAGWithType(-1, -1, set_pr, -1);
        }
        void generateDAGWithType_S(int set_s) {
            generateDAGWithType(-1, -1, -1, set_s);
        }
        void readTaskFromDOT(const std::string &filename);
        void saveAsDot(const std::string &filename);

        void getAllAncestorsSet(std::vector<std::set<int>>& ans_vec);
        void getAllDescendantsSet(std::vector<std::set<int>>& des_vec);
        void getAllInterferenceSet(std::vector<std::set<int>>& ivs_vec,
                                   std::vector<std::set<int>>& ans_vec,
                                   std::vector<std::set<int>>& des_vec);
        void getVols();
    };
}
#endif //DAG_ANALYSIS_DAGTASK_H
