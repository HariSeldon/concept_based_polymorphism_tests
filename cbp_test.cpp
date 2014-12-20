// Modified from
// https://github.com/sean-parent/sean-parent.github.io/wiki/Papers-and-Presentations
// Copyright 2013 Adobe Systems Incorporated
// Distributed under the MIT License (see license at
// http://stlab.adobe.com/licenses.html)
// This file is intended as example code and is not production quality.

#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// Library

template <typename T> void draw(const T &x) {
  std::cout << "draw free function\n";
  std::cout << x << std::endl;
}

struct DrawableInterface {
  virtual ~DrawableInterface() = default;
  virtual void draw_() const = 0;
};

template <typename T> struct DrawableModel : DrawableInterface {
  DrawableModel(T x) : data_(std::move(x)) {}
  void draw_() const {
    std::cout << "draw member function\n";
    draw(data_);
  }

  T data_;
};

class object_t {
public:
  template <typename T>
  object_t(T x)
      : self_(std::make_shared<DrawableModel<T>>(std::move(x))) {}

  friend void draw(const object_t &x) {
    std::cout << "draw object_t friend function\n";
    x.self_->draw_();
  }

private:
  std::shared_ptr<const DrawableInterface> self_;
};

void drawVector(const std::vector<object_t> &objects) {
  std::cout << "<document>" << std::endl;
  for (auto &object : objects)
    draw(object);
  std::cout << "</document>" << std::endl;
}

// -----------------------------------------------------------------------------
// Client

class MyClass {};
void draw(const MyClass &) { std::cout << "MyClass" << std::endl; }

// -----------------------------------------------------------------------------
int main() {
  std::vector<object_t> document;
  document.emplace_back(0);
  document.emplace_back("Hello!");
  document.emplace_back(MyClass());
  drawVector(document);

  return 0;
}
