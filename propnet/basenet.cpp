#include "basenet.hpp"
#include "nodes.hpp"

#include <filesystem>
#include <fstream>
#include <stack>

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

            const auto& role_entries {game_json.at(ROLES_KEY)};
            for (const auto& role_entry : role_entries)
            {
                roles.emplace_back(
                    role_entry.at(ROLE_KEY).get<std::string>(),
                    role_entry.at(SEES_KEY).get<std::vector<std::uint32_t>>(),
                    role_entry.at(LEGALS_KEY).get<std::vector<std::uint32_t>>()
                );
            }

            const auto& entries {game_json.at(ENTRIES_KEY)};
            for (const auto& entry : entries)
            {
                add_entry(entry);
            }

            topologically_sorted_nodes = game_json.at(TOPOLOGICALLY_SORTED_KEY).get<std::vector<std::uint32_t>>();
        }
        catch (const nlohmann::json::exception& error)
        {
            throw ParsingError {error.what()};
        }


        if (terminal == nullptr)
        {
            throw ParsingError {"No terminal state found in definition"};
        }
    }

    std::uint32_t BaseNet::num_nodes() const
    {
        return nodes.size();
    }

    const std::vector<Role>& BaseNet::get_roles() const
    {
        return roles;
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
                    entry.at(INS_PROPS_KEY)
                });
                break;
            case EntryType::Or:
                add_node(OrNode {
                    id,
                    entry.at(INS_PROPS_KEY)
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
                    entry.at(IN_PROPS_KEY)
                });
                break;
            case EntryType::PostTransition:
                add_node(PostTransitionNode {
                    id,
                    entry.at(IN_PROPS_KEY)
                });
                topologically_sorted_nodes.push_back(id);
                break;
            case EntryType::Not:
                add_node(NotNode {
                    id,
                    entry.at(IN_PROPS_KEY)
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
        // TODO: Repetitive, but it works for now at least
        if (type == INITIAL_PROP_TYPE)
        {
            add_node(InitialPropositionNode {
                id,
                gdl
            });
        }
        else if (type == BASE_PROP_TYPE)
        {
            add_node(BasicPropositionNode {
                id,
                gdl,
                entry.at(IN_PROPS_KEY)
            });
        }
        else if (type == INPUT_PROP_TYPE)
        {
            add_node(InitialPropositionNode {
                id,
                gdl,
            });
        }
        else if (type == LEGAL_PROP_TYPE)
        {
            add_node(BasicPropositionNode {
                id,
                gdl,
                entry.at(IN_PROPS_KEY)
            });
        }
        else if (type == GOAL_PROP_TYPE)
        {
            add_node(BasicPropositionNode {
                id,
                gdl,
                entry.at(IN_PROPS_KEY)
            });
        }
        else if (type == SEES_PROP_TYPE)
        {
            add_node(BasicPropositionNode {
                id,
                gdl,
                entry.at(IN_PROPS_KEY)
            });
        }
        else if (type == TERMINAL_PROP_TYPE)
        {
            if (terminal != nullptr)
            {
                throw ParsingError {"More than one terminal state found whilst parsing"};
            }

            terminal = std::make_unique<BasicPropositionNode>(
                id,
                entry.at(GDL_KEY).get<std::string>(),
                entry.at(IN_PROPS_KEY)
            );
            nodes.push_back(terminal);
        }
        else if (type == OTHER_PROP_TYPE)
        {
            add_node(BasicPropositionNode {
                id,
                entry.at(GDL_KEY).get<std::string>(),
                entry.at(IN_PROPS_KEY)
            });
        }
        else
        {
            throw ParsingError {"Got unknown proposition type"};
        }
    }
};
