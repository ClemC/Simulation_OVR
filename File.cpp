#include "File.h"

// Enable Google Test when value is 1
// Without value 0, the project doesn't build
//#define ENABLE_GTEST 1

#if ENABLE_GTEST
std::unique_ptr<Logger> logger(new Logger("log.log", "log.err", false, Severity::error | Severity::info));
#endif

/**
 * File class Version 2.0.
 * Put data in double** File::data_. Use File::printData() to print them.
 * Recognize n dimension.
 * Uses the Regular Expression "([^\\s]+)". => Should be possible to read something like : 'a b      c d     e'.
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
 * Getter of dataCah_
 * @brief File::getDataCah
 * @return
 */
double** File::getDataCah() {
    return dataCah_;
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
 * Getter of totalLinesCah_
 * @brief File::getTotalLinesCah
 */
int File::getTotalLinesCah() {
    return totalLinesCah_;
}

/**
 * Getter of totalLines_
 * @brief File::getTotalLines
 */
int File::getTotalLines() {
    return totalLines_;
}

/**
 * Getter of age_
 * @brief File::age_
 */
double File::getAge() {
    return age_;
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
//void File::parseText(){
//    // static
//    std::regex regex("([^\\s]+)");
//    std::regex regex2("[-+]?([0-9]*\\.[0-9]+|[0-9]+)");

//    // Find max number of valid lines in the file
//    std::ifstream inFile0(filename_);
//    totalLines_ = std::count(std::istreambuf_iterator<char>(inFile0),
//                             std::istreambuf_iterator<char>(), '\n');
//    int i=0;
//    logger->info(logger->get() << "parseText: found " << totalLines_ << " lines.");
//    data_ = new double *[totalLines_*2];
//    std::ifstream inFile(filename_);
//    std::string line;

//    // check each line and register them in _data.
//    while (std::getline(inFile, line)){
//        i++;
//        if (i==1) {// Find the age of the universe
//            auto itUniverse=split(line, regex2);
//            auto words_end = std::sregex_iterator();
//            for (std::sregex_iterator it = itUniverse; it != words_end; ++it) {
//                std::smatch match = *it;
//                std::string match_str = match.str();
//                age_ = atof(match_str.c_str());
//                logger->info(logger->get() << "The universe is now " << age_ << " years old !");
//                // Use 'cout.precision(15);' and 'cout<<age_;' to print all digits.
//            }
//        }
//        if (i==lineDimension_) { // Compute dimension.
//            dimension_ = getCount(line, regex);
//            logger->info(logger->get() << "dimension_ is now " << dimension_ << ". It was found in '" << line << "' (line " << lineDimension_
//                         << ").");
//        }
//        if (i>=lineDimension_) { // Read data
//            checkIfExceptionInFile(line, regex, &i);
//        }
//    }
//    totalLines_ = i-lineDimension_+1;
//    logger->info(logger->get()<< "Found "<<totalLines_<<" usefull lines in '"<<filename_<<"'.\n");
//    //    printData();
//}

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
                     " but should be (dimension_=" << dimension_ << ") in '"<<filename_<<"'.");
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

//////////////////////////////////////////////////////////////////

/**
 * Print all data stored in dataCah_.
 * @brief File::printDataCah
 */
void File::printDataCah() {
    logger->info(logger->get() << "Printing (totalLines_, dimension_) = (" << totalLines_ << ", " << dimension_ << ") : ");
    int i=0, j=0;
    for (i=0; i<=totalLinesCah_ - 1; i++) {
        for (j=0; j<=dimension_ - 1; j++) {
            cout << "(" << i << ", " << j << ") = " << data_[i][j] << "\t";
        }
        cout << "\n";
    }
}

/**
 * Dissimetry between two clusters using maximum distance
 * @brief File::dissimMax
 * @param c1 first cluster
 * @param c2 second cluster
 */
double File::dissimMax(std::vector<int> c1, std::vector<int> c2){
    double res=0.0;
    double x1,y1,z1,x2,y2,z2;
    double dsquare;
    for(std::size_t i=0; i<c1.size(); i++){
        for(std::size_t j=0 ; j<c2.size(); j++){
            x1 = data_[c1[i]][xpos];
            y1 = data_[c1[i]][ypos];
            z1 = data_[c1[i]][zpos];
            x2 = data_[c2[j]][xpos];
            y2 = data_[c2[j]][ypos];
            z2 = data_[c2[j]][zpos];
            dsquare = (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) + (z2-z1)*(z2-z1);
            if(dsquare>res){
                res = dsquare;
            }
        }
    }
    return res;
}

/**
 * Dissimetry between two clusters using gravity center
 * @brief File::dissimG
 * @param c1 first cluster
 * @param c2 second cluster
 */
double File::dissimG(std::vector<int> c1, std::vector<int> c2){
    double x1,y1,z1,x2,y2,z2;
    double xr1,yr1,zr1,xr2,yr2,zr2;
    xr1=0.0;yr1=0.0;zr1=0.0;xr2=0.0;yr2=0.0;zr2=0.0;
    //logger->info(logger->get() << c1.size() << " " << c2.size());
    for(std::size_t i=0; i<c1.size(); i++){
        x1 = data_[c1[i]][xpos];
        y1 = data_[c1[i]][ypos];
        z1 = data_[c1[i]][zpos];
        xr1 += x1;
        yr1 += y1;
        zr1 += z1;
    }
    xr1 /= (double)c1.size();
    yr1 /= (double)c1.size();
    zr1 /= (double)c1.size();
    for(std::size_t i=0; i<c2.size(); i++){
        x2 = data_[c2[i]][xpos];
        y2 = data_[c2[i]][ypos];
        z2 = data_[c2[i]][zpos];
        xr2 += x2;
        yr2 += y2;
        zr2 += z2;
    }
    xr2 /= (double)c2.size();
    yr2 /= (double)c2.size();
    zr2 /= (double)c2.size();
    return (xr2-xr1)*(xr2-xr1)+(yr2-yr1)*(yr2-yr1)+(zr2-zr1)*(zr2-zr1);
}

/**
 * Dissimetry between two clusters using distance between random points
 * Doesn't work
 * @brief File::dissimAlea
 * @param c1 first cluster
 * @param c2 second cluster
 */
double File::dissimAlea(std::vector<int> c1, std::vector<int> c2){
    srand ( time(NULL) );
    int a=0, b;
    int r1, r2;
    double res;
    b=c1.size();
    r1 = (int)(rand()%(b-a) + a);
    b=c2.size();
    r2 = (int)(rand()%(b-a) + a);
    res = (data_[c2[r2]][xpos]-data_[c1[r1]][xpos])*(data_[c2[r2]][xpos]-data_[c1[r1]][xpos]);
    res += (data_[c2[r2]][ypos]-data_[c1[r1]][ypos])*(data_[c2[r2]][ypos]-data_[c1[r1]][ypos]);
    res += (data_[c2[r2]][zpos]-data_[c1[r1]][zpos])*(data_[c2[r2]][zpos]-data_[c1[r1]][zpos]);
    return res;
}

/**
 * Minimum of a symetric square matrix
 * @brief File::minMatDissim
 * @param matrix m
 * @param size of square matrix
 */
std::vector<int> File::minMatDissim (double **m, int size){
    std::vector<int> res;
    double tmp=1000000.0;
    int i0=0, j0=0;
    for(int i=0; i<size; i++){
        for(int j=i+1; j<size; j++){
            if(m[i][j]<tmp){
                tmp = m[i][j];
                i0 = i;
                j0 = j;
            }
        }
    }
    res.push_back(i0);
    res.push_back(j0);
    return res;
}

/**
 * Compute a Hierarchical Clustering (CAH in french), clustering is saved into clusters_
 * @brief File::cah
 * @param cluster, the number of clusters
 */
void File::cah(int cluster){

    logger->info(logger->get() << "Hierarchical Clustering in " << cluster << " clusters" );
    // initalize the number of clusters
    int nbClusters = this->getTotalLines();
    for(int i=0; i<nbClusters; i++){
        std::vector<int> clust;
        clust.push_back(i);
        clusters_.push_back(clust);
    }

    while((int)clusters_.size()>cluster){

        double **matDissim;
        int csize = (int)clusters_.size();
        matDissim = new double*[csize];

        for(int i=0; i<csize; i++){
            matDissim[i] = new double[csize];
            for(int j=i+1; j<csize; j++){
                matDissim[i][j] = dissimG(clusters_[i], clusters_[j]);
            }
        }
        std::vector<int> ijmin = minMatDissim(matDissim, csize);
        logger->info(logger->get() << "Clustering of (" << ijmin[0] << "," << ijmin[1] << ") at level " << csize);
        for(int i=0; i<(int)clusters_[ijmin[1]].size(); i++) {
            clusters_[ijmin[0]].push_back(clusters_[ijmin[1]][i]);
        }
        clusters_.erase (clusters_.begin()+ijmin[1]);

        for(int i=0; i<csize; i++){
            delete matDissim[i];
        }
        delete matDissim;
    }
}

/**
 * Register clusters to the array dataCah_
 * @brief File::registerCluster
 * @param clusters
 */
void File::registerClusters(){
    int i,j;
    double resijx, resijy, resijz, resijmass, resijage;

    //register the number of clusters
    totalLinesCah_ = clusters_.size();
    dataCah_ = new double *[totalLinesCah_];

    //get the mean of each cluster to create dataCah_
    for(i=0; i<totalLinesCah_; i++){
        resijx=0.0;resijy=0.0;resijz=0.0;resijmass=0.0;resijage=0.0;
        dataCah_[i] = new double[dimension_];
        int c = clusters_[i].size();

        for(j=0; j<c; j++){
            resijx += data_[clusters_[i][j]][xpos];
            resijy += data_[clusters_[i][j]][ypos];
            resijz += data_[clusters_[i][j]][zpos];
            resijmass += data_[clusters_[i][j]][mass];
            resijage += data_[clusters_[i][j]][age];
        }

        resijx /= c;
        resijy /= c;
        resijz /= c;
        resijmass /= c;
        resijage /= c;
        dataCah_[i][xpos] = resijx;
        dataCah_[i][ypos] = resijy;
        dataCah_[i][zpos] = resijz;
        dataCah_[i][mass] = resijmass;
        dataCah_[i][age] = resijage;
    }
}
