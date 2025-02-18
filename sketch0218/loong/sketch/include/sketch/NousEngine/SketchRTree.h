#pragma once
#include "nous_mesh/mesh.h"
// #include "nous_mesh/types.h"
#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace sketch
{
/*
 * 优化点：检测树的退化程度（例如利用率、树高等）  触发局部 Reinsertion 或 全树重建。
 * 新增批量插入/删除操作，手动触发 Bulk Load 或 rebuild。
 */
template<class T> class SketchRTree
{
public:
    SketchRTree(int max_children = 4) : m_max_children(max_children) { m_root = std::make_shared<RTreeNode>(); }

    ~SketchRTree() {}

    void insert_object(T obj, nous::AABB3 box) { insert_into_node(m_root, obj, box); }

    void delete_object(T obj, nous::AABB3 box)
    {
        delete_from_node(m_root, obj, box);

        // 如果根节点是内部节点，且只有一个子节点，则提升子节点为新的根
        if(!m_root->is_leaf && m_root->children.size() == 1)
        {
            m_root = m_root->children.front();
        }

        // 如果根节点变成空节点了，并且是非叶子，则新建一个根节点（退化成空树）
        if(!m_root->is_leaf && m_root->children.empty())
        {
            m_root = std::make_shared<RTreeNode>();
        }
    }

    std::unordered_set<T> search(nous::AABB3 box)
    {
        std::unordered_set<T> results;
        search_in_node(m_root, box, results);
        return results;
    }

private:
    struct RTreeNode
    {
        nous::AABB3 box;
        std::vector<std::shared_ptr<RTreeNode>> children;
        std::vector<std::pair<T, nous::AABB3>> objects;
        bool is_leaf = true;
    };

    void insert_into_node(std::shared_ptr<RTreeNode> node, T obj, nous::AABB3 box)
    {
        // 如果是叶子节点，直接存储对象，否则递归选择子节点插入
        if(node->is_leaf)
        {
            node->objects.emplace_back(obj, box);
            node->box = node->box.merged(box);
            if(node->objects.size() > m_max_children)
            {
                split_node(node);// 需要分裂节点
            }
        }
        else
        {
            std::shared_ptr<RTreeNode> best_child = choose_best_child(node, box);
            insert_into_node(best_child, obj, box);
            node->box = node->box.merged(box);// 更新父级 AABB
        }
    }

    void search_in_node(std::shared_ptr<RTreeNode> node, nous::AABB3 box, std::unordered_set<T>& results)
    {
        if(!node)
            return;

        if(!node->box.intersects(box))
            return;

        if(node->is_leaf)
        {
            for(const auto& [obj, obj_box]: node->objects)
            {
                if(obj_box.intersects(box))
                {
                    results.insert(obj);
                }
            }
        }
        else
        {
            for(const auto& child: node->children)
            {
                search_in_node(child, box, results);
            }
        }
    }

    void split_node(std::shared_ptr<RTreeNode> node)
    {
        auto new_node = std::make_shared<RTreeNode>();
        new_node->is_leaf = node->is_leaf;

        // 选择最佳分裂轴和分裂点
        char best_axis = 'x';
        int best_split_index = 0;
        float min_overlap = std::numeric_limits<float>::max();
        float min_area = std::numeric_limits<float>::max();

        // 预排序所有节点，根据轴排序
        std::array<char, 3> axes = { 'x', 'y', 'z' };
        for(char axis: axes)
        {
            // 按当前轴排序
            std::sort(node->objects.begin(), node->objects.end(), [axis](const auto& a, const auto& b) {
                if(axis == 'x')
                    return a.second.center().x() < b.second.center().x();
                if(axis == 'y')
                    return a.second.center().y() < b.second.center().y();
                return a.second.center().z() < b.second.center().z();
            });

            // 选择重叠体积最小的分裂点
            for(int i = 1; i < node->objects.size(); ++i)
            {
                auto left_box = compute_bounding_box({ node->objects.begin(), node->objects.begin() + i });
                auto right_box = compute_bounding_box({ node->objects.begin() + i, node->objects.end() });

                float overlap = left_box.intersection(right_box).volume();
                float area = left_box.volume() + right_box.volume();

                // 更新分裂轴与分裂点选择
                if(overlap < min_overlap || (overlap == min_overlap && area < min_area))
                {
                    min_overlap = overlap;
                    min_area = area;
                    best_axis = axis;
                    best_split_index = i;
                }

                // 如果不重叠则直接分裂
                if(min_overlap == 0)
                    break;
            }
        }

        // 使用最佳轴和分裂点重新排序并拆分
        std::sort(node->objects.begin(), node->objects.end(), [best_axis](const auto& a, const auto& b) {
            if(best_axis == 'x')
                return a.second.center().x() < b.second.center().x();
            if(best_axis == 'y')
                return a.second.center().y() < b.second.center().y();
            return a.second.center().z() < b.second.center().z();
        });

        // 将对象分配到两个子节点
        new_node->objects.assign(node->objects.begin() + best_split_index, node->objects.end());
        node->objects.erase(node->objects.begin() + best_split_index, node->objects.end());

        new_node->box = compute_bounding_box(new_node->objects);
        node->box = compute_bounding_box(node->objects);

        // 挂到父节点
        if(node == m_root)
        {
            auto new_root = std::make_shared<RTreeNode>();
            new_root->is_leaf = false;
            new_root->children.push_back(m_root);
            new_root->children.push_back(new_node);
            new_root->box = node->box.merged(new_node->box);
            m_root = new_root;
        }
        else
        {
            // 如果不是根节点，挂到父节点
            auto parent = find_parent_node(m_root, node);
            parent->children.push_back(new_node);
            parent->box = parent->box.merged(new_node->box);
        }
    }

    std::shared_ptr<RTreeNode> choose_best_child(std::shared_ptr<RTreeNode> node, nous::AABB3 box)
    {
        // 选择增量最小的子节点
        std::shared_ptr<RTreeNode> best_child = nullptr;
        float min_increase = std::numeric_limits<float>::max();

        for(const auto& child: node->children)
        {
            float increase = child->box.merged(box).volume() - child->box.volume();
            if(increase < min_increase)
            {
                min_increase = increase;
                best_child = child;
            }
        }
        return best_child;
    }

    bool delete_from_node(std::shared_ptr<RTreeNode> node, T obj, nous::AABB3 box)
    {
        if(node->is_leaf)
        {
            auto it = std::find_if(node->objects.begin(), node->objects.end(), [&obj](const auto& pair) { return pair.first == obj; });
            if(it != node->objects.end())
            {
                node->objects.erase(it);
                return true;
            }
            return false;
        }
        else
        {
            for(auto it = node->children.begin(); it != node->children.end();)
            {
                if((*it)->box.intersects(box))
                {
                    if(delete_from_node(*it, obj, box))
                    {
                        // 如果子节点空了，从 children 列表中删除
                        if((*it)->is_leaf && (*it)->objects.empty())
                        {
                            it = node->children.erase(it);
                        }
                        else if(!(*it)->is_leaf && (*it)->children.empty())
                        {
                            it = node->children.erase(it);
                        }
                        else
                        {
                            ++it;
                        }

                        // 更新当前节点的 AABB
                        if(!node->children.empty())
                        {
                            node->box = compute_bounding_box(node->objects);
                            for(const auto& child: node->children)
                            {
                                node->box = node->box.merged(child->box);
                            }
                        }
                        else
                        {
                            node->box = nous::AABB3();// 如果当前节点也空了，box变成空
                        }
                        return true;
                    }
                    else
                    {
                        ++it;
                    }
                }
                else
                {
                    ++it;
                }
            }
        }
        return false;
    }

    // void merge_or_rebalance(std::shared_ptr<RTreeNode> node)
    //{
    //     if(node == m_root && node->children.size() == 1)
    //     {
    //         // 如果根节点的子节点数为 1，则提升子节点为新的根节点
    //         m_root = node->children.front();
    //     }
    //     else
    //     {
    //         // 查找并合并相邻兄弟节点，或者重新分配对象
    //         if(node->children.size() == 0)
    //             return;// 根节点已空，表示树为空
    //
    //         // 合并兄弟节点
    //         for(auto& sibling: node->children)
    //         {
    //             if(sibling->objects.size() < m_max_children / 2)
    //             {
    //                 // 如果兄弟节点对象太少，尝试合并
    //                 merge_with_sibling(node, sibling);
    //                 break;
    //             }
    //         }
    //     }
    // }
    //
    // void merge_with_sibling(std::shared_ptr<RTreeNode> node, std::shared_ptr<RTreeNode> sibling)
    //{
    //     // 将兄弟节点的对象移动到当前节点
    //     node->objects.insert(node->objects.end(), sibling->objects.begin(), sibling->objects.end());
    //     node->children.erase(std::remove(node->children.begin(), node->children.end(), sibling), node->children.end());
    //
    //     node->box = compute_bounding_box(node->objects);
    //
    //     // 递归更新父节点的 AABB
    //     auto parent = find_parent_node(m_root, node);
    //     if(parent)
    //     {
    //         parent->box = parent->box.merged(node->box);
    //     }
    // }

    std::shared_ptr<RTreeNode> find_parent_node(std::shared_ptr<RTreeNode> root, std::shared_ptr<RTreeNode> child)
    {
        if(!root || root->is_leaf)
            return nullptr;

        for(auto& node: root->children)
        {
            if(node == child)
                return root;

            auto parent = find_parent_node(node, child);
            if(parent)
                return parent;
        }

        return nullptr;
    }

    nous::AABB3 compute_bounding_box(const std::vector<std::pair<T, nous::AABB3>>& objects)
    {
        nous::AABB3 box;
        for(const auto& [obj, obj_box]: objects)
        {
            box = box.merged(obj_box);
        }
        return box;
    }

private:
    std::shared_ptr<RTreeNode> m_root;
    int m_max_children;
};

}// namespace sketch
