#include <string>
#include <regex>
#include <algorithm>

#include "cpptui.hpp"

using namespace cpptui;

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
            info_label->set_text("Account created!");

            if (msg_timer.has_value())
            {
                app.remove_timer(msg_timer.value());
            }

            msg_timer = app.add_timer(1000, [info_label, &msg_timer]()
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
