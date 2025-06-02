#include <iostream>
#include <vector>
using namespace std;

int main() {
    int N = 3;  // You can change this to 2000 if needed
    std::vector<float> A(N * N, 1.0f);
    std::vector<float> B(N * N, 1.0f);
    std::vector<float> C(N * N, 0.0f);

    // Matrix multiplication: C = A × B
    for (int row = 0; row < N; ++row) {
        for (int col = 0; col < N; ++col) {
            float sum = 0.0f;
            for (int k = 0; k < N; ++k) {
                sum += A[row * N + k] * B[k * N + col];
            }
            C[row * N + col] = sum;
        }
    }

    // Print the top-left 5×5 corner of result matrix C
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            cout << C[i * N + j] << " ";
        }
        cout << endl;
    }

    cin >> N;
    return 0;
}