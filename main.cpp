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


using adjacency_type = std::unordered_map<int, std::vector< std::vector<int> >>;

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
        while ( !vertexes_on_lvl[lvl].empty() || !queue.empty()){
            if (group_weight[group_ind][lvl] >= M){
                if (group_ind + 1 == groups.size()){
                    groups.push_back({});
                    group_weight.push_back(std::vector<int>(L, 0));
                }

                queue = {};        
                group_ind += 1;        
            }
            
            if (queue.empty() && !vertexes_on_lvl[lvl].empty()){
                queue.push(vertexes_on_lvl[lvl].back());
                vertexes_on_lvl[lvl].pop_back();
            }

            if (vertexes_on_lvl[lvl].empty() && queue.empty()){
                break;
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
    

    // delete empty groups
    auto iter = begin(groups);
    while (iter != end(groups)){
        if ((*iter).empty()){
            iter = groups.erase(iter);
        }
        else{
            iter++;
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
    
    // std::cout << std::boolalpha << is_groups_correct(groups) << std::endl;

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
  int READ_COUNT = 100;

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
        print_adj_matr();
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
        return 0;

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
  }}