#pragma once

#include <array>
#include <memory>
#include <vector>
#include <glm/glm.hpp>

using rgb32_t = glm::uvec4; // Assuming a 32-bit RGBA color
using Vec3i32 = glm::ivec3;
using Vec3u32 = glm::uvec3;

constexpr uint32_t LEAF_NODE = 0x80000000;
constexpr uint32_t BRANCH_NODE = 0x40000000;
constexpr uint32_t NODE_TYPE_MASK = 0xC0000000;
constexpr uint32_t INDEX_MASK = 0x3FFFFFFF;

struct SVONode {
    virtual ~SVONode() = default;
    virtual std::unique_ptr<SVONode> clone() const = 0;
};

struct SVOBranch : public SVONode {
    std::array<std::unique_ptr<SVONode>, 8> children;

    SVOBranch() = default;
    SVOBranch(const SVOBranch& other);
    SVOBranch& operator=(const SVOBranch& other);
    ~SVOBranch() final = default;

    std::unique_ptr<SVONode> clone() const override;
};

struct SVOLeaf : public SVONode {
    std::array<rgb32_t, 8> data{};

    SVOLeaf() = default;
    SVOLeaf(const SVOLeaf& other) = default;
    SVOLeaf& operator=(const SVOLeaf& other) = default;
    ~SVOLeaf() final = default;

    std::unique_ptr<SVONode> clone() const override;
};

class SVO {
private:
    using i32 = int32_t;
    using u32 = uint32_t;
    using u64 = uint64_t;

    std::unique_ptr<SVOBranch> root = std::make_unique<SVOBranch>();
    size_t depth = 16;

public:
    SVO() = default;

    void insert(Vec3i32 pos, rgb32_t color);

    Vec3i32 minIncl() const;
    Vec3i32 maxIncl() const;
    Vec3i32 minExcl() const;
    Vec3i32 maxExcl() const;

    rgb32_t& operator[](Vec3i32 pos);
    rgb32_t& at(Vec3i32 pos);
    const rgb32_t& at(Vec3i32 pos) const;

    // New method to flatten the SVO for SSBO
    void flatten(std::vector<uint32_t>& buffer) const;

private:
    rgb32_t& findOrCreate(u64 octreeNodeIndex);
    rgb32_t* find(u64 octreeNodeIndex) const;
    u64 indexOf(Vec3i32 pos) const;
    void ensureSpace(Vec3i32 pos);
    void insert(u64 octreeNodeIndex, rgb32_t color);
    void grow(u32 lim);
    void growOnce();
    uint32_t boundsTest(Vec3i32 v) const;
    void flattenNode(const SVONode* node, std::vector<uint32_t>& buffer, uint32_t& index) const;
};
