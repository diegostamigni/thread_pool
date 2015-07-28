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
#include <boost/format.hpp>
#include <boost/thread.hpp>
using namespace ds;
static constexpr const long default_cycle = 25;
static constexpr const long event_timestamp_lock = 1;

bool task_comparator::operator()(const obj_pair &item1, const obj_pair &item2) const {
  return item1.first > item2.first;
}

thread_pool::thread_pool(std::size_t threads) 
: m_num_threads(threads)
, workers(threads)
, m_working(false)
, m_stop(false)
{
  init(threads);
}

thread_pool::~thread_pool() {
  deinit();
}

void thread_pool::init(std::size_t threads) {
  for (size_t i = 0; i < threads; ++i) {
    switch (i) {
    case 0: {
      auto thread = std::make_unique<boost::thread>(std::bind(&thread_pool::services_tasks_manager, this));
      workers.push_back(std::move(thread));
      break;
    }
    }
  }
}

std::size_t thread_pool::deinit() {
  m_stop = true;
  auto using_threads = workers.size();
  
  try {
    for (auto &thread : workers) {
      if (thread.get() != nullptr) {
	thread->interrupt();
      }
    }
    
    workers.clear();
  }
  
  catch (const std::exception &e) { }
  return using_threads;
}

void thread_pool::stop(bool value) {
  std::lock_guard<std::mutex> guard(m_mutex);
  
  if (value) {
    deinit();
  }
  
  else {
    if (is_stopped()) {
      // add the other events received during the 
      // stop events into the current queue
      for (auto &item : m_tmp_service_tasks) {
	// container_functor_t is not conform to begin() and end()
	// no we cannot use std::copy(..) or std::move(..) directly  on it
	m_service_tasks.push(std::move(item));
      }
      
      m_tmp_service_tasks.clear();
      init(m_num_threads);
    }
  }
  
  m_stop = value;
}

void thread_pool::executor(container_functor_t &c) {
  looper<container_functor_t>(c, [&](container_functor_t &c) {
      if (can_execute<container_functor_t>(c)) {
	std::unique_lock<std::mutex> guard(m_mutex, std::defer_lock);
	obj_pair pair_task;
	
	guard.lock();
	pair_task = c.top();
	c.pop();
	guard.unlock();
	
	working(true);
	pair_task.second();
      }
    });
}

void thread_pool::clear() {
	m_service_tasks = container_functor_t{ };
}

void thread_pool::working(bool value) {
	m_working.store(value);
}

ds::thread_pool_status_t thread_pool::status() const {
  return working()
    ? ds::thread_pool_status_t::WORKING
    : ds::thread_pool_status_t::IDLE;
}
