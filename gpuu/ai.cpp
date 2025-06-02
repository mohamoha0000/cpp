#include <iostream>
#include <vector>
#include <random>
#include <cmath>
using namespace std;
double sigmoid(double x) {
    return 1.0 / (1.0 + std::exp(-x));
}
class Player {
private:
    vector<int> hidden_layer;
    int input_size;
    vector<vector<vector<double>>> w;

    // Random double generator between 0 and 1
    double randomWeight() {
        static random_device rd;
        static mt19937 gen(rd());
        static uniform_real_distribution<> dis(-1.0, 1.0);
        return dis(gen);
    }

public:
    Player(const vector<int>& hidden_layer, int input_size) {
        this->hidden_layer = hidden_layer;
        this->input_size = input_size;

        // Layer 0: from input to first hidden layer
        vector<vector<double>> layer0;
        for (int i = 0; i < hidden_layer[0]; ++i) {
            vector<double> neuron;
            for (int j = 0; j < input_size; ++j)
                neuron.push_back(randomWeight());
            neuron.push_back(1.0); // bias
            layer0.push_back(neuron);
        }
        w.push_back(layer0);

        // Hidden layers
        for (size_t i = 1; i < hidden_layer.size(); ++i) {
            vector<vector<double>> layer;
            for (int j = 0; j < hidden_layer[i]; ++j) {
                vector<double> neuron;
                for (int k = 0; k < hidden_layer[i - 1]; ++k)
                    neuron.push_back(randomWeight());
                neuron.push_back(1.0); // bias
                layer.push_back(neuron);
            }
            w.push_back(layer);
        }
    }

    // Predict function (forward pass)
    vector<double> predict(const vector<double>& input_value) {
        vector<vector<double>> last_layer;
        vector<double> first_layer;

        for (const auto& neuron : w[0]) {
            double sum = 0;
            for (size_t i = 0; i < input_value.size(); ++i)
                sum += input_value[i] * neuron[i];
            sum += neuron.back(); // bias
            first_layer.push_back(sigmoid(sum));
        }

        last_layer.push_back(first_layer);

        // Hidden layers
        for (size_t layer_idx = 1; layer_idx < w.size(); ++layer_idx) {
            vector<double> current_layer;
            for (const auto& neuron : w[layer_idx]) {
                double sum = 0;
                for (size_t j = 0; j < last_layer[layer_idx - 1].size(); ++j)
                    sum += last_layer[layer_idx - 1][j] * neuron[j];
                sum += neuron.back(); // bias
                current_layer.push_back(sigmoid(sum));
            }
            last_layer.push_back(current_layer);
        }

        return last_layer.back();
    }

    const vector<vector<vector<double>>>& get_model() const {
        return w;
    }
};

// Example usage
int main() {
    int n=0;
    vector<int> hidden = {500,200 ,10};
    Player moha1(hidden, 784);//28

    vector<double> input = {5, 56, 6};
    vector<double> output = moha1.predict(input);

    cout << "Output:\n";
    for (double o : output)
        cout << o << " ";
    cout << endl;
    cin>>n;
    return 0;
}
