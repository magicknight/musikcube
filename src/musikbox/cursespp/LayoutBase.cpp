#include <stdafx.h>
#include "LayoutBase.h"
#include "Colors.h"

template <typename T> static int find(std::vector<T>& haystack, T& needle) {
    int i = 0;
    std::vector<T>::iterator it = haystack.begin();
    for (; it != haystack.end(); it++, i++) {
        if ((*it) == needle) {
            return i;
        }
    }

    return -1;
}

bool sortByFocusOrder(IWindowPtr a, IWindowPtr b) {
    int orderA = a->GetFocusOrder();
    int orderB = b->GetFocusOrder();

    if (orderA == orderB) {
        return a->GetId() > b->GetId();
    }

    return orderA > orderB;
}

static inline IWindowPtr adjustFocus(IWindowPtr oldFocus, IWindowPtr newFocus) {
    if (oldFocus) {
        oldFocus->SetFrameColor(BOX_COLOR_WHITE_ON_BLACK);
        oldFocus->Blur();
    }

    if (newFocus) {
        newFocus->SetFrameColor(BOX_COLOR_RED_ON_BLACK);
        newFocus->Focus();
    }

    return newFocus;
}

LayoutBase::LayoutBase(IWindow* parent) 
: Window(parent) {
    this->focused = -1;
    this->SetFrameVisible(false);
}

LayoutBase::~LayoutBase() {

}

void LayoutBase::Show() {
    Window::Show();

    for (size_t i = 0; i < this->children.size(); i++) {
        this->children.at(i)->Show();
    }

    this->IndexFocusables();
    this->SortFocusables();

    if (this->overlay) {
        this->overlay->window->Show();
    }
}

void LayoutBase::Hide() {
    if (this->overlay) {
        this->overlay->window->Hide();
    }

    for (size_t i = 0; i < this->children.size(); i++) {
        this->children.at(i)->Hide();
    }

    Window::Hide();
}

void LayoutBase::BringToTop() {
    this->BringToTop();

    for (size_t i = 0; i < this->children.size(); i++) {
        this->children.at(i)->BringToTop();
    }
}

void LayoutBase::ShowOverlay(IWindowPtr window) {
    if (this->overlay && window != this->overlay->window) {
        throw "there's already a different overlay visible!";
    }

    this->overlay.reset(new Overlay());

    this->overlay->window = window;
    this->overlay->layout = dynamic_cast<ILayout*>(window.get());
    this->overlay->window->Show();
    this->overlay->window->BringToTop();
}

void LayoutBase::CloseOverlay() {
    if (this->overlay) {
        this->overlay->window->Hide();
        this->overlay.reset();
    }
}

IWindowPtr LayoutBase::GetOverlay() {
    return (this->overlay) ? this->overlay->window : IWindowPtr();
}

void LayoutBase::SendToBottom() {
    for (size_t i = 0; i < this->children.size(); i++) {
        this->children.at(i)->SendToBottom();
    }

    this->SendToBottom();
}

void LayoutBase::Repaint() {
    /* repaint bottom up. start with ourselves, then our children,
    recursively. */

    Window::Repaint();

    for (size_t i = 0; i < this->children.size(); i++) {
        this->children.at(i)->Repaint();
    }
}

bool LayoutBase::AddWindow(IWindowPtr window) {
    if (find(this->children, window) >= 0) {
        return true;
    }

    window->SetParent(this);

    this->children.push_back(window);
    AddFocusable(window);

    return true;
}

bool LayoutBase::RemoveWindow(IWindowPtr window) {
    this->RemoveFocusable(window);

    std::vector<IWindowPtr>::iterator it = this->children.begin();
    for ( ; it != this->children.end(); it++) {
        if (*it == window) {
            this->children.erase(it);
            return true;
        }
    }

    RemoveFocusable(window);

    return false;
}

void LayoutBase::AddFocusable(IWindowPtr window) {
    int order = window->GetFocusOrder();
    if (order >= 0 && find(this->focusable, window) < 0) {
        this->focusable.push_back(window);
        this->SortFocusables();
    }
}

void LayoutBase::IndexFocusables() {
    IWindowPtr focusedWindow;
    if (focused >= 0 && (int) this->focusable.size() > focused) {
        focusedWindow = this->focusable.at(focused);
    }

    this->focusable.clear();
    for (size_t i = 0; i < this->children.size(); i++) {
        AddFocusable(this->children.at(i));
    }

    if (focusedWindow) {
        this->focused = find(this->focusable, focusedWindow);
    }
}

void LayoutBase::SortFocusables() {
    IWindowPtr focusedWindow;
    if (focused >= 0 && (int) this->focusable.size() > focused) {
        focusedWindow = this->focusable.at(focused);
    }

    std::sort(
        this->focusable.begin(),
        this->focusable.end(),
        sortByFocusOrder);

    if (focusedWindow) {
        this->focused = find(this->focusable, focusedWindow);
    }

    if (focused == -1 && this->focusable.size() > 0) {
        this->focused = 0;
        adjustFocus(IWindowPtr(), this->focusable[this->focused]);
    }
}

void LayoutBase::RemoveFocusable(IWindowPtr window) {
    std::vector<IWindowPtr>::iterator it = this->focusable.begin();
    for (; it != this->focusable.end(); it++) {
        if (*it == window) {
            this->focusable.erase(it);
            return;
        }
    }
}

size_t LayoutBase::GetWindowCount() {
    return this->children.size();
}

IWindowPtr LayoutBase::GetWindowAt(size_t position) {
    return this->children.at(position);
}

IWindowPtr LayoutBase::FocusNext() {
    if (overlay) {
        if (overlay->layout) {
            return overlay->layout->FocusNext();
        }
        else {
            return overlay->window;
        }
    }
    else {
        IWindowPtr oldFocus = GetFocus();
        if (++this->focused >= (int) this->focusable.size()) {
            this->focused = 0;
        }

        return adjustFocus(oldFocus, GetFocus());
    }
}

IWindowPtr LayoutBase::FocusPrev() {
    if (overlay) {
        if (overlay->layout) {
            return overlay->layout->FocusPrev();
        }
        else {
            return overlay->window;
        }
    }
    else {
        IWindowPtr oldFocus = GetFocus();
        if (--this->focused <= 0) {
            this->focused = (int) this->focusable.size() - 1;
        }

        return adjustFocus(oldFocus, GetFocus());
    }
}

IWindowPtr LayoutBase::GetFocus() {
    if (overlay) {
        return overlay->window;
    }

    if (this->focused >= 0 && this->focusable.size() > 0) {
        return this->focusable[this->focused];
    }

    return IWindowPtr();
}

bool LayoutBase::KeyPress(int64 ch) {
    return false;
}