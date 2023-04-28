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

// class I_for_heap{
//     private:
//         // adjacency_type& adj;
//         int index;
//     public:
//         I_for_heap(int i): index{i}{};
//         bool operator<(const I_for_heap& rhs){
//             return 
//         }
// };




std::vector<std::vector<int>> Solver(int N, int M, int L, std::vector<VertexInfo> infos){
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

bool is_group_correct(std::vector<int> group){
    std::vector<int> weight_lvl(L, 0);
    std::sort(begin(group), end(group));
    for (int i=0; i<group.size(); i++){
        auto v = group[i];
        if (i>0 && group[i] != group[i-1]){
            // primary
            weight_lvl[infos[v].primaryLvl] += infos[v].weight;
        }
        else{
            // secondary
        }
    }
}

int main() {
  std::ifstream fin{ "test1" }; // "test1" "unix/open.txt"
  int TESTS_COUNT;
  fin >> TESTS_COUNT;
  int READ_COUNT = 1;

  for (int test_num=0;test_num < READ_COUNT && test_num < TESTS_COUNT; test_num++){
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
    std::cout << p << std::endl;
  
    
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