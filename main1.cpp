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


void init_all_var(int nn, int mm, int ll){
    adj_list.clear();
    adj_list.resize(ll);
    vertexes_on_lvl.clear();
    vertexes_on_lvl.resize(ll);
    distributed_vertexes.clear();
    distributed_vertexes.resize(ll);
    groups.clear();
    group_weight.clear();
    group_i = 0;
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
    if (queue.empty()){
        // take group with min weight on lvl
        int min_i = 0;
        int min_w = group_weight[min_i][lvl];
        for (int i=0; i<groups.size(); i++){
            if (min_w > group_weight[i][lvl]){
                min_i = i;
                min_w = group_weight[min_i][lvl];
            }
        }
        if (min_w > 0){
            group_i = groups.size();
            make_group();
        }
        else{ 
            group_i = min_i;
        }
        // group_i = min_i;
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


decltype(auto) get_iter_of_vert_in_queue(int v){
    auto iter = begin(queue);
    while (iter != end(queue)){
        if ((*iter)[0] == v){
            return iter;
        }
        iter++;
    }
    return iter;
}


// delete from queue v if it neigbour of other groups (!= g)
void delete_from_queue(int v, int g){
    auto iter = get_iter_of_vert_in_queue(v);
    if (iter != end(queue) && (*iter)[1] != g){
        queue.erase(iter);
    }
}


// if v not in queue: add {v, g} in queue
// if v in queue with other group: delete it
void add_to_queue(int v, int g){
    auto iter = get_iter_of_vert_in_queue(v);
    if (iter == end(queue)){
        queue.push_back({v, g});
    }
    else if ((*iter)[1] != g){
        delete_from_queue(v, g);
    }
}


// if v in vol, then move it to queue as {v, g}
void put_from_vol_to_queue(int v, int g){
    auto v_vol_iter = std::find(begin(vertexes_on_lvl[lvl]),
                                end(vertexes_on_lvl[lvl]), 
                                v);
    if (v_vol_iter != end(vertexes_on_lvl[lvl])){
        vertexes_on_lvl[lvl].erase(v_vol_iter);
        add_to_queue(v, g);
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


void delete_from_queue_group(int g){
    auto iter = begin(queue);
    while (iter != end(queue)){
        if ((*iter)[1] == g){
            iter = queue.erase(iter);
        }
        else{
            iter++;
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


// for current group choose best vertex
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


decltype(auto) get_iter_of_vert_in_distributed(int v, int level){
    auto iter = begin(distributed_vertexes[level]);
    while (iter != end(distributed_vertexes[level])){
        if ((*iter)[0] == v){
            return iter;
        }
        iter++;
    }
    return iter;
}


void add_to_distributed(int g, int v, int level){
    distributed_vertexes[level].push_back({v, g});
}


// is vertex neighbours in distributed with other groups
// true if neigh not in distributed with other groups
bool check_v_neigh_in_distributed(int v, int g, int level){
    auto& dist_v_lvl = distributed_vertexes[level];
    
    for (auto edge: adj_list[level][v]){
        auto neigh = edge[0];
        for (auto vg_dist: dist_v_lvl){
            auto v = vg_dist[0];
            auto g = vg_dist[1];
            // if neigh in distr with other group: then we cant add vertex
            if (v == neigh && g != group_i){
                return false;
            }
        }
    }
    return true;
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
    if (!check_v_neigh_in_distributed(vertex, group_i, vi.secondaryLvl)){
        return is_added;
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


void delete_empty_groups(){
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


std::vector<std::vector<int>> Solver(int nn, int mm, int ll, std::vector<VertexInfo> infosmain){
    init_all_var(nn, mm, ll);
    infos = infosmain;
    fill_adj_and_vert_on_lvl();

    make_group();

    for (lvl=0; lvl<L; lvl++){
        group_i = 0;
        init_queue();
        take_next_group();

        while (true){
            if (group_weight[group_i][lvl] >= M){
                delete_from_queue_group(group_i);
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
                if (group_weight[group_i][lvl] == M){
                    break;
                }
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
    delete_empty_groups();
    
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

    std::vector<std::vector<std::vector<int>>> groups_lvl(groups.size()); // [v, lvl]
    for (int g_i=0; g_i<groups.size(); g_i++){
        for (int v_i=0; v_i<groups[g_i].size(); v_i++){
            int lvl;
            if (v_i>0 && groups[g_i][v_i-1] == groups[g_i][v_i]){
                lvl = infosmain[groups[g_i][v_i]].secondaryLvl;
            }
            else{
                lvl = infosmain[groups[g_i][v_i]].primaryLvl;
            }
            groups_lvl[g_i].push_back({groups[g_i][v_i], lvl});
        }
    }

    for (int i=0; i<groups.size(); i++){
        for (int j=i+1; j<groups.size(); j++){
            for (auto vl1: groups_lvl[i]){
                std::vector<int> edges;
                if (vl1[1] == infos[vl1[0]].primaryLvl){
                    edges = infos[vl1[0]].primaryEdges;
                }
                else{
                    edges = infos[vl1[0]].secondaryEdges;
                }

                for (auto vl2: groups_lvl[j]){
                    if (vl1[0] == vl2[0]){
                        return false;
                    }
                    if (vl1[1] != vl2[1]){
                        continue;
                    }
                    if (edges[vl2[0]] != 0){
                        return false;
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
   
    if (!is_groups_correct(result)){
        cnt_false++;
        // print_adj_matr();
        std::cout << NN << " " << MM << " "<< LL << std::endl;
        std::cout << result.size() << std::endl;
        for (auto group : result) {
            std::sort(group.begin(), group.end());
            // std::cout << group.size() << " ";
            for (size_t i = 0; i < group.size(); ++i) {
                std::cout << group[i];
                if (i + 1 != group.size()) {
                    std::cout << " ";
                }
            }
            std::cout << std::endl;
        }
        std::cout<<std::endl;
    }
    else{
        cnt_true++;
    }
  }
  
  std::cout << "(true, false)" << cnt_true << " " << cnt_false<<std::endl;
}