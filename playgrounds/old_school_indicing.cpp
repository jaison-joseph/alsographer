
vector<int> sizes; // width of each slice in the master array representing the graph
int num_nodes;
int num_workers_;

void oldSchoolIndicing(unsigned long long int n1, unsigned long long int n2) {
    unsigned long long int temp;
    int rank, master_row, master_column, row, column;
    master_row = 0;   // the row wrt the entire graph into which the element goes into 
    temp = n1;     // will contain the row of the subsquare that it goes to 
    for (auto& sz : sizes) {
        if (sz > temp) {
            break;
        }
        temp -= sz;
        ++master_row;
    }
    row = temp;
    master_column = 0;   // the column wrt the entire graph into which the element goes into 
    temp = n2;
    for (auto& sz : sizes) {
        if (sz > temp) {
            break;
        }
        temp -= sz;
        ++master_column;
    }
    column = temp;
    rank = (master_row + master_column) % num_workers_;
    cout << "n1: " << n1 
            << " n2: " << n2
            << " goes to process #: " << rank
            << " arr[" << master_row << ']'
            << '[' << row << ']' 
            << '[' << column << ']'
            << " && " 
            << " arr[" << master_column << ']'
            << '[' << column << ']'
            << '[' << row << ']' 
            << '\n';
}