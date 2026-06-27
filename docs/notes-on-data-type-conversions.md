Alright, let's continue with this static_cast + std::transform methodology. Forget about loading the data type from disk - I have that figured out for the most part, and let's also ignore complex vs real and endianness.

    // Tells us what the primitive is that is written to disk.
    enum class SampleType {
        FLOAT_32,
        FLOAT_64,
        INT_16,
        INT_32,
        UINT_16,
        UINT_32,
        BYTE,
        UBYTE
    };

So I need to convert between Float32 to Float32, Float64, Int_16, Int32, Uint16, Uint32, byte, and ubyte. I need to be able to do the same for every data type in the list to every other data type in the list.

I know all of the types I need to support at compile-time. So shouldn't I be able to use this to map out all of the different functions I need to implement?

---

Yes, exactly — and this is where C++ template metaprogramming actually earns its keep. Since you know all types at compile time, you can express the full N×M matrix of conversions without writing them out manually.

The core tool is a **type list**:

```cpp
#include <tuple>

using SigMFTypes = std::tuple
    float,
    double,
    int16_t,
    int32_t,
    uint16_t,
    uint32_t,
    int8_t,
    uint8_t
>;
```

Then a single templated conversion function covers every cell in that matrix:

```cpp
template <typename InputT, typename OutputT>
std::vector<OutputT> convert(const std::vector<InputT>& in) {
    std::vector<OutputT> out(in.size());
    std::transform(in.begin(), in.end(), out.begin(), [](InputT x) {
        return static_cast<OutputT>(x);
    });
    return out;
}
```

This is one function definition, but the compiler will instantiate a concrete version for every `<InputT, OutputT>` pair you actually call it with. The full 8×8 matrix exists implicitly — you just have to invoke it correctly.

The remaining design question is how you bridge from your runtime `SampleType` enum to these compile-time template parameters. The pattern for that is a dispatch function that maps each enum value to its concrete type:

```cpp
template <typename OutputT>
std::vector<OutputT> dispatch(SampleType src, const void* data, size_t n) {
    switch (src) {
        case SampleType::FLOAT_32: {
            auto* p = static_cast<const float*>(data);
            return convert<float, OutputT>({p, p + n});
        }
        case SampleType::INT_16: {
            auto* p = static_cast<const int16_t*>(data);
            return convert<int16_t, OutputT>({p, p + n});
        }
        // ... one case per SampleType
    }
}
```

Notice that `OutputT` is still a template parameter here — it was fixed at the call site by the user. The switch only dispatches on the *input* side. So you get:

- **Output type:** resolved at compile time via the template
- **Input type:** resolved at runtime via the switch

Which is exactly the half-runtime, half-compile-time structure we discussed earlier. The compiler instantiates `convert<float, OutputT>`, `convert<int16_t, OutputT>`, etc. for whatever `OutputT` the user requested — so across all callers, you do end up with the full N×M set of instantiations, but you never had to write them out.

The one thing worth flagging: if you want to verify at compile time that a user can't request a type *outside* your supported set as `OutputT`, you can add a concept or `static_assert`:

```cpp
template <typename T>
concept SigMFCompatible = requires {
    std::disjunction_v
        std::is_same<T, float>,
        std::is_same<T, double>,
        std::is_same<T, int16_t>
        // ...
    >;
};

template <SigMFCompatible OutputT>
std::vector<OutputT> dispatch(SampleType src, const void* data, size_t n);
```

That way the type safety you wanted is enforced at the boundary, before any runtime dispatch happens.
