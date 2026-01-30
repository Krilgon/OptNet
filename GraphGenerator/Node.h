#pragma once

enum class NodeType {
    PC,
    COMPUTE,
    STORAGE,
    GATEWAY,
    SERVER
};

struct Node {
    int id = -1;
    NodeType type = NodeType::PC;
    int performance = 0;
};
