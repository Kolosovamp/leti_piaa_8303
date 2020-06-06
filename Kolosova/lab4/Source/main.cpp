#include <iostream>
#include <string>
#include <vector>
#include <cmath>


using namespace std;

//префикс-функция
void prefixFunc(std::string& p, std::vector<size_t>& pi){
    size_t j = 0;
    pi[0] = 0;

    for(size_t i = 1;i < p.size();){
        if(p[j] == p[i]){
            pi[i] = j+1;
            j++;
            i++;
        }
        else{
            if(j == 0){
                pi[i]= 0;
                i++;
            }
            else{
                j = pi[j-1];
            }
        }
    }

}

//КМП-алгоритм
void KMP(std::string& t, std::string& p, size_t flowCount, bool isForShift = false){

    vector<size_t> res;
    vector<size_t> pi (p.size());
    prefixFunc(p, pi);
    cout << endl;
    cout << "Значение префикс функции для образца:" <<endl;
    for(auto &el : pi){
        cout << el << " ";
    }
    cout << endl <<endl;

    size_t j, start, end;
    cout << "В алгоритме используется максимально возможное количество потоков." << endl;
    if(t.length() / flowCount < p.length())
	    flowCount = t.length() / p.length();

    for(size_t f = 0; f < flowCount; f++){

        cout << "Обработка потока №" << f + 1 << ": ";
        j = 0;
        size_t r = t.length() % flowCount;
        size_t flowLen = t.length() / flowCount;
        if(r > 0)
            flowLen++;
        start = f * flowLen;//выделяем начальную позицию для текущего потока
        end = start + flowLen;//выделяем конечную позицию для текущего потока
        end += p.length() - 1;
        bool isFound = false;
        if(end > t.size())
            end = t.size();
        for(size_t s = start; s < end; s++)
            cout<<t[s];
        cout << endl << endl;

        for(size_t i = start; i < end;){            
            for (size_t k = start; k < i; k++) {
                cout << t[k];
            }
            cout << " (" << t[i] << ")";
            for(size_t k = i + 1; k < end; k++) {
                cout << t[k];
            }
            cout << endl;
            string shift = "";
            for(size_t k = 0; k < i - start - j + 1; k++){
                shift += " ";
            }


            cout << shift;
            for(size_t k = 0; k < p.size(); k++){
                if( k == j){
                    cout << "(" << p[k] << ")";
                }
                else cout << p[k];
            }

            if (t[i] == p[j]) {
                cout << endl << endl;
                i++;
                j++;
                if(j == p.size()){
                    cout << "Найдено вхождение образца" << endl;
                    cout << "Индекс вхождения: ";
                    cout << i - p.size() << endl << endl;
                    res.push_back(i - p.size());
                    isFound = true;
                    j = pi[j - 1];
                    if(isForShift) {
                        cout<<endl;
                        break;
                    }
                }
            }
            else{
                cout << " - символы не совпадают." << endl << endl;
                if(j == 0) {
                    i++;
                }
                else {
                    j = pi[j-1];
                }
            }
        }
        cout<<endl;
        if(isFound == false && f == flowCount - 1 && res.empty()) {
            cout << "Вхождений не найдено" << endl;;
            cout<<-1<<endl;
            return;
        }
    }
    if(res.empty()) {
        cout << "Вхождений не найдено" <<endl;
        cout<<-1<<endl;
        return;
    }

    cout << "\nРезультат работы программы:" << endl;
    string sep = "";
    for(auto &el : res){
        cout << sep << el;
        sep = ",";
    }
    cout<<endl;

}

void cyclicShift(std::string& strA, std::string& strB, size_t flowCount){

    cout << "Строки равны:" << endl;
    if(strA == strB){
        std::cout<<0<<std::endl;
        return;
    }
    cout << "Строки разного размера:" << endl;
    if(strA.size() != strB.size()){
        std::cout<<-1<<std::endl;
        return;
    }
    strA += strA;
    KMP(strA, strB, flowCount, true);

}

int main() {
    string p, t;
    size_t flowCount;
    cin>>flowCount;
    cin>>p;
    cin>>t;
    KMP(t, p, flowCount);
    //cyclicShift(p, t, flowCount);
    return 0;
}
