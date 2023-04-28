// #include <cassert>
#include <vector>
#include <unordered_map>
#include <algorithm>
// #include <iostream>
#include <vector>
// #include <fstream>
#include <queue>

struct VertexInfo {
    int weight;
    int lvlsCount;
    int primaryLvl, secondaryLvl = -1;
    std::vector<int> primaryEdges, secondaryEdges = {};
};

bool Delete(int/* lvl*/, int/* v*/, int/* u*/);



using adjacency_type = std::unordered_map<int, std::vector< std::vector<int> >>;

std::vector<std::vector<int>> Solver(int N, int M, int L, std::vector<VertexInfo> infos) {

std::vector< adjacency_type > adj_list(L);              
    std::vector< std::vector<int> > vertexes_on_lvl(L);

    // fill adj_list and vertexes_on_lvl
    for (int i=0; i<infos.size(); i++){
        auto& cur_vi = infos[i];
        vertexes_on_lvl[cur_vi.primaryLvl].push_back(i);
        if (cur_vi.lvlsCount == 2){
            vertexes_on_lvl[cur_vi.secondaryLvl].push_back(i);
        }
        for (int j=0; j<N; j++){
            if (cur_vi.primaryEdges[j] != 0){
                adj_list[cur_vi.primaryLvl][i].push_back({j, cur_vi.primaryEdges[j]});
            }

            if (cur_vi.lvlsCount == 2){
                if (cur_vi.secondaryEdges[j] != 0){
                    adj_list[cur_vi.secondaryLvl][i].push_back({j, cur_vi.secondaryEdges[j]});
                } 
            }
        }
    }    
    
    std::vector< std::vector<int> > groups;
    std::vector< std::vector<int> > group_weight;
    groups.push_back({});
    group_weight.push_back(std::vector<int>(L, 0));
    for (int lvl=0; lvl < L; lvl++){
        int group_ind = 0;
        std::queue<int> queue;
        // queue.push(vertexes_on_lvl[lvl].back());
        // vertexes_on_lvl[lvl].pop_back();
        while (vertexes_on_lvl[lvl].size() != 0){
            if (group_weight[group_ind][lvl] >= M){
                if (group_ind + 1 == groups.size()){
                    groups.push_back({});
                    group_weight.push_back(std::vector<int>(L, 0));
                }

                queue = {};        
                group_ind += 1;        
            }
            if (queue.size() == 0){
                queue.push(vertexes_on_lvl[lvl].back());
                vertexes_on_lvl[lvl].pop_back();
            }

            auto vertex = queue.front();
            queue.pop();
            bool v_found = false;
            while (true){
                // take next vertex
                if (    group_weight[group_ind][lvl] + infos[vertex].weight <= M &&
                        infos[vertex].primaryLvl == lvl){
                    v_found = true;
                    break;
                }
 
                if (queue.empty()){
                    break;
                }
                vertex = queue.front();
                queue.pop();
            }

            if (v_found){
                // add v in group
                groups[group_ind].push_back(vertex);
                group_weight[group_ind][lvl] += infos[vertex].weight;

                // add v neighbours in queue
                for (auto edge: adj_list[lvl][vertex]){
                    auto neighbour = edge[0];
                    auto neighbour_iter = std::find(begin(vertexes_on_lvl[lvl]), end(vertexes_on_lvl[lvl]), neighbour);
                    if (neighbour_iter != end(vertexes_on_lvl[lvl])){
                        queue.push(neighbour);
                        vertexes_on_lvl[lvl].erase(neighbour_iter);
                    }
                }
            }                  
        }
    }
    

    // // print adj_list
    // for (int lvl=0; lvl<adj_list.size(); lvl++){
    //     std::cout << lvl << std::endl;
    //     for (auto pr: adj_list[lvl]){
    //         std::cout << pr.first << "--> ";
    //         for (auto vi: pr.second){
    //             std::cout << "(" << vi[0] << ", " << vi[1] << "), ";
    //         }
    //         std::cout << std::endl;
    //     }
    // }
    

    return groups;

//   /* coin is counted only once */
//   Delete(/*lvl=*/1, /*v=*/0, /*u=*/4);
//   Delete(/*lvl=*/1, /*v=*/0, /*u=*/4);

//   /* incorrect deletes */
//   assert(Delete(/*lvl=*/-1, /*v=*/-1, /*u=*/-1));
//   assert(Delete(/*lvl=*/2, /*v=*/0, /*u=*/1));

//   /* correct deletes on stable edges */
//   assert(Delete(/*lvl=*/1, /*v=*/0, /*u=*/0));
//   assert(Delete(/*lvl=*/0, /*v=*/1, /*u=*/2));
//   assert(!Delete(/*lvl=*/0, /*v=*/0, /*u=*/1));

//   Delete(/*lvl=*/2, /*v=*/2, /*u=*/4);
//   return {{0, 0, 1, 1}, {2, 2}, {3}};
}