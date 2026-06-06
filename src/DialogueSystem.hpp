#pragma once
#include <string>
#include <vector>

struct DialogueChoice {
    std::string text;
    int nextNodeId;
};

struct DialogueNode {
    int id;
    std::string npcName;
    std::string text;
    std::vector<DialogueChoice> choices;
    int nextNodeId;
};
