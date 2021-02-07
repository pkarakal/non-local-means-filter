#ifndef NON_LOCAL_MEANS_FILTER_NLM_LIB_H
#define NON_LOCAL_MEANS_FILTER_NLM_LIB_H

#include <iostream>
#include <vector>
#include <string>

#ifdef USE_MATLAB
#include <mat.h>
#include <matrix.h>
void matread(char* file, std::vector<double>& v, const std::string& variable);
#endif

#include "rapidcsv.h"
void read_csv(char* file, std::vector<double>& v);
#endif //NON_LOCAL_MEANS_FILTER_NLM_LIB_H
