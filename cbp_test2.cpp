#include <algorithm>
#include <cassert>
#include <chrono>
#include <iostream>
#include <memory>
#include <random>
#include <vector>

typedef std::chrono::high_resolution_clock HighResolutionClock;
typedef std::chrono::nanoseconds nanoseconds;

struct DrawableInterface {
  virtual ~DrawableInterface() = default;
  virtual void draw() = 0;
};

template <typename type> struct DrawableModel : DrawableInterface {
  explicit DrawableModel(type data) : data(std::move(data)) {}
  void draw() override { 
    data.draw(); 
  }
  type data;
};

// -----------------------------------------------------------------------------
class ObjectCollection {
  template <typename type> struct InternalModel : DrawableModel<type> {
    InternalModel(type input) : DrawableModel<type>(std::move(input)) {}
  };
  std::vector<std::unique_ptr<DrawableInterface>> collection;

public:
  template <typename type> void addObject(type input) {
    collection.emplace_back(new InternalModel<type>(std::move(input)));
  }
  void draw() { 
    for (const auto &x : collection) {
      x->draw(); 
    }
  }
};

int *tmp = new int[1];

class MyClass {
public:
  void draw()  { tmp[0] = 42; }
};

class MyOtherClass {
public:
  void draw()  { tmp[1] = 101; }
};

class BaseClass {
public:
  virtual void draw() {};
};

class FirstClass : public BaseClass {
public:
  void draw() override { tmp[0] = 42; }
};

class SecondClass : public BaseClass {
public:
  void draw() override { tmp[1] = 101; }
};

int SIZE = 100000;
int REPS = 3001;

void conceptVersion() {
  std::random_device randomDevice;
  std::mt19937_64 gen(randomDevice());
  std::bernoulli_distribution distribution(0.5);
  ObjectCollection collection;
  for (int index = 0; index < SIZE; ++index) {
    if(distribution(gen)) {
      collection.addObject(MyClass());
    } else {
      collection.addObject(MyOtherClass());
    } 
  }
  std::vector<long long int> times;

  for (int rep = 0; rep < REPS; ++rep) {
    auto startTime = HighResolutionClock::now();
    collection.draw();
    auto endTime = HighResolutionClock::now();
    auto duration =
        std::chrono::duration_cast<nanoseconds>(endTime - startTime);
    times.push_back(duration.count());
  }

  std::sort(times.begin(), times.end());
  std::cout << "Concept Time: " << times[times.size() / 2] << "\n";
}

void normalVersion() {
  std::random_device randomDevice;
  std::mt19937_64 gen(randomDevice());
  std::bernoulli_distribution distribution(0.5);

  std::vector<BaseClass *> collection(SIZE);
  for (int index = 0; index < SIZE; ++index) {
    if(distribution(gen)) {
      auto x = new FirstClass();
      collection[index] = x;
    } else {
      auto x = new SecondClass();
      collection[index] = x;
    } 
  }

  std::vector<long long int> times;

  for (int rep = 0; rep < REPS; ++rep) {
    auto startTime = HighResolutionClock::now();
    for (auto x : collection) {
      assert(x != NULL);
      x->draw();
    }
    auto endTime = HighResolutionClock::now();
    auto duration =
        std::chrono::duration_cast<nanoseconds>(endTime - startTime);
    times.push_back(duration.count());
  }

  std::sort(times.begin(), times.end());
  std::cout << "Normal Time: " << times[times.size() / 2] << "\n";
}

// -----------------------------------------------------------------------------
int main() {
  conceptVersion();
  normalVersion();
}
