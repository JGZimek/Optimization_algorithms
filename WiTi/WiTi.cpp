#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <limits>
#include <stdexcept>

namespace witi {

struct Task {
    int index;
    int processing_time;
    int weight;
    int deadline;
};

std::vector<Task> read_tasks(const std::string& filename, const std::string& dataname) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open the file.");
    }

    std::string line;
    while (getline(file, line) && line != dataname) {
        // Skip to the specified data section
    }

    int task_count;
    file >> task_count;
    if (file.fail()) {
        throw std::runtime_error("Error reading task count.");
    }

    std::vector<Task> tasks(task_count);
    for (int i = 0; i < task_count; ++i) {
        file >> tasks[i].processing_time >> tasks[i].weight >> tasks[i].deadline;
        tasks[i].index = i + 1;
        if (file.fail()) {
            throw std::runtime_error("Error reading task data.");
        }
    }
    return tasks;
}

void print_order(const std::vector<int>& order) {
    std::cout << "Order of tasks: ";
    for (int index : order) {
        std::cout << index << " ";
    }
    std::cout << std::endl;
}

void print_results(int dataset_number, int cost, const std::vector<int>& order) {
    std::cout << "Dataset " << dataset_number << ":\n";
    std::cout << "Minimum cost: " << cost << std::endl;
    print_order(order);
    std::cout << "-----------------------------------\n";
}

std::pair<int, std::vector<int>> calculate_minimum_cost(const std::vector<Task>& tasks) {
    const int num_tasks = tasks.size();
    const int num_subsets = 1 << num_tasks;

    std::vector<int> cost_table(num_subsets, std::numeric_limits<int>::max());
    std::vector<std::vector<int>> order_table(num_subsets);

    cost_table[0] = 0;

    for (int subset = 1; subset < num_subsets; ++subset) {
        int current_time = 0;
        for (int j = 0; j < num_tasks; ++j) {
            if (subset & (1 << j)) {
                current_time += tasks[j].processing_time;
            }
        }

        for (int j = 0; j < num_tasks; ++j) {
            if (subset & (1 << j)) {
                int previous_subset = subset & ~(1 << j);
                int lateness = std::max(0, current_time - tasks[j].deadline);
                int temp_cost = cost_table[previous_subset] + tasks[j].weight * lateness;
                
                if (temp_cost < cost_table[subset]) {
                    cost_table[subset] = temp_cost;
                    order_table[subset] = order_table[previous_subset];
                    order_table[subset].push_back(tasks[j].index);
                }
            }
        }
    }
    return {cost_table[num_subsets - 1], order_table[num_subsets - 1]};
}

void process_datasets(int start, int end, const std::string& filename) {
    for (int i = start; i <= end; ++i) {
        try {
            std::string data_id = "data." + std::to_string(i) + ":";
            auto tasks = read_tasks(filename, data_id);
            auto [cost, order] = calculate_minimum_cost(tasks);
            print_results(i, cost, order);
        } catch (const std::exception& ex) {
            std::cerr << "Exception occurred while processing dataset " << i << ": " << ex.what() << std::endl;
        }
    }
}

} // namespace witi

int main() {
    const std::string filename = "data.txt";
    witi::process_datasets(10, 20, filename);
    return 0;
}
