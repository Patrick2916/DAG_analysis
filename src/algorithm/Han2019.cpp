//
// Created by SUN Yongzheng on 2022/9/22.
//
#include "../../include/DAGTask.h"
#include "../../include/utils.h"
#include "../../include/DAGAlgorithm.h"

namespace DAG {

    struct Tuple {
        int id;
        DAGVer* v;
        std::map<int, int> Delta;
        double res_time;
    };

    void updateDelta(std::map<int, int> &old_delta, Tuple *t) {
        copy(old_delta.begin(), old_delta.end(), std::inserter(t->Delta, t->Delta.begin()));
        t->Delta[t->v->type] = t->v->id;
    }

    void updateResponsetime(Tuple *old_t, Tuple *t, std::vector<int>& n_type, std::vector<DAGVer*>& V, std::vector<std::set<int>>& ivs_vec) {
        t->res_time = old_t->res_time + t->v->c;
        std::set<int> phi;
        if (old_t->Delta.find(t->v->type) != old_t->Delta.end()) {
            int delta_old_s = old_t->Delta[t->v->type];
            setDifference(ivs_vec[t->v->id], ivs_vec[delta_old_s], phi);
        } else {
            copy(ivs_vec[t->v->id].begin(), ivs_vec[t->v->id].end(), std::inserter(phi, phi.begin()));
        }
        for (const int& index : phi) {
            t->res_time += V[index]->c / double(n_type[t->v->type]);
        }
    }

    bool dominate(DAG_Type *dag,
                  std::vector<std::set<int>> des_vec,
                  std::vector<std::set<int>> ivs_vec,
                  Tuple *old_t, Tuple *new_t) {
        if (old_t->res_time <= new_t->res_time) {
            return false;
        } else {
            for (int s = 0; s < dag->n_type.size(); ++s) {
                if (old_t->Delta.find(s) == old_t->Delta.end()) {
                    continue;
                } else if (new_t->Delta.find(s) != new_t->Delta.end()) {
                    std::set<int> inter_par_des;
                    inter_par_des.clear();
                    if (ivs_vec[old_t->Delta[s]].empty()) {
                        continue;
                    }
                    setInter(ivs_vec[old_t->Delta[s]], des_vec[new_t->Delta[s]], inter_par_des);
                    if (inter_par_des.empty()) {
                        continue;
                    } else {
                        return false;
                    }
                } else {
                    return false;
                }
            }
            return true;
        }
    }

    void simplicitDAG (DAG_Type* dag,
                       std::vector<std::set<int>> des_vec,
                       std::vector<std::set<int>> ans_vec) {
        for (auto& v : dag->V) {
            std::set<int> inter;
            setInter(v->succ, des_vec[v->id], inter);
            setDifference(v->succ, inter, v->succ);
            inter.clear();
            setInter(v->pred, ans_vec[v->id], inter);
            setDifference(v->pred, inter, v->pred);
        }
    }

    double computeResponseTime_Han2019_NEWB2(DAG_Type *dag) {
        int path = 0;
        // to ensure there is no pred of a vertex in TS.
        dag->getTopologicalOrder();
        std::vector<DAGVer*> V(dag->V);
        std::vector<int> topo(dag->topoOrder);
        // computer par() of every vertex.
        std::vector<std::set<int>> ans_vec;
        dag->getAllAncestorsSet(ans_vec);
        std::vector<std::set<int>> des_vec;
        dag->getAllDescendantsSet(des_vec);
        //simplicitDAG(dag, des_vec, ans_vec);
        std::vector<std::set<int>> ivs_vec;
        dag->getAllInterferenceSet(ivs_vec, ans_vec, des_vec);
        //creat the tuple set and init the src tuple.
        std::multimap<int, Tuple*> TS;
        size_t ts_id = 0;
        Tuple *t_src = new Tuple;
        t_src->id = ts_id++;
        t_src->v = V[topo[0]];
        t_src->Delta[V[topo[0]]->type] = topo[0];
        t_src->res_time = V[topo[0]]->c;
        TS.insert(std::make_pair(topo[0], t_src));

        std::vector<double> ret;
        int Vsnk = topo[topo.size()-1];
        // start main algorithm
        while (!TS.empty()) {
            for (const int& i : topo) {
                std::multimap<int, Tuple*> TS_copy(TS);
                auto range = TS_copy.equal_range(i);
                while (range.first != range.second) {
                    for (const int& suc : V[i]->succ) {
                        Tuple *t = new Tuple;
                        t->id = ts_id;
                        t->v = V[suc];
                        updateDelta(range.first->second->Delta, t);
                        updateResponsetime(range.first->second, t, dag->n_type, V, ivs_vec);
                        if (t->v->id == Vsnk) {
                            ret.push_back(t->res_time);
                            ++path;
                            delete t;
                            continue;
                        }
                        bool dominate_flag = false;
                        auto it = TS.equal_range(t->v->id);
                        while (it.first != it.second) {
                            if (dominate(dag, des_vec, ivs_vec, it.first->second, t)) {
                                dominate_flag = true;
                                delete t;
                                break;
                            } else if (dominate(dag, des_vec, ivs_vec, t, it.first->second)) {
                                TS.erase(it.first++);
                                continue;
                            }
                            ++it.first;
                        }
                        if (!dominate_flag) {
                            TS.insert(std::make_pair(t->v->id, t));
                            ++ts_id;
                        }
                    }
                    auto it = TS.equal_range(i);
                    while (it.first != it.second) {
                        if (it.first->second->id == range.first->second->id){
                            TS.erase(it.first++);
                            break;
                        }
                        ++it.first;
                    }
                    ++range.first;
                }
            }
        }
        // find the max R.
        double res_time = -1;
        for (const double &d : ret) {
            res_time = std::max(d, res_time);

        }
        //std::cout << "\npath = " << path << '\n';
        return res_time;
    }

    double computeResponseTime_Han2019_OLDB(DAG_Type* dag) {
        dag->getLength();
        int max_Ms = 0;
        for (const int& ms : dag->n_type) {
            max_Ms = std::max(max_Ms, ms);
        }
        std::vector<double> vols(dag->n_type.size(), 0);
        for (const auto& item : dag->V){
            vols[item->type] += item->c;
        }
        double ret = (1 - 1/double(max_Ms)) * dag->L;
        for (int i = 0; i < dag->n_type.size(); ++i) {
            ret += vols[i] / double(dag->n_type[i]);
        }
        return ret;
    }

    double computeResponseTime_Han2019_NEWB1(DAG_Type *dag) {
        double ret = 0;
        DAG_Type* _dag = new DAG_Type();
        _dag->copyDAGTask(dag);
        _dag->getVols();
        for (auto& v : _dag->V) {
            v->c *= (1 - 1.0 / double(_dag->n_type[v->type]));
        }
        _dag->getLength();
        ret = _dag->L;
        for (int i = 0; i < _dag->n_type.size(); ++i) {
            _dag->vol_s[i] /= double(_dag->n_type[i]);
        }
        for (double& d : _dag->vol_s) {
            ret += d;
        }
        return ret;
    }
}