#include "setup.hpp"
#include "propnet/include/basenet_parser.hpp"

#include <iostream>

namespace setup
{
    propnet::BaseNet load_basenet()
    {
        while (true)
        {
            std::string game;
            try
            {
                std::cout << "Enter game name (spelt same as json): ";
                std::cin >> game;

                std::cout << "Loading basenet\n";
                propnet::BaseNetParser parser {game};
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
};
