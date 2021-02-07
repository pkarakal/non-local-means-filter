#include "nlm_lib.h"

#ifdef USE_MATLAB
void matread(char* file, std::vector<double>& v, const std::string& variable){
    // open MAT-file
    MATFile *pmat = matOpen(file, "r");
    if (pmat == nullptr) return;

    // extract the specified variable
    mxArray *arr = matGetVariable(pmat, variable.c_str());
    if (arr != nullptr && mxIsDouble(arr) && !mxIsEmpty(arr)) {
        // copy data
        mwSize num = mxGetNumberOfElements(arr);
        double *pr = mxGetPr(arr);
        if (pr != nullptr) {
            v.reserve(num); //is faster than resize :-)
            v.assign(pr, pr+num);
        }
    }
    mxDestroyArray(arr);
    matClose(pmat);
}
#endif

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

