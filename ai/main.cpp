#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <ctime>
#include <fstream>
#include <algorithm>

using namespace std;

// دالة sigmoid
double sigmoid(double x) {
    return 1.0 / (1.0 + exp(-x));
}

// مشتقة دالة sigmoid
double sigmoid_derivative(double x) {
    return x * (1.0 - x);
}

class Player {
public:
    vector<int> hidden_layer;
    int output;
    int input;
    vector<double> input_value;
    vector<vector<vector<double>>> w;
    vector<int> game;

    Player(vector<int> hidden_layer_, int input_) {
        hidden_layer = hidden_layer_;
        output = hidden_layer.back();
        input = input_;
        game = {0,0,0,0,0,0,0,0,0};
        w.push_back(vector<vector<double>>());
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<> dis(0.0, 1.0);
        
        for(int i = 0; i < hidden_layer[0]; i++) {
            vector<double> temp;
            for(int x = 0; x < input; x++) {
                temp.push_back(dis(gen));
            }
            temp.push_back(1.0);
            w[0].push_back(temp);
        }
        
        for(int x = 1; x < hidden_layer.size(); x++) {
            w.push_back(vector<vector<double>>());
            for(int i = 0; i < hidden_layer[x]; i++) {
                vector<double> temp;
                for(int x2 = 0; x2 < hidden_layer[x-1]; x2++) {
                    temp.push_back(dis(gen));
                }
                temp.push_back(1.0);
                w[x].push_back(temp);
            }
        }
    }

    vector<double> predict(vector<int>& input_value) {
        vector<vector<double>> last_layer(1);
        for(auto& x : w[0]) {
            double sum_val = 0;
            for(int e = 0; e < input_value.size(); e++) {
                sum_val += input_value[e] * x[e];
            }
            last_layer[0].push_back(sigmoid(sum_val + x.back()));
        }
        
        for(int i = 1; i < w.size(); i++) {
            last_layer.push_back(vector<double>());
            for(auto& x : w[i]) {
                double sum_val = 0;
                for(int e = 0; e < last_layer[i-1].size(); e++) {
                    sum_val += last_layer[i-1][e] * x[e];
                }
                last_layer[i].push_back(sigmoid(sum_val + x.back()));
            }
        }
        return last_layer.back();
    }

    void update() {
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<> dis(-1.0, 1.0);
        for(int x = 0; x < w.size(); x++) {
            for(int i = 0; i < w[x].size(); i++) {
                for(int e = 0; e < w[x][i].size(); e++) {
                    w[x][i][e] += dis(gen);
                }
            }
        }
    }

    vector<vector<vector<double>>> get_model() {
        return w;
    }

    void save() {
        ofstream file("data.pkl", ios::binary);
        // Note: Direct binary serialization of nested vectors isn't straightforward in C++
        // This is a placeholder - proper serialization would need a custom implementation
        file.close();
    }

    void load(string path) {
        ifstream file(path, ios::binary);
        // Note: Deserialization would need custom implementation
        file.close();
    }
};

pair<int,int> startgame(Player& x) {
    int m = 0;
    int score = 0;
    vector<double> pred = x.predict(x.game);
    vector<double> valid_values;
    for(double value : pred) {
        if(0.5 < value && value < 1) {
            valid_values.push_back(value);
        }
    }
    if(valid_values.size() == 1) {
        for(int i = 0; i < pred.size(); i++) {
            if(pred[i] == valid_values[0]) {
                x.game[i] = 1;
                break;
            }
        }
    }
    score = count(x.game.begin(), x.game.end(), 1);
    int chayta = valid_values.size();
    return {score, chayta};
}

void print_board(vector<int>& board) {
    for(int i = 0; i < 9; i += 3) {
        cout << board[i] << " | " << board[i+1] << " | " << board[i+2] << endl;
        if(i < 6) {
            cout << "--+---+--" << endl;
        }
    }
}
int main() {
    vector<Player> models;
    vector<vector<Player>> bestmodel(9);
    int bestscore = 0;
    vector<int> hidden_layer = {10,10,9};
    
    for(int i = 0; i < 100; i++) {
        models.push_back(Player(hidden_layer, 9));
    }

    Player new_model_m(hidden_layer, 9);

    for(int ep = 0; ep < 10000; ep++) {
        int m = 0;
        int eq = 9999;
        int eq_ = 9999;
        
        for(auto& x : models) {
            vector<double> pred = x.predict(x.game);
            m = 0;
            int score = 0;
            int chayta = 0;
            
            for(int e = 0; e < pred.size(); e++) {
                if(pred[e] > 0.5 && pred[e] < 1 && x.game[e] == 0) {
                    m += 1;
                }
            }
            
            if(m == 1) {
                pair<int,int> result = startgame(x);
                score = result.first;
                chayta = result.second;
            }
            
            if(m < eq && m > 0 && score == 0) {
                eq = m;
                bestmodel[m-1].push_back(x);
            }
            else if(score > bestscore) {
                bestmodel[m-1].push_back(x);
                bestscore = score;
            }
            else if(score == bestscore && chayta < eq_) {
                eq_ = chayta;
                bestmodel[0].push_back(x);
            }
        }

        if(ep % 50 == 0) {
            cout << "epouche n: " << ep << endl;
            cout << "best score is : " << bestscore << endl;
        }
        
        if(ep == 10000 || bestscore == 9) {
            break;
        }
        
        models.clear();
        for(auto e = bestmodel.rbegin(); e != bestmodel.rend(); ++e) {
            if(!e->empty()) {
                new_model_m = e->back();
                //new_model_m.game=[0,0,0,0,0,0,0,0,0] converted to C++
                new_model_m.game = {0,0,0,0,0,0,0,0,0};
                
            }
        }
        
        for(int x = 0; x < 100; x++) {
            Player new_model = new_model_m;
            new_model.update();
            models.push_back(new_model);
        }
        bestmodel = vector<vector<Player>>(9);
    }

    bestmodel[0].push_back(new_model_m);
    vector<int> lis = {0,0,0,0,0,0,0,0,0};
    
    cout << "best score is : " << bestscore << endl;
    bestmodel[0].back().save();

    while(true) {
        print_board(lis);
        int a;
        cout << "entre number:";
        cin >> a;
        lis[a-1] = 1;
        print_board(lis);
        cout << "ai think ..." << endl;
        time_t start = time(nullptr);
        while(time(nullptr) - start < 5) {}
        
        vector<double> pred = bestmodel[0].back().predict(lis);
        vector<double> valid_values;
        for(double value : pred) {
            if(0.5 < value && value < 1) {
                valid_values.push_back(value);
            }
        }
        if(valid_values.size() == 1) {
            for(int i = 0; i < pred.size(); i++) {
                if(pred[i] == valid_values[0]) {
                    lis[i] = 1;
                    break;
                }
            }
        }
    }

    /*
    for(int x = 0; x < 9; x++) {
        print_board(lis);
        cout << "ai think ..." << endl;
        time_t start = time(nullptr);
        while(time(nullptr) - start < 5) {}
        vector<double> pred = bestmodel[0].back().predict(lis);
        vector<double> valid_values;
        for(double value : pred) {
            if(0.5 < value && value < 1) {
                valid_values.push_back(value);
            }
        }
        if(valid_values.size() == 1) {
            for(int i = 0; i < pred.size(); i++) {
                if(pred[i] == valid_values[0]) {
                    lis[i] = 1;
                    break;
                }
            }
        }
    }
    */

    return 0;
}