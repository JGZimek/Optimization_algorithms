#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <string>
#include <iomanip>
#include <filesystem>
#include <stdexcept>

namespace sa {

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

void create_output_directory(const std::string& directory) {
    std::filesystem::create_directory(directory);
}

std::vector<Task> simulated_annealing(std::vector<Task> tasks, const std::string& output_directory, int dataset_number) {
    srand(static_cast<unsigned int>(time(NULL)));

    int best_cmax = calculate_cmax(tasks);
    std::vector<Task> best_sequence = tasks;
    float T = 10.0;
    std::ostringstream oss;
    oss << output_directory << "/results_sa_" << std::setw(3) << std::setfill('0') << dataset_number << ".txt";
    std::ofstream MyFile(oss.str());

    for (int i = 0; i < 50000; ++i) {
        int pos1 = rand() % tasks.size();
        int pos2 = rand() % tasks.size();
        float random = static_cast<float>(rand()) / RAND_MAX;

        if (i % 25 == 0) {
            T *= 0.999;
        }

        std::swap(tasks[pos1], tasks[pos2]);

        int current_cmax = calculate_cmax(tasks);
        float P = exp(-(current_cmax - best_cmax) / T);

        if (current_cmax < best_cmax || random < P) {
            best_cmax = current_cmax;
            best_sequence = tasks;
        } else {
            std::swap(tasks[pos1], tasks[pos2]);
        }

        if (i % 1 == 0) {
            MyFile << best_cmax << std::endl;
        }
    }

    MyFile.close();
    return best_sequence;
}

void print_combined_tasks(const std::vector<Task>& initial_tasks, const std::vector<Task>& sorted_tasks) {
    std::cout << std::left << std::setw(8) << "Task"
              << std::setw(30) << "Initial Processing Times"
              << std::setw(10) << "Total"
              << std::setw(30) << "Sorted Processing Times"
              << std::setw(10) << "Total" << std::endl;
    std::cout << "-------------------------------------------------------------------------------------------------------------" << std::endl;
    for (size_t i = 0; i < initial_tasks.size(); ++i) {
        const auto& init_task = initial_tasks[i];
        const auto& sorted_task = sorted_tasks[i];

        std::cout << std::left << std::setw(8) << init_task.index;
        for (const auto& time : init_task.processing_times) {
            std::cout << std::setw(2) << time << " ";
        }
        std::cout << std::right << std::setw(10) << init_task.total_processing_time << "     ";

        for (const auto& time : sorted_task.processing_times) {
            std::cout << std::setw(2) << time << " ";
        }
        std::cout << std::right << std::setw(10) << sorted_task.total_processing_time << std::endl;
    }
}

void print_result_header(const std::string& header) {
    std::cout << "===================================================" << std::endl;
    std::cout << header << std::endl;
    std::cout << "===================================================" << std::endl;
}

void process_dataset(const std::string& filename, const std::string& output_directory, int dataset_number) {
    std::ostringstream oss;
    oss << "data." << std::setw(3) << std::setfill('0') << dataset_number << ":";
    std::string data_id = oss.str();

    auto initial_tasks = read_tasks(filename, data_id);
    int initial_cmax = calculate_cmax(initial_tasks);
    auto sorted_tasks = simulated_annealing(initial_tasks, output_directory, dataset_number);
    int sorted_cmax = calculate_cmax(sorted_tasks);

    print_result_header("Results for dataset " + std::to_string(dataset_number));
    print_combined_tasks(initial_tasks, sorted_tasks);

    std::cout << "\nInitial C_max: " << initial_cmax << "\n";
    std::cout << "Sorted C_max: " << sorted_cmax << "\n";
}

void process_datasets(const std::string& filename, const std::string& output_directory, int start, int end) {
    create_output_directory(output_directory);
    for (int i = start; i <= end; ++i) {
        try {
            process_dataset(filename, output_directory, i);
        } catch (const std::exception& ex) {
            std::cerr << "Exception occurred while processing dataset " << i << ": " << ex.what() << std::endl;
        }
    }

    // Uruchomienie skryptu Pythona do generowania wykresów
    std::string command = "python3 plot_sa_results.py " + output_directory;
    if (std::system(command.c_str()) != 0) {
        std::cerr << "Failed to execute Python script" << std::endl;
    }
}

} // namespace sa

int main() {
    try {
        const std::string filename = "data.txt";
        const std::string output_directory = "results";
        sa::process_datasets(filename, output_directory, 0, 10);
    } catch (const std::exception& ex) {
        std::cerr << "Exception occurred: " << ex.what() << std::endl;
    }

    return 0;
}
