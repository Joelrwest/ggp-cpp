#include "basenet.hpp"
#include "nodes.hpp"

#include <nlohmann/json.hpp>
#include <filesystem>
#include <fstream>

#include <iostream> // TODO: Remove

namespace propnet
{
    /*
    Must be kept in line with the values
    in the Python file games/constants.py
    (which then get outputted to the json variants)
    */
    enum EntryType
    {
        And = 1,
        Or = 2,
        Proposition = 3,
        Not = 5,
        Constant = 6,
        PreTransition = 8,
        PostTransition = 9,
    };

    /*
    Not optimised as this isn't a critical
    section, and is only done once.
    Also kept in a single function to avoid exposing
    the json library to above files (and to avoid creating
    a lot of cluttered classes).
    */
    BaseNet::BaseNet(std::string_view game)
    {
        std::filesystem::path game_path {GAMES_PATH};
        game_path.append(game);
        game_path.concat(".");
        game_path.concat(JSON_EXTENSION);

        std::ifstream game_file {game_path};
        if (!game_file.good())
        {
            throw std::runtime_error {"Could not find given game in json format"};
        }

        const auto game_json = nlohmann::json::parse(game_file);

        roles = game_json[ROLES_KEY].template get<std::vector<std::string>>();

        const auto& entries {game_json[ENTRIES_KEY]};
        const auto num_entries {static_cast<std::uint32_t>(entries.size())};
        std::uint32_t num_transitions {0};
        for (const auto& entry : entries)
        {
            const std::uint32_t id {entry[ID_KEY]};
            const EntryType entry_type {entry[TYPE_KEY]};
            switch (entry_type)
            {
                case EntryType::And:
                    add_node(AndNode {
                        id,
                        entry[INS_PROPS_KEY],
                        entry[OUTS_PROPS_KEY]
                    });
                    break;
                case EntryType::Or:
                    add_node(OrNode {
                        id,
                        entry[INS_PROPS_KEY],
                        entry[OUTS_PROPS_KEY]
                    });
                    break;
                case EntryType::Proposition:
                    break;
                case EntryType::PreTransition:
                    add_node(PreTransitionNode {
                        id,
                        entry[IN_PROPS_KEY],
                        entry[OUT_PROPS_KEY]
                    });
                    break;
                case EntryType::PostTransition:
                    add_node(PostTransitionNode {
                        id,
                        entry[IN_PROPS_KEY],
                        entry[OUT_PROPS_KEY]
                    });
                    break;
                case EntryType::Not:
                    add_node(NotNode {
                        id,
                        entry[IN_PROPS_KEY],
                        entry[OUTS_PROPS_KEY]
                    });
                    break;
                case EntryType::Constant:
                    add_node(ConstantNode {
                        id,
                        entry[VALUE_KEY]
                    });
                    break;
                default:
                    throw std::runtime_error {"Unknown entry type found"};
            }
        }
        std::cout << nodes.size() << '\n';
    }
};
