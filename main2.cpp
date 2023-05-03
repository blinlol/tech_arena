#include <algorithm>
#include <iostream>
#include <vector>
#include <fstream>
#include <unordered_map>
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
    bool in_group=false;
    int group=-1;
    bool is_primary=false;
    int lvl;
    int weight;
    bool was_in_queue=false;

    std::vector<int> neigh_groups;
    Vertex* primary_vertex=nullptr;
    Vertex* secondary_vertex=nullptr;
    std::vector<int> adj_matrix;
    // std::vector<std::vector<int>> adj_list;
    std::vector<Vertex*> neighbours;

    Vertex(int i, bool pr, int l, int w): index{i}, is_primary{pr}, lvl{l}, weight{w} {};
    void init_neighbours();
    bool can_put();
    void put();
    void init_with_group(Group&);
    void add_neigh_group(int);
    int score() const;
    std::unordered_map<int, std::vector<Vertex*>> get_group_neighbours() const;
    void remove_edge(Vertex&);
    void add_edge(Vertex&);
    void delete_edges_with_group(Group&);
    std::vector<Vertex*> get_neighbours_from_group(Group&);
    int cnt_neigh_groups();
};

bool operator < (const Vertex& lhs, const Vertex& rhs){
    return lhs.score() < rhs.score();
}


int make_group();
bool check_vertex_group(Vertex&, Group&);
void base(Vertex&, Group&);
void put_vertex_to_group(Vertex&, Group&);
void put_v_in_queue(Vertex&);
void put_smth_in_queue();
decltype(auto) next_vertex();
void maybe_delete_edge(Vertex&);
void isolate(Group&, int);


int N, M, L;
std::vector<Group> groups;
std::vector<std::vector<Vertex>> graph;
std::vector<Vertex*> queue;


struct Group{
    int index;
    std::vector<int> vertexes;
    std::vector<int> weight;
    Group(int i): index{i}, weight(L, 0){};


    void add(Vertex& v){
        vertexes.push_back(v.index);
        weight[v.lvl] += v.weight;

        // if (weight[v.lvl] == M){
        //     isolate(*this, v.lvl);
        // }
    }



};


void Vertex::init_neighbours(){
    for (int n=0; n<adj_matrix.size(); n++){
        if (adj_matrix[n] != 0){
            // adj_list.push_back({n, adj_matrix[n]});
            int nps;
            if (graph[n].size() == 1 || graph[n][0].lvl == lvl){
                nps = 0;
            }
            else{
                nps = 1;
            }
            neighbours.push_back(&graph[n][nps]);
        }
    }
}


int Vertex::cnt_neigh_groups(){
    std::unordered_set<int> groups_indexes;
    for (int n: neigh_groups){
        groups_indexes.insert(n);
    }
    return groups_indexes.size();
}


bool Vertex::can_put(){
    if (in_group){
        return false;
    }
    int cnt = neigh_groups.size();//cnt_neigh_groups();
    if (cnt > 1){
        return false;
    }
    bool t = true;
    if (cnt == 1){
        t = t && check_vertex_group(*this, groups[neigh_groups[0]]);
    }
    if (!is_primary){
        t = t && primary_vertex->can_put();
        if (cnt == 1){
            t = t && check_vertex_group(*primary_vertex, groups[neigh_groups[0]]);
        }
    }
    return t;
}


void Vertex::put(){
    if (!can_put()){
        return;
    }

    int g;
    int cnt = neigh_groups.size();                ////
    if (cnt == 1){
        g = neigh_groups[0];
    }
    else if (cnt == 0){
        g = make_group();
    }

    put_vertex_to_group(*this, groups[g]);
}


void Vertex::init_with_group(Group& g){
    // say neighbours, change in_group, group  
    for (auto n: neighbours){
        n->add_neigh_group(g.index);
    }
    // add_neigh_group(g.index);
    in_group = true;
    group = g.index;
}


void Vertex::add_neigh_group(int g){
    neigh_groups.push_back(g);
}


int Vertex::score() const{
    return weight;
}


void Vertex::add_edge(Vertex& v){                                 ////////////
    
}


void Vertex::remove_edge(Vertex& v){
    bool is_neighbour = false;
    auto iter_in_neighbours = begin(neighbours);
    for (auto neigh: neighbours){
        if (    neigh->lvl == v.lvl && 
                neigh->index == v.index){
            is_neighbour = true;
            break;
        }
    }
    if (!is_neighbour){
        return;
    }

    // remove from neigh_groups
    if (v.in_group){
        auto iter = std::find(begin(neigh_groups),
                              end(neigh_groups),
                              v.group);
        if (iter != end(neigh_groups)){
            neigh_groups.erase(iter);
        }
    }

    // remove from adj_matr
    if (adj_matrix[v.index] == -1){
        adj_matrix[v.index] = 0;
    }
    else{
        return;
    }

    // // remove from adj_list
    // remove from neighbours
    neighbours.erase(iter_in_neighbours);
}


std::vector<Vertex*> Vertex::get_neighbours_from_group(Group& g){
    std::vector<Vertex*> result;
    // for n in neigbours
    for (auto neigh: neighbours){
        // if n.in_group && n.group == g.index
        // push back n
        if (neigh->in_group && neigh->group == g.index){
            result.push_back(neigh);
        }
    }

    return result;
}


void Vertex::delete_edges_with_group(Group& g){
    // find edges with g
    auto neigh_from_g = get_neighbours_from_group(g);
    // if all edges is weak:
    for (auto neigh: neigh_from_g){
        if (adj_matrix[neigh->index] != -1){
            return;
        }
    }
    // try to delete
    // if one don't delete, return
    for (auto neigh: neigh_from_g){
        if (Delete(lvl, index, neigh->index)){
            remove_edge(*neigh);
            neigh->remove_edge(*this);
        }
        else{
            return;
        }
    }

}



// return [ group_i -> [v1*, ...] ]
std::unordered_map<int, std::vector<Vertex*>>
     Vertex::get_group_neighbours() const {
    std::unordered_map<int, std::vector<Vertex*>> result;
    for (auto neigh: neighbours){
        if (neigh->in_group){
            int g = neigh->group;
            result[g].push_back(neigh);
        }
    }
    return result;
}


void maybe_delete_edge(Vertex& v){
    // it has 2 neigh group
    // one neighbour from first group, other from second
    auto grouped_neighs = v.get_group_neighbours();

    if (grouped_neighs.size() != 2){
        return;
    }

    auto iter = begin(grouped_neighs);
    int g1_i = (*iter).first;
    iter ++;
    int g2_i = (*iter).first;

    if (grouped_neighs[g1_i].size() != 1 &&
        grouped_neighs[g2_i].size() != 1){
            return;
        }
    
    if (grouped_neighs[g1_i].size() != 1){
        int temp = g1_i;
        g1_i = g2_i;
        g2_i = temp;
    }
    
    auto& neigh = *grouped_neighs[g1_i][0];

    if (v.adj_matrix[neigh.index] != -1){
        return;
    }

    // after removal edge with first group, it will be able to add this vertex to second group
    neigh.remove_edge(v);
    v.remove_edge(neigh);

    if (check_vertex_group(v, groups[g2_i])){
        // if it hold true:
        //    delete edge
        if (Delete(v.lvl, v.index, neigh.index)){
            return;
        }
    }

    v.add_edge(neigh);
    neigh.add_edge(v);
}


void isolate(Group& g, int lvl){
    for (auto v: queue){
        if (v->lvl == lvl){
            v->delete_edges_with_group(g);
        }
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
    int cnt = v.neigh_groups.size();// v.cnt_neigh_groups();
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
    g.add(v);
    v.init_with_group(g);
    for (auto n: v.neighbours){
        put_v_in_queue(*n);
    }
    // isolate(g, v.lvl);
}


void put_vertex_to_group(Vertex& v, Group& g){
    if (v.is_primary){
        base(v, g);
        if (v.secondary_vertex != nullptr){
            // base(*v.secondary_vertex, g);
            // v.secondary_vertex->put();
            if (check_vertex_group(*v.secondary_vertex, g)){
                base(*v.secondary_vertex, g);
            }
        }
    }
    else{
        if (check_vertex_group(*v.primary_vertex, g)){
            put_vertex_to_group(*v.primary_vertex, g);
        }
    }
                                                                         /////
    // if (g.weight[v.lvl] == M){
    //     isolate(g, v.lvl);
    // }
}


void put_v_in_queue(Vertex& v){
    if (v.in_group || v.was_in_queue){
        return;
    }
    for (auto vinq: queue){
        // if find v in queue: return; else add to quuee
        if (vinq->lvl == v.lvl && vinq->index == v.index){
            return;
        }
    }
    v.was_in_queue = true;
    queue.push_back(&v);
}


void put_smth_in_queue(){
    int best_v = -1;
    int best_ps = -1;
    for (int v=0; v<N; v++){
        for (int i = graph[v].size() - 1; i >= 0 ; i--){
            if (!graph[v][i].in_group && !graph[v][i].was_in_queue){
                if (best_v == -1 ||
                    graph[best_v][best_ps] < graph[v][i]){
                        best_v = v;
                        best_ps = i;
                    }
            }
        }
    }
    
    if (best_v != -1){
        put_v_in_queue(graph[best_v][best_ps]);
    }
}


decltype(auto) next_vertex(){
    auto best_i = begin(queue);
    // int max_w = (*best_i)->weight;

    for (auto iter=begin(queue); iter != end(queue); iter++){
        if (*(*best_i) < *(*iter)){
            best_i = iter;
            // max_w = (*best_i)->weight;
        }
    }

    auto result = *best_i;
    queue.erase(best_i);
    return result;
}






std::vector<std::vector<int>> Solver(int nn, int mm, int ll, std::vector<VertexInfo> infosmain){
    N=nn; M=mm; L=ll;
    graph.clear(); 
    groups.clear();
    queue.clear();
    graph.resize(N);

    for (int v=0; v<N; v++){
        graph[v].push_back(Vertex(v, true, infosmain[v].primaryLvl, infosmain[v].weight));
        graph[v][0].adj_matrix = infosmain[v].primaryEdges;
        if (infosmain[v].lvlsCount == 2){
            graph[v].push_back(Vertex(v, false, infosmain[v].secondaryLvl, infosmain[v].weight));
            graph[v][1].adj_matrix = infosmain[v].secondaryEdges;
            graph[v][0].secondary_vertex = &graph[v][1];
            graph[v][1].primary_vertex = &graph[v][0];
        }
    }

    for (int v=0; v<N; v++){
        for (int i=0; i<graph[v].size(); i++){
            graph[v][i].init_neighbours();
        }
    }

    put_smth_in_queue();    
    while (!queue.empty()){
        auto& cur_v = *(next_vertex());
        cur_v.put();
        // if (cur_v.can_put()){
        //     cur_v.put();
        // }
        if (queue.empty()){
            put_smth_in_queue();
        }
    }
    
    std::vector<std::vector<int>> answer;
    for (int g=0; g<groups.size(); g++){
        if (groups[g].vertexes.size() != 0){
            answer.push_back(groups[g].vertexes);
        }
    }

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