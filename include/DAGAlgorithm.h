//
// Created by SUN Yongzheng on 2022/9/27.
//

#ifndef DAG_ANALYSIS_DAGALGORITHM_H
#define DAG_ANALYSIS_DAGALGORITHM_H

namespace DAG {
    double computeResponseTime_Han2019_OLDB(DAG_Type* dag);
    double computeResponseTime_Han2019_NEWB1(DAG_Type *dag);
    double computeResponseTime_Han2019_NEWB2(DAG_Type *dag);
}

#endif //DAG_ANALYSIS_DAGALGORITHM_H
