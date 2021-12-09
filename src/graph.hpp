#pragma once

#include "adjacency_matrix.hpp"

#include <cassert>
#include <climits>
#include <cstddef>
#include <iosfwd>
#include <string>
#include <vector>
#include <stdexcept>

namespace bhk
{

class graph
{
public:
    explicit graph(std::vector<std::string> nodes, bhk::adjacency_matrix distances)
        : _nodes(std::move(nodes))
        , _distance(std::move(distances))
    {
        if (_nodes.size() > sizeof(std::size_t) * CHAR_BIT)
        {
            throw std::runtime_error("can only track as many nodes as bits in std::size_t");
        }
    }

    [[nodiscard]]
    std::uint32_t shortest_path() const;

    static graph parse_from(std::istream &input);

private:
    std::vector<std::string> _nodes;
    bhk::adjacency_matrix _distance;
};

} // namespace bhk
