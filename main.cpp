#include "B+Tree/multi_BPT.h"
#include "STLite/myString.h"

using std::cin;
using std::cout;
using my::multiBPT;
using sjtu::vector;

int main() {
    int n;
    cin >> n;
    multiBPT<my::string<>, int> map("myFile");
    my::string<> keyWord;
    for (int i = 0; i < n; ++i) {
        cin >> keyWord;
        if (keyWord == "insert") {
            my::string<> index;
            int value;
            cin >> index >> value;
            map.insert(index, value);
        }
        else if (keyWord == "find") {
            my::string<> index;
            cin >> index;
            vector<int> output;
            map.find(index, output);
            if (output.empty()) cout << "null\n";
            else {
                for (int j: output) cout << j << ' ';
                cout << '\n';
            }
        } else if (keyWord == "delete") {
            my::string<> index;
            int value;
            cin >> index >> value;
            map.erase(index, value);
        } else throw std::exception();
    }
    return 0;
}


