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


int get_next_vertex(std::vector<int>& queue){
    auto vertex = queue.front();
    queue.erase(begin(queue));
    return vertex;
}


void add_vertex_to_group(std::vector<std::vector<int>>& groups, 
                         std::vector<std::vector<int>>& group_weight,
                         int group_i, 
                         int vertex, 
                         int v_weight,
                         int lvl){
    groups[group_i].push_back(vertex);
    group_weight[group_i][lvl] += v_weight;
}


void mark_neighbours( adjacency_type& adj_list_lvl, 
                      std::vector<int>& vertexes_on_lvl_lvl,
                      std::vector<int>& queue,
                      int vertex){

    for (auto edge: adj_list_lvl[vertex]){
        auto neigh = edge[0];
        auto neigh_iter = std::find(begin(vertexes_on_lvl_lvl), end(vertexes_on_lvl_lvl), neigh);
        if (neigh_iter != end(vertexes_on_lvl_lvl)){
            vertexes_on_lvl_lvl.erase(neigh_iter);
            queue.push_back(neigh);
        }
    }
}


std::vector<std::vector<int>> Solver(int N, int M, int L, std::vector<VertexInfo> infos){
    std::vector< adjacency_type > adj_list(L);              
    std::vector< std::vector<int> > vertexes_on_lvl(L);

    // fill adj_list and vertexes_on_lvl
    fill_adj_and_vert_on_lvl(adj_list, vertexes_on_lvl, infos, N, M, L);
    
    std::vector<std::vector<int>> groups;
    std::vector<std::vector<int>> group_weight;
    make_group(groups, group_weight, N, M, L);

    // examine lvl
    for (int lvl=0; lvl<L; lvl++){
        // take group, add in it all posible vertexes from vertexes_on_lvl
        int group_i = 0;
        std::vector<int> queue;
        while (true){
            // if weight(group) > M: take next group, clear queue, 
            if (group_weight[group_i][lvl] >= M){
                group_i += 1;
                queue = {};
                if (groups.size() == group_i){
                    // create new group
                    make_group(groups, group_weight, N, M, L);
                }
            }

            // if all vertexes examined: leave loop, examine next lvl
            if (queue.empty() && vertexes_on_lvl[lvl].empty()){
                break;
            }

            // if queue.empty() put vertex in it
            if (queue.empty()){
                auto v = vertexes_on_lvl[lvl].back();
                vertexes_on_lvl[lvl].pop_back();
                queue.push_back(v);
            }

            // find next suitable vertex in queue, remove from queue not matched
            while (!queue.empty()){
                auto vertex = get_next_vertex(queue);
                auto vi = infos[vertex];
                
                if (    group_weight[group_i][lvl] + vi.weight <= M &&
                        lvl == infos[vertex].primaryLvl){

                    // if vertex is found: put it in group, put neighbours in queue from vertexes on lvl
                    add_vertex_to_group(groups, group_weight, group_i, vertex, vi.weight, lvl);

                    // mark neighbours
                    mark_neighbours(adj_list[lvl], vertexes_on_lvl[lvl], queue, vertex);
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