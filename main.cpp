#include <iostream>
#include <algorithm>
#include <vector>
#include "include/DAGTask.h"
#include "include/utils.h"
#include "include/DAGAlgorithm.h"
#include <ctime>
#include <fstream>
#include <unistd.h>

int main() {
    DAG::DAG_Type *dag = new DAG::DAG_Type();
    dag->generateDAGWithType();
    dag->saveAsDot("./test.dot");
    double result = DAG::computeResponseTime_Han2019_NEWB2(dag);
}
