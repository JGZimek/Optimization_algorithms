#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <limits>
#include <stdexcept>
#include "task_loader.h"

namespace witi {

using Task = Task_WiTi;

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
                current_time += tasks[j].P;
            }
        }

        for (int j = 0; j < num_tasks; ++j) {
            if (subset & (1 << j)) {
                int previous_subset = subset & ~(1 << j);
                int lateness = std::max(0, current_time - tasks[j].D);
                int temp_cost = cost_table[previous_subset] + tasks[j].W * lateness;
                
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

void process_dataset(const std::string& filename, int dataset_number) {
    std::ostringstream oss;
    oss << "data." << dataset_number << ":";
    std::string data_id = oss.str();

    auto tasks = TaskLoader::read_tasks_witi(filename, data_id);
    auto [cost, order] = calculate_minimum_cost(tasks);
    print_results(dataset_number, cost, order);
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

} // namespace witi

int main() {
    try {
        const std::string filename = "../data/data_witi.txt"; // relative path to the data file
        witi::process_datasets(filename, 10, 20);
    } catch (const std::exception& ex) {
        std::cerr << "Exception occurred: " << ex.what() << std::endl;
    }
    return 0;
}
