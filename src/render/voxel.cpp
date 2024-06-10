
#include "voxel.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define DEBUG_ASSERT_UNREACHABLE() std::cerr << "Unreachable code reached in " << __FILE__ << ":" << __LINE__ << std::endl; std::abort()
#define ALWAYS_ASSERT(cond) if (!(cond)) { std::cerr << "Assertion failed: " << #cond << " in " << __FILE__ << ":" << __LINE__ << std::endl; std::abort(); }

// Custom copy constructor for SVOBranch
SVOBranch::SVOBranch(const SVOBranch& other) {
    for (size_t i = 0; i < 8; ++i) {
        if (other.children[i]) {
            children[i] = other.children[i]->clone();
        }
    }
}

// Custom assignment operator for SVOBranch
SVOBranch& SVOBranch::operator=(const SVOBranch& other) {
    if (this == &other) return *this;
    for (size_t i = 0; i < 8; ++i) {
        if (other.children[i]) {
            children[i] = other.children[i]->clone();
        } else {
            children[i].reset();
        }
    }
    return *this;
}

// Clone method for SVOBranch
std::unique_ptr<SVONode> SVOBranch::clone() const {
    return std::make_unique<SVOBranch>(*this);
}

// Clone method for SVOLeaf
std::unique_ptr<SVONode> SVOLeaf::clone() const {
    return std::make_unique<SVOLeaf>(*this);
}


void SVO::insert(Vec3i32 pos, rgb32_t color) {
    std::cout << "Inserting voxel color " << color.r << ", " << color.g << ", " << color.b << " at position " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
    ensureSpace(pos);
    auto octreeNodeIndex = indexOf(pos);
    insert(octreeNodeIndex, color);
}

Vec3i32 SVO::minIncl() const {
    return Vec3i32(-(1 << depth));
}

Vec3i32 SVO::maxIncl() const {
    return Vec3i32((1 << depth) - 1);
}

Vec3i32 SVO::minExcl() const {
    return Vec3i32(-(1 << depth) - 1);
}

Vec3i32 SVO::maxExcl() const {
    return Vec3i32(1 << depth);
}

rgb32_t& SVO::operator[](Vec3i32 pos) {
    ensureSpace(pos);
    auto octreeNodeIndex = indexOf(pos);
    return findOrCreate(octreeNodeIndex);
}

rgb32_t& SVO::at(Vec3i32 pos) {
    u32 lim = boundsTest(pos);
    ALWAYS_ASSERT(lim == 0);
    auto octreeNodeIndex = indexOf(pos);
    auto* result = find(octreeNodeIndex);
    ALWAYS_ASSERT(result != nullptr);
    return *result;
}

const rgb32_t& SVO::at(Vec3i32 pos) const {
    u32 lim = boundsTest(pos);
    ALWAYS_ASSERT(lim == 0);
    auto octreeNodeIndex = indexOf(pos);
    auto* result = find(octreeNodeIndex);
    ALWAYS_ASSERT(result != nullptr);
    return *result;
}

rgb32_t& SVO::findOrCreate(u64 octreeNodeIndex) {
    SVONode* node = root.get();
    for (size_t s = depth * 3; s != size_t(-3); s -= 3) {
        u32 octDigit = (octreeNodeIndex >> s) & 0b111;
        if (s != 0) {
            auto* branch = dynamic_cast<SVOBranch*>(node);
            if (branch->children[octDigit] != nullptr) {
                node = branch->children[octDigit].get();
            } else {
                auto* child = s == 3 ? static_cast<SVONode*>(new SVOLeaf)
                                     : static_cast<SVONode*>(new SVOBranch);
                node = child;
                branch->children[octDigit] = std::unique_ptr<SVONode>(child);
            }
        } else {
            auto* leaf = dynamic_cast<SVOLeaf*>(node);
            return leaf->data[octDigit];
        }
    }
    DEBUG_ASSERT_UNREACHABLE();
}

rgb32_t* SVO::find(u64 octreeNodeIndex) const {
    SVONode* node = root.get();
    for (size_t s = depth * 3; s != size_t(-3); s -= 3) {
        u32 octDigit = (octreeNodeIndex >> s) & 0b111;
        if (s != 0) {
            auto* branch = dynamic_cast<SVOBranch*>(node);
            if (branch->children[octDigit] == nullptr) {
                return nullptr;
            } else {
                node = branch->children[octDigit].get();
            }
        } else {
            auto* leaf = dynamic_cast<SVOLeaf*>(node);
            return &leaf->data[octDigit];
        }
    }
    DEBUG_ASSERT_UNREACHABLE();
}

glm::u64 SVO::indexOf(Vec3i32 pos) const {
    Vec3u32 uPos = glm::uvec3(pos - minIncl());
    return (static_cast<u64>(uPos.x) << 40) | (static_cast<u64>(uPos.y) << 20) | static_cast<u64>(uPos.z);
}

void SVO::ensureSpace(Vec3i32 pos) {
    if (u32 lim = boundsTest(pos); lim != 0) {
        grow(lim);
    }
}

void SVO::insert(u64 octreeNodeIndex, rgb32_t color) {
    findOrCreate(octreeNodeIndex) = color;
}

void SVO::grow(u32 lim) {
    while ((1u << depth) <= lim) {
        growOnce();
        depth++;
    }
}

void SVO::growOnce() {
    auto newRoot = std::make_unique<SVOBranch>();
    for (size_t i = 0; i < 8; ++i) {
        if (root->children[i] != nullptr) {
            newRoot->children[i] = std::move(root->children[i]);
        }
    }
    root = std::move(newRoot);
}

uint32_t SVO::boundsTest(Vec3i32 v) const {
    constexpr auto absForBoundsTest = [](int32_t x) -> uint32_t {
        return static_cast<uint32_t>(x < 0 ? -x - 1 : x);
    };
    static_assert (absForBoundsTest(-5) == 4);
    u32 max = absForBoundsTest(v[0]) | absForBoundsTest(v[1]) | absForBoundsTest(v[2]);
    return max >= (1u << depth) ? max : 0;
}


void SVO::flatten(std::vector<uint32_t>& buffer) const {
    uint32_t index = 0;
    flattenNode(root.get(), buffer, index);
}

void SVO::flattenNode(const SVONode* node, std::vector<uint32_t>& buffer, uint32_t& index) const {
    if (auto branch = dynamic_cast<const SVOBranch*>(node)) {
        uint32_t nodeIndex = index++;
        buffer.push_back(BRANCH_NODE | nodeIndex);

        uint32_t childrenIndices[8] = { 0 };

        for (size_t i = 0; i < 8; ++i) {
            if (branch->children[i]) {
                childrenIndices[i] = index;
                // std::cout << "Flattening child " << i << " at index " << index << std::endl;
                flattenNode(branch->children[i].get(), buffer, index);
            }
        }

        for (size_t i = 0; i < 8; ++i) {
            // std::cout << "Pushing back child index " << childrenIndices[i] << std::endl;
            buffer.push_back(childrenIndices[i]);
        }
    } else if (auto leaf = dynamic_cast<const SVOLeaf*>(node)) {
        uint32_t nodeIndex = index++;
        buffer.push_back(LEAF_NODE | nodeIndex);
        // std::cout << "Flattening leaf at index " << nodeIndex << std::endl;
        for (const auto& voxel : leaf->data) {
            buffer.push_back(voxel.r);
            buffer.push_back(voxel.g);
            buffer.push_back(voxel.b);
            buffer.push_back(voxel.a);
            // std::cout << "Pushing back voxel color " << voxel.r << ", " << voxel.g << ", " << voxel.b << ", " << voxel.a << std::endl;
        }
    }
    // preint the contents of the buffer
    // for (size_t i = 0; i < buffer.size(); ++i) {
    //     uint32_t nodeType = buffer[i] & NODE_TYPE_MASK;
    //     if (nodeType == BRANCH_NODE) {
    //         std::cout << "Buffer[" << i << "] = BRANCH_NODE" << std::endl;
    //     } else if (nodeType == LEAF_NODE) {
    //         std::cout << "Buffer[" << i << "] = LEAF_NODE" << std::endl;
    //     } else {
    //         std::cout << "Buffer[" << i << "] = " << buffer[i] << std::endl;
    //     }
    // }
}

