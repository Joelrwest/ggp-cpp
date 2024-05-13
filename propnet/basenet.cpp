#include "basenet.hpp"
#include "nodes.hpp"

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

    ParsingError::ParsingError(const char* message) :
        std::runtime_error {message}
    {}

    /*
    Not optimised as this isn't a critical
    section, and is only done once.

    Also kept in a single function to avoid exposing
    the json library to above files (and to avoid creating
    a lot of cluttered classes).

    Completely terrible and unreadable, but gets
    the job done and I don't wanna spend any more
    time than needed parsing stuff in C++.
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
            throw ParsingError {"Could not find given game in json format"};
        }

        try
        {
            const auto game_json = nlohmann::json::parse(game_file);

            roles = game_json.at(ROLES_KEY).get<std::vector<std::string>>();

            const auto& entries {game_json.at(ENTRIES_KEY)};
            for (const auto& entry : entries)
            {
                add_entry(entry);
            }
            std::cout << nodes.size() << '\n';
        }
        catch (const nlohmann::json::exception& error)
        {
            throw ParsingError {error.what()};
        }
    }

    void BaseNet::add_entry(const nlohmann::json& entry)
    {
        const std::uint32_t id {entry.at(ID_KEY)};
        const EntryType entry_type {entry.at(TYPE_KEY)};
        switch (entry_type)
        {
            case EntryType::And:
                add_node(AndNode {
                    id,
                    entry.at(INS_PROPS_KEY),
                    entry.at(OUTS_PROPS_KEY)
                });
                break;
            case EntryType::Or:
                add_node(OrNode {
                    id,
                    entry.at(INS_PROPS_KEY),
                    entry.at(OUTS_PROPS_KEY)
                });
                break;
            case EntryType::Proposition:
                add_proposition(
                    id,
                    entry.at(PROP_TYPE_KEY).get<std::string>(),
                    entry.at(GDL_KEY).get<std::string>(),
                    entry
                );
                break;
            case EntryType::PreTransition:
                add_node(PreTransitionNode {
                    id,
                    entry.at(IN_PROPS_KEY),
                    entry.at(OUT_PROPS_KEY)
                });
                break;
            case EntryType::PostTransition:
                add_node(PostTransitionNode {
                    id,
                    entry.at(IN_PROPS_KEY),
                    entry.at(OUT_PROPS_KEY)
                });
                break;
            case EntryType::Not:
                add_node(NotNode {
                    id,
                    entry.at(IN_PROPS_KEY),
                    entry.at(OUTS_PROPS_KEY)
                });
                break;
            case EntryType::Constant:
                add_node(TrueNode {
                    id
                });
                break;
            default:
                throw ParsingError {"Unknown entry type found"};
        }
    }

    void BaseNet::add_proposition(std::uint32_t id, std::string_view type, std::string&& gdl, const nlohmann::json& entry)
    {
        /*
        TODO: Repetitive, but it works for now at least

        Must also match games/constants.py
        */
        if (type == "init")
        {
            // add_node(InitNode {
            //     id,
            //     entry.at(VALUE_KEY]
            // });
        }
        else if (type == "base")
        {
            //
        }
        else if (type == "input")
        {
            //
        }
        else if (type == "legal")
        {
            //
        }
        else if (type == "goal")
        {
            //
        }
        else if (type == "sees")
        {
            //
        }
        else if (type == "terminal")
        {
            if (terminal != nullptr)
            {
                throw ParsingError {"More than one terminal state found whilst parsing"};
            }

            // terminal = std::make_unique<BasicPropositionNode>();
            // nodes.push_back(terminal);
        }
        else if (type == "other")
        {
            //
        }
        else
        {
            throw ParsingError {"Got unknown proposition type"};
        }
    }
};
