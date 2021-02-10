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

#ifdef USE_OCTAVE
#include <oct.h>
#include <builtin-defun-decls.h>
#include <interpreter.h>
void octave_read(char* file, std::vector<double>& v, const std::string& variable);
#endif

#ifdef USE_OPENCV
#include <algorithm>
#include <opencv4/opencv2/opencv.hpp>
void opencv_read(char* file, std::vector<double>& v);
#endif

#include "rapidcsv.h"
void read_csv(char* file, std::vector<double>& v);


#include <boost/program_options.hpp>
#include <ostream>
#include <algorithm>
#include <iterator>
void parse_cli_args(int nargs, char** args, std::vector<std::string>& paths,
          std::vector<std::string>& format, bool noise);

#endif //NON_LOCAL_MEANS_FILTER_NLM_LIB_H
