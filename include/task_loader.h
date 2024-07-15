#ifndef TASK_LOADER_H
#define TASK_LOADER_H

#include <vector>
#include <string>
#include <stdexcept>
#include <fstream>
#include <sstream>

struct Task_NEH {
    int index;
    std::vector<int> processing_times;
    int total_processing_time = 0;
};

struct Task_SA {
    int index;
    std::vector<int> processing_times;
    int total_processing_time = 0;
};

struct Task_WiTi {
    int index;
    int P, W, D;
};

class TaskLoader {
public:
    static std::vector<Task_NEH> read_tasks_neh(const std::string& filename, const std::string& dataname);
    static std::vector<Task_SA> read_tasks_sa(const std::string& filename, const std::string& dataname);
    static std::vector<Task_WiTi> read_tasks_witi(const std::string& filename, const std::string& dataname);
};

#endif // TASK_LOADER_H
