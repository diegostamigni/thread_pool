# priority_queue
### MIT License

This is a simple implementation of a priority_queue using the C++ STL APIs.
What I'm trying to do (with a future commits) is to create a lockfree (waitfree) version of it.

Pull requests are appreciated. 

```c++
enum class priority_t {
	HIGH,
	MED,
	LOW
};

using obj_pair = std::pair<priority_t, std::string>;

struct comparator {
    bool operator()(const obj_pair &item1, const obj_pair &item2) {
        return item1.first > item2.first;
    }
};

using queue_t = ds::priority_queue<obj_pair, std::vector<obj_pair>, comparator>;
```

#####Usage:

```c++
queue_t hq;
hq.emplace(priority_t::MED, "world");
hq.emplace(priority_t::HIGH, "Hello");
hq.emplace(priority_t::LOW, "!");

// -- pop
obj_pair item1, item2, item3;
hq.pop(item1); hq.pop(item2); hq.pop(item3);
std::cout << item1.second << " " << item2.second << " " << item3.second << std::endl;
```
#####Output: 
Hello world !
