#ifndef NON_LOCAL_MEANS_FILTER_NLM_LIB_H
#define NON_LOCAL_MEANS_FILTER_NLM_LIB_H

#include <iostream>
#include <vector>
#include <string>
#include "definitions.h"

#ifdef USE_MATLAB
#include <mat.h>
#include <matrix.h>
void matread(const char* file, std::vector<double>& v, const std::string& variable);
#endif

#ifdef USE_OCTAVE
#include <oct.h>
#include <builtin-defun-decls.h>
#include <interpreter.h>
void octave_read(const char* file, std::vector<double>& v, const std::string& variable);
#endif

#ifdef USE_OPENCV
#include <algorithm>
#include <opencv4/opencv2/opencv.hpp>
void opencv_read(const char* file, std::vector<double>& v);
#endif

#include "rapidcsv.h"
void read_csv(const char* file, std::vector<double>& v, int& n, int& d);
void write_csv(const char* file, std::vector<std::vector<double>>& v);


#include <boost/program_options.hpp>
#include <ostream>
#include <algorithm>
#include <iterator>
void parse_cli_args(int nargs, char** args, std::vector<std::string>& paths,
					std::vector<std::string>& format,
					std::vector<std::string>& variable,
					std::vector<int>& patchSize, bool noise);


#endif //NON_LOCAL_MEANS_FILTER_NLM_LIB_H
