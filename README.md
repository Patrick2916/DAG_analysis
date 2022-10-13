# DAG_analysis
this repo offer some tools for DAG analysis. you could call these functions and develop them easily. it only implements a DAG scheduling algorithm by far, and I will add more algorithms in the future.
<br>
Some of the ideas form https://github.com/mive93/dag-scheduling
## How to compile
```
git clone https://github.com/Patrick2916/DAG_analysis
cd DAG_analysis
mkdir build && cd build
cmake ..
make
```
## input and output
this library could generate DAGs with different parameters and could save them, it could also accept exist DAGs. The acceptable input and output file format is DOT document.if you want to draw the .dot document in a picture, install graphviz via the command on Linux:
```
sudo apt-get install graphviz
```
and then use this command to get a png:
```
dot -Tpng test.dot > test.png
```
## algorithm
[1] Meiling Han et al. “Response time bounds for typed dag parallel tasks on heterogeneous multi-cores”. (IEEE Transactions on Parallel and Distributed Systems 2019)
## problem
I don't know why when the number of vertices in a DAG becomes too much, the Han2019_NEW-B-2 algorithm in this code executes much slower than the paper evaluation, if you find the solution, contract with me, thanks!
