
#include <iostream>
#include <string>
#include <fstream>
#include <chrono>

#include <boost/filesystem.hpp>

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <ctime>

using namespace std;
using namespace boost::filesystem;

#define STATUS_FILE ".timer-status"

#define SECONDS_MINUTE 60
#define SECONDS_HOUR (SECONDS_MINUTE * SECONDS_MINUTE)

int main() {
  const struct passwd * const pw = getpwuid(getuid());
  const string statusFilePath = string(pw->pw_dir) + "/" + STATUS_FILE;

  if (exists(path(statusFilePath))) {
    std::ifstream statusFile(statusFilePath);

    std::string line;
    if (std::getline(statusFile, line)) {
      const size_t commaPosition = line.find(",", 0, 1);
      if (commaPosition != string::npos) {
        const string project = line.substr(0, commaPosition);
        const long start = std::stol(line.substr(commaPosition + 1));
       
        long timeDifference = time(0) - start / 1000;
        int hours = (timeDifference - (timeDifference % SECONDS_HOUR)) / SECONDS_HOUR;

        timeDifference -= hours * SECONDS_HOUR;
        int minutes = (timeDifference - (timeDifference % SECONDS_MINUTE)) / SECONDS_MINUTE;
        int seconds = timeDifference - minutes * SECONDS_MINUTE;

        auto pad = [&](int value) { return value < 10 ? "0" : ""; };
        std::cout << project << " - " << pad(hours) << hours << ":" << pad(minutes) << minutes << ":" << pad(seconds) << seconds << "\n";
      }
    }
  }
}
