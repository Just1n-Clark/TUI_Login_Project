/**
 * Justin Clark
 * Created 03/09/26
 * NOTE: Compile with flag -std=c++20 for std::format to work
 */

#include <string>
#include <regex>
#include <algorithm>
#include <fstream>
#include <functional>
#include <format>
#include <iostream>
#include <sstream>

#include "cpptui.hpp"

using namespace cpptui;

enum class AccountStatus
{
    Success,
    Partial,
    Fail
};

bool is_valid(const std::string& field_input, size_t min_length)
{   
    std::regex pattern(R"(^[\w]+$)");

    if (!std::regex_search(field_input, pattern) || field_input.length() < min_length)
    {
        return false;
    }

    // Passed validation
    return true;
}

/**
 * @brief Writes hashes to file using specified format
 * @param filename filename to write to
 * @param username hashed username
 * @param password hashed password
 * @return result of successful operation (true/false)
 */
bool write_to_file(
    const std::string& filename,
    const size_t& username,
    const size_t& password
)
{
    std::ofstream file(filename, std::ios::app);

    if (!file.is_open())
    {
        // File did not open properly
        return false;
    }

    // Format == user:pass\n
    file << username << ":" << password << std::endl;
    file.close();

    return true;
}

/**
 * @brief validate account existence
 * @param filename filename to check against
 * @param user_hash hashed username
 * @param pass_hash hashed password
 * @return AccountStatus enum, { Success, Partial, Fail }
 */
AccountStatus check_account(
    const std::string& filename,
    std::size_t& user_hash,
    std::size_t& pass_hash
)
{
    std::ifstream file(filename);

    if (!file.is_open())
    {
        return AccountStatus::Fail;
    }

    std::string file_entry;

    while (std::getline(file, file_entry))
    {
        std::istringstream ss(file_entry);
        std::string user;
        std::string pass;

        std::getline(ss, user, ':');
        std::getline(ss, pass);

        if (stoul(user) == user_hash)
        {
            if (stoul(pass) == pass_hash)
            {
                return AccountStatus::Success;
            }
            return AccountStatus::Partial;
        }
    }

    file.close();

    return AccountStatus::Fail;
}

int main()
{
    Theme::set_theme(Theme::TokyoNight());

    App app;

    std::string real_password;

    // Widgets
    auto root = std::make_shared<Vertical>();

    auto header_label = std::make_shared<Label>("Justin's Login System (q to quit)");
    auto username_label = std::make_shared<Label>("");
    auto password_label = std::make_shared<Label>("");
    auto info_label = std::make_shared<Label>("");

    auto user_field = std::make_shared<Input>();
    user_field->placeholder = "Username";

    auto passwd_field = std::make_shared<Input>();

    passwd_field->placeholder = "Password";

    std::optional<TimerId> msg_timer;

    auto btn = std::make_shared<Button>("Submit", [
        &user_field,
        &username_label,
        &passwd_field,
        &password_label,
        &info_label,
        &app,
        &msg_timer,
        &real_password
    ]()
    {
        std::string username = user_field->get_value();
        bool user_is_valid;
        bool pass_is_valid;
        if (is_valid(username, 5))
        {
            user_is_valid = true;
            username_label->set_text("");
        }
        else
        {
            username_label->set_text("Username invalid!");
            user_is_valid = false;
        }

        std::string password = real_password;

        if (is_valid(password, 8))
        {
            pass_is_valid = true;
            password_label->set_text("");
        }
        else
        {
            password_label->set_text("Password invalid!");
            pass_is_valid = false;
        }

        if (user_is_valid && pass_is_valid)
        {
            // Preliminary setup            
            std::string filename = "accounts.txt";
            std::size_t username_hash = std::hash<std::string>{}(username);
            std::size_t password_hash = std::hash<std::string>{}(password);
            
            switch (check_account(filename, username_hash, password_hash))
            {
                // Success
                case AccountStatus::Success:
                    info_label->set_text(std::format("Welcome {}!", username));
                    break;
                case AccountStatus::Partial:
                    info_label->set_text("Incorrect username or password");
                    break;
                case AccountStatus::Fail:
                    info_label->set_text(std::format("Account not found, created account for user {}", username));
                    write_to_file(filename, username_hash, password_hash);
            }

            if (msg_timer.has_value())
            {
                app.remove_timer(msg_timer.value());
            }

            msg_timer = app.add_timer(3000, [info_label, &msg_timer]()
            {
                info_label->set_text("");
                msg_timer.reset();
            });

            user_field->set_value("");
            passwd_field->set_value("");
            real_password = "";
        }
    });

    app.register_exit_key('q');

    root->add(header_label);

    root->add(username_label);
    root->add(user_field);

    root->add(password_label);
    root->add(passwd_field);

    root->add(std::make_shared<VerticalSpacer>(1));
    root->add(info_label);

    root->add(std::make_shared<VerticalSpacer>(1));
    root->add(btn);
    root->add(std::make_shared<VerticalSpacer>(1));

    passwd_field->on_change = [&real_password, passwd_field](std::string current_val)
    {
        if (current_val.empty())
        {
            real_password = "";
            return;
        }

        std::string display_passwd = passwd_field->get_value();
        
        size_t current_stars = std::count(current_val.begin(), current_val.end(), '*');
        size_t display_stars = std::count(display_passwd.begin(), display_passwd.end(), '*');
        
        if (current_val.length() > real_password.length())
        {
            real_password += current_val.back();
        }
        else if (current_val.length() < real_password.length())
        {
            real_password.pop_back();
        }

        std::string stars = std::string(real_password.length(), '*');

        if (current_val != stars) { passwd_field->set_value(stars); }
    };

    app.run(root);
    return 0;
}
