#include "nlm_lib.h"

namespace rc = rapidcsv;

void read_csv(char* file, std::vector<double>& v){
    rc::Document document = rc::Document();
    document.Load(file);
    int n = document.GetRowCount();
    int d = document.GetColumnCount();
    char *end;
    v = std::vector<double>(n * d);
    // create a n*d vector from row vectors.
    for (int i = 0; i < n; ++i) {
        auto vec = document.GetRow<double>(i);
        std::copy(vec.begin(), vec.end(), v.begin() + (i * d));
        vec.clear();
    }
}

