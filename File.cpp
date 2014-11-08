#include "File.h"
#include <boost/regex.hpp>

/**
 * File class Version 2.0.
 * Put data in double** File::data_. Use File::printData() to print them.
 * Recognize n dimension.
 * Uses the Regular Expression "([^\\s]+)". => It is possible to read something like that : 'a b      c d     e'.
 * Automatically detects the dimension (n) by reading the 3rd line.
 * If a line is empty, it is ignored.
 * If a line has not the same number of columns, it is ignored.
 * @brief File::File
 */
File::File()
{
}

File::File(std::string filename):filename_(filename)
{
}

/**
 * Getter of data_
 * @brief File::getData
 * @return
 */
double** File::getData() {
    return data_;
}

/**
 * Getter of totalLines_
 * @brief File::getTotalLines_
 */
int File::getTotalLines() {
    return totalLines_;
}





/**
 * Parse text files using regex. Skip if line dimension is 0 (only blank).
 * Stores data in_data.
 * @brief File::parseText
 */
void File::parseText(){
    // Find max number of valid lines in the file
    std::ifstream inFile0(filename_);
    totalLines_ = std::count(std::istreambuf_iterator<char>(inFile0),
                             std::istreambuf_iterator<char>(), '\n');
    std::regex regex("([^\\s]+)");
    int i=0;
    cout << totalLines_ <<"\n";
    data_ = new double *[totalLines_*2];
    std::ifstream inFile(filename_);
    std::string line;

    // check each line and register them in _data.
    while (std::getline(inFile, line)){
        i++;
        if (i==lineDimension_) { //Compute dimension.
            dimension_ = getCount(line, regex);
            logger->info(logger->get() << "dimension_ is now " << dimension_ << ". It was found in '" << line << "' (line " << lineDimension_
                         << ").");
        }
        if (i>=lineDimension_) {
            checkIfExceptionInFile(line, regex, &i);
        }
    }
    totalLines_ = i-lineDimension_+1;
    logger->info(logger->get()<< "Found "<<totalLines_<<" usefull lines in '"<<filename_<<"'.\n");
    //    printData();
}

/**
 * Test if the file filename_ exists.
 * @brief File::exists_test
 */
void File::exists_test() {
    bool isFile = false;
    ifstream f(filename_.c_str());
    if (f.good()) {
        f.close();
        isFile=true;
    } else {
        f.close();
        isFile=false;
    }
    if (!isFile) {
        logger->info(logger->get() << "ERROR : File '" << filename_ << "' not found. Please Copy '" <<
            filename_ << "' to '../build-Simulation-Clang-Debug/' and" <<
            "'./Shaders/*' to '../build-Simulation-Clang-Debug/' and './Textures/*' to '../build-Simulation-Clang-Debug/'\n");
        exit(-1);
    } else {
        logger->info(logger->get()<<"File '" << filename_ << "' found.");
    }
}

/**
 * Count number of regex in a line.
 * @brief File::getCount
 * @param text
 * @param regex
 * @return
 */
int File::getCount(std::string text, std::regex regex) {
    int res = 0;
    auto words_begin =
            std::sregex_iterator(text.begin(), text.end(), regex);
    auto words_end = std::sregex_iterator();
    res = std::distance(words_begin, words_end);

    return res;
}

/**
 * Split a text to an iterator with a regex. Use registerData() to go throw the iterator.
 * @brief File::split
 * @param text
 * @param regex
 * @return
 */
std::sregex_iterator File::split(std::string text, std::regex regex) {
    return std::sregex_iterator(text.begin(), text.end(), regex);
}

/**
 * Check if there is an exception in the file. (1 line has more column).
 * @brief File::checkIfExceptionInFile
 * @param text
 * @param regex
 * @param i
 * @return
 */
std::sregex_iterator File::checkIfExceptionInFile(std::string text, std::regex regex, int* i) {
    auto words_begin = std::sregex_iterator(text.begin(), text.end(), regex);
    auto words_end = std::sregex_iterator();
    int dimLine = std::distance(words_begin, words_end);
    bool skip = false;

    if (dimLine == 0 || dimLine != dimension_) {
        logger->info(logger->get() << "checkIfExceptionInFile: W: Skipping blank or invalid line at l "<< *i << ". dimLine was " << dimLine <<
                     " but shouldn't be 0 or (dimension_=" << dimension_ << ") in '"<<filename_<<"'.");
        skip = true;
        (*i)--;
    }
    registerData(words_begin, words_end, *i - lineDimension_, skip);

    return words_end;
}

/**
 * Register data to the array, only launched if the line is valid.
 * @brief File::registerData
 * @param words_begin
 * @param words_end
 * @param i
 */
void File::registerData(std::sregex_iterator words_begin, std::sregex_iterator words_end, int i, bool skip){
    int j=0;
    if (!skip) {
        data_[i] = new double[dimension_];
        for (std::sregex_iterator it = words_begin; it != words_end; ++it) {
            std::smatch match = *it;
            std::string match_str = match.str();
            data_[i][j] = atof(match_str.c_str());
            //            cout << "(" << i << ", " << j << ") = " << data_[i][j] << "\t";
            j++;
        }
    }
    //        cout << "\n";
}

/**
 * Print all data stored in data_.
 * @brief File::printData
 */
void File::printData() {
    logger->info(logger->get() << "Printing (totalLines_, dimension_) = (" << totalLines_ << ", " << dimension_ << ") : ");
    int i=0, j=0;
    for (i=0; i<=totalLines_ - 1; i++) {
        for (j=0; j<=dimension_ - 1; j++) {
            cout << "(" << i << ", " << j << ") = " << data_[i][j] << "\t";
        }
        cout << "\n";
    }
}
