#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector> 
#include <cstdlib> 
#include <time.h>
#include <math.h>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <limits.h>
using namespace std;

struct Data {
    int x;//number od task
    vector<int> p;  //time of execution of subsequent tasks
    int total_task_time = 0; //total time of execution of subsequent tasks
};

struct Times {
    int execution_time;
    int time_from_begin;
    int time_from_end;
};

vector<Data> load_data(int numer, int& n, int& m, ifstream& file) {
    string s;
    string target = "data.";
    stringstream ss;
    ss << setw(3) << setfill('0') << numer << ":";
    target += ss.str();
    cout << target << " ";
    while (s != target) file >> s;
    file >> n >> m;
    vector<Data> data(n);
    for (int i = 0; i < n; i++) {
        data[i].x = i;
        for (int j = 0; j < m; j++) {
            int time;
            file >> time;
            data[i].p.push_back(time);
        }
    }
    return data;
}

void sorting_data(vector<Data>& data, int& n, int& m) {
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++) {
            data[i].total_task_time += data[i].p[j];
        }
    }

    sort(data.begin(), data.end(), [](const Data& task1, const Data& task2) {
        if (task1.total_task_time != task2.total_task_time) {
            return task1.total_task_time > task2.total_task_time;
        }
        else {
            return task1.x < task2.x;
        }
        });
}

int quickneh(vector<Data>& data, int& n, int& m, int& the_best_time) {
    vector<vector<Times>> graph(n, vector<Times>(m));
    
    for (int i = 0; i < n; i++)
    {
        if (i < n - 1)the_best_time = INT_MAX;
        for (int j = 0; j < i + 1; j++)
        {
            for (int machine = 0; machine < m; machine++)
            {
                graph[j][machine].execution_time = data[j].p[machine];
            }
        }

        graph[0][0].time_from_begin = data[0].p[0];
        for (int j = 0; j < i + 1; j++)
        {
            for (int machine = 0; machine < m; machine++)
            {
                if (machine == 0 and j != 0) {
                    graph[j][machine].time_from_begin = data[j].p[machine] + graph[j - 1][machine].time_from_begin;
                }
                if (j == 0 and machine != 0) {
                    graph[j][machine].time_from_begin = data[j].p[machine] + graph[j][machine - 1].time_from_begin;
                }
                if (j != 0 and machine != 0) {
                    graph[j][machine].time_from_begin = data[j].p[machine] + max(graph[j][machine - 1].time_from_begin, graph[j - 1][machine].time_from_begin);
                }
            }
        }

        graph[i][m - 1].time_from_end = data[i].p[m - 1];
        for (int j = i; j >= 0; j--)
        {
            for (int machine = m - 1; machine >= 0; machine--)
            {
                if (machine == m - 1 and j != i) {
                    graph[j][machine].time_from_end = data[j].p[machine] + graph[j + 1][machine].time_from_end;
                }
                if (j == i and machine != m - 1) {
                    graph[j][machine].time_from_end = data[j].p[machine] + graph[j][machine + 1].time_from_end;
                }
                if (j != i and machine != m - 1) {
                    graph[j][machine].time_from_end = data[j].p[machine] + max(graph[j][machine + 1].time_from_end, graph[j + 1][machine].time_from_end);
                }
            }
        }

        //algorithm
        if (i != n - 1) {
            int the_best_node = -1;
            int the_best_place;
            int time;
            int memory = 0;
            for (int place = 0; place < i + 2; place++) {
                for (int node = 0; node < m; node++)
                {
                    time = 0;
                    if (place == 0) {
                        for (int u = 0; u <= node; u++) {
                            time += data[i + 1].p[u];
                        }
                        time += graph[place][node].time_from_end;
                        if (time > the_best_node) {
                            the_best_node = time;
                        }
                    }
                    time = 0;
                    if (place == i + 1) {
                        for (int u = 0; u <= node; u++) {
                            time += data[i + 1].p[m - 1 - u];
                        }
                        time += graph[place - 1][m - 1 - node].time_from_begin;
                        if (time > the_best_node) {
                            the_best_node = time;
                        }
                    }
                    time = 0;
                    if (place != i + 1 and place != 0) {
                        time = max(memory, graph[place - 1][node].time_from_begin) + data[i + 1].p[node] + graph[place][node].time_from_end;
                        memory = max(memory, graph[place - 1][node].time_from_begin) + data[i + 1].p[node];
                        if (time > the_best_node) {
                            the_best_node = time;

                        }
                    }
                }
                memory = 0;
                if (the_best_node < the_best_time) {
                    the_best_time = the_best_node;
                    the_best_place = place;
                }
                the_best_node = -1;
            }
            rotate(data.begin() + the_best_place, data.begin() + i + 1, data.begin() + i + 2);
        }
    }
    return the_best_time;
}

int main()
{
    auto start = std::chrono::high_resolution_clock::now();
    
    vector<Data> data;
    int n, m;
    ifstream file("neh.data.txt");
    for (int number = 0; number < 121; number++) {
        
        data = load_data(number, n, m, file);
        sorting_data(data, n, m);

        int the_best_time;
        the_best_time = quickneh(data, n, m, the_best_time); 
           
        cout << the_best_time << endl;
       /* for (int t = 0; t < n; t++)
        {
            cout << data[t].x + 1 << " ";
        }*/
        //cout << endl;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Total time: " << elapsed.count() << " seconds" << std::endl;

    file.close();
    return 0;
}

