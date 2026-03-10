#include <string>
#include <regex>

#include "cpptui.hpp"

using namespace cpptui;

bool is_valid(const std::string& field_input)
{
    // if (field_input.empty()) return false;
    
    std::regex pattern(R"(^[\w]+$)");

    if (!std::regex_search(field_input, pattern))
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
    auto root = std::make_shared<Vertical>();

    auto header_label = std::make_shared<Label>("Justin's Login System (q to quit)");
    auto username_label = std::make_shared<Label>("");
    auto password_label = std::make_shared<Label>("");

    auto user_field = std::make_shared<Input>();
    user_field->placeholder = "Username";

    auto passwd_field = std::make_shared<Input>();
    passwd_field->placeholder = "Password";

    auto btn = std::make_shared<Button>("Submit", [
        user_field,
        username_label,
        passwd_field,
        password_label
    ]()
    {
        std::string username = user_field->get_value();
        
        if (is_valid(username))
        {
            username_label->set_text(username);
        }
        else
        {
            username_label->set_text("Username invalid!");
        }

        std::string password = passwd_field->get_value();

        if (is_valid(password))
        {
            password_label->set_text(password);
        }
        else
        {
            password_label->set_text("Password invalid!");
        }
    });

    app.register_exit_key('q');

    root->add(header_label);

    root->add(username_label);
    root->add(user_field);

    root->add(password_label);
    root->add(passwd_field);

    root->add(std::make_shared<VerticalSpacer>(1));
    root->add(btn);
    root->add(std::make_shared<VerticalSpacer>(1));

    app.run(root);
    return 0;
}
