#include "setup.hpp"
#include "../propnet/include/parser.hpp"
#include "../agents/include/factory.hpp"

#include <iostream>

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

                std::cout << "Loading propnet\n";
                propnet::Parser parser {game};
                const auto propnet {parser.create_propnet()};
                std::cout << "Propnet loaded\n";

                return propnet;
            }
            catch (const propnet::ParsingError& error)
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
                std::cout << "Enter agent name for role '" << role.get_name() << "': ";
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

    std::vector<std::unique_ptr<agents::Agent>> create_agents(const propnet::Propnet& propnet)
    {
        std::vector<std::unique_ptr<agents::Agent>> agents {};
        for (const auto& role : propnet.get_roles())
        {
            if (role.get_name() == propnet::Parser::RANDOM_PLAYER_NAME)
            {
                agents.push_back(std::make_unique<agents::RandomAgent>(role));
            }
            else
            {
                agents.push_back(std::move(create_agent(role, propnet)));
            }
        }

        return agents;
    }
}
