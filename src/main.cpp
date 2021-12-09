#include "graph.hpp"

#include <fstream>
#include <iostream>

static void run(int argc, char *argv[])
{
    if (argc != 2)
    {
        throw std::runtime_error("expected exactly 1 input file provided as argument");
    }

    const char *filepath = argv[1];
    std::ifstream input(filepath);

    if (!input)
    {
        throw std::runtime_error(std::string("no such file or directory: ") + filepath);
    }

    bhk::graph graph = bhk::graph::parse_from(input);
    std::uint32_t shortest_path = graph.shortest_path();
    std::cout << "shortest Hamiltonian path cost: " << shortest_path << '\n';
}

int main(int argc, char *argv[])
{
    try
    {
        run(argc, argv);
    }
    catch (const std::exception &exception)
    {
        std::cerr << "fatal error: " << exception.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
