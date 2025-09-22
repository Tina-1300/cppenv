#include <iostream>
#include <cppenv/cppenv.hpp>


int main(){

    cppenv::EnvManager envManager;

    if (!envManager.load_from_file(".env")){
        std::cerr << "Unable to load .env file\n";
    }

    std::optional<std::string> db_url = envManager.get_value("DATABASE_URL");

    std::optional<std::string> api_key_telegram = envManager.get_value("TELEGRAM_TOKEN");

    std::optional<std::string> api_key_discord = envManager.get_value("DISCORD_TOKEN");

    std::optional<std::string> api_key_netflix = envManager.get_value("NETFLIX_TOKEN");

    std::optional<int> server_port = envManager.get_value_as<int>("SERVER_PORT");

    if (!db_url && !api_key_telegram && !api_key_discord && !api_key_netflix && !server_port){
        std::cerr << "The environment variables were not found\n";
    }

    std::cout << "DATABASE_URL : " << *db_url << "\n" << "TELEGRAM_TOKEN : " << *api_key_telegram << "\n";

    std::cout << "DISCORD_TOKEN : " << *api_key_discord << "\n" << "NETFLIX_TOKEN : " << *api_key_netflix << "\n" << "SERVER_PORT : " << *server_port  << "\n";



    return 0;
}