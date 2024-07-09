#include "setup.hpp"
#include "../../player/include/player.hpp"
#include "../agents/include/human.hpp"
#include "../propnet/include/parser.hpp"

#include <iostream>
#include <stdexcept>

namespace setup
{
propnet::Propnet load_propnet()
{
    while (true)
    {
        std::string game;
        try
        {
            std::cout << "Enter game name (spelt same as json): ";
            std::cin >> game;

            return load_propnet(game);
        }
        catch (const propnet::ParsingError &error)
        {
            std::cout << "Please try again.\n";
        }
    }
}

propnet::Propnet load_propnet(std::string_view game)
{
    std::cout << "Loading propnet\n";
    try
    {
        propnet::Parser parser{game};
        const auto propnet{parser.create_propnet()};
        std::cout << "Propnet loaded\n";

        return propnet;
    }
    catch (const propnet::ParsingError &error)
    {
        std::cout << "Error occurred whilst trying to parse " << game << ": " << error.what() << '\n';
        throw error;
    }
}

std::unique_ptr<agents::Agent> create_agent(const propnet::Role &role, const propnet::Propnet &propnet)
{
    while (true)
    {
        std::string name{};
        try
        {
            std::cout << "Enter agent name for role '" << role.get_name() << "': ";
            std::cin >> name;

            return agent_factory(name, role, propnet);
        }
        catch (const std::invalid_argument &error)
        {
            std::cout << "Error occurred whilst trying to parse " << name << ": " << error.what() << '\n';
            std::cout << "Please try again.\n";
        }
    }
}

std::vector<std::unique_ptr<agents::Agent>> create_agents(const propnet::Propnet &propnet)
{
    std::vector<std::unique_ptr<agents::Agent>> agents{};
    for (const auto &player_role : propnet.get_player_roles())
    {
        agents.push_back(std::move(create_agent(player_role, propnet)));
    }

    if (propnet.is_randomness())
    {
        const auto &random_role{propnet.get_random_role()};
        agents.push_back(std::make_unique<agents::RandomAgent>(*random_role));
    }

    return agents;
}

std::unique_ptr<agents::Agent> agent_factory(std::string_view name, const propnet::Role &role,
                                             const propnet::Propnet &propnet)
{
    if (name == agents::RandomAgent::NAME)
    {
        return std::make_unique<agents::RandomAgent>(role);
    }
    else if (name == agents::FirstAgent::NAME)
    {
        return std::make_unique<agents::FirstAgent>(role);
    }
    else if (name == agents::LastAgent::NAME)
    {
        return std::make_unique<agents::LastAgent>(role);
    }
    else if (name == agents::HumanAgent::NAME)
    {
        return std::make_unique<agents::HumanAgent>(role);
    }
    else if (name == player::Player<>::NAME)
    {
        return std::make_unique<player::Player<>>(role, propnet);
    }
    else
    {
        throw std::invalid_argument{"Given unkown agent name"};
    }
}
} // namespace setup
