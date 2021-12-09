#include "graph.hpp"

#include <cmath>
#include <cstdint>
#include <regex>
#include <unordered_set>

std::uint32_t bhk::graph::shortest_path() const
{
    std::size_t node_count = _nodes.size();

    // The number of subsets for a set is given by 2^N.
    std::size_t node_subsets = 1U << node_count;

    std::vector<std::uint32_t> lowest_costs(
        node_subsets * node_count,
        std::numeric_limits<std::uint32_t>::max()
    );

    auto lowest_cost = [&](std::size_t set, std::size_t last) -> std::uint32_t &
    {
        assert(set < node_subsets && "the set of nodes must be bounded by the number of node subsets");
        assert(last < node_count && "the ID of the last node must be less than the number of nodes");
        std::size_t index = set * node_count + last;
        return lowest_costs[index];
    };

    // Iterate through all possible subsets of nodes. Binary representation means that simply
    // incrementing the value produces the next subset. We interpret each set as a set of nodes
    // that we need to pass through, starting from the dummy node. Eventually, once the subset is
    // equal to the full set, we will have all the necessary information required to construct paths
    // that pass through all nodes in the graph, and subsequently determine which has the lowest
    // cost.
    for (std::size_t set = 1; set < node_subsets; ++set)
    {
        // Iterate through all nodes present within the set. We take each node to be the last node
        // visited.
        for (std::size_t node = 0; node < node_count; ++node)
        {
            std::size_t node_bit = 1U << node;

            // Skip nodes not present in the set.
            if ((set & node_bit) == 0) continue;

            // Determine the set of nodes visited up to this point.
            std::size_t previous_set = set ^ node_bit;

            if (previous_set == 0)
            {
                // This is the first node in the path. We came from the dummy node so the cost to
                // get here is 0.
                lowest_cost(set, node) = 0;
            }
            else
            {
                // This was not the first node in the path. We need to iterate through all previous
                // nodes (nodes that we came from to get here) in order to find the path that had
                // the lowest cost.
                for (std::size_t previous_node = 0; previous_node < node_count; ++previous_node)
                {
                    std::size_t previous_node_bit = 1U << previous_node;

                    // Skip nodes not present in the previous set.
                    if ((previous_set & previous_node_bit) == 0) continue;

                    // Here, we mean "preceding" as in "already travelled".
                    std::uint32_t preceding_cost = lowest_cost(previous_set, previous_node);
                    std::uint32_t cost = _distance.get(previous_node, node);
                    std::uint32_t total_cost = preceding_cost + cost;

                    std::uint32_t &best = lowest_cost(set, node);
                    best = std::min(best, total_cost);
                }
            }
        }
    }

    // The shortest Hamiltonian path is the one that passes through all nodes and ends at the dummy
    // node.
    std::size_t all_nodes = node_subsets - 1;
    return lowest_cost(all_nodes, 0);
}

bhk::graph bhk::graph::parse_from(std::istream &input)
{
    // We need to determine number of nodes first since the adjacency matrix needs to be
    // pre-allocated. We can get this from the number of edges since
    // E = N * (N - 1) / 2 <=> N^2 - N - 2E = 0 which can be easily solved for N using the quadratic
    // formula.

    // Each line defines exactly one edge.
    std::size_t edge_count = std::count(
        std::istreambuf_iterator<char>(input),
        std::istreambuf_iterator<char>(),
        '\n'
    );

    if (edge_count == 0)
    {
        throw std::runtime_error(
            "invalid input: could not determine graph edges as no line breaks were detected"
        );
    }

    auto node_count = static_cast<std::size_t>(
        std::ceil((1.0 + std::sqrt(1.0 + 4.0 * 2.0 * static_cast<double>(edge_count))) / 2.0)
    );

    // Rewind stream to beginning since counting the number of lines moved the cursor.
    input.seekg(0, std::ios::beg);

    // Allocate an extra dummy node to be used during calculation of the shortest Hamiltonian path.
    // We will designate its ID 0. This trades some wasted memory for ease-of-use and code clarity.
    // The alternative would be to encode a dummy node implicitly in the cost calculation
    // algorithms, which would clutter the algorithm with more conditionals.
    adjacency_matrix distances(node_count + 1);

    // We need to not only ensure uniqueness, but also maintain insertion order so as to allow node
    // ID to be represented in the index. We construct `nodes` with a size of 1 to pre-fill it with
    // the dummy node.

    std::vector<std::string> nodes(1);
    std::unordered_set<std::string> unique_nodes;

    // We could technically do away with this map since we could just perform a binary search and
    // get the index, but the map is more intuitive.
    std::unordered_map<std::string, std::size_t> id_for_node;

    for (std::string line; std::getline(input, line); )
    {
        static std::regex pattern(R"((\w+) to (\w+) = (\d+))");

        std::smatch capture_groups;
        if (!std::regex_match(line, capture_groups, pattern))
        {
            throw std::runtime_error("could not parse line: " + line);
        }

        std::string origin = capture_groups[1];
        std::string endpoint = capture_groups[2];

        // We only want to insert a node into `nodes` if it is unique. Inserting into the set first
        // allows us to detect this condition.

        if (auto [it, inserted] = unique_nodes.insert(origin); inserted)
        {
            nodes.push_back(origin);
            id_for_node[origin] = nodes.size() - 1;
        }

        if (auto [it, inserted] = unique_nodes.insert(endpoint); inserted)
        {
            nodes.push_back(endpoint);
            id_for_node[endpoint] = nodes.size() - 1;
        }

        std::string distance_str = capture_groups[3];
        std::uint32_t distance = std::stoul(distance_str);

        std::size_t a = id_for_node[origin];
        std::size_t b = id_for_node[endpoint];
        distances.update(a, b, distance);
    }

    return graph(std::move(nodes), std::move(distances));
}
