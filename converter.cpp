#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <iostream>

void convert_img_to_csv(char *file);

namespace py= pybind11;

void convert_to_csv(char* file, char* variable){
    py::module_ convert = py::module_::import("convert_to_csv");
    if(strcmp(variable, "--image") == 0) {
        py::object result = convert.attr("convert_img_to_csv")(file);
    } else {
        py::object result = convert.attr("convert_mat_to_csv")(file, variable);
    }
}

int main(int argc, char **argv){
    if(argc > 2){
        py::scoped_interpreter guard{};
        for(int i= 1; i < argc; i+=2){
            convert_to_csv(argv[i], argv[i+1]);
        }
        return 0;
    } else {
        std::cerr <<"No file or no variable specified. Exiting ..." <<std::endl;
        std::cerr<<"Usage: " << argv[0] << " [ /path/to/file1 variable1 /path/to/file2 variable2 /path/to/fileN"
            << "variableN ]" <<std::endl;
        std::cerr<<"If you want to convert an image to csv just replace variable with --image" <<std::endl;
        return 1;
    }
}

