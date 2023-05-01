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
namespace all_var{
    std::vector< adjacency_type > adj_list;              
    std::vector< std::vector<int> > vertexes_on_lvl;
    std::vector< std::vector< std::vector<int> > > distributed_vertexes;    
    std::vector<std::vector<int>> groups;
    std::vector<std::vector<int>> group_weight;
    std::vector<std::vector<int>> queue; 
    std::vector<VertexInfo> infos;
    int group_i;
    int lvl;
    int vertex;
    int N;
    int M;
    int L;
}

using namespace all_var;


void init_all_var_with_L(int nn, int mm, int ll){
    adj_list.clear();
    adj_list.resize(ll);
    vertexes_on_lvl.clear();
    vertexes_on_lvl.resize(ll);
    distributed_vertexes.clear();
    distributed_vertexes.resize(ll);
    groups.clear();
    group_weight.clear();
    N = nn;
    M = mm;
    L = ll;
}


void fill_adj_and_vert_on_lvl(){
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

 
void make_group(){
    groups.push_back({});
    group_weight.push_back(std::vector<int>(L, 0));
}


void take_next_group(){ 
    // take most popular in queue group
    
    if (queue.empty()){
        // take group with min weight on lvl
        int min_i = 0;
        int min_w = group_weight[min_i][lvl];
        for (int i=0; i<groups.size(); i++){
            if (min_w < group_weight[i][lvl]){
                min_i = i;
                min_w = group_weight[min_i][lvl];
            }
        }
        group_i = min_i;
    }

    else{
        // find most popular in queue group
        // make vector with cnts of each group in qeueu
        int max_i = 0;
        std::vector<int> cnts_group(groups.size(), 0);
        for (auto vg: queue){
            cnts_group[vg[1]] += 1;
            if (cnts_group[max_i] < cnts_group[vg[1]]){
                max_i = vg[1];
            }
        }
        // take group with max cnt
        group_i = max_i;
    }
}


// if v in vol, then move it to queue as {v, g}
void put_from_vol_to_queue(int v, int g){
    auto v_vol_iter = std::find(begin(vertexes_on_lvl[lvl]),
                                end(vertexes_on_lvl[lvl]), 
                                v);
    if (v_vol_iter != end(vertexes_on_lvl[lvl])){
        vertexes_on_lvl[lvl].erase(v_vol_iter);
        queue.push_back({v, g});
    }
}


bool put_primary_from_vol_to_queue(int g){
    for (auto v: vertexes_on_lvl[lvl]){
        if (infos[v].primaryLvl == lvl){
            put_from_vol_to_queue(v, group_i);
            return true;
        }
    }
    return false;
}


// delete from queue v if it neigbour of other groups (!= g)
void delete_from_queue(int v, int g){
    for (auto iter=begin(queue); iter != end(queue); iter++){
        if ((*iter)[0] == v){
            if ((*iter)[1] != g){
                queue.erase(iter);
            }
            break;
        }
    }
}


void init_queue(){
    queue.clear();
    // warrant at least one vertex in queue
    // put vertexes from distributed into queue
    for (auto vg : distributed_vertexes[lvl]){
        auto v = vg[0];
        auto g = vg[1];

        for (auto edge: adj_list[lvl][v]){
            auto neigh = edge[0];
            // if neigh in queue and it neigh with other group: delete from queue
            delete_from_queue(neigh, g);

            // if neigh in vertexes_on_lvl_lvl, then move it to queue [neigh, group]
            put_from_vol_to_queue(neigh, g);
        }
    }
    
    if (queue.empty() && !vertexes_on_lvl[lvl].empty()){
        // find primary, add to queue      
        if (!put_primary_from_vol_to_queue(group_i)){
            // all v in vol is secondary
            put_from_vol_to_queue(vertexes_on_lvl[lvl][0], group_i);
        }
    }
}


bool is_group_in_queue(){
    for (auto vg : queue){
        if (vg[1] == group_i){
            return true;
        }
    }
    return false;
}


void take_next_vertex_from_queue(){
    int best_i = -1;
    auto best_v = queue[0][0];
    for (int i=0; i<queue.size(); i++){
        auto v = queue[i][0];
        auto g = queue[i][1];

        if (g == group_i){
            if (best_i == -1 || infos[best_v].weight < infos[v].weight){
                best_i = i;
                best_v = queue[best_i][0];
            }
        }
    }

    vertex = queue[best_i][0];
    queue.erase(begin(queue) + best_i);
}


void add_to_group(int g, int v, int level){
    groups[g].push_back(v);
    group_weight[g][level] += infos[v].weight;
}


void add_to_distributed(int g, int v, int level){
    distributed_vertexes[level].push_back({v, g});
}


bool handle_primary_vertex(){
    // put vertex to group
    bool is_added = true;
    add_to_group(group_i, vertex, lvl);  // lvl == infos[vertex].primaryLvl

    // put neigh from vol to queue
    for (auto edge: adj_list[lvl][vertex]){
        auto neigh = edge[0];
        put_from_vol_to_queue(neigh, group_i);
    }

    auto vi = infos[vertex];

    // if can add secondary vertex:
    // it exist 
    if (vi.lvlsCount != 2 || vi.secondaryLvl < vi.primaryLvl){
        return is_added;
    }

    // erase from vol secondary vertex
    auto& vol_seclvl = vertexes_on_lvl[vi.secondaryLvl];
    auto sec_vol_iter = std::find(begin(vol_seclvl),
                                  end(vol_seclvl),
                                  vertex);
    if (sec_vol_iter != end(vol_seclvl)){
        vol_seclvl.erase(sec_vol_iter);
    }

    // its neighs do not distributed
    for (auto edge: adj_list[vi.secondaryLvl][vertex]){
        auto neigh = edge[0];
        for (auto vg_dist: distributed_vertexes[vi.secondaryLvl]){
            auto v = vg_dist[0];
            auto g = vg_dist[1];
            // if neigh in distr with other group: then we cant add secondary vertex
            if (v == neigh && g != group_i){
                return is_added;
            }
        }
    }
    // check group_weight[g][secondaryLvl]
    if (group_weight[group_i][vi.secondaryLvl] + vi.weight > M){
        return is_added;
    }

    //  add secondary to group
    add_to_group(group_i, vertex, vi.secondaryLvl);
    //  add secondary to distributed[secondaryLvl]
    add_to_distributed(group_i, vertex, vi.secondaryLvl);
    return is_added;
}


/// 
bool handle_secondary_vertex(){
    bool is_added = false;
    auto vi = infos[vertex];
    // check if we can add primary to group
    //    it on higher lvl
    if (vi.primaryLvl < lvl){
        return is_added;
    }
    //    check weight
    if (    vi.weight + group_weight[group_i][vi.primaryLvl] > M ||
            vi.weight + group_weight[group_i][vi.secondaryLvl] > M){
        return is_added;
    }
    //    its neighs not in distributed on primaryLvl with other groups
    for (auto edge: adj_list[vi.primaryLvl][vertex]){
        auto neigh = edge[0];
        for (auto vg_dist: distributed_vertexes[vi.primaryLvl]){
            auto v = vg_dist[0];
            auto g = vg_dist[1];
            // if neigh in distr with other group: then we cant add secondary vertex
            if (v == neigh && g != group_i){
                return is_added;
            }
        }
    }

    is_added = true;

    // if we can:
    //    add secondary to group
    add_to_group(group_i, vertex, vi.secondaryLvl);
    //    add primary to group
    add_to_group(group_i, vertex, vi.primaryLvl);
    //    add secondary neighs to queue
    for (auto edge: adj_list[lvl][vertex]){
        auto neigh = edge[0];
        put_from_vol_to_queue(neigh, group_i);
    }

    //    add primary to distributed
    add_to_distributed(group_i, vertex, vi.primaryLvl);
    //    remove both from vol
    auto& vol_prlvl = vertexes_on_lvl[vi.primaryLvl];
    auto pr_vol_iter = std::find(begin(vol_prlvl),
                                  end(vol_prlvl),
                                  vertex);
    if (pr_vol_iter != end(vol_prlvl)){
        vol_prlvl.erase(pr_vol_iter);
    }
    
    return is_added;
}


std::vector<std::vector<int>> Solver(int nn, int mm, int ll, std::vector<VertexInfo> infosmain){
    init_all_var_with_L(nn, mm, ll);
    infos = infosmain;
    fill_adj_and_vert_on_lvl();

    make_group();

    for (lvl=0; lvl<L; lvl++){
        init_queue();
        take_next_group();

        while (true){
            if (group_weight[group_i][lvl] >= M){
                take_next_group();
            }

            if (queue.empty() && vertexes_on_lvl[lvl].empty()){
                break;
            }
            if (!is_group_in_queue() && vertexes_on_lvl[lvl].empty()){
                take_next_group();
            }
            else if (!is_group_in_queue() && !vertexes_on_lvl[lvl].empty()){
                // put vertex from vol to queue
                if (!put_primary_from_vol_to_queue(group_i)){
                    put_from_vol_to_queue(vertexes_on_lvl[lvl][0], group_i);
                }
            }

            while (is_group_in_queue()){
                take_next_vertex_from_queue();
                if (group_weight[group_i][lvl] + infos[vertex].weight > M){
                    continue;
                }

                bool is_added = false;
                if (lvl == infos[vertex].primaryLvl){
                    is_added = handle_primary_vertex();
                }
                else if (lvl == infos[vertex].secondaryLvl){
                    is_added = handle_secondary_vertex();
                }
                
                // ???
                if (is_added){
                    break;
                }
            }
        }
    }

    return groups;
}
















































































std::vector<VertexInfo> infosmain;
long long int p;
int NN, MM, LL;


bool Delete(int lvl, int v, int u) {
  if (lvl < 0 || lvl >= L ||
      v < 0 || v >= N ||
      u < 0 || u >= N) {
    return true;
  }
  if (infosmain[v].primaryLvl != lvl && infosmain[v].secondaryLvl != lvl) {
    return true;
  }
  int edge = (infosmain[v].primaryLvl == lvl ? infosmain[v].primaryEdges[u] : infosmain[v].secondaryEdges[u]);
  if (edge >= 2) {
    p++;
    if (infosmain[v].primaryLvl == lvl) {
      infosmain[v].primaryEdges[u] -= 2;
    } else {
      infosmain[v].secondaryEdges[u] -= 2;
    }
    if (infosmain[u].primaryLvl == lvl) {
      infosmain[u].primaryEdges[v] -= 2;
    } else {
      infosmain[u].secondaryEdges[v] -= 2;
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
                weight_lvl[infosmain[v].primaryLvl] += infosmain[v].weight;
            }
            else{
                // secondary
                weight_lvl[infosmain[v].secondaryLvl] += infosmain[v].weight;
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
                        if (infosmain[v1].primaryEdges[v2] != 0){
                            std::cout << "v2 in v1.primaryEdges: (lvl, v1, v2)" << infosmain[v1].secondaryLvl << " " << v1 << " "<<v2  << std::endl;
                            return false;
                        }
                    }
                    else{
                        // secondary
                        if (infosmain[v1].secondaryEdges[v2] != 0){
                            std::cout << "v2 in v1.secondaryEdges: (lvl, v1, v2)" << infosmain[v1].secondaryLvl << " " << v1 << " "<<v2 << std::endl;
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
    for(auto v=0; v<infosmain.size(); v++){
        for (auto n=0; n<infosmain[v].primaryEdges.size(); n++){
            if (infosmain[v].primaryEdges[n] != 0){
                fout << infosmain[v].primaryLvl << "_" << v << "-" << infosmain[v].primaryLvl << "_" << n << std::endl;
            }
        }
        if (infosmain[v].lvlsCount == 2){
            for (auto n=0; n<infosmain[v].primaryEdges.size(); n++){
                if (infosmain[v].secondaryEdges[n] != 0){
                    fout << infosmain[v].secondaryLvl << "_" << v << "-" << infosmain[v].secondaryLvl << "_" << n << std::endl;
                }
            }
        }
    }
}

int main() {
  std::ifstream fin{ "unix/open.txt" }; // "test1" "test2" "unix/open.txt"
  int TESTS_COUNT;
  fin >> TESTS_COUNT;
  int READ_COUNT = 2000;
  int cnt_true{0}, cnt_false{0};

  for (int test_num=0;test_num < READ_COUNT && test_num < TESTS_COUNT; test_num++){
    // std::cout << "############# " << test_num << std::endl;
    infosmain.clear();
    NN, MM, LL;
    fin >> NN >> MM >> LL;
    for (int i=0; i<NN; i++){
        VertexInfo vinf;
        fin >> vinf.weight >> vinf.lvlsCount >> vinf.primaryLvl;
        if (vinf.lvlsCount == 2){
            fin >> vinf.secondaryLvl;
        }
        for (int edge_i=0; edge_i<NN; edge_i++ ){
            int type;
            fin >> type;
            vinf.primaryEdges.push_back(type);
        }
        if (vinf.lvlsCount == 2){
            for (int edge_i=0; edge_i<NN; edge_i++ ){
                int type;
                fin >> type;
                vinf.secondaryEdges.push_back(type);
            }
        }
        infosmain.push_back(vinf);
    }
    
    std::vector<VertexInfo> convertedinfosmain = infosmain;
    for (int v = 0; v < NN; ++v) {
      for (int& e : convertedinfosmain[v].primaryEdges) {
        if (e >= 2) e = -1;
      }
      for (int& e : convertedinfosmain[v].secondaryEdges) {
        if (e >= 2) e = -1;
      }
    }

    std::vector<std::vector<int>> result = Solver(NN, MM, LL, convertedinfosmain);
   
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
   
    if (!is_groups_correct(result)){
        cnt_false++;
        print_adj_matr();
        std::cout << NN << " " << MM << " "<< LL << std::endl;
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
    }
    else{
        cnt_true++;
    }
  }
  
  std::cout << "(true, false)" << cnt_true << " " << cnt_false<<std::endl;
}