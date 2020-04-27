#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <queue>
#include <fstream>
#include <string.h>
#include <cstdlib>
//#include <unistd.h>


using namespace std;

static char startVertex;
static char finalVertex;
static vector <char> path;//вектор пути
static vector <char>:: iterator vecIt;//итератор вектора пути
static char maxVertex;

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

static vector <Edge> graph;


bool cmp(const Edge &a, const Edge &b);
bool cmpPriority(pair <char, double> a, pair <char, double> b);
bool cmpForOpenset(pair<char, double> a, pair<char, double> b);
void findPath();//обычный жадный алгоритм
void findPathWithAStar();//A* алгоритм
void reconstructPath(map <char, char> vertexMap);//построение пути
double heuristicFunction(char curVertex);//эвристическая функция

int main()
{
    string input;
    maxVertex = 'a';//переменная для хранения максимальной вершины
    ofstream out;
    out.open("/home/marina/Документы/piaa_2/Source/graphFile.dot");//очищает файл, если в нем остались данные с предыдущего вызова
    out.open("/home/marina/Документы/piaa_2/Source/graphFile.dot", ofstream::app);
    out.clear();
    out.write("digraph MyGraph {\n", 18);
    //считываем вершины для поиска пути
    getline(cin, input);
    startVertex = input[0];
    finalVertex = input[2];

    while(getline(cin, input) && input != ""){
        if(input[0] > maxVertex)
            maxVertex = input[0];
        if(input[2] > maxVertex)
            maxVertex = input[2];
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


   findPathWithAStar();
   //findPath();

   //вывод пути
   for(vecIt = path.begin(); vecIt != path.end(); vecIt++){
       cout<<*vecIt;
   }
   cout<<endl;
   system("g++ ./Source/visualization.cpp -o showGraph");
   return 0;
}

void findPath(){

    char currentVertex = startVertex;//текущая вершина
    vector <Edge>::iterator it;//итератор графа
    bool flag = false;//есть смежные вершины ли нет
    cout<<"Начинаем поиск кратчайшего пути со стартовой вершины."<<endl;

    for(it = graph.begin(); it != graph.end(); it++){

        if(currentVertex == finalVertex){//текущая вершина - финальная
            path.push_back(currentVertex);
            cout<<"Путь найден."<<endl;
            return;
        }
        if(it->name.first == currentVertex && !it->checked){//находим смежное и еще непроверенное ребро с минимальной длиной
            flag = true;
            path.push_back(currentVertex);//добавляем текущую вершину в путь
            cout<<"Текущий вид пути:"<<endl;
            for(vecIt = path.begin(); vecIt != path.end(); vecIt++){
                cout<<*vecIt;
            }
            cout<<endl;
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

    return;
}


void findPathWithAStar(){
    vector <Edge>::iterator graphIt;//итератор графа

    //vector <char> checked;//обработанные вершины
    map <char, bool> inChecked;//для проверки вхождения в список проверенных

    vector <pair<char, double>> openset;//список вершин для обработки с приоритетами
    vector <pair<char, double>>::iterator opensetIterator;//итератор для списка обработки
    map <char, bool> inOpenSet;//для проверки вхождения в очередь для обработки

    char curVertex;//текущая вершина
    map <char, char> vertexMap;//карта пройденных вершин
    map <char, double> minPathToVertex;//минимальные пути до вершины

    bool needChangeValues = false;//флаг, говорящий о необходимости изменений кратчайшего пути

    //map <char, double> priority;//приоритеты вершин


    //инициализация
    for(char cur = 'a'; cur <= maxVertex; cur++){
        inChecked[cur] = false;
        inOpenSet[cur] = false;
    }
    minPathToVertex[startVertex] = 0;//путь из вершины в саму себя
    //добавляем в очередь на проверку стартовую вершину с приоритетом
    openset.push_back(pair<char, double>(startVertex, minPathToVertex[startVertex] + heuristicFunction(startVertex)));
    curVertex = startVertex;

    cout<<"Текущая вершина - "<<curVertex<<endl;
    cout<<"Начинаем обработку вершин с приоритетами."<<endl;
    while(!openset.empty()){//обработка вершин

        //выбираем текущую вершину с минимальным приоритетом
        opensetIterator = min_element(openset.begin(), openset.end(), cmpPriority);
        pair<char, double> min_priority = *opensetIterator;
        curVertex = min_priority.first;
        cout<<"Вершина с минимальным приоритетом - "<<curVertex<<endl;

        if(curVertex == finalVertex){//достигли финальной вершины
            cout<<"Путь найден."<<endl;
            reconstructPath(vertexMap);
            return;
        }

        //Перемещаем вершину в список проверенных
        openset.erase(opensetIterator);
        inOpenSet[curVertex] = false;
        inChecked[curVertex] = true;
        //checked.push_back(curVertex);
        cout<<"Добавляем эту вершину в список проверенных и начинаем проверять соседей."<<endl;

        //доходим до смежных ребер
        for(graphIt = graph.begin(); graphIt != graph.end(); graphIt++){
            if(graphIt->name.first == curVertex){//смежные ребра в графе расположены подряд
                break;
            }
        }

        //проверка соседей текущей вершины
        for(;graphIt->name.first == curVertex && graphIt != graph.end();graphIt++){//проверяем всех соседей текущей вершины
            char nextVertex = graphIt->name.second;

            //пропускаем соседей из закрытого списка
            if(inChecked[nextVertex]){//пропускаем проверенные вершины
                cout<<"Вершина "<<nextVertex<<" уже проверена."<<endl;
                continue;
            }
            if(!inOpenSet[nextVertex]){//соседа нет в списке для проверки
                inOpenSet[nextVertex] = true;
                openset.push_back(pair<char, double>(nextVertex, 9999));//добавляем соседа в список для проверки
                opensetIterator = openset.end();
                opensetIterator--;
                needChangeValues = true;
            }else{//сосед в очереди на проверку
                if(minPathToVertex[curVertex] + graphIt->lenght < minPathToVertex[nextVertex]){//текущий путь меньше записанного
                    needChangeValues = true;
                    pair<char, double> v;
                    v.first = nextVertex;
                    v.second = minPathToVertex[nextVertex] + heuristicFunction(nextVertex);
                    opensetIterator = find(openset.begin(), openset.end(), v);
                }else{
                    needChangeValues = false;
                }
            }
            cout<<"Проверяем вершину "<<nextVertex<<endl;
            if(needChangeValues){//нужно пересчитать путь и изменить приоритет
                cout<<"Расчитываем приоритет."<<endl;
                vertexMap[nextVertex] = curVertex;//записывается вершина, из кт мы прошли
                minPathToVertex[nextVertex] = minPathToVertex[curVertex] + graphIt->lenght;
                opensetIterator->second = minPathToVertex[nextVertex] + heuristicFunction(nextVertex);
            }
        }
    }
    return;
}
void reconstructPath(map <char, char> vertexMap){
    //поиск начинается от финиша
    vector <char> buf_path;
    char curVertex = finalVertex;
    buf_path.push_back(curVertex);
    while (curVertex != startVertex) {//пока не дойдем до начальной вершины
        //получаем вершину, из которой пришли в текущую
        curVertex = vertexMap[curVertex];
        buf_path.push_back(curVertex);
    }
    //разворачиваем вектор
    for(vecIt = buf_path.end() - 1; vecIt >= buf_path.begin(); vecIt--){
        path.push_back(*vecIt);
    }
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

double heuristicFunction(char curVertex){//эвристичнкая функция - близость символов
    return finalVertex - curVertex;
}

bool cmpPriority(pair <char, double> a, pair <char, double> b){//сравнение вершин по приоритетам
    return  a.second < b.second;
}

bool cmpForOpenset(pair<char, double> a, pair<char, double> b){//сравнение вершин по названию
    return a.first < b.first;
}
