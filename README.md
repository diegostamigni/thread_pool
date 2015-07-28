# thread_pool
### MIT License

This is a simple implementation of a thread_pool using the C++ STL APIs. For now, I require boost::thread (and its releated deps, boost::system) in order to have a method to ::interrupt() not-joinable thread. In the future, I'll remove that deps and improving the `pausing/killing' state of the thread_pool
Pull requests are appreciated. 

Objects that can be enqueued inside ds::thread_pool must have implemented the function operator ```bool operator()();``` because the pool itself will call that operator in order to execute what the object is supposed to do.

```c++
class task {
public:
  explicit task(const std::string &c)
    : m_value{c} {}
  
  virtual bool operator()() {
    std::cout << m_value << std::endl;
    return true;
  }

private:
  std::string m_value;
};
```

#####Usage:

```c++
ds::thread_pool t;
t.enqueue<task>(ds::priority_t::LOW, "!");
t.enqueue<task>(ds::priority_t::MED, "world");
t.enqueue<task>(ds::priority_t::HIGH, "hello");
```

#####Output: 
hello
world
!
