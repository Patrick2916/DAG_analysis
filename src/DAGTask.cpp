//
// Created by SUN Yongzheng on 2022/9/20.
//
#include "../include/DAGTask.h"
#include "../include/utils.h"

namespace DAG {
    /*
     *
     */
    void DAGTask::getLength() {
        getTopologicalOrder();
        for (const int& index : topoOrder) {
            double max = 0;
            for (const int& pre : V[index]->pred) {
                max = std::max(V[pre]->l, max);
            }
            V[index]->l = max + V[index]->c;
        }
        L = V[topoOrder[n_V-1]]->l;
    }

    /*
     *
     */
    bool DAGTask::checkIndexAndIdsAreEqual() {
        for(int i = 0; i < V.size(); ++i)
            if(V[i]->id != i)
                return false;
        return true;
    }

    /*
     * using breathe first method to find if tar is an ancestor of ver.
     */
    bool DAGTask::isAncestor(DAG::DAGVer *ver, DAG::DAGVer *tar) {
        if (ver == nullptr || tar == nullptr) {
            return false;
        }
        std::queue<int> que;
        que.push(ver->id);
        int index;
        while (!que.empty()) {
            index = que.front();
            que.pop();
            for (const int& pre : V[index]->pred) {
                if (tar->id == pre) {
                    return true;
                }
                que.push(pre);
            }
        }
        return false;
    }

    /*
     * if you want to know if ver is tar's descendant, you could check if tar is ver's ancestor.
     */
    bool DAGTask::isDescendant(DAG::DAGVer *ver, DAG::DAGVer *tar) {
        if (ver == nullptr || tar == nullptr) {
            return false;
        }
        return isAncestor(tar, ver);

    }


    /*
     *
     */
    void DAGTask::getAncestorsSet(DAG::DAGVer *ver, std::set<int>& ans) {
        if (ver == nullptr) {
            return;
        }
        std::queue<int> que;
        que.push(ver->id);
        int index;
        while (!que.empty()) {
            index = que.front();
            que.pop();
            for (const int& pre : V[index]->pred) {
                ans.insert(pre);
                que.push(pre);
            }
        }
    }

    /*
     *
     */
    void DAGTask::getDescendantsSet(DAG::DAGVer *ver, std::set<int>& des) {
        if (ver == nullptr) {
            return;
        }
        std::queue<int> que;
        que.push(ver->id);
        int index;
        while (!que.empty()) {
            index = que.front();
            que.pop();
            for (const int& suc : V[index]->succ) {
                des.insert(suc);
                que.push(suc);
            }
        }
    }

    /*
     *
     */
    void DAGTask::getInterferenceSet(DAGVer* ver, std::set<int>& ivs) {
        std::set<int> ans;
        std::set<int> des;
        getAncestorsSet(ver, ans);
        getDescendantsSet(ver, des);
        std::set<int> union_pred_succ;
        setUnion(ans, des, union_pred_succ);
        union_pred_succ.insert(ver->id);
        int index = 0;
        for (auto iter = union_pred_succ.begin(); index < n_V;) {
            if (*iter == index) {
                ++iter;
                ++index;
            } else {
                ivs.insert(index++);
            }
        }
    }

    /*
     *
     */
    void DAGTask::getTopologicalOrder() {
        if (!checkIndexAndIdsAreEqual()) {
            ErrorOutput("index aren't equal to the ids");
        }
        topoOrder.clear();
        std::vector<int> in_degree(n_V, 0);
        for (int i = 0; i < n_V; ++i) {
            in_degree[i] = V[i]->pred.size();
        }
        while (std::find(in_degree.begin(), in_degree.end(), 0) != in_degree.end()) {
            for (int i = 0; i < in_degree.size(); ++i) {
                if (in_degree[i] == 0) {
                    topoOrder.push_back(i);
                    in_degree[i] = -1;
                    for(const int& suc : V[i]->succ) {
                        --in_degree[suc];
                    }
                }
            }
        }
        if (topoOrder.size() != V.size()) {
            ErrorOutput("There are some vertics don't in the topoOrder");
        }
    }

    void DAG_Type::getAllAncestorsSet(std::vector<std::set<int>>& ans_vec) {
        ans_vec.resize(n_V);
        for (int i = 1; i < n_V; ++i) {
            setUnion(ans_vec[topoOrder[i]], V[topoOrder[i]]->pred, ans_vec[topoOrder[i]]);
            for (const int& pre : V[topoOrder[i]]->pred) {
                setUnion(ans_vec[topoOrder[i]], ans_vec[pre], ans_vec[topoOrder[i]]);
            }
        }
    }

    void DAG_Type::getAllDescendantsSet(std::vector<std::set<int>>& des_vec) {
        des_vec.resize(n_V);
        for (int i = n_V - 2; i >= 0; --i) {
            setUnion(des_vec[topoOrder[i]], V[topoOrder[i]]->succ, des_vec[topoOrder[i]]);
            for (const int& suc : V[topoOrder[i]]->succ) {
                setUnion(des_vec[topoOrder[i]], des_vec[suc], des_vec[topoOrder[i]]);
            }
        }
    }

    void DAG_Type::getAllInterferenceSet(std::vector<std::set<int>>& ivs_vec,
                                         std::vector<std::set<int>>& ans_vec,
                                         std::vector<std::set<int>>& des_vec) {
        ivs_vec.resize(n_V);
        if (ans_vec.empty()) {
            getAllAncestorsSet(ans_vec);
        }
        if (des_vec.empty()) {
            getAllDescendantsSet(des_vec);
        }
        std::set<int> all;
        for (int i = 0; i < n_V; ++i) {
            all.insert(i);
        }
        for (int i = 0; i < n_V; ++i) {
            std::set<int> uni_ans_des;
            setUnion(ans_vec[topoOrder[i]], des_vec[topoOrder[i]], uni_ans_des);
            uni_ans_des.insert(topoOrder[i]);
            setDifference(all, uni_ans_des, ivs_vec[topoOrder[i]]);
            for (auto iter = ivs_vec[topoOrder[i]].begin(); iter!= ivs_vec[topoOrder[i]].end();) {
                if (V[(*iter)]->type != V[topoOrder[i]]->type) {
                    ivs_vec[topoOrder[i]].erase(iter++);
                } else {
                    iter++;
                }
            }
        }
    }

    void DAG_Type::getVols() {
        vol_s.clear();
        vol_s.resize(n_type.size(), 0);
        for (auto& v : V) {
            vol_s[v->type] += v->c;
        }
    }
}
