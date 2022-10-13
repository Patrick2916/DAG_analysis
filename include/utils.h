//
// Created by SUN Yongzheng on 2022/9/21.
//

#ifndef DAG_ANALYSIS_UTILS_H
#define DAG_ANALYSIS_UTILS_H

#include <iostream>
#include <cstring>
#include <set>
#include <vector>
#include <queue>
#include <algorithm>

#define ErrorOutput(err) {  \
    std::cerr << std::string(err) << '\n'  \
    << __FILE__ << ':' << __LINE__ << '\n'  \
    << "Aborting...\n";  \
    exit(EXIT_FAILURE);  \
}


enum DOTLine_t {DOT_BEGIN, DOT_END, DOT_NODE, DOT_EDGE, DAG_INFO, VOID_LINE};

struct DOTLine {
    DOTLine_t line_type;
    std::vector<int> n_type;
    int id = -1;
    int id_from = -1;
    int id_to = -1;
    int type = -1;
    int prio =-1;
    float wcet = 0;
    float period = 0;
    float deadline = 0;
    int mode = 0;
};

void parseString2Vector(std::string& s, std::vector<int>& vec_int);

template<typename T>
void setInter(std::set<T>& a, const std::set<T>& b, std::set<T>& c) {
    if (a.empty() || b.empty()) {
        c.clear();
        return;
    } else {
        std::set_intersection(a.begin(), a.end(),b.begin(), b.end(), std::inserter(c, c.begin()));
    }
}

template<typename T>
void setDifference(std::set<T>& a, const std::set<T>& b, std::set<T>& c) {
    if (a.empty()) {
        return;
    } else if (b.empty()) {
        std::copy(a.begin(), a.end(), std::inserter(c, c.begin()));
    } else {
        std::set_difference(a.begin(), a.end(), b.begin(), b.end(), std::inserter(c, c.begin()));
    }
}

template<typename T>
void setDifference(std::vector<T>& a, std::set<T>& b, std::set<T>& c) {
    if (a.empty()) {
        return;
    } else if (b.empty()) {
        std::copy(a.begin(), a.end(), std::inserter(c, c.begin()));
    } else {
        std::set_difference(a.begin(), a.end(), b.begin(), b.end(), std::inserter(c, c.begin()));
    }
}

template<typename T>
void setUnion(std::set<T>& a, const std::set<T>& b, std::set<T>& c) {
    if (a.empty() && b.empty()) {
        c.clear();
        return;
    } else if (a.empty()) {
        std::copy(b.begin(), b.end(), std::inserter(c, c.begin()));
    } else if (b.empty()) {
        std::copy(a.begin(), a.end(), std::inserter(c, c.begin()));
    } else {
        std::set_union(a.begin(), a.end(),b.begin(), b.end(), std::inserter(c, c.begin()));
    }
}
int intRand(const int v_min, const int v_max);

double doubleRand(const double v_min, const double v_max);

std::vector<std::pair<std::string, std::string>> separateOnComma(const std::string& line);

void parseDOTLine(const std::string& line, DOTLine& line_info);

void UUniFast(int n, double U, std::vector<double>& vectU);




#endif //DAG_ANALYSIS_UTILS_H
