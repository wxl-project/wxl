auto output = TextBlock {visibility = Visibility::Collapsed};

auto box = PasswordBox {
    width = 300,
    onPasswordChanged += [output](PasswordBox const& self) {
        auto const typed = self.password();
        if (typed.empty() || typed == u"Password") {
            output.visibility(Visibility::Visible);
            output.text(u"'Password' is not allowed.");
            self.password(u"");
        } else {
            output.text(u"");
            output.visibility(Visibility::Collapsed);
        }
    },
};