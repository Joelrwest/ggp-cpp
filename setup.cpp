#include "setup.hpp"
#include "propnet/include/basenet_parser.hpp"
#include "agents/include/factory.hpp"
#include "agents/include/simple.hpp"

#include <iostream>

namespace setup
{
    propnet::Basenet load_basenet()
    {
        while (true)
        {
            std::string game;
            try
            {
                std::cout << "Enter game name (spelt same as json): ";
                std::cin >> game;

                std::cout << "Loading basenet\n";
                propnet::BasenetParser parser {game};
                const auto basenet {parser.create_basenet()};
                std::cout << "Basenet loaded\n";

                return basenet;
            }
            catch (const propnet::ParsingError error)
            {
                std::cout << "Error occurred whilst trying to parse " << game << ": " << error.what() << '\n';
                std::cout << "Please try again.\n";
            }
        }
    }

    std::unique_ptr<agents::Agent> create_agent(const propnet::Role& role, const propnet::Propnet& propnet)
    {
        while (true)
        {
            std::string name {};
            try
            {
                std::cout << "Enter agent name: ";
                std::cin >> name;

                return agents::agent_factory(name, role, propnet);
            }
            catch(const std::invalid_argument& error)
            {
                std::cout << "Error occurred whilst trying to parse " << name << ": " << error.what() << '\n';
                std::cout << "Please try again.\n";
            }
        }
    }

    std::vector<std::unique_ptr<agents::Agent>> create_agents(const propnet::Basenet& basenet, const propnet::Propnet& propnet)
    {
        static constexpr auto RANDOM_PLAYER_NAME {"random"};

        std::vector<std::unique_ptr<agents::Agent>> agents {};
        for (const auto& role : basenet.get_roles())
        {
            if (role.get_name() == RANDOM_PLAYER_NAME)
            {
                agents.push_back(std::make_unique<agents::RandomAgent>(role, propnet));
            }
            else
            {
                agents.push_back(std::move(create_agent(role, propnet)));
            }
        }

        return agents;
    }
};
