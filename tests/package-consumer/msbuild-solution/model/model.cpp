#include "model.h"

namespace model {

Model::Model() {
    items.push_back(wxl::core::u16_text {u"Bread"});
    items.push_back(wxl::core::u16_text {u"Milk"});
}

void Model::add() {
    if (draft.get().empty()) return;
    items.push_back(draft.get());
    draft.set(wxl::core::u16_text {});
}

void Model::removeFirst() {
    if (!items.empty()) items.erase(0);
}

}
