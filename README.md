# cppenv

cppenv is a header-only C++20 library that makes it easy to manage .env files in your projects.

---

## ✨ Features 

- Load `.env` files securely and easily

- Header-only : just include and use

- Simple API for accessing environment variables

- Ideal for config management in C++ projects

---

## 🚀 Quick Start



1. *Install :*  
    Just copy the cppenv directory into your project - no build step is required.

2. *Use :*

```cpp
#include <cppenv/cppenv.hpp>
#include <iostream>

int main(){
    cppenv::EnvManager env;

    // Load the .env file.
    if (!env.load(".env")) {
        std::cerr << "Failed to load .env file\n";
        return 1;
    }

    // Get a string value.
    if (auto database_url = env.get("DATABASE_URL")) {
        std::cout << "Database URL: " << *database_url << '\n';
    }

    // Get a typed value.
    if (auto port = env.get_as<int>("PORT")) {
        std::cout << "Server port: " << *port << '\n';
    }

    // Get a boolean value.
    if (auto debug = env.get_as<bool>("DEBUG")) {
        std::cout << "Debug: " << (*debug ? "enabled" : "disabled") << '\n';
    }

    return 0;
}
```


