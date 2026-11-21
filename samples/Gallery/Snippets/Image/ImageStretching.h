auto const picture = Image {width = 100, height = 100, source = u"Assets/SampleMedia/valley.jpg", stretch = Stretch::None};

auto const stretching = [picture](Stretch fit) { return [picture, fit](auto&&...) { picture.stretch(fit); }; };
auto options = RadioButtons {
    header = u"Image stretch mode",
    RadioButton {content = u"None", groupName = u"ImageStretch", isChecked = true, onChecked += stretching(Stretch::None)},
    RadioButton {content = u"Fill", groupName = u"ImageStretch", onChecked += stretching(Stretch::Fill)},
    RadioButton {content = u"Uniform", groupName = u"ImageStretch", onChecked += stretching(Stretch::Uniform)},
    RadioButton {content = u"UniformToFill", groupName = u"ImageStretch", onChecked += stretching(Stretch::UniformToFill)},
};

auto example = picture;