#pragma once

#include "propnet.hpp"

#include <concepts>
#include <nlohmann/json.hpp>
#include <optional>
#include <stdexcept>
#include <string_view>

namespace propnet
{
class ParsingError : public std::runtime_error
{
  public:
    ParsingError(const char *message);
};

class Parser
{
  public:
    Parser(std::string_view game);

    Propnet create_propnet();

  private:
    static constexpr auto GAMES_PATH{"games/json/"};
    static constexpr auto JSON_EXTENSION{"json"};

    static constexpr auto RANDOM_PLAYER_NAME{"random"};
    static constexpr auto ROLES_KEY{"roles"};
    static constexpr auto ROLE_KEY{"role"};
    static constexpr auto GOAL_KEY{"goal"};
    static constexpr auto GOALS_KEY{"goals"};
    static constexpr auto SEES_KEY{"sees"};
    static constexpr auto LEGAL_TO_INPUT_KEY{"legal_to_input"};
    static constexpr auto LEGAL_KEY{"legal"};
    static constexpr auto INPUT_KEY{"input"};
    static constexpr auto ENTRIES_KEY{"entries"};
    static constexpr auto ID_KEY{"id"};
    static constexpr auto TYPE_KEY{"type"};
    static constexpr auto IN_PROPS_KEY{"in"};
    static constexpr auto INS_PROPS_KEY{"ins"};
    static constexpr auto PROP_TYPE_KEY{"proposition_type"};
    static constexpr auto DISPLAY_KEY{"display"};
    static constexpr auto VALUE_KEY{"value"};
    static constexpr auto TOPOLOGICALLY_SORTED_KEY{"topologically_sorted"};

    static constexpr auto INITIAL_PROP_TYPE{"init"};
    static constexpr auto BASE_PROP_TYPE{"base"};
    static constexpr auto INPUT_PROP_TYPE{"input"};
    static constexpr auto LEGAL_PROP_TYPE{"legal"};
    static constexpr auto GOAL_PROP_TYPE{"goal"};
    static constexpr auto SEES_PROP_TYPE{"sees"};
    static constexpr auto TERMINAL_PROP_TYPE{"terminal"};
    static constexpr auto OTHER_PROP_TYPE{"other"};

    template <std::derived_from<Node> T> void add_node(T node);

    template <std::derived_from<PropositionNode> T> void add_proposition_node(T proposition_node);

    void add_entry(const nlohmann::json &entry);
    void add_proposition(PropId id, std::string_view type, std::string &&display, const nlohmann::json &entry);

    std::vector<Role> player_roles{};
    std::optional<Role> random_role{std::nullopt};
    std::vector<std::shared_ptr<const Node>> nodes{};
    std::unordered_map<PropId, std::shared_ptr<const PropositionNode>> propositions{};
    std::optional<PropId> terminal{};
    std::vector<PropId> topologically_sorted_nodes{};
    std::vector<PropId> non_post_topologically_sorted_nodes{};
    std::unordered_set<PropId> post_transition_nodes{};
};

template <std::derived_from<Node> T> void Parser::add_node(T node)
{
    nodes.emplace_back(std::make_shared<const T>(node));
}

template <std::derived_from<PropositionNode> T> void Parser::add_proposition_node(T proposition_node)
{
    const auto id{proposition_node.get_id()};
    const auto ptr{std::make_shared<const T>(proposition_node)};
    nodes.emplace_back(ptr);
    propositions[id] = ptr;
}
} // namespace propnet
