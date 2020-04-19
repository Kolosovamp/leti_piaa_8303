#include <iostream>
#include <cmath>
#include <ctime>

#define MAX_N 40
#define MIN_N 2
using namespace std;


static int N;
static int table[MAX_N][MAX_N];
static int proportion;
static string coord;


void initializeField();//инициализируем поле
void fill_2Square();//частный случай кратности двум
void fillOddSquare();//закрасить четыре очевидных квадрата
bool paintSq(int N, int start_x, int start_y, int color, int tableSize);//попытка вставить обрезок в опред место
void backTracking(int x, int y, int size, int color, int& K);
void printTable();
void printCoord(int color);

int main()
{
    //0 - некорректный размер квадрата (для захода в след цикл)
    N = 0;
    int K;
    proportion = 1;
   /* while(N < MIN_N || N > MAX_N){
        cout<<"Введите корректный размер столешницы."<<endl;
        cin>>N;
    }*/
    cin>>N;
    initializeField();//заполнили нулями
    long startTime = clock();

    if (N % 2 == 0){//в четном случае лучший вариант очевиден, координаты зависят от размера стола
        K = 4;
        cout<<K<<endl;
        fill_2Square();
        coord.clear();
        for(int i = 1; i <= K; i++)//печать координат для каждого цвета=квадрата
            printCoord(i);

        cout<<coord;
        cout<<"Время работы: "<<double(clock() - startTime)/CLOCKS_PER_SEC<<" сек"<<endl;
	
        return 0;

    }
    else{

        //ищем делитель
        for(int i = 2; i <= N; i++){
            if(N % i == 0){
                proportion = N/i;
                N = i;
                break;
            }
        }

        K = 1600;//потенциально недостижимое число (число свободных клеток при максимальной величине квадрата)

        //1 var
        fillOddSquare();
        if(N > 3)
            paintSq(2, N/2 + 1,N/2, 5, N);
        else paintSq(1,N/2+1, N/2, 5, N);
        backTracking(N/2, N/2 + 2, 4*(N/2 + 1)/5, 6, K);

        //2 var
        if(N > 3){

            initializeField();
            fillOddSquare();
            paintSq(1, N/2, N/2 + 2, 5, N);
            paintSq(3, N/2 + 1, N/2, 6, N);
            backTracking(N/2, N/2 + 3, 4*(N/2 + 1)/5, 7, K);
        }


        cout<<K<<endl;
        cout<<coord;
        cout<<"Время работы: "<<double(clock() - startTime)/CLOCKS_PER_SEC<<" сек"<<endl;
        
    }
    return 0;
}

void initializeField(){

    for (int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            table[i][j] = 0;
        }
    }
    return;

}

void fill_2Square(){

    int color = 1;
    for (int i = 0; i < N; i = i + N/2){
        for(int j = 0; j < N; j = j +N/2){
            paintSq(N/2, i, j, color, N);
            color++;
        }
    }
    return;

}

void fillOddSquare(){

    paintSq(N/2 + 1,  0, 0, 1, N);
    paintSq(N/2, N/2 + 1, 0, 2, N);
    paintSq(N/2, 0, N/2 +1, 3, N);
    paintSq(1, N/2, N/2 + 1, 4, N);

}

bool paintSq(int N, int start_x, int start_y, int color, int tableSize){

    if (start_x + N > tableSize || start_y + N > tableSize){//передача неверных координат
        return false;
    }

    for(int i = start_x; i < start_x + N; i++){
        for(int j = start_y; j < start_y + N; j++){
            if(table[i][j] != 0){//не хватает свободной площади
                return false;
            }
        }
    }

    //функция закрашивает область только в том случае, если свободной площади хватает
    for(int i = start_x; i < start_x + N; i++){
        for(int j = start_y; j < start_y + N; j++){
            table[i][j] = color;
        }
    }

    return true;

}

void printTable(){

    cout<<"Текущий вид поля:\n";
        for (int i = 0; i < N; i++){
            for(int l = 1; l <= proportion; l++){
                for (int j = 0; j < N; j++){
                    for(int l = 1; l <= proportion; l++){
                    if(table[i][j] < 10) cout<<" ";
                    cout << table[i][j]<<" ";
                    }
                }
                cout<<endl;
            }
        }

}


void printCoord(int clr){

    int sqSize = 0;
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            if(table[i][j] == clr){
                sqSize++;
            }
        }
    }
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            if(table[i][j] == clr){
                string s = to_string(proportion*i + 1);
                s.append(" ");
                s.append(to_string(proportion*j + 1));
                s.append(" ");
                s.append(to_string(int(proportion*sqrt(sqSize))));
                s.append("\n");
                coord.append(s);
                return;
            }
        }
    }

}


void backTracking(int x, int y, int size, int color, int& K){

    if(size == 0)
        return;
    if(color > K)
        return;

    if (y >= N){//x - номер строки, у - номер столбца
        x++;
        y = N/2;
    }

    if (x >= N){//некуда больше идти и все квадраты вставлены

        if(color - 1 < K){
            K = color - 1;
            coord.clear();

            for(int i = 1; i <= K; i++){
                printCoord(i);
            }
        }

        return;
    }

    while(size){

        if(paintSq(size, x, y, color, N)){
            backTracking(x, y + size, 4*(N/2 + 1)/5, color + 1, K);//попытка вставить максимальный обрезок
            //удалить только что вставленный квадрат
            for(int i = x; i < x + size; i++){
                for(int j = y; j < y + size; j++){
                    table[i][j] = 0;
                }
            }
        }
        size--;//не удалось вставить, потому что слишком большой размер

    }

    if(table[x][y]){//не удалось вставить?
        backTracking(x, y+1, 4*(N/2 + 1)/5, color, K);
    }

}
