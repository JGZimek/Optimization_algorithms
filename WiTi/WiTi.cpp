#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

struct Task_WiTi
{
    int index;
    int P, W, D;
};

std::vector<Task_WiTi> read_tasks(const std::string &filename, const std::string &dataname)
{
    std::ifstream file(filename);

    if (!file.is_open())
    {
        throw std::runtime_error("Unable to open the file.");
    }

    std::string line;
    while (getline(file, line) && line != dataname)
        ;

    int vector_length;
    std::vector<Task_WiTi> tasks_vector;
    if (!(file >> vector_length))
    {
        throw std::runtime_error("Error reading vector length.");
    }

    for (int i = 1; i < vector_length + 1; ++i)
    {
        Task_WiTi task;
        if (file >> task.P >> task.W >> task.D)
        {
            task.index = i;
            tasks_vector.push_back(task);
        }
        else
        {
            throw std::runtime_error("Error reading task data.");
        }
    }
    return tasks_vector;
}

void print_tasks_order(std::vector<Task_WiTi> &tasks_vector)
{
    std::cout << "Task Index: ";
    for (size_t i = 0; i < tasks_vector.size(); i++)
    {
        std::cout << tasks_vector[i].index << " ";
    }
    std::cout << std::endl;
}

void print_tasks_order(std::vector<int> &order)
{
    std::cout << "Task Index: ";
    for (size_t i = 0; i < order.size(); i++)
    {
        std::cout << order[i] << " ";
    }
    std::cout << std::endl;
}

std::pair<int, std::vector<int>> calculate_sum_witi(std::vector<Task_WiTi> &tasks_vector)
{
    int n = tasks_vector.size();
    std::vector<int> cost_table(1 << n, 99999);
    std::vector<std::vector<int>> order_table(1 << n);
    cost_table[0] = 0;

    for (size_t i = 1; i < (1 << n); i++)
    {
        int C = 0;
        for (size_t j = 0, b = 1; j < n; j++, b *= 2)
            if (i & b)
                C += tasks_vector[j].P;

        for (size_t j = 0, b = 1; j < n; j++, b *= 2)
        {
            if (i & b)
            {
                int temp_cost = cost_table[i - b] + tasks_vector[j].W * std::max(0, C - tasks_vector[j].D);
                if (temp_cost < cost_table[i])
                {
                    cost_table[i] = temp_cost;
                    order_table[i] = order_table[i - b];
                    order_table[i].push_back(tasks_vector[j].index);
                }
            }
        }
    }
    return {cost_table[(1 << n) - 1], order_table[(1 << n) - 1]};
}

int main()
{
    try
    {
        for(int i = 10; i <= 20; i++) 
        {
            std::string data_id = "data." + std::to_string(i) + ":";
            std::vector<Task_WiTi> tasks_vector = read_tasks("data.txt", data_id);
            //print_tasks_order(tasks_vector);
            auto [cost, order] = calculate_sum_witi(tasks_vector);
            //print_tasks_order(order);
            std::cout << "Data " + std::to_string(i) << " cost: " << cost << std::endl;
        }
    }
    catch (const std::exception &ex)
    {
        std::cerr << "Exception occurred: " << ex.what() << std::endl;
    }

    return 0;
}

