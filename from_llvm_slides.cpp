class FunctionPassManager {
  typedef detail::PassConcept<Function *> FunctionPassConcept;
  template <typename PassT>
  struct FunctionPassModel : detail::PassModel<Function *, PassT> {
    FunctionPassModel(PassT Pass)
        : detail::PassModel<Function *, PassT>(std::move(Pass)) {}
  };
  std::vector<std::unique_ptr<FunctionPassConcept>> Passes;

public:
  template <typename FunctionPassT> void addPass(FunctionPassT Pass) {
    Passes.emplace_back(new FunctionPassModel<FunctionPassT>(std::move(Pass)));
  }
  void run(Function *F) {
    for (const auto &P : Passes)
      P->run(F);
  }
};

struct PassConcept {
  virtual ~PassConcept() {}
  virtual void run() = 0;
};

template <typename PassT>
struct PassModel : PassConcept {
  explicit PassModel(PassT Pass) : Pass(std::move(Pass)) {}
  void run() override { Pass.run(); }
  PassT Pass;
};

template <typename FunctionPassT> class ModuleToFunctionPassAdaptor {
  FunctionPassT Pass;

public:
  explicit ModuleToFunctionPassAdaptor(FunctionPassT Pass)
      : Pass(std::move(Pass)) {}
  void run(Module *M) {
    for (Function *F : *M)
      Pass.run(F);
  }
};
