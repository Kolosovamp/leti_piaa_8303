#include <iostream>
#include <vector>
#include <map>
#include<set>
#include <algorithm>
#include <climits>

using namespace std;

void inputGraph(size_t N);//чтение и запись графа и сети
int findPath(int bandwidth, char cur, map<char, bool> check, char stock);//поиск дополняющего пути
int findMaxFlow(char source, char stock);//поиск максимального потока
void printGraph();//печать результата

class Edge{

public:
    //остаточная пропускная способность ребра
    int maxBandwidth;
    //фактический поток по обратному ребру
    int factBandwidth;

    Edge(): maxBandwidth(0), factBandwidth(0){}
    Edge(int maxBandwidth){
        this->maxBandwidth = maxBandwidth;
        this->factBandwidth = 0;
    }

};

static vector<pair<char, char>> graph;//граф
static map<char, map<char, Edge>> residualNet;//остаточная сеть
static map<char, vector<char>> adjacentVertexMap;// карта смежных вершин

int main()
{
    size_t N;
    char source, stock;
    cin>>N;
    cin>>source;
    cin>>stock;

    inputGraph(N);
    cout<<"Запускаем поиск максимального потока"<<endl;
    findMaxFlow(source, stock);
    cout<<"Результат работы алгоритма:"<<endl;
    printGraph();


    return 0;
}


void inputGraph(size_t N){

    for(size_t i = 0; i < N; i++){
        char v1, v2;
        int bandwidth;//пропускная способность
        cin>>v1>>v2>>bandwidth;

        graph.push_back(pair<char, char>(v1, v2));
        residualNet[v1][v2] = Edge(Edge(bandwidth));
        adjacentVertexMap[v1].push_back(v2);
        adjacentVertexMap[v2].push_back(v1);
    }

    //сортировка графа в лексикографическом порядке
    sort(graph.begin(), graph.end(), [](pair<char, char> a, pair<char, char> b){
             if(a.first == b.first)
                 return a.second < b.second;
             return a.first < b.first;
         });

    for(auto it : adjacentVertexMap){
        //сортируем смежные вершины по близости к начальной
        char startV = it.first;
        sort(it.second.begin(), it.second.end(), [&startV](const char &a, const char &b) -> bool{
                 return abs(a - startV) < abs(b - startV);
             });
    }
    return;
}

int findPath(int bandwidth, char cur, map<char, bool> check, char stock){

    if(check[cur])
        return  0;
    check[cur] = true;
    cout<<cur;
    //текущая вершина - сток
    if(cur == stock)
        return bandwidth;

    //обход соседей
    for(auto &next : adjacentVertexMap[cur]){

        int factBandwidth = residualNet[cur][next].factBandwidth;
        int maxBandwidth = residualNet[cur][next].maxBandwidth;

        if(factBandwidth > 0){
            int newBandwidth = findPath(min(bandwidth, factBandwidth), next, check, stock);
            if(newBandwidth > 0){
                //изменяем остаточную пропускную способность и увеличиваем поток по обратному ребру
                residualNet[next][cur].maxBandwidth += newBandwidth;
                residualNet[cur][next].factBandwidth -= newBandwidth;
                //возвращаем минимальную пропускную способность пути
                return  newBandwidth;
            }
        }

        if(maxBandwidth > 0){
            int newBandwidth = findPath(min(bandwidth, maxBandwidth), next, check, stock);
            if(newBandwidth > 0){
                //изменяем остаточную пропускную способность и увеличиваем поток по обратному ребру
                residualNet[next][cur].factBandwidth += newBandwidth;
                residualNet[cur][next].maxBandwidth -= newBandwidth;
                //возвращаем минимальную пропускную способность пути
                return newBandwidth;
            }
        }

    }
    return 0;

}

int findMaxFlow(char source, char stock){

    int flow = 0, maxFlow = 0;

    for(;;){
        map<char, bool> checked;
        //поиск увеличивающего пути
        cout<<endl<<"Проходим по вершинам"<<endl;
        flow = findPath(INT_MAX, source, checked, stock);
        if(flow == 0 || flow == INT_MAX){//путь не найден
            cout<<endl<<"Увеличивающий путь не найден. ";
            cout<<"Максимальный поток - ";
            cout<<maxFlow<<endl;
            return 0;
        }
        //увеличение макисмального потока на данной итерации
        cout<<endl<<"Пропускная способность найденного увеличивающего пути - ";
        cout<<flow<<endl;
        maxFlow += flow;
    }

}

void printGraph(){

    for(auto const &elem : graph){
        cout<<elem.first<<" "<<elem.second<<" "<<residualNet[elem.second][elem.first].factBandwidth<<endl;
    }

}
