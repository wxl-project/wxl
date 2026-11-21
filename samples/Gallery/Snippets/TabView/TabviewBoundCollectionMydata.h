// The data of the tabs are the application's own; the TabView is given their numbers, and a template whose root is a TabViewItem
// with the properties made from the data.
struct MyData {
    std::u16string header;
    Frame content;
};
struct Model {
    std::vector<MyData> datas;
    IndexList order = IndexList::observable();

    void add() {
        auto const index = static_cast<int>(datas.size());
        MyData doc {u"MyData Doc " + gallery::numberText(index), Frame {}};
        gallery::showSample(doc.content, index % 3 + 1);
        datas.push_back(std::move(doc));
        order.append(index);
    }
};
auto const model = gallery::hold<Model>();
for (int i = 0; i < 3; ++i) {
    model->add();
}

auto const dataOf = [model](Object const& item) -> MyData const& { return model->datas[static_cast<size_t>(unboxValue<int64_t>(item))]; };

auto example = TabView {
    minHeight = 475,
    Margin {-12},
    selectedIndex = 0,
    tabItemsSource = model->order,
    tabItemTemplate = boundTemplate(u"TabViewItem",
                                    {{u"Content", [dataOf](Object const& item) { return dataOf(item).content; }},
                                     {u"Header", [dataOf](Object const& item) { return boxValue(dataOf(item).header); }},
                                     {u"IconSource", [](Object const&) { return SymbolIconSource {symbol = Symbol::Placeholder}; }}}),
    onAddTabButtonClick += [model](TabView const&) { model->add(); },
    onTabCloseRequested += [model](TabView const&, TabViewTabCloseRequestedEventArgs& args) {
        for (uint32_t i = 0; i < model->order.size(); ++i) {
            if (model->order.getAt(i) == unboxValue<int64_t>(args.item())) {
                model->order.removeAt(i);
                break;
            }
        }
    },
};