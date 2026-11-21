auto const soundButton = [](char16_t const* labelText, ElementSoundKind sound) {
    return Button {content = std::u16string {u"\u25B6 "} + labelText, elementSoundMode = ElementSoundMode::Off, automationName = labelText,
                   onClick += [sound](auto&&...) { ElementSoundPlayer::play(sound); }};
};

auto example = StackPanel {
    orientation.vertical,
    spacing = 5,
    soundButton(u"Focus", ElementSoundKind::Focus),
    soundButton(u"Invoke", ElementSoundKind::Invoke),
    soundButton(u"Show", ElementSoundKind::Show),
    soundButton(u"Hide", ElementSoundKind::Hide),
    soundButton(u"MovePrevious", ElementSoundKind::MovePrevious),
    soundButton(u"MoveNext", ElementSoundKind::MoveNext),
    soundButton(u"GoBack", ElementSoundKind::GoBack),
};