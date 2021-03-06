//
// The MIT License (MIT)
// 
// Copyright (c) 2015 Diego Stamigni
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include <thread_pool.hpp>
#include <iostream>
#include <chrono>

class example {
public:
  explicit example(const std::string &c)
    : m_value{c} {}
  
  virtual bool operator()() {
    std::cout << m_value << std::endl;
    return true;
  }

private:
  std::string m_value;
};

int main(int argc, char **argv) {
  ds::thread_pool t;
  t.enqueue<example>(ds::priority_t::LOW, "!");
  t.enqueue<example>(ds::priority_t::MED, "world");
  t.enqueue<example>(ds::priority_t::HIGH, "hello");
  std::this_thread::sleep_for(std::chrono::seconds(3));
  return 0;
}


















