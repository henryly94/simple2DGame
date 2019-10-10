#ifndef FACTORY_H_
#define FACTORY_H_

#include <vector>
#include <utility>
#include <memory>

template<typename T>
class Factory {
 public:
  using register_type = std::vector<std::unique_ptr<T>>;

  template<typename ...Params>
  static T* GetNewInstance(Params&&... params) {
    std::unique_ptr<T> t = std::make_unique<T>(std::forward<Params>(params)...); 
    T* ret = t.get();
    registry().push_back(std::move(t));
    return ret;
  }
 private:
  static register_type& registry() {
    static register_type _;
    return _;
  }
};

#endif // FACTORY_H_
