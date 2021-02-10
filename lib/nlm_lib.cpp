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

#ifdef USE_OCTAVE
void octave_read(char* file, std::vector<double>& v , const std::string& variable) {
    oc::interpreter inter {};
    try {
        inter.initialize_history (false);
        inter.initialize_load_path (false);
        inter.initialize();
        if (! inter.initialized ()){
            std::cerr << "Octave interpreter initialization failed!"
            << std::endl;
            exit (1);
        }
        octave_value_list in{};
        in(0) = file;
        in(1) ="-mat";
        in(2) = variable;
        octave_value_list out = Fload(inter, in, 1);
        octave_value val = out(0).subsref(".", {ovl(variable)});
        Array<double> arr=  val.array_value().as_matrix();
        v = std::vector<double>(arr.numel());
        v = std::vector<double>(arr.data(), arr.data() + arr.numel());
    }  catch (const octave::exit_exception& ex){
        std::cerr << "Octave interpreter exited with status = "
                  << ex.exit_status () << std::endl;
    }
    catch (const octave::execution_exception& e)    {
        std::cerr << "error encountered in Octave evaluator!" << std::endl;
        std::cerr << e.info();
    }
}
#endif

#ifdef USE_OPENCV
static inline double divide (double x) { return x/255; }

void opencv_read(char* file, std::vector<double>& v){
    cv::Mat image;
    image = cv::imread( file, 0 );
    if ( !image.data )
    {
        std::cerr << "No image data" << std::endl;
        return;
    }
    v = std::vector<double>(image.data, image.data + image.total());
    // divides vector entries by 255 and stores them back (limits values between [0,255]
    std::transform(v.begin(), v.end(), v.begin(), divide);
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

namespace po = boost::program_options;
template<class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v)
{
    copy(v.begin(), v.end(), std::ostream_iterator<T>(os, " "));
    return os;
}

void parse_cli_args(int nargs, char** args, std::vector<std::string>& paths, std::vector<std::string>& format, bool noise){
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "produce help message")
            ("path,p", po::value< std::vector<std::string> >(),
             "input path to file")
            ("format,f", po::value< std::vector<std::string> >(),
                    "input file format. accepted values are mat, jpg, jpeg and png")
            ("add-noise",po::bool_switch(&noise) ,"add noise to image?");
    po::variables_map vm;
    po::store(po::parse_command_line(nargs, args,desc), vm);
    po::notify(vm);
    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return;
    }
    paths = vm.count("path") ? vm["path"].as< std::vector<std::string> >() : std::vector<std::string>();
    format = vm.count("format") ? vm["format"].as< std::vector<std::string> >() : std::vector<std::string>();
}
