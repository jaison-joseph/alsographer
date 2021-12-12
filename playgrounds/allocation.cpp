
// proof that the allocation is correct wrt dimensions 
#include <iostream>
#include <vector>
#include <fstream>
#include <string>

using namespace std;

int num_nodes_ = 4;
int node_count_ = num_nodes_ - 1;
vector<int> sizes;

void allocate(int world_rank_) {
    if (world_rank_ != 0) {
        int i, j;
        i = 0;
        j = world_rank_ - 1;
        cout << "\n " << world_rank_ << "\n";
        for (int k = 0 ; k < node_count_ ; ++k) {
            cout << " [";
            vector<vector<int>> foo;
            for (int it = 0; it < sizes[i] ; ++it) {
                foo.push_back(vector<int> (sizes[j], 0));
            }
            cout << '[' << foo.size() << ']'
                 << '[' << foo[0].size() << ']'
                 << ",";
            // arr.push_back(foo);
            ++i;
            --j;
            if (j < 0) {
                j = node_count_ - 1;
            }
            cout << "] \n";
        }
    }
}

int main() {
    sizes.push_back(4);
    sizes.push_back(3);
    sizes.push_back(3);
    for (int i = 0 ; i < num_nodes_ ; ++i) {
        allocate(i);
    }
}