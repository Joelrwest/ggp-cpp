#include "../include/parser.hpp"

#include <filesystem>
#include <fstream>

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

ParsingError::ParsingError(const char *message) : std::runtime_error{message}
{
}

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
Parser::Parser(std::string_view game) : game_name{game}
{
    std::filesystem::path game_path{GAMES_PATH};
    game_path.append(game);
    game_path.concat(".");
    game_path.concat(JSON_EXTENSION);

    std::ifstream game_file{game_path};
    if (!game_file.good())
    {
        throw ParsingError{"Could not find given game in json format"};
    }

    try
    {
        const auto game_json = nlohmann::json::parse(game_file);

        const auto &entries{game_json.at(ENTRIES_KEY)};
        for (const auto &entry : entries)
        {
            add_entry(entry);
        }

        topologically_sorted_nodes = game_json.at(TOPOLOGICALLY_SORTED_KEY).get<std::vector<PropId>>();

        const auto &role_entries{game_json.at(ROLES_KEY)};
        for (const auto &role_entry : role_entries)
        {
            std::vector<std::shared_ptr<const PropositionNode>> sees{};
            const auto &sees_entries{role_entry.at(SEES_KEY).get<std::vector<PropId>>()};
            for (const auto &sees_entry : sees_entries)
            {
                const auto node{propositions.at(sees_entry)};
                sees.emplace_back(node);
            }

            std::vector<Role::Legal> legals{};
            const auto &legal_to_input_entries{role_entry.at(LEGAL_TO_INPUT_KEY)};
            for (const auto &legal_to_input_entry : legal_to_input_entries)
            {
                const auto legal{legal_to_input_entry.at(LEGAL_KEY).get<PropId>()};
                const auto input{legal_to_input_entry.at(INPUT_KEY).get<PropId>()};
                const auto node{propositions.at(legal)};
                legals.emplace_back(Role::Legal{
                    .node = node,
                    .legal = legal,
                    .input = input,
                });
            }

            std::vector<Role::Goal> goals{};
            const auto &goals_entries{role_entry.at(GOALS_KEY)};
            for (const auto &goals_entry : goals_entries)
            {
                const auto goal{goals_entry.at(GOAL_KEY).get<PropId>()};
                const auto value{goals_entry.at(VALUE_KEY).get<Reward>()};
                const auto node{propositions.at(goal)};
                goals.emplace_back(Role::Goal{
                    .node = node,
                    .value = value,
                });
            }

            const auto &role_name{role_entry.at(ROLE_KEY).get<std::string>()};
            const auto is_random_role{role_name == RANDOM_PLAYER_NAME};
            if (is_random_role)
            {
                if (random_role.has_value())
                {
                    throw std::logic_error{"Cannot be more than 1 random player"};
                }
                else
                {
                    random_role.emplace(role_name, sees, legals, goals);
                }
            }
            else
            {
                player_roles.emplace_back(role_name, sees, legals, goals);
            }
        }
    }
    catch (const nlohmann::json::exception &error)
    {
        throw ParsingError{error.what()};
    }

    if (!terminal.has_value())
    {
        throw ParsingError{"No terminal state found in definition"};
    }

    std::copy_if(topologically_sorted_nodes.begin(), topologically_sorted_nodes.end(),
                 std::back_inserter(non_post_topologically_sorted_nodes),
                 [this](const auto id) { return !post_transition_nodes.contains(id); });
}

Propnet Parser::create_propnet()
{
    return Propnet{game_name,
                   player_roles,
                   random_role,
                   nodes,
                   terminal.value(),
                   topologically_sorted_nodes,
                   non_post_topologically_sorted_nodes};
}

void Parser::add_entry(const nlohmann::json &entry)
{
    const PropId id{entry.at(ID_KEY)};
    const EntryType entry_type{entry.at(TYPE_KEY)};
    switch (entry_type)
    {
    case EntryType::And:
        add_node(AndNode{id, entry.at(INS_PROPS_KEY).get<std::vector<PropId>>()});
        break;
    case EntryType::Or:
        add_node(OrNode{id, entry.at(INS_PROPS_KEY).get<std::vector<PropId>>()});
        break;
    case EntryType::Proposition:
        add_proposition(id, entry.at(PROP_TYPE_KEY).get<std::string>(), entry.at(DISPLAY_KEY).get<std::string>(),
                        entry);
        break;
    case EntryType::PreTransition:
        add_node(PreTransitionNode{
            id,
            entry.at(IN_PROPS_KEY),
        });
        break;
    case EntryType::PostTransition:
        add_node(PostTransitionNode{id, entry.at(IN_PROPS_KEY)});
        topologically_sorted_nodes.push_back(id);
        post_transition_nodes.insert(id);
        break;
    case EntryType::Not:
        add_node(NotNode{id, entry.at(IN_PROPS_KEY)});
        break;
    case EntryType::Constant:
        add_node(TrueNode{id});
        break;
    default:
        throw ParsingError{"Unknown entry type found"};
    }
}

void Parser::add_proposition(PropId id, std::string_view type, std::string &&display, const nlohmann::json &entry)
{
    if (type == INITIAL_PROP_TYPE)
    {
        add_proposition_node(InitialPropositionNode{
            id,
            display,
        });
    }
    else if (type == BASE_PROP_TYPE)
    {
        add_proposition_node(BasicPropositionNode{
            id,
            display,
            entry.at(IN_PROPS_KEY),
        });
    }
    else if (type == INPUT_PROP_TYPE)
    {
        add_proposition_node(InputPropositionNode{
            id,
            display,
        });
    }
    else if (type == LEGAL_PROP_TYPE)
    {
        add_proposition_node(BasicPropositionNode{
            id,
            display,
            entry.at(IN_PROPS_KEY),
        });
    }
    else if (type == GOAL_PROP_TYPE)
    {
        add_proposition_node(BasicPropositionNode{
            id,
            display,
            entry.at(IN_PROPS_KEY),
        });
    }
    else if (type == SEES_PROP_TYPE)
    {
        add_proposition_node(BasicPropositionNode{
            id,
            display,
            entry.at(IN_PROPS_KEY),
        });
    }
    else if (type == TERMINAL_PROP_TYPE)
    {
        if (terminal.has_value())
        {
            throw ParsingError{"More than one terminal state found whilst parsing"};
        }

        add_proposition_node(BasicPropositionNode{
            id,
            display,
            entry.at(IN_PROPS_KEY),
        });

        terminal.emplace(id);
    }
    else if (type == OTHER_PROP_TYPE)
    {
        add_proposition_node(BasicPropositionNode{
            id,
            display,
            entry.at(IN_PROPS_KEY),
        });
    }
    else
    {
        throw ParsingError{"Got unknown proposition type"};
    }
}
} // namespace propnet
