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

#pragma once

#include <atomic>
#include <functional>
#include <boost/noncopyable.hpp>
#include <utility>
#include <priority_queue.hpp>
#include <queue>
#include <type_traits>
#include <chrono>
#include <vector>
#include <string>

// forward declaration 
// we're not exposing boost::thread
namespace boost {
  class thread;
}

namespace ds {
  enum class thread_pool_status_t: int { IDLE, WORKING };
  enum class priority_t : int { HIGH, MED, LOW };
  
  using functor_t = std::function<bool()>;
  using obj_pair = std::pair<ds::priority_t, functor_t>;
  
  class task_comparator {
  public:
    bool operator()(const obj_pair &item1, const obj_pair &item2) const;
  };
  
  using container_vect = std::vector<obj_pair>;
  using container_functor_t = std::priority_queue<obj_pair, container_vect, task_comparator>;
  constexpr std::size_t default_threads_count = 1;
  
  class thread_pool : private boost::noncopyable {
  public:
    explicit thread_pool(std::size_t threads = default_threads_count);
    void stop(bool value);
    bool working() const { return m_working.load(); }
    ds::thread_pool_status_t status() const;
    bool is_stopped() const { return m_stop; }
    
    template <typename Type, typename... Args>
    void enqueue(ds::priority_t priority, Args&&... args) {      
      std::unique_lock<std::mutex> guard(m_mutex, std::defer_lock);
      auto task = std::make_shared<Type>(std::forward<Args>(args)...);
      auto func = [task]() { return (*task)(); };
      
      if (!is_stopped()) {
	guard.lock();
	m_service_tasks.emplace(priority, func);
      }
      
      else {
	guard.lock();
	m_tmp_service_tasks.emplace_back(priority, func);
      } 
    }
    
    virtual ~thread_pool();
    virtual void clear();
    
  protected:
    virtual void init(std::size_t threads = default_threads_count);
    virtual std::size_t deinit();
    void working(bool value);
    
    private:
    // pool
    std::mutex m_mutex;
    std::size_t m_num_threads;
    std::vector<std::unique_ptr<boost::thread>> workers;
    container_functor_t m_service_tasks;
    container_vect m_tmp_service_tasks;    
    std::atomic_bool m_working;
    std::atomic_bool m_stop;
    void init_looper();
    
    template <typename Container>
    void looper(Container &c, const std::function<void(Container &c)> &executor) {
      for (;;) {
	if (c.empty()) {
	  working(false);
	  std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
	
	else {
	  executor(c);
	  std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
      }
    }
    
    void executor(container_functor_t &c);
    
    template<typename T>
    bool can_execute(const T &container, bool check_timestamp = true) {
      if (!container.empty()) {
	bool stopped = is_stopped();
	return (!stopped);
      }
      
      return false;
    }
    
    void services_tasks_manager() {
      executor(m_service_tasks);
    }
  };
} // ds
