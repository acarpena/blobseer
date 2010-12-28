#include <boost/random.hpp>
#include <boost/date_time.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

int main(int argc, char* argv[]) {
    int nb_characters = 1000000;
    if (argc == 2)
        sscanf(argv[1], "%d", &nb_characters);
    boost::mt19937 prnd;
    prnd.seed(boost::posix_time::microsec_clock::local_time().time_of_day().total_nanoseconds());
    std::ofstream f("mt19937-seed.bin",std::ios::binary);
    int buffer;
    for (int i = 0; i < nb_characters; i += 4) {
        buffer = prnd();
        f.write((char*) &buffer,4);
    }
    f.close();
    return 0;
}
