#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <string>
#include <limits>
#include <iomanip>

namespace neh {

struct Task {
    int index;
    std::vector<int> processing_times;
    int total_processing_time = 0;
};

std::vector<Task> read_tasks(const std::string& filename, const std::string& dataname) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open the file.");
    }

    std::string line;
    bool data_section_found = false;

    // Szukaj sekcji danych
    while (getline(file, line)) {
        if (line.find(dataname) != std::string::npos) {
            data_section_found = true;
            break;
        }
    }

    if (!data_section_found) {
        throw std::runtime_error("Data section not found: " + dataname);
    }

    int rows, cols;
    file >> rows >> cols;
    if (file.fail()) {
        throw std::runtime_error("Error reading rows and columns.");
    }

    std::vector<Task> tasks(rows);
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

void print_tasks(const std::vector<Task>& tasks) {
    std::cout << std::left << std::setw(8) << "Task"
              << std::setw(20) << "Processing Times"
              << std::setw(10) << "Total" << std::endl;
    std::cout << "---------------------------------------------------" << std::endl;
    for (const auto& task : tasks) {
        std::cout << std::left << std::setw(8) << task.index << " ";
        for (const auto& time : task.processing_times) {
            std::cout << std::setw(2) << time << " ";
        }
        std::cout << std::right << std::setw(10) << task.total_processing_time << std::endl;
    }
}

int calculate_cmax(const std::vector<Task>& tasks) {
    int num_tasks = tasks.size();
    int num_machines = tasks[0].processing_times.size();
    
    std::vector<std::vector<int>> completion_times(num_tasks, std::vector<int>(num_machines, 0));

    for (int i = 0; i < num_tasks; ++i) {
        for (int j = 0; j < num_machines; ++j) {
            if (i == 0 && j == 0) {
                completion_times[i][j] = tasks[i].processing_times[j];
            } else if (i == 0) {
                completion_times[i][j] = completion_times[i][j - 1] + tasks[i].processing_times[j];
            } else if (j == 0) {
                completion_times[i][j] = completion_times[i - 1][j] + tasks[i].processing_times[j];
            } else {
                completion_times[i][j] = std::max(completion_times[i - 1][j], completion_times[i][j - 1]) + tasks[i].processing_times[j];
            }
        }
    }
    return completion_times[num_tasks - 1][num_machines - 1];
}

std::vector<Task> neh_algorithm(std::vector<Task> tasks) {
    std::sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b) {
        return a.total_processing_time > b.total_processing_time;
    });

    std::vector<Task> sorted_tasks;
    for (const auto& task : tasks) {
        sorted_tasks.push_back(task);
        int best_cmax = std::numeric_limits<int>::max();
        std::vector<Task> best_sequence;

        for (size_t j = 0; j <= sorted_tasks.size(); ++j) {
            std::vector<Task> temp_sequence = sorted_tasks;
            temp_sequence.insert(temp_sequence.begin() + j, temp_sequence.back());
            temp_sequence.pop_back();

            int current_cmax = calculate_cmax(temp_sequence);

            if (current_cmax < best_cmax) {
                best_cmax = current_cmax;
                best_sequence = std::move(temp_sequence);
            }
        }
        sorted_tasks = std::move(best_sequence);
    }
    return sorted_tasks;
}

void print_result_header(const std::string& header) {
    std::cout << "===================================================" << std::endl;
    std::cout << header << std::endl;
    std::cout << "===================================================" << std::endl;
}

void process_dataset(const std::string& filename, int dataset_number) {
    std::ostringstream oss;
    oss << "data." << std::setw(3) << std::setfill('0') << dataset_number << ":";
    std::string data_id = oss.str();
    
    auto tasks = read_tasks(filename, data_id);

    print_result_header("Initial tasks for dataset " + std::to_string(dataset_number));
    print_tasks(tasks);

    int initial_cmax = calculate_cmax(tasks);
    std::cout << "Initial C_max: " << initial_cmax << "\n\n";

    auto sorted_tasks = neh_algorithm(tasks);

    print_result_header("Sorted tasks for dataset " + std::to_string(dataset_number));
    print_tasks(sorted_tasks);

    int sorted_cmax = calculate_cmax(sorted_tasks);
    std::cout << "Sorted C_max: " << sorted_cmax << "\n";
}

void process_datasets(const std::string& filename, int start, int end) {
    for (int i = start; i <= end; ++i) {
        try {
            process_dataset(filename, i);
        } catch (const std::exception& ex) {
            std::cerr << "Exception occurred while processing dataset " << i << ": " << ex.what() << std::endl;
        }
    }
}

} // namespace neh

int main() {
    try {
        const std::string filename = "data.txt";
        neh::process_datasets(filename, 0, 10);
    } catch (const std::exception& ex) {
        std::cerr << "Exception occurred: " << ex.what() << std::endl;
    }

    return 0;
}
