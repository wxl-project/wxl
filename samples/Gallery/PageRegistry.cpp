// Реестр страниц контролов: каждая страница регистрирует себя в своём файле
// макросом REGISTER_GALLERY_PAGE, конструктор PageRegistry ставит запись в
// общий односвязный список, а pageFor ищет в нём фабрику по идентификатору.

#include "Pages.h"

namespace {

// Голова списка регистраций. Function-local static: занулена при загрузке
// программы, до того как конструкторы PageRegistry из разных единиц
// трансляции начнут вставать в список, поэтому их порядок не важен.
gallery::PageRegistry*& head() {
    static gallery::PageRegistry* head = nullptr;
    return head;
}

}  // namespace

gallery::PageRegistry::PageRegistry(std::wstring_view pageId, gallery::ControlPage factory)
    : pageId_(pageId), factory_(factory), next_(head()) {
    head() = this;
}

gallery::ControlPage gallery::pageFor(std::wstring_view uniqueId) {
    for (gallery::PageRegistry const* node = head(); node != nullptr; node = node->next_) {
        if (node->pageId_ == uniqueId) {
            return node->factory_;
        }
    }
    return nullptr;
}
