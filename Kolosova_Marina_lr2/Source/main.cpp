#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <string.h>


using namespace std;


class Edge{
public:
    pair<char, char> name;
    double lenght;
    bool checked;//для жадного алгоритма

    bool isCorrectEdge(char e1, char e2) const{
        return this->name.first == e1 && this->name.second == e2;
    }
    Edge(){}
    Edge(char v1, char v2, double lenght){
        this->name.first = v1;
        this->name.second = v2;
        this->lenght = lenght;
        this->checked = false;
    }
    friend bool operator== (const Edge &e1, const Edge &e2) {
        return e1.name.first == e2.name.first && e1.name.second == e2.name.second;
    }

};



bool cmp(const Edge &a, const Edge &b);
bool cmpPriority(pair <char, double> a, pair <char, double> b);
bool cmpForOpenset(pair<char, double> a, pair<char, double> b);
void findPath(char startVertex, char finalVertex, vector<Edge> &graph);//обычный жадный алгоритм
void findPathWithAStar(char startVertex, char finalVertex, vector<Edge> &graph);//A* алгоритм
void reconstructPath(map <char, char> vertexMap, char startVertex, char finalVertex);//построение пути
double heuristicFunction(char curVertex, char finalVertex);//эвристическая функция
void printPath(vector<char> path);

int main()
{
    string input;
    vector <Edge> graph;
    char startVertex, finalVertex;//переменная для хранения максимальной вершины
    ofstream out;
    out.open("/home/marina/Документы/piaa_2/graphFile.dot");//очищает файл, если в нем остались данные с предыдущего вызова
    out.open("/home/marina/Документы/piaa_2/graphFile.dot", ofstream::app);
    out.clear();
    out.write("digraph MyGraph {\n", 18);
    getline(cin, input);
    startVertex = input[0];
    finalVertex = input[2];

    while(getline(cin, input) && input != ""){        
        graph.push_back(Edge(input[0], input[2], stod(input.substr(4))));
   }   
   sort(graph.begin(), graph.end(), cmp);
   cout<<endl;

   cout<<"В таком виде граф хранится в памяти:"<<endl;
   vector <Edge>::iterator graphIt;
   for(graphIt = graph.begin(); graphIt != graph.end(); graphIt++){
       cout<<graphIt->name.first<<" "<<graphIt->name.second<<" "<<graphIt->lenght<<endl;

       char *newStr = new char[50];
       strcat(newStr, "    ");
       newStr[4] = graphIt->name.first;
       newStr[5] = '\0';
       strcat(newStr, " -> ");
       newStr[9] = graphIt->name.second;
       newStr[10] = '\0';
       strcat(newStr, " [label=");
       sprintf(newStr+18, "%f", graphIt->lenght);
       newStr[23] = '\0';
       strcat(newStr, "];\n");

       out.write(newStr, strlen(newStr));
   }
   out.write("}\n", 2);


   findPathWithAStar(startVertex, finalVertex, graph);
   //findPath(startVertex, finalVertex, graph);

   system("g++ ./Source/visualization.cpp -o showGraph");

   return 0;
}

void findPath(char startVertex, char finalVertex, vector<Edge> &graph){

    cout<<"Начинаем поиск кратчайшего пути со стартовой вершины "<<startVertex<<"."<<endl;

    {
        char currentVertex = startVertex;//текущая вершина
        bool flag = false;//есть смежные вершины ли нет
        vector<char> path;
        auto it = graph.begin();

        for(;it != graph.end(); it++){

            if(currentVertex == finalVertex){//текущая вершина - финальная
                path.push_back(currentVertex);
                cout<<"Путь найден."<<endl;
                printPath(path);
                return;
            }
            if(it->name.first == currentVertex && !it->checked){//находим смежное и еще непроверенное ребро с минимальной длиной
                flag = true;
                path.push_back(currentVertex);//добавляем текущую вершину в путь
                currentVertex = it->name.second;
                cout<<"Следующая вершина - "<<currentVertex<<endl;
                it->checked = true;//ставим отметку о проверке
                flag = false;
                it = graph.begin();//начинаем обход сначала

            }

            if(!flag && it == graph.end() - 1){//нет путей и смежных ребер для текущей вершины

                //ребро, выбор которого был ошибочный
                it = find(graph.begin(), graph.end(), Edge(path[path.size()-1], currentVertex, 0));

                cout<<"Данная вершина не имеет выходящих ребер. Исключаем ее из пути."<<endl;
                currentVertex = path[path.size()-1];//возвращаемся к предыдущей вершине
                it->checked =  true;//ставим отметку о проверке
                path.pop_back();//удаляем ошибочную вершину из пути

                it = graph.begin();//начинаем обход сначала
            }
        }
    }

    return;
}


void findPathWithAStar(char startVertex, char finalVertex, vector<Edge> &graph){


    map <char, double> openset;//список вершин для обработки с приоритетам
    map <char, double> minPathToVertex;//минимальные пути до вершины

    minPathToVertex[startVertex] = 0;//путь из вершины в саму себя
    openset[startVertex] = minPathToVertex[startVertex] + heuristicFunction(startVertex, finalVertex);
    cout<<"Текущая вершина - "<<startVertex<<endl;
    cout<<"Начинаем обработку вершин с приоритетами."<<endl;

    {
        map <char, char> vertexMap;//карта пройденных вершин
        vector<char> inChecked;//для проверки вхождения в список проверенных
        while(!openset.empty()){//обработка вершин

            //выбираем текущую вершину с минимальным приоритетом
            char curVertex = min_element(openset.begin(), openset.end(), cmpPriority)->first;
            cout<<"Вершина с минимальным приоритетом - "<<curVertex<<endl;

            if(curVertex == finalVertex){//достигли финальной вершины
                cout<<"Путь найден."<<endl;
                reconstructPath(vertexMap, startVertex, finalVertex);
                return;
            }


            cout<<"Добавляем эту вершину в список проверенных и начинаем проверять соседей."<<endl;
            openset.erase(curVertex);
            inChecked.push_back(curVertex);

            //доходим до смежных ребер
            auto graphIt = graph.begin();
            for(;graphIt != graph.end(); graphIt++){
                if(graphIt->name.first == curVertex){//смежные ребра в графе расположены подряд
                    break;
                }
            }

            //проверка соседей текущей вершины
            for(;graphIt->name.first == curVertex && graphIt != graph.end();graphIt++){//проверяем всех соседей текущей вершины
                char nextVertex = graphIt->name.second;

                //пропускаем соседей из закрытого списка
                if(find(inChecked.begin(), inChecked.end(), nextVertex) != inChecked.end()){//пропускаем проверенные вершины
                    cout<<"Вершина "<<nextVertex<<" уже проверена."<<endl;
                    continue;
                }

                bool needChangeValues = false;
                if(openset.find(nextVertex) == openset.end()){//соседа нет в списке для проверки
                    needChangeValues = true;
                }else{//сосед в очереди на проверку
                    if(minPathToVertex[curVertex] + graphIt->lenght < minPathToVertex[nextVertex]){//текущий путь меньше записанного
                        needChangeValues = true;
                    }else{
                        needChangeValues = false;
                    }
                }
                cout<<"Проверяем вершину "<<nextVertex<<endl;
                if(needChangeValues){//нужно пересчитать путь и изменить приоритет
                    cout<<"Расчитываем приоритет."<<endl;
                    vertexMap[nextVertex] = curVertex;//записывается вершина, из кт мы прошли
                    minPathToVertex[nextVertex] = minPathToVertex[curVertex] + graphIt->lenght;
                    openset[nextVertex] = minPathToVertex[nextVertex] + heuristicFunction(nextVertex, finalVertex);
                }
            }
        }
    }

    return;
}
void reconstructPath(map <char, char> vertexMap, char startVertex, char finalVertex){

    vector<char> buf_path;

    {
        char curVertex = finalVertex;
        buf_path.push_back(curVertex);

        while (curVertex != startVertex) {//пока не дойдем до начальной вершины
            //получаем вершину, из которой пришли в текущую
            curVertex = vertexMap[curVertex];
            buf_path.push_back(curVertex);
        }
    }

    //разворачиваем вектор
    vector<char> path;
    for(auto it = buf_path.end() - 1; it >= buf_path.begin(); it--){
        path.push_back(*it);
    }
    printPath(path);
    return;
}



bool cmp(const Edge &a, const Edge &b){

    if(a.name.first == b.name.first){//сравнение по длине ребра
        if(a.lenght == b.lenght)
            return a.name.second < b.name.second;
        return a.lenght < b.lenght;
    }
    else return a.name.first < b.name.first;//сравнение по первой вершине
}

double heuristicFunction(char curVertex, char finalVertex){//эвристичнкая функция - близость символов
    return finalVertex - curVertex;
}

bool cmpPriority(pair <char, double> a, pair <char, double> b){//сравнение вершин по приоритетам
    return  a.second < b.second;
}

bool cmpForOpenset(pair<char, double> a, pair<char, double> b){//сравнение вершин по названию
    return a.first < b.first;
}

void printPath(vector<char> path){
    for(auto it = path.begin(); it != path.end(); it++){
        cout<<*it;
    }
    cout<<endl;
}
