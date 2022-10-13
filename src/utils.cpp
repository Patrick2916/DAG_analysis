//
// Created by SUN Yongzheng on 2022/9/23.
//
#include "../include/utils.h"
#include <vector>
#include <cmath>
#include <cstdlib>
#include <algorithm>

int intRand(const int v_min, const int v_max) {
    return rand() % (v_max - v_min + 1) + v_min;
}

double doubleRand(const double v_min, const double v_max) {
    double plus = 0;
    if (v_max - v_min > 1) {
        plus = intRand(int(v_min), int(v_max));
    } else {
        plus = v_min;
    }
    return (rand() % (int)(RAND_MAX * (v_max - v_min))) / double(RAND_MAX) + plus;
}

void UUniFast(int n, double U, std::vector<double>& vectU) {
    double sumU = U;
    double next_sum = 0;
    srand((int)time(NULL));
    for (int i = 1; i < n; i++) {
        double rand_data = pow(rand() / double(RAND_MAX) , 1.0 / (n - i));
        next_sum = sumU * rand_data;
        vectU.push_back(sumU - next_sum);
        sumU = next_sum;
    }
    vectU.push_back(sumU);
}

std::vector<std::pair<std::string, std::string>> separateOnComma(const std::string& line){
    std::string left = "", rigth = "";
    bool read_equal = false;
    std::vector<std::pair<std::string, std::string>> pairs;

    for (const char& c: line) {
        if (c == '\"' || c == ' ') {
            continue;
        } else if (c == '=') {
            read_equal = true;
            continue;
        } else if (c == ',') {
            if (!read_equal || left.empty() || rigth.empty()) {
                ErrorOutput("Invalid dot file.");
            }
            pairs.push_back(std::make_pair(left, rigth));
            left.clear(); rigth.clear(); read_equal = false;
            continue;
        } else {
            if (read_equal) {
                rigth += c;
            } else {
                left += c;
            }
        }
    }

    if (!read_equal || left.empty() || rigth.empty()) {
        ErrorOutput("Invalid dot file.");
    }
    pairs.push_back(std::make_pair(left, rigth));

    return pairs;
}

void parseString2Vector(std::string& s, std::vector<int>& vec_int){
    std::string num = "";
    for (const char& c : s) {
        if (c == '(' || c == ' ' || c == ')') {
            continue;
        } else if (c == '#') {
            vec_int.push_back(std::stoi(num));
            num = "";
        } else {
            num += c;
        }
    }
}

void parseDOTLine(const std::string& line, DOTLine& line_info) {
    size_t start_graph = line.find("{");
    size_t end_graph = line.find("}");
    size_t arrow = line.find("->");
    size_t box = line.find("box");
    size_t start_node = line.find("[");
    size_t end_node  = line.find("]");

    if ( start_graph != std::string::npos) {
        line_info.line_type = DOTLine_t::DOT_BEGIN;
    } else if ( end_graph != std::string::npos) {
        line_info.line_type = DOTLine_t::DOT_END;
    } else if ( arrow != std::string::npos) {
        line_info.line_type = DOTLine_t::DOT_EDGE;

        std::string id_from_str = line.substr(0, arrow);
        line_info.id_from = std::stoi( id_from_str );

        std::string id_to_str = line.substr(arrow + 2, line.find(";") - (arrow + 2));
        line_info.id_to = std::stoi( id_to_str );
    } else if ( box != std::string::npos) {
        line_info.line_type = DOTLine_t::DAG_INFO;
        std::string brackets_content = line.substr(start_node + 1 , end_node - (start_node + 1));
        auto pairs = separateOnComma(brackets_content);
        for(auto p:pairs){
            if(p.first == "D")
                line_info.deadline = std::stof(p.second);
            if(p.first == "Types")
                parseString2Vector(p.second, line_info.n_type);
        }
    } else if ( start_node != std::string::npos && end_node != std::string::npos) {
        line_info.line_type = DOTLine_t::DOT_NODE;

        std::string node_id_str = line.substr(0, start_node);
        line_info.id = std::stoi(node_id_str);

        std::string brackets_content = line.substr(start_node + 1 , end_node - (start_node + 1));
        auto pairs = separateOnComma(brackets_content);
        for(auto p:pairs){
            if(p.first == "c")
                line_info.wcet = std::stof(p.second);
            if(p.first == "type")
                line_info.type = std::stoi(p.second);
            if(p.first == "prio")
                line_info.prio = std::stoi(p.second);
        }
    } else {
        line_info.line_type = DOTLine_t::VOID_LINE;
    }
}