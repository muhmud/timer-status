
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>

#include <boost/filesystem.hpp>

#include <ctime>
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;
using namespace boost::filesystem;

#define STATUS_FILE ".timer-status"

#define SECONDS_MINUTE 60
#define SECONDS_HOUR (SECONDS_MINUTE * SECONDS_MINUTE)

int main() {
  const struct passwd *const pw = getpwuid(getuid());
  const string statusFilePath = string(pw->pw_dir) + "/" + STATUS_FILE;

  if (exists(path(statusFilePath))) {
    std::ifstream statusFile(statusFilePath);

    std::string line;
    if (std::getline(statusFile, line)) {
      auto get = [&](int previousComma) {
        return previousComma != string::npos
                   ? line.find(
                         ",",
                         !previousComma ? previousComma : previousComma + 1, 1)
                   : string::npos;
      };
      const size_t firstComma = get(0);
      const size_t secondComma = get(firstComma);
      const size_t thirdComma = get(secondComma);
      const size_t fourthComma = get(thirdComma);
      const size_t fifthComma = get(fourthComma);
      const size_t sixthComma = get(fifthComma);
      if (firstComma != string::npos && secondComma != string::npos &&
          thirdComma != string::npos && fourthComma != string::npos &&
          fifthComma != string::npos && sixthComma) {
        auto select = [&](int start, int end) {
          return line.substr(start, end - start);
        };
        const string status = select(0, firstComma);
        const string project = select(firstComma + 1, secondComma);
        const string directory = select(secondComma + 1, thirdComma);
        const long anchor = std::stol(select(thirdComma + 1, fourthComma));
        const long workDone = std::stol(select(fourthComma + 1, fifthComma));
        const long start = std::stol(select(fifthComma + 1, sixthComma));
        const std::string end = line.substr(sixthComma + 1);

        if (status != "STOPPED") {
          long timeDifference =
              (workDone + (end != "null" ? std::stol(end) : time(0) * 1000) -
               start) /
              1000;
          const int hours =
              (timeDifference - (timeDifference % SECONDS_HOUR)) / SECONDS_HOUR;

          timeDifference -= hours * SECONDS_HOUR;
          const int minutes =
              (timeDifference - (timeDifference % SECONDS_MINUTE)) /
              SECONDS_MINUTE;
          const int seconds = timeDifference - minutes * SECONDS_MINUTE;

          auto pad = [&](int value) { return value < 10 ? "0" : ""; };
          std::cout << project << " - " << pad(hours) << hours << ":"
                    << pad(minutes) << minutes << ":" << pad(seconds) << seconds
                    << "\n";
        }
      }
    }
  }
}
