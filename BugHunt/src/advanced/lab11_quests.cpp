#include "../labs.h"
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace
{
// 기획 데이터 테이블 (형식: 퀘스트ID: 선행퀘스트1 선행퀘스트2 ...)
const char* kQuestTable = R"(
dragon_hunt: forge_sword find_map
forge_sword: meet_blacksmith
meet_blacksmith: village_intro
find_map: visit_cartographer
visit_cartographer:
village_intro: forge_sword
)";

using QuestDb = std::map<std::string, std::vector<std::string>>;

QuestDb loadQuests(const char* table)
{
    QuestDb db;
    std::istringstream lines(table);
    std::string line;
    while (std::getline(lines, line))
    {
        const size_t colon = line.find(':');
        if (colon == std::string::npos)
            continue;

        std::vector<std::string>& prereqs = db[line.substr(0, colon)];
        std::istringstream words(line.substr(colon + 1));
        std::string word;
        while (words >> word)
            prereqs.push_back(word);
    }
    return db;
}

// 이 퀘스트를 받기 전에 깨야 하는 퀘스트가 모두 몇 개인지 센다.
int countAllPrereqs(const QuestDb& db, const std::string& questId)
{
    int count = 0;
    for (const std::string& prereq : db.at(questId))
        count += 1 + countAllPrereqs(db, prereq);
    return count;
}
}

void lab11_quests()
{
    const QuestDb db = loadQuests(kQuestTable);
    expect("dragon_hunt 선행 퀘스트 수", 5, countAllPrereqs(db, "dragon_hunt"));
}
