#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace bhk
{

class adjacency_matrix
{
public:
    explicit adjacency_matrix(std::size_t node_count)
        : _node_count(node_count)
        , _matrix(_node_count * _node_count)
    {
    }

    void update(std::size_t a, std::size_t b, std::uint32_t distance)
    {
        _matrix[index_at(a, b)] = distance;
        _matrix[index_at(b, a)] = distance;
    }

    [[nodiscard]]
    std::uint32_t get(std::size_t a, std::size_t b) const
    {
        return _matrix[index_at(a, b)];
    }

private:
    [[nodiscard]]
    std::size_t index_at(std::size_t row, std::size_t column) const
    {
        assert((row < _node_count) && "row must be less than matrix width");
        assert((column < _node_count) && "column must be less than matrix width");
        return row * _node_count + column;
    }

private:
    std::size_t _node_count;
    std::vector<std::uint32_t> _matrix;
};

} // namespace bhk
