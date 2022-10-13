//
// Created by SUN Yongzheng on 2022/9/22.
//
#include "../include/DAGTask.h"
#include "../include/utils.h"
#include <fstream>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <random>

namespace DAG {

    void DAGTask::makeItADAG() {
        std::vector<int> noPred;
        std::vector<int> noSucc;
        for (auto& v : V) {
            if (v->pred.empty()) {
                noPred.push_back(v->id);
            }
        }

        if (noPred.size() > 1) {
            for (int i = 1; i < noPred.size(); ++i) {
                V[noPred[0]]->succ.insert(V[noPred[i]]->id);
                V[noPred[i]]->pred.insert(V[noPred[0]]->id);
            }
        }

        for (auto& v : V) {
            if (v->succ.empty()) {
                noSucc.push_back(v->id);
            }
        }
        if (noSucc.size() > 1) {
            for (int i = 1; i < noSucc.size(); ++i) {
                V[noSucc[0]]->pred.insert(V[noSucc[i]]->id);
                V[noSucc[i]]->succ.insert(V[noSucc[0]]->id);
            }
        }

    }

    /*
     * D = 100；
     * type |S| = [2, 5], Ms = [2, 10];
     * |V| = [20, 100];
     * random_number = [0, 1], pr = [0.1, 1];
     * total number of vertices of each type to be
     * The total utilization U
     * vol(G) = U * 100;
     * WCET to each type
     * for each point of each experiment, we randomly generate 1000 DAG tasks.
     * UUniFast to distribute WECT to every vertex.
     */
    void DAG_Type::generateDAGWithType(double set_u, int set_nv, double set_pr, int set_s) {
        srand((int)time(NULL));
        D = 100;
        int N_type = set_s == -1 ? intRand(2, 5) : set_s;
        n_type.resize(N_type);
        int sum_Ms = 0;
        for (int& m : n_type) {
            m = intRand(2, 10);
            sum_Ms += m;
        }

        n_V = set_nv == -1 ? intRand(20, 80) : set_nv;
        pr = set_pr == -1 ? doubleRand(0.1, 1) : set_pr;
        std::vector<int> N_ver_type(N_type);
        for (int i = 0; i < N_type; ++i) {
            N_ver_type[i] = std::ceil(double(n_V * n_type[i]) / sum_Ms);
        }

        u = set_u == -1 ? doubleRand(1.0, sum_Ms / 3) : set_u;
        vol = u * D;
        vol_s.resize(N_type);
        for (int i = 0; i < N_type; ++i) {
            vol_s[i] = double(n_type[i]) / sum_Ms;
        }

        std::vector<std::vector<double>> wect_ver;
        for (int i = 0; i < N_type; ++i) {
            std::vector<double> vectU;
            UUniFast(N_ver_type[i], vol_s[i], vectU);
            for (double& d : vectU) {
                d *= vol;
            }
            wect_ver.push_back(vectU);
        }

        for (int i = 0; i < N_type; ++i) {
            for (int j = 0; j < wect_ver[i].size(); ++j) {
                V.push_back(new DAGVer(i, wect_ver[i][j]));
            }
        }

        //shuffle the vector of vertics and assgin the ids.
        std::random_device rd;
        std::mt19937 rng(rd());
        std::shuffle(V.begin(), V.end(), rng);
        for (int i = 0; i < V.size(); ++i) {
            V[i]->id = i;
        }

        n_V = V.size();
        std::vector<int> topo;
        for (int i = 0; i < n_V; ++i) {
            topo.push_back(i);
        }
        std::mt19937 rngTopo(rd());
        std::shuffle(topo.begin(), topo.end(), rngTopo);

        for (int i = 0; i < n_V; ++i) {
            for (int j = i + 1; j < n_V; ++j) {
                if (doubleRand(0, 1) < pr) {
                    V[topo[i]]->succ.insert(topo[j]);
                    V[topo[j]]->pred.insert(topo[i]);
                }
            }
        }

        makeItADAG();
    }

    void DAG_Type::printDAGInfo() {
        std::cout << "Deadline = " << D << "\nPeriod = " << T << '\n';
        std::cout << "n_type = [";
        for (const int& i : n_type) {
            std::cout << i << ' ';
        }
        std::cout << "]\n";
        for (auto& item : V) {
            std::cout << "id = " <<item->id << "\tc = " << item->c << "\ttype = "<< item->type << '\n'
                      << "pred = [";
            for (const int& pre : item->pred) {
                std::cout << pre << ' ';
            }
            std::cout << "]\nsucc = [";
            for (const int& suc : item->succ) {
                std::cout << suc << ' ';
            }
            std::cout << "]\n";
        }
    }


    void DAG_Type::readTaskFromDOT(const std::string &filename){
        std::ifstream dot_dag(filename);
        std::string line;
        int node_count = 0;
        int form_id, to_id;

        std::map<int, int> id_pos;
        while (std::getline(dot_dag, line)) {
            DOTLine dot_line;
            parseDOTLine(line, dot_line);

            if (dot_line.line_type == DOTLine_t::DAG_INFO) {
                n_type = dot_line.n_type;
                T = dot_line.period;
                D = dot_line.deadline;
            } else if (dot_line.line_type == DOTLine_t::DOT_NODE) {
                DAGVer *v = new DAGVer();
                v->id = node_count;
                v->c = dot_line.wcet;
                id_pos[dot_line.id] = node_count;
                if (dot_line.type != -1) {
                    v->type = dot_line.type;
                }
                V.push_back(v);
                node_count++;
            } else if (dot_line.line_type == DOTLine_t::DOT_EDGE){
                form_id = id_pos[dot_line.id_from];
                to_id = id_pos[dot_line.id_to];
                V[form_id]->succ.insert(to_id);
                V[to_id]->pred.insert(form_id);
            }
        }
        dot_dag.close();
        n_V = V.size();
        for (const auto& v: V) {
            vol += v->c;
        }
        u = vol / D;
    }

    void DAG_Type::saveAsDot(const std::string &filename){
        std::ofstream of;
        of.open(filename, std::ios::out);
        if (!of){
            ErrorOutput("file error!")
        }
        of << "digraph Task {\n";
        of << "i [shape=box, label=\"inf, D=" << D << ", Types="<< '(';
        for (int i : n_type) {
            of << i << "# ";
        }
        of << ")\"]; \n";
        for (const auto &item: V){
            of << item->id
               << " [label=\"" << item->id << ", c="<< item->c  << ", type=" << item->type << ", prio=" << item->prio << "\"";
            of << "];\n";
        }

        for (const auto &item: V){
            for(auto suc: item->succ)
                of << item->id << " -> " << suc << ";\n";
        }
        of << "}";
        of.close();
    }
}