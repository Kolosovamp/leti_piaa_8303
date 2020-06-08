#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <queue>
#include <algorithm>
#include <unordered_map>

using namespace std;

class Tree {

    string dbgStr = ""; // Для отладки
    char value; // Значение узла
    size_t numOfPattern = 0; // Номер введенного паттерна
    Tree *parent = nullptr; // Родитель ноды
    Tree* suffixLink = nullptr; // Суффиксная ссылка
    Tree* finishLink = nullptr; // конечная ссылка
    unordered_map <char, Tree*> children; // Потомки узла
public:
    Tree() : value('\0') {}
    Tree(char val) : value(val) {} // Конструктор ноды
    void initialization(vector<string> patterns){
        for(auto &pattern : patterns){
            this->insert(pattern);
        }
    }
    void printInfo(Tree *curr){

        cout << curr->dbgStr << ':' << endl;

        if (curr->suffixLink)
               cout << "\tСуффиксная ссылка: " << (curr->suffixLink == this ? "Корень" : curr->suffixLink->dbgStr) << endl;
        if(curr->finishLink)
               cout << "\tКонечная ссылка: " << (curr->finishLink->dbgStr) << endl;

        if(curr -> parent)
            cout << "\tРодитель: "  << (curr->parent->value ? curr->parent->dbgStr :  "Корень") << endl;

        if (!curr->children.empty())
            cout << "\tПотомок: ";
        for (auto child : curr->children) {
            cout << child.second->value << ' ';

        }
    }
    // Вставка подстроки в бор
    void insert(const string &str) {
        auto curr = this;
        static size_t countPatterns = 0;

        for (char c : str) { // Идем по строке
            // Если из текущей вершины по текущему символу не было создано перехода
            if (curr->children.find(c) == curr->children.end()) {
                // Создаем переход
                curr->children[c] = new Tree(c);
                curr->children[c]->parent = curr;
                curr->children[c]->dbgStr += curr->dbgStr + c;
            }
            // Спускаемся по дереву
            curr = curr->children[c];
        }

        cout << "Вставляем строку: " << str << endl;
        printBor();

        // Показатель терминальной вершины, значение которого равно порядковому номеру добавления шаблона
        curr->numOfPattern = ++countPatterns;
    }

    //печать бора
    void printBor() {
        cout << "Текущее состояние бора:" << endl;

        queue<Tree *> queue;
        queue.push(this);

        while (!queue.empty()) {

            auto curr = queue.front();
            if (!curr->value)
                cout << "Корень:" << endl;
            else
                printInfo(curr);
                for (auto child : curr->children) {
                    queue.push(child.second);
                }

            queue.pop();
            cout << endl;
        }
        cout << endl;

    }


    // Функция для поиска подстроки в строке при помощи автомата
    vector<size_t> find(const char c) {
        static const Tree *curr = this; // Вершина, с которой необходимо начать следующий вызов
        cout << "Ищем '" << c << "' из: " << (curr->dbgStr.empty() ? "Корень" : curr->dbgStr) << endl; // Дебаг

        for (; curr != nullptr; curr = curr->suffixLink) {
            // Обходим потомков, если искомый символ среди потомков не найден, то
            // переходим по суффиксной ссылке для дальнейшего поиска
            for (auto child : curr->children)
                if (child.first == c) { // Если символ потомка равен искомому
                    curr = child.second; // Значение текущей вершины переносим на этого потомка
                    vector<size_t> visited; // Вектор номеров найденных терм. вершин
                    // Обходим суффиксы, т.к. они тоже могут быть терминальными вершинами
                    for (auto temp = curr; temp->suffixLink; temp = temp->suffixLink)
                        if (temp->numOfPattern)
                            visited.push_back(temp->numOfPattern - 1);
                    //

                    cout << "Символ найден!" << endl; // Дебаг
                    return visited;
                }

            if (curr->suffixLink) {
                cout << "Переходим по суффиксной ссылке: ";
                cout << (curr->suffixLink->dbgStr.empty() ? "Корень" : curr->suffixLink->dbgStr) << endl;
            }
        }
        cout << "Символ не найден!" << endl; // Дебаг

        curr = this;
        return {};
    }

    // Функция для построения недетерминированного автомата
    void makeAutomaton() {

        cout << "Строим автомат: " << endl;
        queue<Tree *> queue; // Очередь для обхода в ширину

        for (auto child : children) // Заполняем очередь потомками корня
            queue.push(child.second);

        while (!queue.empty()) {
            auto curr = queue.front(); // Обрабатываем вершину из очереди
            printInfo(curr);
            // Заполняем очередь потомками текущей верхушки
            for (auto child : curr->children) {
                queue.push(child.second);
            }

            if (!curr->children.empty())
                cout << endl;

            queue.pop();
            auto p = curr->parent; // Ссылка на родителя обрабатываемой вершины
            char x = curr->value; // Значение обрабатываемой вершины
            if (p)
                p = p->suffixLink; // Если родитель существует, то переходим по суффиксной ссылке

            // Пока можно переходить по суффиксной ссылке или пока
            // не будет найден переход в символ обрабатываемой вершины
            while (p && p->children.find(x) == p->children.end())
                p = p->suffixLink; // Переходим по суффиксной ссылке

            // Суффиксная ссылка для текущей вершины равна корню, если не смогли найти переход
            // в дереве по символу текущей вершины, иначе равна найденной вершине
            curr->suffixLink = p ? p->children[x] : this;

            // Дебаг
            cout << "\tСуффиксная ссылка: " << (curr->suffixLink == this ? "Корень" : curr->suffixLink->dbgStr) << endl << endl;
        }

        // Дебаг
        cout << endl;
        printBor();
    }



    void makeFinishLink(){

        cout << "Строим конечные ссылки" << endl;

        queue<Tree *> queue;
        queue.push(this);

        while (!queue.empty()) {

            auto curr = queue.front();
            auto next = curr;
            //проходим по суффиксным ссылкам каждой вершины автомата
            while(1){

                if(next->suffixLink && next->suffixLink->value){//есть возможность перейти по суффиксной ссылке не в корень
                    next = next->suffixLink;//переходим
                }
                else break;//цепочка суффиксных ссылок закончилась

                if(next->numOfPattern){//вершина - терминальная
                    curr->finishLink = next;//строим конечную ссылку
                    break;
                }


            }
            //обход в ширину
            for (auto child : curr->children) {
                queue.push(child.second);
            }

            queue.pop();
        }
        printBor();
    }

    void findMaxLinkChain(){//индивидуализация поиск максимальных цепей

        size_t maxSuffixChain = 0;
        size_t maxFinishChain = 0;
        size_t buf = 0;//для хранения длины цепочки из текущей вершины

        queue<Tree *> queue;
        queue.push(this);

        while (!queue.empty()) {

            auto curr = queue.front();
            auto next = curr;

            //проходим по суффиксным ссылкам каждой вершины автомата
            if(curr->value)
                cout << curr->dbgStr << ":" << endl << "\tСуффиксная цепочка ";
            cout << curr->dbgStr;
            buf = 0;
            while(1){
                if(next->suffixLink ){//&& next->suffixLink->value){//есть возможность перейти по суффиксной ссылке не в корень
                    next = next->suffixLink;//переходим
                    cout << "->" << next->dbgStr;
                    buf++;//увеличиваем длину цепи

                }
                else break;//цепочка суффиксных ссылок закончилась
            }
            cout << "Корень" << endl;
            maxSuffixChain = max(maxSuffixChain, buf);
            //cout << "Текущая максимальная длина цепи суффиксных ссылок: " << maxSuffixChain << endl;

            buf = 0;
            next = curr;
            if(curr->finishLink)
                cout << "\tЦепочка конечных ссылок " << curr->dbgStr;
            else cout << endl;
            while(1){
                if(next->finishLink ){//есть возможность перейти по конечной ссылке
                    next = next->finishLink;//переходим
                    if(next->dbgStr != "")
                        cout << "->" << next->dbgStr;
                    buf++;//увеличиваем длину цепи
                }
                else break;//цепочка суффиксных ссылок закончилась
            }
            maxFinishChain = max(maxFinishChain, buf);
            //cout << "Текущая максимальная длина цепи конечных ссылок: " << maxFinishChain <<endl;

            //обход в ширину
            for (auto child : curr->children) {
                queue.push(child.second);
            }

            queue.pop();
            cout << endl;
        }
        cout << endl;

        cout << "Максимальная длина цепи из суффиксных ссылок - " << maxSuffixChain << endl;
        cout << "Максимальная длина цепи из конечных ссылок - " << maxFinishChain << endl;
        cout << endl;

    }

    ~Tree() { // Деструктор ноды
        for (auto child : children) delete child.second;
    }


};

auto AhoCorasick(const string &text, const vector <string> &patterns)
{
    Tree bor;
    set <pair<size_t, size_t>> result;

    bor.initialization(patterns);
    bor.makeAutomaton(); // Из полученного бора создаем автомат (путем добавления суффиксных ссылок)
    bor.makeFinishLink();//добавляем конечные ссылки
    bor.findMaxLinkChain();//поиск максимальных длин цепей ссылок


    {
        size_t j = 0;
        for(auto &el : text){//поиск для каждого символа строки
            for(auto pos : bor.find(el))// Проходим по всем найденным позициям, записываем в результат
                result.emplace(j - patterns[pos].size() + 2, pos + 1);
            j++;
        }
    }

    return result;
}

int main()
{
    string text;
    size_t n;
    cin >> text >> n;
    vector <string> patterns(n);//словарь

    for(auto &pattern : patterns){
        cin >> pattern;
    }

    auto res = AhoCorasick(text, patterns);
    for (auto r : res)
        cout << r.first << ' ' << r.second << endl;

    return 0;
}
