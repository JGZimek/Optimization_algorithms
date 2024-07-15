#include "task_loader.h"

void find_data_section(std::ifstream& file, const std::string& dataname) {
    std::string line;
    bool data_section_found = false;

    while (getline(file, line)) {
        if (line.find(dataname) != std::string::npos) {
            data_section_found = true;
            break;
        }
    }

    if (!data_section_found) {
        throw std::runtime_error("Data section not found: " + dataname);
    }
}

std::vector<Task_NEH> TaskLoader::read_tasks_neh(const std::string& filename, const std::string& dataname) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open the file.");
    }

    find_data_section(file, dataname);

    int rows, cols;
    file >> rows >> cols;
    if (file.fail()) {
        throw std::runtime_error("Error reading rows and columns.");
    }

    std::vector<Task_NEH> tasks(rows);
    for (int i = 0; i < rows; ++i) {
        tasks[i].index = i + 1;
        tasks[i].processing_times.resize(cols);
        for (int j = 0; j < cols; ++j) {
            file >> tasks[i].processing_times[j];
            tasks[i].total_processing_time += tasks[i].processing_times[j];
        }
        if (file.fail()) {
            throw std::runtime_error("Error reading task data.");
        }
    }
    return tasks;
}

std::vector<Task_SA> TaskLoader::read_tasks_sa(const std::string& filename, const std::string& dataname) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open the file.");
    }

    find_data_section(file, dataname);

    int rows, cols;
    file >> rows >> cols;
    if (file.fail()) {
        throw std::runtime_error("Error reading rows and columns.");
    }

    std::vector<Task_SA> tasks(rows);
    for (int i = 0; i < rows; ++i) {
        tasks[i].index = i + 1;
        tasks[i].processing_times.resize(cols);
        for (int j = 0; j < cols; ++j) {
            file >> tasks[i].processing_times[j];
            tasks[i].total_processing_time += tasks[i].processing_times[j];
        }
        if (file.fail()) {
            throw std::runtime_error("Error reading task data.");
        }
    }
    return tasks;
}

std::vector<Task_WiTi> TaskLoader::read_tasks_witi(const std::string& filename, const std::string& dataname) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open the file.");
    }

    find_data_section(file, dataname);

    int vector_length;
    std::vector<Task_WiTi> tasks_vector;
    if (!(file >> vector_length)) {
        throw std::runtime_error("Error reading vector length.");
    }

    for (int i = 1; i < vector_length + 1; ++i) {
        Task_WiTi task;
        if (file >> task.P >> task.W >> task.D) {
            task.index = i;
            tasks_vector.push_back(task);
        } else {
            throw std::runtime_error("Error reading task data.");
        }
    }
    return tasks_vector;
}
