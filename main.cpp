#include <algorithm>
#include <iostream>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <queue>

// #include "my_solution.cpp"

bool Delete(int, int, int);

struct VertexInfo {
    int weight;
    int lvlsCount;
    int primaryLvl, secondaryLvl = -1;
    std::vector<int> primaryEdges, secondaryEdges = {};
    
};
std::ostream& operator << (std::ostream& out, VertexInfo i){
        out << "[" << i.weight << " " << i.lvlsCount << " " << i.primaryLvl << " " << i.secondaryLvl << "]";
        return out;
    }





















using adjacency_type = std::unordered_map<int, std::vector< std::vector<int> >>;

void fill_adj_and_vert_on_lvl(std::vector< adjacency_type >& adj_list,              
                              std::vector< std::vector<int> >& vertexes_on_lvl, 
                              std::vector< VertexInfo >& infos, 
                              int N, int M, int L){

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
}


void delete_empty_groups(std::vector<std::vector<int>>& groups){
    auto iter = begin(groups);
    while (iter != end(groups)){
        if ((*iter).empty()){
            iter = groups.erase(iter);
        }
        else{
            iter++;
        }
    } 
}


void make_group(std::vector<std::vector<int>>& groups,
                std::vector<std::vector<int>>& group_weight, 
                int N, int M, int L){
    groups.push_back({});
    group_weight.push_back(std::vector<int>(L, 0));
}


int get_next_vertex(std::vector<std::vector<int>>& queue, 
                    std::vector<VertexInfo>& infos,
                    int group){
    
    int best_i = -1;
    auto best_v = queue[0][0];
    for (int i=0; i<queue.size(); i++){
        auto v = queue[i][0];
        auto g = queue[i][1];

        if (g == group){
            if (best_i == -1 || infos[best_v].weight < infos[v].weight){
                best_i = i;
                best_v = queue[best_i][0];
            }
        }
    }

    auto vertex = queue[best_i][0];
    queue.erase(begin(queue) + best_i);
    return vertex;
}


void add_primary_vertex_to_group(std::vector<std::vector<int>>& groups, 
                         std::vector<std::vector<int>>& group_weight,
                         int group_i, 
                         int vertex, 
                         int v_weight,
                         int lvl){
    groups[group_i].push_back(vertex);
    group_weight[group_i][lvl] += v_weight;
}


void mark_primary_neighbours( adjacency_type& adj_list_lvl, 
                      std::vector<int>& vertexes_on_lvl_lvl,
                      std::vector<std::vector<int>>& queue,
                      int vertex,
                      int group){

    for (auto edge: adj_list_lvl[vertex]){
        auto neigh = edge[0];
        
        // check in queue
        for (auto iter=begin(queue); iter != end(queue); iter++){
            if ((*iter)[0] == neigh){
                if ((*iter)[1] != group){
                    queue.erase(iter);
                }
                break;
            }
        }

        // check in vertexes_on_lvl_lvl
        auto neigh_iter = std::find(begin(vertexes_on_lvl_lvl), end(vertexes_on_lvl_lvl), neigh);
        if (neigh_iter != end(vertexes_on_lvl_lvl)){
            vertexes_on_lvl_lvl.erase(neigh_iter);
            queue.push_back({neigh, group});
        }
    }
}


void mark_secondary_neighbours( adjacency_type& adj_list_lvl,
                                std::vector<int>& vertexes_on_lvl_lvl,
                                std::vector<std::vector<int>>& queue,
                                int vertex,
                                int group){

    for (auto edge: adj_list_lvl[vertex]){
        auto neigh = edge[0];
        
        // check in queue
        for (auto iter=begin(queue); iter != end(queue); iter++){
            if ((*iter)[0] == neigh){
                if ((*iter)[1] != group){
                    queue.erase(iter);
                }
                break;
            }
        }

        // check in vertexes_on_lvl_lvl
        auto neigh_iter = std::find(begin(vertexes_on_lvl_lvl), end(vertexes_on_lvl_lvl), neigh);
        if (neigh_iter != end(vertexes_on_lvl_lvl)){
            vertexes_on_lvl_lvl.erase(neigh_iter);
            queue.push_back({neigh, group});
        }
    }
}

std::vector<std::vector<int>> init_queue( adjacency_type& adj_list_lvl,              
                             std::vector<int>& vertexes_on_lvl_lvl,
                             std::vector< std::vector<int> >& distributed_vertexes_lvl,
                             int N, int M, int L){
    
    // queue = [ [vert, group] ]
    std::vector< std::vector<int> > queue;
    for (auto vg : distributed_vertexes_lvl){
        auto vertex = vg[0];
        auto group = vg[1];

        for (auto edge: adj_list_lvl[vertex]){
            auto neigh = edge[0];
            // if neigh in queue and it neigh with other group: delete from queue
            for (auto iter=begin(queue); iter != end(queue); iter++){
                if ((*iter)[0] == neigh){
                    if ((*iter)[1] != group){
                        queue.erase(iter);
                    }
                    break;
                }
            }

            // if neigh in vertexes_on_lvl_lvl, then move it to queue [neigh, group]
            auto neigh_vol_iter = std::find(begin(vertexes_on_lvl_lvl), end(vertexes_on_lvl_lvl), neigh);
            if (neigh_vol_iter != end(vertexes_on_lvl_lvl)){
                vertexes_on_lvl_lvl.erase(neigh_vol_iter);
                queue.push_back({neigh, group});
            }
        }
    }

    return queue;                                         
}


void delete_from_queue_group( std::vector< std::vector<int> >& queue,
                              int group){
    
    auto iter = begin(queue);
    while (iter != end(queue)){
        if ((*iter)[1] == group){
            iter = queue.erase(iter);
        }
        else{
            iter++;
        }
    }
}


bool is_group_in_queue(std::vector<std::vector<int>>& queue,
                       int group) {
    for (auto vg : queue){
        if (vg[1] == group){
            return true;
        }
    }
    return false;
}



void add_secondary_vertex_to_group_and_distributed( adjacency_type& adj_secondary_lvl,
                                                    std::vector<int>& vertexes_on_seclvl,
                                                    std::vector<std::vector<int>>& distributed_vertexes_seclvl,
                                                    std::vector<std::vector<int>>& groups,
                                                    std::vector<std::vector<int>>& group_weight,
                                                    std::vector<VertexInfo>& infos,
                                                    int group_i, 
                                                    int vertex, 
                                                    int N, int M, int L){
    
    auto vi = infos[vertex];

    if (vi.lvlsCount != 2 || vi.secondaryLvl < vi.primaryLvl){ 
        return;
    }
    
    auto iter = std::find(begin(vertexes_on_seclvl), end(vertexes_on_seclvl), vertex);
    vertexes_on_seclvl.erase(iter);

    // check group_weight
    if (group_weight[group_i][vi.secondaryLvl] + vi.weight > M){
        return;
    }
    
    // check if neighbours in other groups
    for (auto vg_distributed: distributed_vertexes_seclvl){
        for (auto edge: adj_secondary_lvl[vertex]){
            auto neigh = edge[0];
            if (neigh == vg_distributed[0] && group_i != vg_distributed[1]){
                return;
            }
        }
    }
    
    // secondary vertex satisfie condition
    // add vertex to group
    groups[group_i].push_back(vertex);

    // add weight
    group_weight[group_i][vi.secondaryLvl] += vi.weight;

    // add vertex to distributes_vertexes
    distributed_vertexes_seclvl.push_back({vertex, group_i});
};


void add_secondary_vertex_to_group( std::vector<adjacency_type>& adj_list,
                                    std::vector<std::vector<int>>& vertexes_on_lvl,
                                    std::vector<std::vector<int>>& distributed_vertexes_primlvl,
                                    std::vector<std::vector<int>>& groups,
                                    std::vector<std::vector<int>>& group_weight,
                                    std::vector<VertexInfo>& infos,
                                    std::vector<std::vector<int>>& queue,
                                    int group_i, 
                                    int vertex, 
                                    int N, int M, int L){
    
    auto vi = infos[vertex];

    // check weight
    if (group_weight[group_i][vi.primaryLvl] + vi.weight > M ||
        group_weight[group_i][vi.secondaryLvl] + vi.weight > M){
            return;
        }
                                                    
    // check primary neighbours distributed in other groups
    for (auto vg_distributed: distributed_vertexes_primlvl){
        for (auto edge: adj_list[vi.primaryLvl][vertex]){
            auto neigh = edge[0];
            if (neigh == vg_distributed[0] && group_i != vg_distributed[1]){
                return;
            }
        }
    }

    // can add primary and secondary in group
    // add both in group
    groups[group_i].push_back(vertex);
    groups[group_i].push_back(vertex);
    
    // add both weight
    group_weight[group_i][vi.primaryLvl] += vi.weight;
    group_weight[group_i][vi.secondaryLvl] += vi.weight;

    // delete primary from vertexes_on_lvl[pr_lvl]
    auto iter = std::find(begin(vertexes_on_lvl[vi.primaryLvl]),
                          end(vertexes_on_lvl[vi.primaryLvl]), 
                          vertex);
    vertexes_on_lvl[vi.primaryLvl].erase(iter);

    // add primary to distributed
    distributed_vertexes_primlvl.push_back({vertex, group_i});

    // add secondary neighbours to the queue
    mark_secondary_neighbours(adj_list[vi.secondaryLvl], vertexes_on_lvl[vi.secondaryLvl], queue, vertex, group_i);
}

std::vector<std::vector<int>> Solver(int N, int M, int L, std::vector<VertexInfo> infos){
    std::vector< adjacency_type > adj_list(L);              
    std::vector< std::vector<int> > vertexes_on_lvl(L);
    std::vector< std::vector< std::vector<int> > > distributed_vertexes(L);

    // fill adj_list and vertexes_on_lvl
    fill_adj_and_vert_on_lvl(adj_list, vertexes_on_lvl, infos, N, M, L);
    
    std::vector<std::vector<int>> groups;
    std::vector<std::vector<int>> group_weight;
    make_group(groups, group_weight, N, M, L);

    // examine lvl
    for (int lvl=0; lvl<L; lvl++){
        // take group, add in it all posible vertexes from vertexes_on_lvl
        int group_i = 0;

        // init queue with distributed_vertexes
        std::vector< std::vector<int> > queue = init_queue(adj_list[lvl], vertexes_on_lvl[lvl], distributed_vertexes[lvl], N, M, L);

        while (true){
            // if weight(group) > M: take next group, clear queue, 
            if (group_weight[group_i][lvl] >= M ){
                delete_from_queue_group(queue, group_i);
                
                group_i += 1;
                if (groups.size() == group_i){
                    // create new group
                    make_group(groups, group_weight, N, M, L);
                }
            }

            // if all vertexes examined: leave loop, examine next lvl
            if (queue.empty() && vertexes_on_lvl[lvl].empty()){
                break;
            }

            // if current group not in queue then put vertex in it
            if (!is_group_in_queue(queue, group_i)){
                int best_i = 0;
                auto best_v = vertexes_on_lvl[lvl][best_i];
                for (int i=0; i<vertexes_on_lvl[lvl].size(); i++){
                    auto v = vertexes_on_lvl[lvl][i];
                    if (infos[best_v].weight < infos[v].weight){
                        best_i = i;
                        best_v = v;
                    }
                }
                vertexes_on_lvl[lvl].erase(begin(vertexes_on_lvl[lvl]) + best_i);
                queue.push_back({best_v, group_i});
            }

            // find next suitable vertex in queue, remove from queue not matched
            while (is_group_in_queue(queue, group_i)){
                auto vertex = get_next_vertex(queue, infos, group_i);
                auto vi = infos[vertex];
                
                if (    group_weight[group_i][lvl] + vi.weight <= M &&
                        lvl == infos[vertex].primaryLvl){

                    // if vertex is found: put it in group, put neighbours in queue from vertexes on lvl
                    add_primary_vertex_to_group(groups, group_weight, group_i, vertex, vi.weight, lvl);
                    add_secondary_vertex_to_group_and_distributed(adj_list[vi.secondaryLvl], 
                                                                  vertexes_on_lvl[vi.secondaryLvl],
                                                                  distributed_vertexes[vi.secondaryLvl], 
                                                                  groups,
                                                                  group_weight,
                                                                  infos, 
                                                                  group_i,
                                                                  vertex,
                                                                  N, M, L);

                    // mark neighbours
                    mark_primary_neighbours(adj_list[lvl], vertexes_on_lvl[lvl], queue, vertex, group_i);
                    break;
                }
                else if ( group_weight[group_i][lvl] + vi.weight <= M &&
                          lvl == vi.secondaryLvl){
                    add_secondary_vertex_to_group(adj_list, 
                                                  vertexes_on_lvl,
                                                  distributed_vertexes[vi.primaryLvl],
                                                  groups,
                                                  group_weight,
                                                  infos,
                                                  queue, 
                                                  group_i,
                                                  vertex, 
                                                  N, M, L);
                    break;
                }
            }
        }
    }

    delete_empty_groups(groups);
    return groups;
};


 













std::vector<VertexInfo> infos;
long long int p;
int N, M, L;


bool Delete(int lvl, int v, int u) {
  if (lvl < 0 || lvl >= L ||
      v < 0 || v >= N ||
      u < 0 || u >= N) {
    return true;
  }
  if (infos[v].primaryLvl != lvl && infos[v].secondaryLvl != lvl) {
    return true;
  }
  int edge = (infos[v].primaryLvl == lvl ? infos[v].primaryEdges[u] : infos[v].secondaryEdges[u]);
  if (edge >= 2) {
    p++;
    if (infos[v].primaryLvl == lvl) {
      infos[v].primaryEdges[u] -= 2;
    } else {
      infos[v].secondaryEdges[u] -= 2;
    }
    if (infos[u].primaryLvl == lvl) {
      infos[u].primaryEdges[v] -= 2;
    } else {
      infos[u].secondaryEdges[v] -= 2;
    }
  }
  return edge % 2 == 0;
}

bool is_groups_correct(std::vector< std::vector<int> > groups){
    if (groups.empty()){
        return true;
    }

    for (int i=0; i<groups.size(); i++){
        auto group1 = groups[i];
        if (group1.empty()){
            return false;
        }
    
        std::sort(begin(group1), end(group1));

        std::vector<int> weight_lvl(L, 0);
        for (int i=0; i<group1.size(); i++){
            auto v = group1[i];
            if (i==0 || i>0 && group1[i] != group1[i-1]){
                // primary
                weight_lvl[infos[v].primaryLvl] += infos[v].weight;
            }
            else{
                // secondary
                weight_lvl[infos[v].secondaryLvl] += infos[v].weight;
            }
        }
        for (auto w: weight_lvl){
            if (w > M){
                std::cout << "w>M" << std::endl;
                return false;
            }
        }

        for (int j=i+1; j < groups.size(); j++){
            auto group2 = groups[j];
            for (int v1_i=0; v1_i<group1.size(); v1_i++){
                auto v1 = group1[v1_i];
                for (int v2_i=0; v2_i<group2.size(); v2_i++){
                    auto v2 = group2[v2_i];
                    if (v1 == v2){
                        std::cout << "sec and pr in other groups" << std::endl;
                        return false;
                    }

                    if (v1_i==0 || v1_i>0 && v1 != group1[v1_i - 1]){
                        // primary
                        // if v2 in primaryEdges[v1]
                        if (infos[v1].primaryEdges[v2] != 0){
                            std::cout << "v2 in v1.primaryEdges: (lvl, v1, v2)" << infos[v1].secondaryLvl << " " << v1 << " "<<v2  << std::endl;
                            return false;
                        }
                    }
                    else{
                        // secondary
                        if (infos[v1].secondaryEdges[v2] != 0){
                            std::cout << "v2 in v1.secondaryEdges: (lvl, v1, v2)" << infos[v1].secondaryLvl << " " << v1 << " "<<v2 << std::endl;
                            return false;
                        }
                    }

                }
            }
        }
    }
    return true;
}

void print_adj_matr(){
    std::ofstream fout{"edges.txt"};
    for(auto v=0; v<infos.size(); v++){
        for (auto n=0; n<infos[v].primaryEdges.size(); n++){
            if (infos[v].primaryEdges[n] != 0){
                fout << infos[v].primaryLvl << "_" << v << "-" << infos[v].primaryLvl << "_" << n << std::endl;
            }
        }
        if (infos[v].lvlsCount == 2){
            for (auto n=0; n<infos[v].primaryEdges.size(); n++){
                if (infos[v].secondaryEdges[n] != 0){
                    fout << infos[v].secondaryLvl << "_" << v << "-" << infos[v].secondaryLvl << "_" << n << std::endl;
                }
            }
        }
    }
}

int main() {
  std::ifstream fin{ "unix/open.txt" }; // "test1" "test2" "unix/open.txt"
  int TESTS_COUNT;
  fin >> TESTS_COUNT;
  int READ_COUNT = 300;
  int cnt_true{0}, cnt_false{0};

  for (int test_num=0;test_num < READ_COUNT && test_num < TESTS_COUNT; test_num++){
    // std::cout << "############# " << test_num << std::endl;
    infos.clear();
    N, M, L;
    fin >> N >> M >> L;
    for (int i=0; i<N; i++){
        VertexInfo vinf;
        fin >> vinf.weight >> vinf.lvlsCount >> vinf.primaryLvl;
        if (vinf.lvlsCount == 2){
            fin >> vinf.secondaryLvl;
        }
        for (int edge_i=0; edge_i<N; edge_i++ ){
            int type;
            fin >> type;
            vinf.primaryEdges.push_back(type);
        }
        if (vinf.lvlsCount == 2){
            for (int edge_i=0; edge_i<N; edge_i++ ){
                int type;
                fin >> type;
                vinf.secondaryEdges.push_back(type);
            }
        }
        infos.push_back(vinf);

    }
    
    std::vector<VertexInfo> convertedInfos = infos;
    for (int v = 0; v < N; ++v) {
      for (int& e : convertedInfos[v].primaryEdges) {
        if (e >= 2) e = -1;
      }
      for (int& e : convertedInfos[v].secondaryEdges) {
        if (e >= 2) e = -1;
      }
    }

    // for (auto i : convertedInfos){
    //     std::cout << i<< std::endl;
    // }

    std::vector<std::vector<int>> result = Solver(N, M, L, convertedInfos);
    
    if (!is_groups_correct(result)){
        cnt_false++;
        print_adj_matr();
        std::cout << N << " " << M << " "<< L << std::endl;
        std::cout << result.size() << std::endl;
    for (auto group : result) {
        std::sort(group.begin(), group.end());
        std::cout << group.size() << " ";
        for (size_t i = 0; i < group.size(); ++i) {
            std::cout << group[i];
            if (i + 1 != group.size()) {
                std::cout << " ";
            }
        }
        std::cout << std::endl;
    }
        // return 0;

    // std::cout << std::boolalpha << is_groups_correct(result) << std::endl;

    // std::cout << result.size() << std::endl;
    // for (auto group : result) {
    //     std::sort(group.begin(), group.end());
    //     std::cout << group.size() << " ";
    //     for (size_t i = 0; i < group.size(); ++i) {
    //         std::cout << group[i];
    //         if (i + 1 != group.size()) {
    //             std::cout << " ";
    //         }
    //     }
    //     std::cout << std::endl;
    // }
    // std::cout << p << std::endl;
  
    
    }
    else{
        cnt_true++;
    }







//   N = 5, M = 10, L = 3;
//   infos = {VertexInfo{/*weight=*/4, /*lvlsCount=*/2, /*primaryLvl=*/1, /*secondaryLvl=*/0,
//                    /*primaryEdges=*/{0, 0, 0, 0, 2}, /*secondaryEdges=*/{0, 1, 0, 0, 0}},
//            VertexInfo{/*weight=*/4, /*lvlsCount=*/2, /*primaryLvl=*/2, /*secondaryLvl=*/0,
//                    /*primaryEdges=*/{0, 0, 0, 0, 0}, /*secondaryEdges=*/{1, 0, 0, 0, 0}},
//            VertexInfo{/*weight=*/7, /*lvlsCount=*/2, /*primaryLvl=*/0, /*secondaryLvl=*/2,
//                    /*primaryEdges=*/{0, 0, 0, 0, 0}, /*secondaryEdges=*/{0, 0, 0, 0, 3}},
//            VertexInfo{/*weight=*/4, /*lvlsCount=*/1, /*primaryLvl=*/1, /*secondaryLvl=*/-1,
//                    /*primaryEdges=*/{0, 0, 0, 0, 3}, /*secondaryEdges=*/{}},
//            VertexInfo{/*weight=*/4, /*lvlsCount=*/2, /*primaryLvl=*/1, /*secondaryLvl=*/2,
//                    /*primaryEdges=*/{2, 0, 0, 3, 0}, /*secondaryEdges=*/{0, 0, 3, 0, 0}}};

//   std::vector<VertexInfo> convertedInfos = infos;
//   for (int v = 0; v < N; ++v) {
//     for (int& e : convertedInfos[v].primaryEdges) {
//       if (e >= 2) e = -1;
//     }
//     for (int& e : convertedInfos[v].secondaryEdges) {
//       if (e >= 2) e = -1;
//     }
//   }

//   std::vector<std::vector<int>> result = Solver(N, M, L, convertedInfos);

//   std::cout << result.size() << std::endl;
//   for (auto group : result) {
//     std::sort(group.begin(), group.end());
//     std::cout << group.size() << " ";
//     for (size_t i = 0; i < group.size(); ++i) {
//       std::cout << group[i];
//       if (i + 1 != group.size()) {
//         std::cout << " ";
//       }
//     }
//     std::cout << std::endl;
//   }
//   std::cout << p << std::endl;
//   return 0;
  }
  
  std::cout << "(true, false)" << cnt_true << " " << cnt_false<<std::endl;
}