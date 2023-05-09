#include <algorithm>
#include <iostream>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <map>
#include <queue>
#include <unordered_set>

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
bool is_groups_correct(std::vector< std::vector<int> >);

























































struct Group;
struct Vertex{
    int index;
    bool in_group = false;
    int group = -1;
    bool is_primary = false;
    int lvl;
    int weight;
    bool was_in_queue = false;
    int degree;
    int degree_solid;
    int sum_neigh_weight=0;
    // fdewfv = fail delete edge with fat vertex
    bool fdewfv = false;
    bool transit = false;
    int true_max_w=false;

    std::vector<int> neigh_groups;
    Vertex* primary_vertex=nullptr;
    Vertex* secondary_vertex=nullptr;
    std::vector<int> adj_matrix;
    // std::vector<std::vector<int>> adj_list;
    std::vector<Vertex*> neighbours;
    std::unordered_map<int, std::vector<Vertex*>> group_neighbours;
    std::unordered_set<int> set_neigh_group;

    Vertex(int i, bool pr, int l, int w): index{i}, is_primary{pr}, lvl{l}, weight{w} {};
    void init_neighbours();
    bool can_put();
    void put();
    void init_with_group(Group&);
    void add_neigh_group(int, Vertex*);
    int score();
    std::unordered_map<int, std::vector<Vertex*>> get_group_neighbours();
    void remove_edge(Vertex&);
    void add_edge(Vertex&);
    void delete_edges_with_group(Group&);
    void delete_edges_with_all_groups();
    void delete_all_edges();
    void delete_some_edges();
    bool delete_edge(Vertex&);
    std::vector<Vertex*>& get_neighbours_from_group(Group&);
    int cnt_neigh_groups();
    int cnt_weak_edges();
};

bool operator < (Vertex& lhs, Vertex& rhs){
    return lhs.score() < rhs.score();
}

 
int make_group();
bool check_vertex_group(Vertex&, Group&);
void base(Vertex&, Group&);
void put_vertex_to_group(Vertex&, Group&);
void put_v_in_queue(Vertex&);
void put_smth_in_queue();
Vertex* next_vertex();
void maybe_delete_edge(Vertex&);
void isolate(Group&, int);
void rule_of_fdewfv(Vertex&, Vertex&);
void push_queue_weight();

float n, ng, nw, ngw;
float dcnt=0, unstable_cnt=0;
int N, M, L;
std::vector<Group> groups;
std::vector<std::vector<Vertex>> graph;
std::vector<Vertex*> queue;
std::vector<std::vector<Vertex*>> queue_lvl;
std::vector<int> cnt_v_on_lvl(L, 0);
std::vector<std::vector<Vertex*>> v_with_weight;
std::vector<std::pair<int, int>> queue_group_lvl;


struct Group{
    int index;
    std::vector<int> vertexes;
    std::vector<int> weight;
    std::vector<Vertex*> vertexes_vector;
    Group(int i): index{i}, weight(L, 0){};


    void add(Vertex& v){
        if (weight[v.lvl] == 0 && v.weight != M){
            queue_group_lvl.push_back(std::make_pair(index, v.lvl));
        }

        vertexes.push_back(v.index);
        vertexes_vector.push_back(&v);
        weight[v.lvl] += v.weight;
    }
};


void Vertex::init_neighbours(){
    degree = 0;
    degree_solid = 0;
    for (int n=0; n<adj_matrix.size(); n++){
        if (adj_matrix[n] != 0){
            degree ++;
            if (adj_matrix[n] == 1){
                degree_solid++;
            }
            if (adj_matrix[n] == -1){
                unstable_cnt++;
            }
            // adj_list.push_back({n, adj_matrix[n]});
            int nps;
            if (graph[n].size() == 1 || graph[n][0].lvl == lvl){
                nps = 0;
            }
            else{
                nps = 1;
            }
            neighbours.push_back(&graph[n][nps]);
            sum_neigh_weight += graph[n][nps].weight;
        }
    }
}


bool Vertex::can_put(){
    if (in_group || transit){
        return false;
    }
    int cnt = set_neigh_group.size();
    // if (cnt > 1){
    //     return false;
    // }
    bool t = true;
    if (cnt == 1){
        t = t && check_vertex_group(*this, groups[neigh_groups[0]]);
    }

    if (!is_primary){
        if (primary_vertex->in_group){
            if (cnt == 1){
                t = t && (primary_vertex->group == neigh_groups[0]);
            }
        }
        else{
            if (cnt==1){
                t = t && check_vertex_group(*primary_vertex, groups[neigh_groups[0]]);
                t = t && primary_vertex->can_put();
            }
        }
    }
    if (!t){
        transit = true;
    }
    return t;
}


void Vertex::put(){
    if (!can_put()){
        return;
    }

    int g;
    int cnt = set_neigh_group.size();                
    if (cnt == 1){
        g = neigh_groups[0];
    }
    else{
        g = make_group();
    }

    put_vertex_to_group(*this, groups[g]);
}


void Vertex::init_with_group(Group& g){
    // say neighbours, change in_group, group  
    for (auto n: neighbours){
        if (!n->transit && !n->in_group){
            n->add_neigh_group(g.index, this);
            queue.push_back(n);
            // queue_lvl[lvl].push_back(n);
        }
    }
    in_group = true;
    group = g.index;
    ng++;
    ngw += weight;
}


void Vertex::add_neigh_group(int g, Vertex* v){
    neigh_groups.push_back(g);
    set_neigh_group.insert(g);
    group_neighbours[g].push_back(v);
}


int Vertex::score(){
    // if (!is_primary){
    //     return weight + 9;
    // }
    return 10 * weight + 4 * graph[index].size() - 1 * degree; // 8 * weight - 0.5 * double(degree) + 1 * graph[index].size() + 2.5 * true_max_w;
}


void Vertex::remove_edge(Vertex& v){
    degree--;
    if (adj_matrix[v.index] == 1){
        degree_solid--;
    }
    sum_neigh_weight -= v.weight;
    // remove from neigh_groups
    if (v.in_group){
        auto iter = std::find(begin(neigh_groups),
                              end(neigh_groups),
                              v.group);
        if (iter != end(neigh_groups)){
            neigh_groups.erase(iter);
            auto iter = std::find(begin(neigh_groups),
                        end(neigh_groups),
                        v.group);
            if (iter == end(neigh_groups)){
                set_neigh_group.erase(v.group);
            }
            
        }
        auto& t = group_neighbours[v.group];
        auto itert = std::find(begin(t), end(t), &v);
        if (itert != end(t)){
            t.erase(itert);
        }
    }

    // remove from adj_matr
    adj_matrix[v.index] = 0;

    // remove from neighbours
    for (auto iter = begin(neighbours); iter != end(neighbours); iter++){
        if ((*iter)->index == v.index && (*iter)->lvl == v.lvl){
            neighbours.erase(iter);

            break;
        }
    }
}


std::vector<Vertex*>& Vertex::get_neighbours_from_group(Group& g){
    return group_neighbours[g.index];
    // std::vector<Vertex*> result;
    // // for n in neigbours
    // for (auto neigh: neighbours){
    //     // if n.in_group && n.group == g.index
    //     // push back n
    //     if (neigh->in_group && neigh->group == g.index){
    //         result.push_back(neigh);
    //     }
    // }

    // return result;
}


bool Vertex::delete_edge(Vertex& neigh){
    if (adj_matrix[neigh.index] != -1){
        return false;
    }
    dcnt++;
    if (Delete(lvl, index, neigh.index)){
        remove_edge(neigh);
        neigh.remove_edge(*this);
        return true;
    }
    else{
        adj_matrix[neigh.index] = 1;
        neigh.adj_matrix[index] = 1;
        // rule_of_fdewfv(*this, neigh);
        return false;
    }
}
void Vertex::delete_edges_with_group(Group& g){
    // if (weight <= 1){
        // return;
        // transit = true;
        // if (secondary_vertex != nullptr){
        //     secondary_vertex->transit = true;
        // }
    // }
    if (transit){
        return;
    }

    // find edges with g
    auto neigh_from_g = get_neighbours_from_group(g);
    // if all edges is weak:
    for (auto neigh: neigh_from_g){
        if (adj_matrix[neigh->index] != -1){
            transit=true;
            if (secondary_vertex != nullptr){
                secondary_vertex->transit = true;
            }
            return;
        }
    }

    // try to delete
    // if one don't delete, return
    bool succes = true;
    for (auto neigh: neigh_from_g){
        if (!delete_edge(*neigh)){
            transit = true;
            if (secondary_vertex != nullptr){
                secondary_vertex->transit = true;
            }
            return;
        }
        // delete_edge(*neigh);
        // if (!succes){
        //     adj_matrix[neigh->index] = 1;
        //     neigh->adj_matrix[index] = 1;
        // }
        // else if (!delete_edge(*neigh)){
        //     succes = false;
        //     transit = true;
        //     if (secondary_vertex != nullptr){
        //         secondary_vertex->transit = true;
        //     }
        // }
    }

}


void isolate(Group& g, int lvl){
    if (g.weight[lvl] < double(M) * 0.45){
        return;
    }
    
    if (dcnt > unstable_cnt * 0.24){
        return;
    }
    
    // for (auto iter = begin(queue_lvl[lvl]); iter != end(queue_lvl[lvl]); iter++){
    //     (*iter)->delete_edges_with_group(g);
    // }
    // queue_lvl[lvl].clear();
    auto iter = begin(queue);
    while (iter != end(queue)){
        auto v = *iter;
        if (v->lvl == lvl){
            v->delete_edges_with_group(g);
            iter = queue.erase(iter);
            // iter++;
        }
        else{
            iter++;
        }
        // v->delete_edges_with_group(g);
    }
}


int make_group(){
    groups.push_back(Group(groups.size()));
    return groups.size() - 1;
}


bool check_vertex_group(Vertex& v, Group& g){
    if (v.in_group && v.group != g.index){
        return false;
    }
    int cnt = v.set_neigh_group.size();
    if (cnt > 1){
        return false;
    }
    if (cnt == 1){
        if (v.neigh_groups[0] != g.index){
            return false;
        }
    }
    if (g.weight[v.lvl] + v.weight > M){
        return false;
    }
    return true;
}


void base(Vertex& v, Group& g){
    v.init_with_group(g);
    g.add(v);
}


void put_vertex_to_group(Vertex& v, Group& g){    
    if (v.is_primary){
        base(v, g);
        isolate(g, v.lvl);
        if (v.secondary_vertex != nullptr){
            if (  check_vertex_group(*v.secondary_vertex, g) &&
                  v.secondary_vertex->can_put()){
                base(*v.secondary_vertex, g);
                isolate(g, v.secondary_vertex->lvl);
            }
            else{
                v.secondary_vertex->transit = true;
            }
        }
    }

    else{
        if (v.weight <= 1){
            return;
        }

        if (  check_vertex_group(*v.primary_vertex, g) &&                     /// if w > 1?
              v.primary_vertex->can_put()){
            put_vertex_to_group(*v.primary_vertex, g);
        }
        else{
            v.transit = true;
        }

    }
}


Vertex* next_vertex(){
    if (queue_group_lvl.empty()){
        auto best_i = begin(queue);

        for (auto iter=begin(queue); iter != end(queue); iter++){
            if (*(*best_i) < *(*iter)){
                best_i = iter;
            }
        }

        auto result = *best_i;
        queue.erase(best_i);
        return result;
    }
    else{
        auto condition = [](Vertex* v, Group* g, int lvl){
            auto i = v->set_neigh_group.find(g->index);
            return (v->lvl == lvl) && (i != end(v->set_neigh_group))
                    && check_vertex_group(*v, *g);
        };
        auto gl = queue_group_lvl.front();
        std::vector<Vertex*> suitable_vertexes;

        for (auto iter=begin(queue); iter != end(queue); iter++){
            if (condition(*iter, &groups[gl.first], gl.second)){ 
                suitable_vertexes.push_back(*iter);
            }
        }

        if (suitable_vertexes.empty()){
            isolate(groups[gl.first], gl.second);
            queue_group_lvl.erase(begin(queue_group_lvl));
            if (queue.empty()){
                push_queue_weight();
            }
            return next_vertex();
        }
        else{
            auto best_i = begin(suitable_vertexes);
            for (auto iter = begin(suitable_vertexes); iter != end(suitable_vertexes); iter++){
                if (*(*best_i) < *(*iter)){
                    best_i = iter;
                }
            }

            auto result = *best_i;
            auto i = std::find(begin(queue), end(queue), result);
            queue.erase(i);
            
            return result;
        }
    }
}


void push_queue_weight(){
    // queue.clear();               ///
    int m = M;
    for (m = M; m >= 0; m--){
        if (!v_with_weight[m].empty()){
            break;
        }
    }
    if (m <= 0){
        return;
    }

    auto iter=begin(v_with_weight[m]);
    while (iter != end(v_with_weight[m])){
        auto v = *iter;
        if (v->in_group || v->transit){
            iter = v_with_weight[m].erase(iter);
        }
        else{
            queue.push_back(*iter);
            iter++;
        }
    }

    if (queue.empty()){
        push_queue_weight();
    }
}


void edit_weight(int k){
    for (int v = 0; v < N; v++){
        for (int i=0; i < graph[v].size(); i++){
            // M-k
            if (graph[v][i].weight == M){
                graph[v][i].true_max_w = 1;
            }
            if (graph[v][i].weight >= M - k){
                graph[v][i].weight = M;
            }
        }
    }
}


std::vector<std::vector<int>> Solver(int nn, int mm, int ll, std::vector<VertexInfo> infosmain){
    N=nn; M=mm; L=ll;
    n=0, ng=0, nw=0, ngw=0;
    dcnt = 0; unstable_cnt=0;
    graph.clear(); 
    groups.clear();
    queue.clear();
    graph.resize(N);
    cnt_v_on_lvl.clear();
    cnt_v_on_lvl.resize(L, 0);
    queue_group_lvl.clear();
    v_with_weight.clear();
    v_with_weight.resize(M + 1);

   
    for (int v=0; v<N; v++){
        graph[v].push_back(Vertex(v, true, infosmain[v].primaryLvl, infosmain[v].weight));
        graph[v][0].adj_matrix = infosmain[v].primaryEdges;
        n++;
        nw += graph[v][0].weight;
        cnt_v_on_lvl[graph[v][0].lvl]++;
        if (infosmain[v].lvlsCount == 2){
            graph[v].push_back(Vertex(v, false, infosmain[v].secondaryLvl, infosmain[v].weight));
            graph[v][1].adj_matrix = infosmain[v].secondaryEdges;
            graph[v][0].secondary_vertex = &graph[v][1];
            graph[v][1].primary_vertex = &graph[v][0];
            cnt_v_on_lvl[graph[v][1].lvl]++;
            n++;
            nw += graph[v][1].weight;
        }
    }   
    for (int v=0; v<N; v++){
        for (int i=0; i<graph[v].size(); i++){
            graph[v][i].init_neighbours();
            int w = graph[v][i].weight;
            v_with_weight[w].push_back(&graph[v][i]);
        }
    }

    // edit_weight(1);
    push_queue_weight();

    while (!queue.empty()){
        auto& cur_v = *(next_vertex());
        cur_v.put();
        if (queue.empty()){
            push_queue_weight();
        }
    }

    std::vector<std::vector<int>> answer;
    for (int g=0; g<groups.size(); g++){
        if (groups[g].vertexes.size() != 0){
            answer.push_back(groups[g].vertexes);
        }
    }



// int n=N;
// int ng=0;
// int nw=0;
// int ngw=0;
// int cnt_non_tr = 0;
// for (int v=0; v<N; v++){
//     nw += graph[v][0].weight;
//     if (graph[v][0].in_group){
//         ngw += graph[v][0].weight;
//         ng++;
//     }
//     if (!graph[v][0].in_group && !graph[v][0].transit && graph[v][0].weight != 1){
//         cnt_non_tr++;
//     }
//     if (graph[v].size() == 2){
//         n++;
//         nw += graph[v][1].weight;
//         if (graph[v][1].in_group){
//             ngw += graph[v][1].weight;
//             ng++;
//         }
//         if (!graph[v][1].in_group && !graph[v][1].transit && graph[v][0].set_neigh_group.empty()){
//             cnt_non_tr++;
//         }
//     }
// }
std::cout << ng / n  << "\t" << ngw / nw<<"\t" ; // << std::endl;
// std::cout << double(cnt_non_tr) << "\t";
std::cout << dcnt / unstable_cnt << std::endl;

    return answer;

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
                if (vl1[1] == infosmain[vl1[0]].primaryLvl){
                    edges = infosmain[vl1[0]].primaryEdges;
                }
                else{
                    edges = infosmain[vl1[0]].secondaryEdges;
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