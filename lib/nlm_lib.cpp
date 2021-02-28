#include "nlm_lib.h"
#ifdef USE_OPENMP
#include <omp.h>
#endif

#ifdef USE_MATLAB
void matread(const char* file, std::vector<double>& v, const std::string& variable){
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
namespace oc = octave;
void octave_read(const char* file, std::vector<double>& v , const std::string& variable) {
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

void opencv_read(const char* file, std::vector<double>& v){
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

void read_csv(const char* file, std::vector<double>& v, int& n, int& d){
    rc::Document document = rc::Document();
    document.Load(file);
    n = document.GetRowCount();
    d = document.GetColumnCount();
    char *end;
    v = std::vector<double>(n * d);
    // create a n*d vector from row vectors.
    for (int i = 0; i < n; ++i) {
        auto vec = document.GetRow<double>(i);
        std::copy(vec.begin(), vec.end(), v.begin() + (i * d));
        vec.clear();
    }
}

void write_csv(const char* file, std::vector<std::vector<double>>& v){
    rc::Document document = rc::Document("", rc::LabelParams(-1, -1));
    uint i = 0;
    for(std::vector<double>& vec: v) {
        document.SetRow(i, vec);
        ++i;
    }
    document.Save(std::string(file)+"_denoised.csv");
}

namespace po = boost::program_options;
template<class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v)
{
    copy(v.begin(), v.end(), std::ostream_iterator<T>(os, " "));
    return os;
}

void parse_cli_args(int nargs, char** args, std::vector<std::string>& paths,
					std::vector<std::string>& format, std::vector<std::string>& variable,
					std::vector<int>& patchSize, bool noise){
    po::options_description desc("Allowed options");
  desc.add_options()("help", "produce help message")(
	  "path,p", po::value<std::vector<std::string>>(), "input path to file")(
	  "format,f", po::value<std::vector<std::string>>(),
	  "input file format. accepted values are mat, jpg, jpeg and png")(
	  "add-noise", po::bool_switch(&noise),
	  "add noise to image?")("variable", po::value<std::vector<std::string>>(),
							 "specify the variable to search in the mat-file")(
	  "patchSize", po::value<std::vector<int>>(),
	  "specify the patch size");
  po::variables_map vm;
    po::store(po::parse_command_line(nargs, args,desc), vm);
    po::notify(vm);
    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return;
    }
    paths = vm.count("path") ? vm["path"].as< std::vector<std::string> >() : std::vector<std::string>();
    format = vm.count("format") ? vm["format"].as< std::vector<std::string> >() : std::vector<std::string>();
    variable = vm.count("variable") ? vm["variable"].as< std::vector<std::string> >() : std::vector<std::string>();
    patchSize = vm.count("patchSize") ? vm["patchSize"].as< std::vector<int> >() : std::vector<int>();
}

void gaussian_kernel(std::vector<std::vector<double>>& kernel, int kernelSize, double sigma){
    double sum = 0;
    double mu = ((double) kernelSize)/2.0;
    double alpha = 0.5* M_PI * std::pow(sigma,2);
    int i=0;
    double maxElement;
#ifdef USE_OPENMP
    omp_set_num_threads(omp_get_max_threads());
#pragma omp parallel shared(sum, maxElement)
#pragma omp for
#endif
    for (i = 0; i < kernelSize; i++){
        for (int j = 0; j < kernelSize; j++){
            kernel.at(i).at(j) = alpha * std::exp(-((j - mu)*(j - mu) + (i - mu)*(i - mu)) / (2*std::pow(sigma,2)));
#ifdef USE_OPENMP
#pragma omp atomic
#endif
            sum += kernel.at(i).at(j);
        }
    }
    for(std::vector<double>& vec: kernel){
        for(double& item: vec)
            item /= sum;
    }

    for(std::vector<double>& vec: kernel){
        double local_max = *std::max_element(vec.begin(), vec.end());
        if (maxElement< local_max)
#pragma omp critical
            maxElement = local_max;
    }

    for(std::vector<double>& vec: kernel){
        for(double& item: vec)
            item /= maxElement;
    }
}

static inline double gaussian_noise(double sigma, double x){
  return (1 / (sigma*sqrt(2*M_PI)))*exp((-std::pow(x,2)) / (2*std::pow(sigma,2)));
}

double fRand(double fMin, double fMax){
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_real_distribution<double> dist(0.0, 1.0);
  double f = dist(mt);
  return fMin + f * (fMax - fMin);
}

void add_gaussian_noise(const std::vector<double>& image, int size,
						double scalar, std::vector<double>& noise){
  srand(time(nullptr));
  int i;
  double value, effect;
#ifdef USE_OPENMP
  omp_set_num_threads(omp_get_max_threads());
#pragma omp parallel private(i, value, effect)
#pragma omp for
#endif
  for(i=0; i < size; i++){
	value    = fRand(0.0, 1.0);
	effect   = gaussian_noise(2.0, value) - 0.1;
	noise.at(i) = (scalar*effect + 1) * image.at(i);
  }
}
