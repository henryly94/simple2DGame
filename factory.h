#ifndef FACTORY_H_
#define FACTORY_H_

#include <absl/container/flat_hash_set.h>
#include <memory>
#include <utility>

template <typename T> class Factory {
public:
  using register_type = absl::flat_hash_set<T *>;

  template <typename... Params> static T *GetNewInstance(Params &&... params) {
    T *t = new T(std::forward<Params>(params)...);
    registry().insert(t);
    return t;
  }

  static void RemoveInstance(T *instance) {
    if (instance != nullptr && registry().contains(instance)) {
      registry().erase(instance);
      delete instance;
    }
  }

private:
  static register_type &registry() {
    static register_type _;
    return _;
  }
};

#endif // FACTORY_H_
